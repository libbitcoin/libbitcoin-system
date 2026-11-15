/**
 * Copyright (c) 2011-2026 libbitcoin developers
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/system/compression/deflate.hpp>

#include <algorithm>
#include <functional>
#include <queue>
#include <utility>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>
#include <bitcoin/system/math/math.hpp>
#include "rfc1951.hpp"

namespace libbitcoin {
namespace system {

using namespace rfc1951;

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)

// Match search: hash of minimum_match bytes, chained within the window.
constexpr size_t hash_bits = 15;
constexpr size_t hash_mask = sub1(power2(hash_bits));
constexpr size_t window_mask = sub1(window_size);
constexpr size_t maximum_chain = 128;
constexpr uint32_t no_position = max_uint32;

// Symbols and input bytes coded per block, each block coded by its own
// frequencies.
constexpr size_t block_symbols = 16384;
constexpr size_t block_bytes = 65536;

// Input bytes buffered beyond the window, and those held back for a match.
constexpr size_t input_bytes = window_size;
constexpr size_t lookahead = maximum_match + minimum_match;

// Bits of a stored block header (excluding alignment) and its lengths.
constexpr size_t block_header_bits = 3;
constexpr size_t stored_overhead_bits = sub1(byte_bits) + 2u * bits<uint16_t>;

// A code length code and its repeat count bits.
struct length_token
{
    uint16_t code;
    uint16_t extra;
};

using lz_symbols = std_vector<uint32_t>;
using length_tokens = std_vector<length_token>;

// A literal (zero distance), or a match length and distance.
constexpr uint32_t to_symbol(size_t value, size_t distance) NOEXCEPT
{
    return possible_narrow_cast<uint32_t>(bit_or(shift_left(distance,
        bits<uint16_t>), value));
}

constexpr size_t value_of(uint32_t symbol) NOEXCEPT
{
    return bit_and<uint32_t>(symbol, max_uint16);
}

constexpr size_t distance_of(uint32_t symbol) NOEXCEPT
{
    return shift_right(symbol, bits<uint16_t>);
}

template <size_t Size>
struct code_tree
{
    std_array<uint8_t, Size> lengths{};
    std_array<uint16_t, Size> codes{};
};

template <size_t Size>
using frequencies = std_array<uint32_t, Size>;

using literal_tree = code_tree<literal_codes>;
using distance_tree = code_tree<distance_codes>;
using length_tree = code_tree<length_codes>;

// Bits are written from the low end of each byte (rfc1951 3.1.1).
class stream_writer
{
public:
    stream_writer(data_chunk& out, size_t& buffer, size_t& count) NOEXCEPT
      : out_{ out }, buffer_{ buffer }, bit_count_{ count }
    {
    }

    void write(size_t value, size_t count) NOEXCEPT
    {
        buffer_ |= shift_left(value, bit_count_);
        bit_count_ += count;
        while (bit_count_ >= byte_bits)
        {
            out_.push_back(narrow_cast<uint8_t>(buffer_));
            buffer_ = shift_right(buffer_, byte_bits);
            bit_count_ -= byte_bits;
        }
    }

    void align() NOEXCEPT
    {
        if (is_nonzero(bit_count_))
            write(zero, byte_bits - bit_count_);
    }

    void bytes(const data_slice& data) NOEXCEPT
    {
        out_.insert(out_.end(), data.begin(), data.end());
    }

private:
    data_chunk& out_;
    size_t& buffer_;
    size_t& bit_count_;
};

// Symbol coding.
// ----------------------------------------------------------------------------

template <size_t Size>
static size_t index_of(const std_array<uint16_t, Size>& bases,
    size_t value) NOEXCEPT
{
    const auto it = std::upper_bound(bases.begin(), bases.end(), value);
    return sub1(sign_cast<size_t>(std::distance(bases.begin(), it)));
}

// Codes are written most significant bit first (rfc1951 3.1.1).
static uint16_t reverse_code(uint16_t code, size_t length) NOEXCEPT
{
    uint16_t out{};
    for (size_t bit{}; bit < length; ++bit)
    {
        out = bit_or(shift_left(out), bit_and(code, 1_u16));
        code = shift_right(code);
    }

    return out;
}

// Canonical codes from lengths (rfc1951 3.2.2).
template <size_t Size>
static void assign_codes(code_tree<Size>& out) NOEXCEPT
{
    std_array<size_t, add1(maximum_bits)> count{};
    for (const auto length: out.lengths)
        ++count[length];

    size_t code{};
    count[zero] = zero;
    std_array<size_t, add1(maximum_bits)> next{};
    for (size_t length = one; length <= maximum_bits; ++length)
    {
        code = shift_left(code + count[sub1(length)]);
        next[length] = code;
    }

    for (size_t symbol{}; symbol < Size; ++symbol)
    {
        const auto length = out.lengths[symbol];
        if (is_nonzero(length))
            out.codes[symbol] = reverse_code(narrow_cast<uint16_t>(
                next[length]++), length);
    }
}

// Huffman code lengths of at most limit bits, at least two codes so that
// every code is complete (rfc1951 3.2.2).
template <size_t Size>
static void build_tree(code_tree<Size>& out, const frequencies<Size>& counts,
    size_t limit) NOEXCEPT
{
    using item = std::pair<uint64_t, size_t>;
    struct node
    {
        uint64_t weight;
        size_t parent;
    };

    std_vector<item> leaves{};
    for (size_t symbol{}; symbol < Size; ++symbol)
        if (is_nonzero(counts[symbol]))
            leaves.emplace_back(counts[symbol], symbol);

    for (size_t symbol{}; leaves.size() < two; ++symbol)
        if (is_zero(counts[symbol]))
            leaves.emplace_back(one, symbol);

    std_vector<node> nodes{};
    std::priority_queue<item, std_vector<item>, std::greater<item>> queue{};
    for (const auto& leaf: leaves)
    {
        queue.emplace(leaf.first, nodes.size());
        nodes.push_back({ leaf.first, no_position });
    }

    while (queue.size() > one)
    {
        const auto left = queue.top();
        queue.pop();
        const auto right = queue.top();
        queue.pop();

        const auto parent = nodes.size();
        const auto weight = left.first + right.first;
        nodes[left.second].parent = parent;
        nodes[right.second].parent = parent;
        nodes.push_back({ weight, no_position });
        queue.emplace(weight, parent);
    }

    // Each parent follows its children, the root is last.
    std_vector<size_t> depths(nodes.size(), zero);
    for (auto index = sub1(nodes.size()); !is_zero(index);)
    {
        --index;
        depths[index] = add1(depths[nodes[index].parent]);
    }

    std_array<size_t, add1(maximum_bits)> count{};
    for (size_t leaf{}; leaf < leaves.size(); ++leaf)
        ++count[std::min(depths[leaf], limit)];

    // Lengthen codes until the clamped lengths satisfy the Kraft equality.
    const auto capacity = power2<uint64_t>(limit);
    uint64_t total{};
    for (size_t length = one; length <= limit; ++length)
        total += shift_left<uint64_t>(count[length], limit - length);

    for (; total > capacity; --total)
    {
        --count[limit];
        for (auto length = sub1(limit); !is_zero(length); --length)
        {
            if (is_nonzero(count[length]))
            {
                --count[length];
                count[add1(length)] += two;
                break;
            }
        }
    }

    // The least frequent symbols take the longest codes.
    std::sort(leaves.begin(), leaves.end());
    out.lengths.fill(0_u8);
    auto leaf = leaves.begin();
    for (auto length = limit; !is_zero(length); --length)
        for (auto remaining = count[length]; !is_zero(remaining); --remaining)
            out.lengths[(leaf++)->second] = narrow_cast<uint8_t>(length);

    assign_codes(out);
}

// Fixed Huffman codes (rfc1951 3.2.6).
static const literal_tree& fixed_literals() NOEXCEPT
{
    static const auto code = []() NOEXCEPT
    {
        size_t start{};
        literal_tree out{};
        for (size_t range{}; range < fixed_ends.size(); ++range)
        {
            const auto end = fixed_ends[range];
            std::fill(std::next(out.lengths.begin(), start),
                std::next(out.lengths.begin(), end), fixed_lengths[range]);

            start = end;
        }

        assign_codes(out);
        return out;
    }();

    return code;
}

static const distance_tree& fixed_distances() NOEXCEPT
{
    static const auto code = []() NOEXCEPT
    {
        distance_tree out{};
        out.lengths.fill(fixed_distance_length);
        assign_codes(out);
        return out;
    }();

    return code;
}

// Matching (rfc1951 4).
// ----------------------------------------------------------------------------

static size_t hash_key(const data_slice& in, size_t position) NOEXCEPT
{
    constexpr auto shift = hash_bits / minimum_match;
    const size_t first = in[position];
    const size_t second = in[add1(position)];
    const size_t third = in[position + two];
    return bit_and(bit_xor(bit_xor(shift_left(first, two * shift),
        shift_left(second, shift)), third), hash_mask);
}

// Blocks (rfc1951 3.2.3).
// ----------------------------------------------------------------------------

template <size_t Size>
static size_t coded_bits(const code_tree<Size>& code,
    const frequencies<Size>& counts) NOEXCEPT
{
    size_t out{};
    for (size_t symbol{}; symbol < Size; ++symbol)
        out += counts[symbol] * code.lengths[symbol];

    return out;
}

// Run length coding of the code lengths (rfc1951 3.2.7).
static length_tokens run_lengths(const data_chunk& lengths) NOEXCEPT
{
    constexpr size_t maximum_repeat = 6;
    constexpr size_t maximum_zeros = 10;
    constexpr size_t maximum_long_zeros = 138;
    constexpr auto minimum_repeat = repeat_base[0];
    constexpr auto minimum_zeros = repeat_base[1];
    constexpr auto minimum_long_zeros = repeat_base[2];

    length_tokens out{};
    for (size_t index{}; index < lengths.size();)
    {
        const auto length = lengths[index];
        size_t run{ one };
        while (index + run < lengths.size() && lengths[index + run] == length)
            ++run;

        index += run;
        if (is_zero(length))
        {
            while (run >= minimum_long_zeros)
            {
                const auto count = std::min(run, maximum_long_zeros);
                out.push_back({ repeat_long_zeros,
                    narrow_cast<uint16_t>(count - minimum_long_zeros) });
                run -= count;
            }

            if (run >= minimum_zeros)
            {
                const auto count = std::min(run, maximum_zeros);
                out.push_back({ repeat_zeros,
                    narrow_cast<uint16_t>(count - minimum_zeros) });
                run -= count;
            }
        }
        else
        {
            out.push_back({ length, zero });
            --run;
            while (run >= minimum_repeat)
            {
                const auto count = std::min(run, maximum_repeat);
                out.push_back({ repeat_previous,
                    narrow_cast<uint16_t>(count - minimum_repeat) });
                run -= count;
            }
        }

        for (; !is_zero(run); --run)
            out.push_back({ length, zero });
    }

    return out;
}

static void write_symbols(stream_writer& writer, const lz_symbols& data,
    size_t begin, size_t end, const literal_tree& literals,
    const distance_tree& distances) NOEXCEPT
{
    for (auto index = begin; index < end; ++index)
    {
        const auto value = value_of(data[index]);
        const auto offset = distance_of(data[index]);
        if (is_zero(offset))
        {
            writer.write(literals.codes[value], literals.lengths[value]);
            continue;
        }

        const auto length = index_of(length_base, value);
        const auto code = first_length + length;
        writer.write(literals.codes[code], literals.lengths[code]);
        writer.write(value - length_base[length], length_extra[length]);

        const auto distance = index_of(distance_base, offset);
        writer.write(distances.codes[distance], distances.lengths[distance]);
        writer.write(offset - distance_base[distance],
            distance_extra[distance]);
    }

    writer.write(literals.codes[end_of_block], literals.lengths[end_of_block]);
}

static void write_stored(stream_writer& writer, const data_slice& span,
    bool last) NOEXCEPT
{
    auto begin = span.begin();
    auto remaining = span.size();
    do
    {
        const auto size = std::min(remaining, maximum_stored);
        remaining -= size;

        const auto length = narrow_cast<uint16_t>(size);
        writer.write(last && is_zero(remaining) ? one : zero, one);
        writer.write(stored_block, two);
        writer.align();
        writer.write(length, bits<uint16_t>);
        writer.write(bit_not(length), bits<uint16_t>);

        const auto end = std::next(begin, size);
        writer.bytes(data_slice(begin, end));
        begin = end;
    } while (!is_zero(remaining));
}

static void write_block(stream_writer& writer, const lz_symbols& data,
    size_t begin, size_t end, const data_slice& span, bool last) NOEXCEPT
{
    frequencies<literal_codes> literal_counts{};
    frequencies<distance_codes> distance_counts{};
    size_t extra_bits{};
    for (auto index = begin; index < end; ++index)
    {
        const auto value = value_of(data[index]);
        const auto offset = distance_of(data[index]);
        if (is_zero(offset))
        {
            ++literal_counts[value];
            continue;
        }

        const auto length = index_of(length_base, value);
        const auto distance = index_of(distance_base, offset);
        ++literal_counts[first_length + length];
        ++distance_counts[distance];
        extra_bits += length_extra[length] + distance_extra[distance];
    }

    ++literal_counts[end_of_block];

    // Dynamic codes and their code length coding.
    literal_tree literals{};
    distance_tree distances{};
    build_tree(literals, literal_counts, maximum_bits);
    build_tree(distances, distance_counts, maximum_bits);

    auto literal_count = literal_limit;
    while (literal_count > first_length &&
        is_zero(literals.lengths[sub1(literal_count)]))
        --literal_count;

    auto distance_count = distance_codes;
    while (distance_count > one &&
        is_zero(distances.lengths[sub1(distance_count)]))
        --distance_count;

    data_chunk lengths{};
    const auto literal_end = std::next(literals.lengths.begin(), literal_count);
    const auto distance_end = std::next(distances.lengths.begin(),
        distance_count);
    lengths.insert(lengths.end(), literals.lengths.begin(), literal_end);
    lengths.insert(lengths.end(), distances.lengths.begin(), distance_end);
    const auto runs = run_lengths(lengths);

    frequencies<length_codes> length_counts{};
    for (const auto& run: runs)
        ++length_counts[run.code];

    length_tree code_lengths{};
    build_tree(code_lengths, length_counts, maximum_length_bits);

    auto length_count = length_codes;
    while (length_count > minimum_length_count &&
        is_zero(code_lengths.lengths[length_order[sub1(length_count)]]))
        --length_count;

    size_t runs_bits{};
    for (const auto& run: runs)
    {
        runs_bits += code_lengths.lengths[run.code];
        if (run.code >= repeat_previous)
            runs_bits += repeat_extra[run.code - repeat_previous];
    }

    const auto dynamic_bits = block_header_bits + literal_count_bits +
        distance_count_bits + length_count_bits +
        length_count * code_length_bits + runs_bits + extra_bits +
        coded_bits(literals, literal_counts) +
        coded_bits(distances, distance_counts);

    const auto fixed_bits = block_header_bits + extra_bits +
        coded_bits(fixed_literals(), literal_counts) +
        coded_bits(fixed_distances(), distance_counts);

    const auto pieces = std::max(one, ceilinged_divide(span.size(),
        maximum_stored));
    const auto stored_bits = pieces * (block_header_bits +
        stored_overhead_bits) + span.size() * byte_bits;

    if (stored_bits <= std::min(fixed_bits, dynamic_bits))
    {
        write_stored(writer, span, last);
        return;
    }

    writer.write(last ? one : zero, one);
    if (fixed_bits <= dynamic_bits)
    {
        writer.write(fixed_block, two);
        write_symbols(writer, data, begin, end, fixed_literals(),
            fixed_distances());
        return;
    }

    writer.write(dynamic_block, two);
    writer.write(literal_count - first_length, literal_count_bits);
    writer.write(distance_count - one, distance_count_bits);
    writer.write(length_count - minimum_length_count, length_count_bits);
    for (size_t index{}; index < length_count; ++index)
        writer.write(code_lengths.lengths[length_order[index]],
            code_length_bits);

    for (const auto& run: runs)
    {
        writer.write(code_lengths.codes[run.code],
            code_lengths.lengths[run.code]);
        if (run.code >= repeat_previous)
            writer.write(run.extra, repeat_extra[run.code - repeat_previous]);
    }

    write_symbols(writer, data, begin, end, literals, distances);
}

// Compares whole words, advancing length past equal bytes, false if unequal.
template <typename xWord>
INLINE bool match_words(size_t& length, const uint8_t* left,
    const uint8_t* right, size_t limit) NOEXCEPT
{
    if constexpr (have<xWord>)
    {
        constexpr auto bytes = sizeof(xWord);
        for (; limit - length >= bytes; length += bytes)
        {
            const auto index = f::first_unequal(
                f::load<xWord>(std::next(left, length)),
                f::load<xWord>(std::next(right, length)));

            if (index < bytes)
            {
                length += index;
                return false;
            }
        }
    }

    return true;
}

// The length of the match at candidate to that at position, up to limit, or
// zero if the byte at best differs (the match cannot exceed best).
static size_t match_length(const data_chunk& window, size_t candidate,
    size_t position, size_t best, size_t limit) NOEXCEPT
{
    if (window[candidate + best] != window[position + best])
        return zero;

    size_t length{};
    const auto left = std::next(window.data(), candidate);
    const auto right = std::next(window.data(), position);
    if (match_words<xint512_t>(length, left, right, limit) &&
        match_words<xint256_t>(length, left, right, limit) &&
        match_words<xint128_t>(length, left, right, limit))
        while (length < limit && left[length] == right[length])
            ++length;

    return length;
}

// deflater
// ----------------------------------------------------------------------------

deflater::deflater() NOEXCEPT
  : heads_(add1(hash_mask), no_position),
    chains_(window_size, no_position)
{
}

void deflater::write(data_chunk& out, const data_slice& data) NOEXCEPT
{
    auto it = data.begin();
    for (auto remaining = data.size(); !is_zero(remaining);)
    {
        const auto size = std::min(input_bytes, remaining);
        const auto end = std::next(it, size);
        window_.insert(window_.end(), it, end);
        remaining -= size;
        it = end;
        tokenize(out, false);
    }
}

void deflater::flush(data_chunk& out) NOEXCEPT
{
    tokenize(out, true);
    emit(out, true);
    stream_writer{ out, buffer_, bit_count_ }.align();
    *this = deflater{};
}

void deflater::insert(size_t position) NOEXCEPT
{
    const auto key = hash_key(window_, position);
    chains_[bit_and(position, window_mask)] = heads_[key];
    heads_[key] = possible_narrow_cast<uint32_t>(position);
}

// Positions shift by the window, so each keeps its chain slot.
void deflater::slide() NOEXCEPT
{
    constexpr auto shift = possible_narrow_cast<uint32_t>(window_size);
    const auto rebase = [](uint32_t& position) NOEXCEPT
    {
        position = position == no_position || position < shift ?
            no_position : position - shift;
    };

    window_.erase(window_.begin(), std::next(window_.begin(), window_size));
    position_ -= window_size;
    std::for_each(heads_.begin(), heads_.end(), rebase);
    std::for_each(chains_.begin(), chains_.end(), rebase);
}

// Greedy longest match within the window at each position, holding back the
// lookahead until the stream ends.
void deflater::tokenize(data_chunk& out, bool final) NOEXCEPT
{
    while (position_ < window_.size() &&
        (final || window_.size() - position_ >= lookahead))
    {
        if (position_ >= two * window_size)
            slide();

        const auto size = window_.size();
        size_t best{}, best_distance{};
        if (position_ + minimum_match <= size)
        {
            const auto limit = std::min(maximum_match, size - position_);
            auto candidate = heads_[hash_key(window_, position_)];
            for (size_t chain{}; candidate != no_position &&
                chain < maximum_chain; ++chain)
            {
                const auto distance = position_ - candidate;
                if (distance > window_size)
                    break;

                const auto length = match_length(window_, candidate,
                    position_, best, limit);

                if (length > best)
                {
                    best = length;
                    best_distance = distance;
                    if (best == limit)
                        break;
                }

                candidate = chains_[bit_and<size_t>(candidate, window_mask)];
            }

            insert(position_);
        }

        if (best < minimum_match)
        {
            add(out, to_symbol(window_[position_], zero), one);
            ++position_;
            continue;
        }

        add(out, to_symbol(best, best_distance), best);
        const auto end = position_ + best;
        for (++position_; position_ < end; ++position_)
            if (position_ + minimum_match <= size)
                insert(position_);
    }
}

// A full block is coded once another symbol follows it.
void deflater::add(data_chunk& out, uint32_t symbol, size_t size) NOEXCEPT
{
    if (symbols_.size() == block_symbols || raw_.size() >= block_bytes)
        emit(out, false);

    const auto begin = std::next(window_.begin(), position_);
    symbols_.push_back(symbol);
    raw_.insert(raw_.end(), begin, std::next(begin, size));
}

void deflater::emit(data_chunk& out, bool last) NOEXCEPT
{
    stream_writer writer{ out, buffer_, bit_count_ };
    write_block(writer, symbols_, zero, symbols_.size(), raw_, last);
    symbols_.clear();
    raw_.clear();
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

data_chunk deflate(const data_slice& in) NOEXCEPT
{
    data_chunk out{};
    deflater encoder{};
    encoder.write(out, in);
    encoder.flush(out);
    return out;
}

} // namespace system
} // namespace libbitcoin

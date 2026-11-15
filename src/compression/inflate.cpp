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
#include <bitcoin/system/compression/inflate.hpp>

#include <algorithm>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>
#include "rfc1951.hpp"

namespace libbitcoin {
namespace system {

using namespace rfc1951;

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

using huffman = inflater::huffman;

// A root table entry: the symbol and its code length (zero if longer).
constexpr size_t entry_bits = 4;
constexpr auto entry_mask = sub1(power2<uint16_t>(entry_bits));
constexpr auto root_size = power2(inflater::root_bits);
constexpr auto root_mask = sub1(root_size);

// Stored block lengths.
constexpr size_t stored_length_bits = 2u * bits<uint16_t>;

enum class decoded : uint8_t
{
    found,
    more,
    invalid
};

// Codes.
// ----------------------------------------------------------------------------

constexpr uint16_t to_entry(size_t symbol, size_t length) NOEXCEPT
{
    return possible_narrow_cast<uint16_t>(bit_or(shift_left(symbol,
        entry_bits), length));
}

constexpr size_t entry_symbol(uint16_t entry) NOEXCEPT
{
    return shift_right(entry, entry_bits);
}

constexpr size_t entry_length(uint16_t entry) NOEXCEPT
{
    return bit_and(entry, entry_mask);
}

// Codes are written most significant bit first (rfc1951 3.1.1).
constexpr size_t reverse_code(size_t code, size_t length) NOEXCEPT
{
    size_t out{};
    for (size_t bit{}; bit < length; ++bit)
    {
        out = bit_or(shift_left(out), bit_and(code, one));
        code = shift_right(code);
    }

    return out;
}

// Canonical code and root table from lengths, false if over-subscribed
// (rfc1951 3.2.2).
static bool build(huffman& out, const uint8_t* lengths, size_t size) NOEXCEPT
{
    out = {};
    for (size_t symbol{}; symbol < size; ++symbol)
        ++out.count[lengths[symbol]];

    auto left = one;
    out.count[zero] = zero;
    for (auto length = one; length <= maximum_bits; ++length)
    {
        left = shift_left(left);
        if (out.count[length] > left)
            return false;

        left -= out.count[length];
    }

    size_t code{};
    std_array<uint16_t, add1(maximum_bits)> offsets{};
    std_array<size_t, add1(maximum_bits)> next{};
    for (auto length = one; length <= maximum_bits; ++length)
    {
        code = shift_left(code + out.count[sub1(length)]);
        next[length] = code;
        if (length < maximum_bits)
            offsets[add1(length)] = offsets[length] + out.count[length];
    }

    for (size_t symbol{}; symbol < size; ++symbol)
    {
        const size_t length = lengths[symbol];
        if (is_zero(length))
            continue;

        out.symbol[offsets[length]++] = possible_narrow_cast<uint16_t>(symbol);
        if (length > inflater::root_bits)
        {
            ++next[length];
            continue;
        }

        const auto entry = to_entry(symbol, length);
        const auto step = power2(length);
        for (auto index = reverse_code(next[length]++, length);
            index < root_size; index += step)
            out.table[index] = entry;
    }

    return true;
}

// Complete code, or if partial a single one bit code or none.
static bool complete(const huffman& code, bool partial) NOEXCEPT
{
    size_t used{}, left{ one };
    for (auto length = one; length <= maximum_bits; ++length)
    {
        left = shift_left(left) - code.count[length];
        used += code.count[length];
    }

    if (is_zero(left))
        return true;

    return partial && (is_zero(used) ||
        (is_one(used) && is_one(code.count[one])));
}

// The symbol of the code that begins bits, of which available are valid.
static decoded decode(size_t& symbol, size_t& length, const huffman& code,
    uint64_t bits, size_t available) NOEXCEPT
{
    const auto root = possible_narrow_cast<size_t>(bit_and<uint64_t>(bits,
        root_mask));

    const auto entry = code.table[root];
    if (!is_zero(entry_length(entry)))
    {
        length = entry_length(entry);
        if (length > available)
            return decoded::more;

        symbol = entry_symbol(entry);
        return decoded::found;
    }

    // Codes are read most significant bit first (rfc1951 3.1.1).
    size_t value{}, first{}, index{};
    for (length = one; length <= maximum_bits; ++length)
    {
        if (length > available)
            return decoded::more;

        const auto bit = possible_narrow_cast<size_t>(bit_and<uint64_t>(
            shift_right(bits, sub1(length)), one));

        value = bit_or(value, bit);

        const size_t count = code.count[length];
        if (value < first + count)
        {
            symbol = code.symbol[index + value - first];
            return decoded::found;
        }

        index += count;
        first = shift_left(first + count);
        value = shift_left(value);
    }

    return decoded::invalid;
}

static const std_array<huffman, two>& fixed() NOEXCEPT
{
    static const auto codes = []() NOEXCEPT
    {
        size_t start{};
        std_array<uint8_t, literal_codes> lengths{};
        for (size_t range{}; range < fixed_ends.size(); ++range)
        {
            const auto end = fixed_ends[range];
            std::fill(std::next(lengths.begin(), start),
                std::next(lengths.begin(), end), fixed_lengths[range]);

            start = end;
        }

        std_array<uint8_t, distance_codes> distance{};
        distance.fill(fixed_distance_length);

        std_array<huffman, two> out{};
        build(out.front(), lengths.data(), lengths.size());
        build(out.back(), distance.data(), distance.size());
        return out;
    }();

    return codes;
}

// inflater
// ----------------------------------------------------------------------------

inflater::inflater() NOEXCEPT = default;

bool inflater::done() const NOEXCEPT
{
    return state_ == state::done;
}

bool inflater::write(data_chunk& out, size_t& consumed, const data_slice& in,
    size_t limit) NOEXCEPT
{
    in_ = in.data();
    size_ = in.size();
    position_ = zero;
    produced_ = zero;
    limit_ = is_zero(limit) ? max_size_t : limit;
    const auto history = window_.size();

    auto valid = true, starved = false;
    while (valid && state_ != state::done && !full())
    {
        step result{};
        switch (state_)
        {
            case state::header: result = header(); break;
            case state::stored_length: result = stored_length(); break;
            case state::stored_copy: result = stored_copy(); break;
            case state::table: result = table(); break;
            case state::code_lengths: result = code_lengths(); break;
            case state::lengths: result = lengths(); break;
            case state::codes: result = codes(); break;
            case state::done: result = step::next; break;
        }

        if (result == step::invalid)
            valid = false;
        else if (result == step::more &&
            (state_ == state::stored_copy || !pull()))
        {
            starved = true;
            break;
        }
    }

    // The bits of a step starved of input are retained for its completion.
    if (!starved)
        restore();
    const auto begin = std::next(window_.begin(), history);
    out.insert(out.end(), begin, window_.end());
    if (window_.size() > window_size)
        window_.erase(window_.begin(), std::prev(window_.end(), window_size));

    consumed = position_;
    in_ = nullptr;
    size_ = zero;
    return valid;
}

// private
// ----------------------------------------------------------------------------

bool inflater::full() const NOEXCEPT
{
    return produced_ >= limit_;
}

// Bits are taken from the low end of each byte (rfc1951 3.1.1).
bool inflater::pull() NOEXCEPT
{
    if (position_ == size_)
        return false;

    const uint64_t byte = in_[position_++];
    buffer_ |= shift_left(byte, bit_count_);
    bit_count_ += byte_bits;
    return true;
}

// Fills the buffer to at least 56 bits from eight bytes of input. Bits of the
// partially taken byte above the count are those that the next refill or pull
// sets in the same position.
void inflater::refill() NOEXCEPT
{
    const auto bytes = unsafe_from_little_endian<uint64_t>(std::next(in_,
        position_));

    const auto taken = (sub1(bits<uint64_t>) - bit_count_) / byte_bits;
    buffer_ |= shift_left(bytes, bit_count_);
    position_ += taken;
    bit_count_ += taken * byte_bits;
}

// Returns whole bytes of the buffer to the input of the current write.
void inflater::restore() NOEXCEPT
{
    const auto whole = bit_count_ / byte_bits;
    position_ -= whole;
    bit_count_ -= whole * byte_bits;
    buffer_ = bit_and(buffer_, unmask_right<uint64_t>(bit_count_));
}

void inflater::drop(size_t bits) NOEXCEPT
{
    buffer_ = shift_right(buffer_, bits);
    bit_count_ -= bits;
}

// Block header (rfc1951 3.2.3).
inflater::step inflater::header() NOEXCEPT
{
    if (bit_count_ < add1(two))
        return step::more;

    last_ = is_one(bit_and<uint64_t>(buffer_, one));
    const auto type = bit_and<uint64_t>(shift_right(buffer_), 3);
    drop(add1(two));

    switch (type)
    {
        case stored_block:
        {
            drop(bit_count_ % byte_bits);
            state_ = state::stored_length;
            return step::next;
        }
        case fixed_block:
        {
            literals_ = fixed().front();
            distances_ = fixed().back();
            state_ = state::codes;
            return step::next;
        }
        case dynamic_block:
        {
            state_ = state::table;
            return step::next;
        }
        default:
        {
            return step::invalid;
        }
    }
}

// Uncompressed block, byte aligned (rfc1951 3.2.4).
inflater::step inflater::stored_length() NOEXCEPT
{
    if (bit_count_ < stored_length_bits)
        return step::more;

    const auto length = bit_and<uint64_t>(buffer_, max_uint16);
    const auto complement = bit_and<uint64_t>(shift_right(buffer_,
        bits<uint16_t>), max_uint16);

    if (length != bit_xor<uint64_t>(complement, max_uint16))
        return step::invalid;

    drop(stored_length_bits);
    restore();
    remaining_ = possible_narrow_cast<size_t>(length);
    state_ = state::stored_copy;
    return step::next;
}

inflater::step inflater::stored_copy() NOEXCEPT
{
    if (is_zero(remaining_))
    {
        state_ = last_ ? state::done : state::header;
        return step::next;
    }

    const auto size = std::min({ remaining_, size_ - position_,
        limit_ - produced_ });

    if (is_zero(size))
        return step::more;

    const auto begin = std::next(in_, position_);
    window_.insert(window_.end(), begin, std::next(begin, size));
    position_ += size;
    produced_ += size;
    remaining_ -= size;
    return step::next;
}

// Dynamic Huffman code counts (rfc1951 3.2.7).
inflater::step inflater::table() NOEXCEPT
{
    constexpr auto width = literal_count_bits + distance_count_bits +
        length_count_bits;

    if (bit_count_ < width)
        return step::more;

    literal_count_ = possible_narrow_cast<size_t>(bit_and<uint64_t>(buffer_,
        sub1(power2(literal_count_bits)))) + first_length;
    distance_count_ = possible_narrow_cast<size_t>(bit_and<uint64_t>(
        shift_right(buffer_, literal_count_bits),
        sub1(power2(distance_count_bits)))) + one;
    length_count_ = possible_narrow_cast<size_t>(bit_and<uint64_t>(
        shift_right(buffer_, literal_count_bits + distance_count_bits),
        sub1(power2(length_count_bits)))) + minimum_length_count;

    drop(width);
    if (literal_count_ > literal_limit || distance_count_ > distance_codes)
        return step::invalid;

    index_ = zero;
    lengths_.fill(0_u8);
    state_ = state::code_lengths;
    return step::next;
}

// Code length code lengths (rfc1951 3.2.7).
inflater::step inflater::code_lengths() NOEXCEPT
{
    for (; index_ < length_count_; ++index_)
    {
        if (bit_count_ < code_length_bits)
            return step::more;

        const auto length = bit_and<uint64_t>(buffer_,
            sub1(power2(code_length_bits)));

        lengths_[length_order[index_]] = narrow_cast<uint8_t>(length);
        drop(code_length_bits);
    }

    if (!build(code_lengths_, lengths_.data(), length_codes) ||
        !complete(code_lengths_, false))
        return step::invalid;

    index_ = zero;
    lengths_.fill(0_u8);
    state_ = state::lengths;
    return step::next;
}

// Literal/length and distance code lengths (rfc1951 3.2.7).
inflater::step inflater::lengths() NOEXCEPT
{
    const auto total = literal_count_ + distance_count_;
    while (index_ < total)
    {
        size_t symbol{}, length{};
        const auto found = decode(symbol, length, code_lengths_, buffer_,
            bit_count_);

        if (found != decoded::found)
            return found == decoded::more ? step::more : step::invalid;

        if (symbol < repeat_previous)
        {
            drop(length);
            lengths_[index_++] = narrow_cast<uint8_t>(symbol);
            continue;
        }

        const auto kind = symbol - repeat_previous;
        const size_t extra = repeat_extra[kind];
        if (bit_count_ < length + extra)
            return step::more;

        if (symbol == repeat_previous && is_zero(index_))
            return step::invalid;

        const auto value = (symbol == repeat_previous) ?
            lengths_[sub1(index_)] : 0_u8;

        auto repeat = repeat_base[kind] + possible_narrow_cast<size_t>(
            bit_and<uint64_t>(shift_right(buffer_, length),
                sub1(power2(extra))));

        if (repeat > total - index_)
            return step::invalid;

        drop(length + extra);
        while (!is_zero(repeat--))
            lengths_[index_++] = value;
    }

    if (is_zero(lengths_[end_of_block]))
        return step::invalid;

    const auto distance_lengths = std::next(lengths_.data(), literal_count_);
    if (!build(literals_, lengths_.data(), literal_count_) ||
        !build(distances_, distance_lengths, distance_count_) ||
        !complete(literals_, true) || !complete(distances_, true))
        return step::invalid;

    state_ = state::codes;
    return step::next;
}

// The match of size at distance, copied in whole parts that do not overlap,
// each the pattern so far (a distance less than the size repeats).
void inflater::copy(size_t size, size_t distance) NOEXCEPT
{
    const auto end = window_.size();
    const auto start = end - distance;
    window_.resize(end + size);

    const auto from = std::next(window_.begin(), start);
    auto to = std::next(window_.begin(), end);
    for (auto remaining = size; !is_zero(remaining);)
    {
        const auto part = std::min(remaining,
            sign_cast<size_t>(std::distance(from, to)));

        to = std::copy_n(from, part, to);
        remaining -= part;
    }
}

// Literals and length/distance pairs, each decoded whole (rfc1951 3.2.5).
// Each is decoded from a refilled buffer while eight bytes of input remain.
inflater::step inflater::codes() NOEXCEPT
{
    while (!full())
    {
        if (size_ - position_ >= sizeof(uint64_t))
            refill();

        size_t symbol{}, length{};
        auto found = decode(symbol, length, literals_, buffer_, bit_count_);
        if (found != decoded::found)
            return found == decoded::more ? step::more : step::invalid;

        if (symbol < end_of_block)
        {
            drop(length);
            window_.push_back(narrow_cast<uint8_t>(symbol));
            ++produced_;
            continue;
        }

        if (symbol == end_of_block)
        {
            drop(length);
            state_ = last_ ? state::done : state::header;
            return step::next;
        }

        const auto index = symbol - first_length;
        if (index >= length_base.size())
            return step::invalid;

        auto used = length;
        const size_t length_extra_bits = length_extra[index];
        if (bit_count_ < used + length_extra_bits)
            return step::more;

        const auto size = length_base[index] + possible_narrow_cast<size_t>(
            bit_and<uint64_t>(shift_right(buffer_, used),
                sub1(power2(length_extra_bits))));

        used += length_extra_bits;
        size_t code{};
        found = decode(code, length, distances_, shift_right(buffer_, used),
            bit_count_ - used);

        if (found != decoded::found)
            return found == decoded::more ? step::more : step::invalid;

        if (code >= distance_codes)
            return step::invalid;

        used += length;
        const size_t distance_extra_bits = distance_extra[code];
        if (bit_count_ < used + distance_extra_bits)
            return step::more;

        const auto distance = distance_base[code] + possible_narrow_cast<size_t>(
            bit_and<uint64_t>(shift_right(buffer_, used),
                sub1(power2(distance_extra_bits))));

        if (distance > window_.size())
            return step::invalid;

        drop(used + distance_extra_bits);
        copy(size, distance);
        produced_ += size;
    }

    return step::next;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

// One-shot.
// ----------------------------------------------------------------------------

// Decoding pauses past the limit, which then fails.
static size_t pause_of(size_t limit) NOEXCEPT
{
    return is_zero(limit) || limit == max_size_t ? zero : add1(limit);
}

bool inflate(data_chunk& out, const data_slice& in, size_t limit) NOEXCEPT
{
    size_t consumed{};
    return inflate(out, consumed, in, limit) && consumed == in.size();
}

bool inflate(data_chunk& out, size_t& consumed, const data_slice& in,
    size_t limit) NOEXCEPT
{
    out.clear();
    inflater decoder{};
    if (!decoder.write(out, consumed, in, pause_of(limit)) || !decoder.done())
        return false;

    return is_zero(limit) || out.size() <= limit;
}

} // namespace system
} // namespace libbitcoin

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
#include <bitcoin/system/wallet/addresses/qr_encoder.hpp>

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

constexpr size_t mode_bits = 4;
constexpr std::string_view alphabet{ "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:" };

// Error correction codewords per block, by level and version.
constexpr std::array<std::array<uint8_t, 40>, 4> block_error
{
    {
        {
            7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28,
            30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30,
            30, 30, 30, 30, 30, 30
        },
        {
            10, 16, 26, 18, 24, 16, 18, 22, 22, 26, 30, 22, 22, 24, 24, 28, 28,
            26, 26, 26, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
            28, 28, 28, 28, 28, 28
        },
        {
            13, 22, 18, 26, 18, 24, 18, 22, 20, 24, 28, 26, 24, 20, 30, 24, 28,
            28, 26, 30, 28, 30, 30, 30, 30, 28, 30, 30, 30, 30, 30, 30, 30, 30,
            30, 30, 30, 30, 30, 30
        },
        {
            17, 28, 22, 16, 22, 28, 26, 26, 24, 28, 24, 28, 22, 24, 24, 30, 28,
            28, 26, 28, 30, 24, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
            30, 30, 30, 30, 30, 30
        }
    }
};

// Error correction blocks, by level and version.
constexpr std::array<std::array<uint8_t, 40>, 4> block_count
{
    {
        {
            1, 1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 4, 4, 6, 6, 6, 6, 7, 8, 8, 9, 9,
            10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25
        },
        {
            1, 1, 1, 2, 2, 4, 4, 4, 5, 5, 5, 8, 9, 9, 10, 10, 11, 13, 14, 16,
            17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43,
            45, 47, 49
        },
        {
            1, 1, 2, 2, 4, 4, 6, 6, 8, 8, 8, 10, 12, 16, 12, 17, 16, 18, 21,
            20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56,
            59, 62, 65, 68
        },
        {
            1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25,
            25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66,
            70, 74, 77, 81
        }
    }
};

// Character count indicator bits, by mode and version range.
constexpr std::array<std::array<uint8_t, 3>, 4> count_bits
{
    {
        { 10, 12, 14 },
        { 9, 11, 13 },
        { 8, 16, 16 },
        { 8, 10, 12 }
    }
};

static constexpr bool is_digit(char character) NOEXCEPT
{
    return character >= '0' && character <= '9';
}

static constexpr bool is_alpha(char character) NOEXCEPT
{
    return alphabet.find(character) != std::string_view::npos;
}

static constexpr uint8_t to_byte(char character) NOEXCEPT
{
    return possible_sign_cast<uint8_t>(character);
}

static uint16_t to_word(const std::string& value, size_t index) NOEXCEPT
{
    return bit_or<uint16_t>(shift_left<uint16_t>(to_byte(value[index]),
        byte_bits), to_byte(value[add1(index)]));
}

static bool is_digit_at(const std::string& value, size_t index) NOEXCEPT
{
    return index < value.size() && is_digit(value[index]);
}

static bool is_alpha_at(const std::string& value, size_t index) NOEXCEPT
{
    return index < value.size() && is_alpha(value[index]);
}

// Galois field GF(2^8) multiplication, modulo x^8 + x^4 + x^3 + x^2 + 1.
static constexpr uint8_t multiply(uint8_t left, uint8_t right) NOEXCEPT
{
    constexpr uint16_t modulus{ 0x011d };
    constexpr uint16_t none{};
    uint16_t product{};
    for (auto bit = byte_bits; !is_zero(bit); --bit)
    {
        const auto overflow = get_right(product, sub1(byte_bits));
        product = bit_xor(shift_left(product), overflow ? modulus : none);
        product = bit_xor<uint16_t>(product,
            get_right(right, sub1(bit)) ? left : none);
    }

    return narrow_cast<uint8_t>(product);
}

// public
// ----------------------------------------------------------------------------

size_t qr_encoder::codewords(uint8_t version) NOEXCEPT
{
    if (version < minimum_version || version > maximum_version)
        return zero;

    const size_t value{ version };
    auto modules = (16u * value + 128u) * value + 64u;

    if (value >= 2u)
    {
        const auto alignments = value / 7u + 2u;
        modules -= (25u * alignments - 10u) * alignments - 55u;
    }

    if (value >= 7u)
        modules -= 36u;

    return modules / byte_bits;
}

size_t qr_encoder::data_codewords(uint8_t version,
    recovery_level level) NOEXCEPT
{
    const auto total = codewords(version);
    return is_zero(total) ? zero : total - error_codewords(version, level);
}

data_chunk qr_encoder::encode(uint8_t& version, const std::string& value,
    recovery_level level, encode_mode mode, bool case_sensitive) NOEXCEPT
{
    if (value.empty() || version > maximum_version)
        return {};

    const auto kanji = (mode == encode_mode::kanji);
    const auto text = case_sensitive ? value : to_upper(value, kanji);
    const auto parts = split(text, version, kanji);
    const auto actual = select_version(parts, version, level);
    const auto data = to_data(text, parts, actual, level);

    if (data.empty())
        return {};

    version = actual;
    return interleave(data, actual, level);
}

// protected
// ----------------------------------------------------------------------------

size_t qr_encoder::error_codewords(uint8_t version,
    recovery_level level) NOEXCEPT
{
    return block_error[static_cast<size_t>(level)][sub1(version)] *
        blocks(version, level);
}

size_t qr_encoder::blocks(uint8_t version, recovery_level level) NOEXCEPT
{
    return block_count[static_cast<size_t>(level)][sub1(version)];
}

size_t qr_encoder::length_bits(segment_mode mode, uint8_t version) NOEXCEPT
{
    const auto range = version <= 9u ? zero : (version <= 26u ? one : two);
    return count_bits[static_cast<size_t>(mode)][range];
}

size_t qr_encoder::payload_bits(segment_mode mode, size_t size) NOEXCEPT
{
    constexpr std::array<size_t, 3> numeric_remainder{ 0, 4, 7 };

    switch (mode)
    {
        case segment_mode::numeric:
            return (size / 3u) * 10u + numeric_remainder[size % 3u];
        case segment_mode::alpha_numeric:
            return to_half(size) * 11u + (is_odd(size) ? 6u : zero);
        case segment_mode::eight_bit:
            return to_bits(size);
        default:
        case segment_mode::kanji:
            return to_half(size) * 13u;
    }
}

uint8_t qr_encoder::minimum_fit(size_t bits, recovery_level level) NOEXCEPT
{
    const auto bytes = ceilinged_divide(bits, byte_bits);

    for (auto version = minimum_version; version < maximum_version; ++version)
        if (data_codewords(version, level) >= bytes)
            return version;

    return maximum_version;
}

qr_encoder::segment_mode qr_encoder::classify(const std::string& value,
    size_t index, bool kanji) NOEXCEPT
{
    if (index >= value.size())
        return segment_mode::end;

    if (is_digit(value[index]))
        return segment_mode::numeric;

    if (is_alpha(value[index]))
        return segment_mode::alpha_numeric;

    if (kanji && add1(index) < value.size())
    {
        const auto word = to_word(value, index);
        if ((word >= 0x8140 && word <= 0x9ffc) ||
            (word >= 0xe040 && word <= 0xebbf))
            return segment_mode::kanji;
    }

    return segment_mode::eight_bit;
}

std::string qr_encoder::to_upper(const std::string& value, bool kanji) NOEXCEPT
{
    auto text = value;
    for (size_t index{}; index < text.size(); ++index)
    {
        if (classify(text, index, kanji) == segment_mode::kanji)
        {
            ++index;
            continue;
        }

        auto& character = text[index];
        if (is_ascii_lower(character))
            character = possible_sign_narrow_cast<char>(
                character + ('A' - 'a'));
    }

    return text;
}

qr_encoder::segments qr_encoder::split(const std::string& value,
    uint8_t version, bool kanji) NOEXCEPT
{
    segments parts{};
    for (size_t start{}; start < value.size();)
    {
        switch (classify(value, start, kanji))
        {
            case segment_mode::numeric:
                start += eat_numeric(parts, value, start, version, kanji);
                break;
            case segment_mode::alpha_numeric:
                start += eat_alpha_numeric(parts, value, start, version, kanji);
                break;
            case segment_mode::kanji:
                start += eat_kanji(parts, value, start, kanji);
                break;
            default:
                start += eat_eight_bit(parts, value, start, version, kanji);
                break;
        }
    }

    return parts;
}

size_t qr_encoder::estimate_bits(const segments& parts,
    uint8_t version) NOEXCEPT
{
    const auto actual = is_zero(version) ? minimum_version : version;

    size_t bits{};
    for (const auto& part: parts)
    {
        const auto length = length_bits(part.mode, actual);
        const auto count = part.mode == segment_mode::kanji ?
            to_half(part.size) : part.size;
        const auto chunks = ceilinged_divide(count, power2<size_t>(length));
        bits += payload_bits(part.mode, part.size) + chunks * (mode_bits + length);
    }

    return bits;
}

size_t qr_encoder::stream_bits(const segments& parts, uint8_t version) NOEXCEPT
{
    size_t bits{};
    for (const auto& part: parts)
    {
        const auto length = length_bits(part.mode, version);
        const auto units = part.mode == segment_mode::kanji ? two : one;
        const auto limit = sub1(power2<size_t>(length)) * units;
        const auto whole = part.size / limit;
        const auto remain = part.size % limit;

        bits += whole * (mode_bits + length + payload_bits(part.mode, limit));
        if (!is_zero(remain))
            bits += mode_bits + length + payload_bits(part.mode, remain);
    }

    return bits;
}

uint8_t qr_encoder::select_version(const segments& parts, uint8_t version,
    recovery_level level) NOEXCEPT
{
    uint8_t prior{};
    uint8_t estimate{};
    do
    {
        prior = estimate;
        estimate = minimum_fit(estimate_bits(parts, prior), level);
        if (is_zero(prior) && estimate > minimum_version)
            --estimate;
    } while (estimate > prior);

    auto actual = std::max(version, estimate);
    for (auto fit = minimum_fit(stream_bits(parts, actual), level);
        fit > actual; fit = minimum_fit(stream_bits(parts, actual), level))
        actual = fit;

    return actual;
}

data_chunk qr_encoder::to_data(const std::string& value, const segments& parts,
    uint8_t version, recovery_level level) NOEXCEPT
{
    constexpr size_t terminator_bits = 4;
    constexpr std::array<uint8_t, 2> pads{ 0xec, 0x11 };

    const auto capacity = data_codewords(version, level);
    const auto maximum = to_bits(capacity);
    const auto bits = stream_bits(parts, version);
    if (bits > maximum)
        return {};

    data_chunk out{};
    out.reserve(capacity);
    write::bits::data sink(out);

    for (const auto& part: parts)
    {
        const auto end = part.start + part.size;
        const auto kanji = part.mode == segment_mode::kanji;
        const auto count = kanji ? to_half(part.size) : part.size;

        sink.write_bits(power2(static_cast<size_t>(part.mode)), mode_bits);
        sink.write_bits(count, length_bits(part.mode, version));

        switch (part.mode)
        {
            case segment_mode::numeric:
            {
                for (auto index = part.start; index < end;)
                {
                    const auto digits = std::min(end - index, size_t{ 3 });
                    uint64_t number{};
                    for (size_t digit{}; digit < digits; ++digit, ++index)
                        number = number * 10u + alphabet.find(value[index]);

                    sink.write_bits(number, payload_bits(part.mode, digits));
                }

                break;
            }
            case segment_mode::alpha_numeric:
            {
                for (auto index = part.start; index < end;)
                {
                    const auto chars = std::min(end - index, two);
                    uint64_t number{};
                    for (size_t character{}; character < chars; ++character, ++index)
                        number = number * alphabet.size() + alphabet.find(value[index]);

                    sink.write_bits(number, payload_bits(part.mode, chars));
                }

                break;
            }
            case segment_mode::eight_bit:
            {
                for (auto index = part.start; index < end; ++index)
                    sink.write_byte(to_byte(value[index]));

                break;
            }
            default:
            case segment_mode::kanji:
            {
                for (auto index = part.start; index < end; index += two)
                {
                    size_t word{ to_word(value, index) };
                    word -= (word <= 0x9ffcu ? 0x8140u : 0xc140u);
                    const auto number = (word >> byte_bits) * 0xc0u +
                        (word & 0xffu);

                    sink.write_bits(number, payload_bits(part.mode, two));
                }

                break;
            }
        }
    }

    if (maximum - bits <= terminator_bits)
    {
        sink.write_bits(zero, maximum - bits);
    }
    else
    {
        const auto bytes = ceilinged_divide(bits + terminator_bits, byte_bits);
        sink.write_bits(zero, to_bits(bytes) - bits);
        for (auto pad = bytes; pad < capacity; ++pad)
            sink.write_byte(pads[is_odd(pad - bytes) ? one : zero]);
    }

    sink.flush();
    return out;
}

data_chunk qr_encoder::to_error(const data_slice& data, size_t size) NOEXCEPT
{
    // Reed-Solomon generator polynomial, excluding the leading coefficient.
    data_chunk generator(size);
    generator.back() = 0x01;

    uint8_t root{ 0x01 };
    for (size_t term{}; term < size; ++term)
    {
        for (size_t index{}; index < size; ++index)
        {
            generator[index] = multiply(generator[index], root);
            if (add1(index) < size)
                generator[index] = bit_xor(generator[index],
                    generator[add1(index)]);
        }

        root = multiply(root, 0x02);
    }

    data_chunk remainder(size);
    for (const auto byte: data)
    {
        const auto factor = bit_xor(byte, remainder.front());
        std::rotate(remainder.begin(), std::next(remainder.begin()),
            remainder.end());
        remainder.back() = {};

        for (size_t index{}; index < size; ++index)
            remainder[index] = bit_xor(remainder[index],
                multiply(generator[index], factor));
    }

    return remainder;
}

data_chunk qr_encoder::interleave(const data_chunk& data, uint8_t version,
    recovery_level level) NOEXCEPT
{
    const auto count = blocks(version, level);
    const auto error = error_codewords(version, level) / count;
    const auto total = codewords(version);
    const auto shorts = count - (total % count);
    const auto short_data = total / count - error;

    std_vector<data_chunk> data_blocks{};
    std_vector<data_chunk> error_blocks{};
    data_blocks.reserve(count);
    error_blocks.reserve(count);

    auto it = data.cbegin();
    for (size_t block{}; block < count; ++block)
    {
        const auto size = short_data + (block < shorts ? zero : one);
        const auto end = std::next(it, size);
        data_blocks.emplace_back(it, end);
        error_blocks.push_back(to_error(data_blocks.back(), error));
        it = end;
    }

    data_chunk out{};
    out.reserve(total);

    for (size_t index{}; index <= short_data; ++index)
        for (const auto& block: data_blocks)
            if (index < block.size())
                out.push_back(block[index]);

    for (size_t index{}; index < error; ++index)
        for (const auto& block: error_blocks)
            out.push_back(block[index]);

    return out;
}

// private
// ----------------------------------------------------------------------------

size_t qr_encoder::eat_numeric(segments& out, const std::string& value,
    size_t start, uint8_t version, bool kanji) NOEXCEPT
{
    constexpr auto numeric = segment_mode::numeric;
    const auto header = mode_bits + length_bits(numeric, version);

    auto end = start;
    while (is_digit_at(value, end))
        ++end;

    const auto run = end - start;
    const auto bits = payload_bits(numeric, run) + header;

    switch (classify(value, end, kanji))
    {
        case segment_mode::eight_bit:
        {
            constexpr auto mode = segment_mode::eight_bit;
            if (bits + payload_bits(mode, one) > payload_bits(mode, add1(run)))
                return eat_eight_bit(out, value, start, version, kanji);

            break;
        }
        case segment_mode::alpha_numeric:
        {
            constexpr auto mode = segment_mode::alpha_numeric;
            if (bits + payload_bits(mode, one) > payload_bits(mode, add1(run)))
                return eat_alpha_numeric(out, value, start, version, kanji);

            break;
        }
        default:
            break;
    }

    out.push_back({ numeric, start, run });
    return run;
}

size_t qr_encoder::eat_alpha_numeric(segments& out, const std::string& value,
    size_t start, uint8_t version, bool kanji) NOEXCEPT
{
    constexpr auto alpha = segment_mode::alpha_numeric;
    constexpr auto numeric = segment_mode::numeric;
    constexpr auto eight = segment_mode::eight_bit;
    const auto alpha_header = mode_bits + length_bits(alpha, version);
    const auto numeric_header = mode_bits + length_bits(numeric, version);

    auto end = start;
    while (is_alpha_at(value, end))
    {
        if (!is_digit(value[end]))
        {
            ++end;
            continue;
        }

        auto digits = end;
        while (is_digit_at(value, digits))
            ++digits;

        const auto resume = is_alpha_at(value, digits) ? numeric_header : zero;
        const auto divided = payload_bits(alpha, end - start) +
            payload_bits(numeric, digits - end) + numeric_header + resume;

        if (divided < payload_bits(alpha, digits - start))
            break;

        end = digits;
    }

    const auto run = end - start;
    if (end < value.size() && !is_alpha(value[end]))
    {
        const auto bits = payload_bits(alpha, run) + alpha_header;
        if (bits + payload_bits(eight, one) > payload_bits(eight, add1(run)))
            return eat_eight_bit(out, value, start, version, kanji);
    }

    out.push_back({ alpha, start, run });
    return run;
}

size_t qr_encoder::eat_kanji(segments& out, const std::string& value,
    size_t start, bool kanji) NOEXCEPT
{
    auto end = start;
    while (classify(value, end, kanji) == segment_mode::kanji)
        end += two;

    const auto run = end - start;
    out.push_back({ segment_mode::kanji, start, run });
    return run;
}

size_t qr_encoder::eat_eight_bit(segments& out, const std::string& value,
    size_t start, uint8_t version, bool kanji) NOEXCEPT
{
    constexpr auto eight = segment_mode::eight_bit;
    const auto eight_header = mode_bits + length_bits(eight, version);

    auto end = add1(start);
    while (end < value.size())
    {
        const auto mode = classify(value, end, kanji);
        if (mode == segment_mode::kanji)
            break;

        if (mode == eight)
        {
            ++end;
            continue;
        }

        const auto numeric = (mode == segment_mode::numeric);
        auto stop = end;
        while (numeric ? is_digit_at(value, stop) : is_alpha_at(value, stop))
            ++stop;

        const auto header = mode_bits + length_bits(mode, version);
        const auto resume = classify(value, stop, kanji) == eight ?
            eight_header : zero;
        const auto divided = payload_bits(eight, end - start) +
            payload_bits(mode, stop - end) + header + resume;

        if (divided < payload_bits(eight, stop - start))
            break;

        end = stop;
    }

    const auto run = end - start;
    out.push_back({ eight, start, run });
    return run;
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace wallet
} // namespace system
} // namespace libbitcoin

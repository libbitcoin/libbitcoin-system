/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_RADIX_BASE_16_IPP
#define LIBBITCOIN_SYSTEM_RADIX_BASE_16_IPP

#include <algorithm>
#include <ranges>
#include <string>
#include <string_view>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

// base16 (hexidecimal):
// Base 16 is an ascii data encoding with a domain of 16 symbols (characters).
// 16 is 2^4 so base16 is a 4<=>4 bit mapping.
// Invalid padding in byte packing is not detectable as it is byte-aligned.
// So this implementation is limited to an 8<=>8 bit mapping.
// There is no need for bit streaming or padding, and both sides are
// authoritative. Therefore base16 is our canonical byte encoding.

namespace libbitcoin {
namespace system {

constexpr bool is_between(uint8_t value, uint8_t low, uint8_t high) NOEXCEPT
{
    return low <= value && value <= high;
}

constexpr char to_base16_character(char digit) NOEXCEPT
{
    return (is_between(digit, 0, 9) ? '0' : 'a' - '\xa') + digit;
}

constexpr uint8_t from_base16_characters(char high, char low) NOEXCEPT
{
    const auto from_base16_digit = [](char character) NOEXCEPT
    {
        if (is_between(character, 'A', 'F'))
            return narrow_sign_cast<uint8_t>(character - 'A' + '\xA');

        if (is_between(character, 'a', 'f'))
            return narrow_sign_cast<uint8_t>(character - 'a' + '\xa');

        return narrow_sign_cast<uint8_t>(character - '0' + '\x0');
    };

    return (from_base16_digit(high) << to_half(byte_bits)) |
        from_base16_digit(low);
}

// published
// ============================================================================

// The C standard library function 'isxdigit' is more trouble than it is worth.
// "Some implementations (e.g. Microsoft in 1252 codepage) may classify
// additional single-byte characters as digits...To use these functions safely
// with plain chars (or signed chars), the argument should first be converted
// to unsigned char... Similarly, they should not be directly used with standard
// algorithms when the iterator's value type is char." [e.g. std::string]
// en.cppreference.com/w/cpp/string/byte/isxdigit
template <typename Byte, if_one_byte<Byte>>
constexpr bool is_base16(Byte character) NOEXCEPT
{
    return
        (is_between(possible_sign_cast<uint8_t>(character), '0', '9')) ||
        (is_between(possible_sign_cast<uint8_t>(character), 'a', 'f')) ||
        (is_between(possible_sign_cast<uint8_t>(character), 'A', 'F'));
}

// Unspecified (but safe/defined) behavior if characters are not base16. 
constexpr uint8_t encode_octet(
    const char(&string)[add1(octet_width)]) NOEXCEPT
{
    return from_base16_characters(string[0], string[1]);
}

// Encoding of data_slice to hex string.
// ----------------------------------------------------------------------------

SCONSTEXPR std::string encode_base16(const data_slice& data) NOEXCEPT
{
    std::string out;
    out.resize(data.size() * octet_width);
    auto digit = out.begin();

    for (const auto byte: data)
    {
        *digit++ = to_base16_character(shift_right(byte, to_half(byte_bits)));
        *digit++ = to_base16_character(bit_and(byte, 0x0f_u8));
    }

    return out;
}

// std::string is SCONSTEXPR
SRCONSTEXPR std::string encode_hash(const data_slice& hash) NOEXCEPT
{
    std::string out;
    out.resize(hash.size() * octet_width);
    auto digit = out.begin();

    // views_reverse is RCONSTEXPR
    for (const auto byte: std::views::reverse(hash))
    {
        *digit++ = to_base16_character(shift_right(byte, to_half(byte_bits)));
        *digit++ = to_base16_character(bit_and(byte, 0x0f_u8));
    }

    return out;
}

// Decoding of hex string to data_array or data_chunk.
// ----------------------------------------------------------------------------

VCONSTEXPR bool decode_base16(data_chunk& out,
    const std::string& in) NOEXCEPT
{
    if (!is_multiple(in.size(), octet_width))
        return false;

    if (!std::all_of(in.begin(), in.end(), is_base16<char>))
        return false;

    out.resize(in.size() / octet_width);
    auto data = out.begin();

    for (auto digit = in.begin(); digit != in.end();)
    {
        const auto hi = *digit++;
        const auto lo = *digit++;
        *data++ = from_base16_characters(hi, lo);
    }

    return true;
}

template <size_t Size>
constexpr bool decode_base16(data_array<Size>& out,
    const std::string_view& in) NOEXCEPT
{
    if (!is_product(in.size(), octet_width, Size))
        return false;

    if (!std::all_of(in.begin(), in.end(), is_base16<char>))
        return false;

    auto data = out.begin();

    for (auto digit = in.begin(); digit != in.end();)
    {
        const auto hi = *digit++;
        const auto lo = *digit++;
        *data++ = from_base16_characters(hi, lo);
    }

    return true;
}

template <size_t Size>
constexpr bool decode_hash(data_array<Size>& out,
    const std::string_view& in) NOEXCEPT
{
    if (in.size() != Size * octet_width)
        return false;

    if (!std::all_of(in.begin(), in.end(), is_base16<char>))
        return false;

    auto data = out.begin();

    for (auto digit = in.rbegin(); digit != in.rend();)
    {
        const auto lo = *digit++;
        const auto hi = *digit++;
        *data++ = from_base16_characters(hi, lo);
    }

    return true;
}

// Literal decodings of hex string, errors reflected as zero-filled data.
// ----------------------------------------------------------------------------

template <size_t Size, if_odd<Size>>
SVCONSTEXPR std::string base16_string(const char(&string)[Size]) NOEXCEPT
{
    return to_string(base16_chunk(string));
}

template <size_t Size, if_odd<Size>>
data_chunk base16_chunk(const char(&string)[Size]) NOEXCEPT
{
    data_chunk out;
    decode_base16(out, string);
    return out;
}

template <size_t Size, if_odd<Size>>
constexpr data_array<to_half(sub1(Size))>
base16_array(const char(&string)[Size]) NOEXCEPT
{
    data_array<to_half(sub1(Size))> out{};
    if (!decode_base16(out, string))
        out.fill(0);

    return out;
}

template <size_t Size, if_odd<Size>>
constexpr data_array<to_half(sub1(Size))>
base16_hash(const char(&string)[Size]) NOEXCEPT
{
    data_array<to_half(sub1(Size))> out{};
    if (!decode_hash(out, string))
        out.fill(0);

    return out;
}

} // namespace system
} // namespace libbitcoin

#endif

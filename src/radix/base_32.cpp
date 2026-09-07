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
#include <bitcoin/system/radix/base_32.hpp>

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>

// base32
// Base 32 is an ascii data encoding with a domain of 32 symbols (characters).
// 32 is 2^5 so base32 is a 5<=>8 bit mapping.
// 8 characters (32^8) are encoded into one 5 byte (8^5) chunk before mapping.
// So after 5 byte encoding base32 is a 40<=>40 bit mapping.
// The 5 bit encoding is authoritative as byte encoding is padded.
// Encoding is canonical RFC 4648 (upper case, padded to 8 characters).
// Decoding accepts either case and unpadded input (as in .onion and .i2p).

namespace libbitcoin {
namespace system {

constexpr char pad = '=';
constexpr size_t bits = 5;
constexpr size_t bytes = 5;
constexpr size_t characters = 8;
const static char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

// Characters required to encode the given number of bytes (unpadded).
constexpr size_t unpadded(size_t size) NOEXCEPT
{
    return ceilinged_divide(size * byte_bits, bits);
}

// Unpadded lengths are valid only if they can be produced by encoding.
constexpr bool valid_unpadded(size_t length) NOEXCEPT
{
    const auto remainder = floored_modulo(length, characters);
    return remainder == 0
        || remainder == 2
        || remainder == 4
        || remainder == 5
        || remainder == 7;
}

constexpr bool decode_character(uint8_t& out, char character) NOEXCEPT
{
    if (character >= 'A' && character <= 'Z')
        out = possible_narrow_and_sign_cast<uint8_t>(character - 'A');
    else if (character >= 'a' && character <= 'z')
        out = possible_narrow_and_sign_cast<uint8_t>(character - 'a');
    else if (character >= '2' && character <= '7')
        out = possible_narrow_and_sign_cast<uint8_t>(character - '2' + 26);
    else
        return false;

    return true;
}

std::string encode_base32(const data_slice& unencoded) NOEXCEPT
{
    std::string encoded{};
    const auto size = unencoded.size();
    encoded.reserve(ceilinged_divide(size, bytes) * characters);

    size_t count{};
    uint64_t value{};
    for (const auto byte: unencoded)
    {
        value = bit_or<uint64_t>(shift_left(value, byte_bits), byte);
        count += byte_bits;

        while (count >= bits)
        {
            count -= bits;
            const auto at = unmask_right(shift_right(value, count), bits);
            encoded.push_back(table[at]);
        }
    }

    // Zero-fill the trailing partial character.
    if (!is_zero(count))
    {
        const auto at = unmask_right(shift_left(value, bits - count), bits);
        encoded.push_back(table[at]);
    }

    // Pad to a multiple of eight characters.
    const auto remainder = floored_modulo(unpadded(size), characters);
    encoded.append(is_zero(remainder) ? zero : characters - remainder, pad);
    return encoded;
}

bool decode_base32(data_chunk& out, const std::string& in) NOEXCEPT
{
    // Padding is valid only as the tail of a full eight character group.
    const auto padded = in.find(pad);
    const auto length = (padded == std::string::npos) ? in.size() : padded;
    const auto pads = in.size() - length;

    if (padded != std::string::npos)
    {
        const auto remainder = floored_modulo(length, characters);

        if (!is_zero(floored_modulo(in.size(), characters)) ||
            in.find_first_not_of(pad, padded) != std::string::npos ||
            is_zero(remainder) ||
            remainder + pads != characters)
            return false;
    }

    if (!valid_unpadded(length))
        return false;

    data_chunk decoded{};
    decoded.reserve(floored_divide(length * bits, byte_bits));

    size_t count{};
    uint64_t value{};
    for (size_t index{}; index < length; ++index)
    {
        uint8_t symbol{};
        if (!decode_character(symbol, in[index]))
            return false;

        value = bit_or<uint64_t>(shift_left(value, bits), symbol);
        count += bits;

        if (count >= byte_bits)
        {
            count -= byte_bits;
            decoded.push_back(possible_narrow_cast<uint8_t>(
                unmask_right(shift_right(value, count), byte_bits)));
        }
    }

    // Trailing partial byte bits must be zero (canonical encoding).
    if (!is_zero(unmask_right(value, count)))
        return false;

    out = std::move(decoded);
    return true;
}

} // namespace system
} // namespace libbitcoin

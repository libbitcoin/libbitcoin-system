/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/formats/base_32.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <bitcoin/system/utility/data.hpp>

namespace libbitcoin {
namespace system {

static constexpr size_t checksum_size = 6;
static constexpr size_t prefix_min_size = 1;
static constexpr size_t combined_max_size = 90;
static constexpr uint8_t null = 255;
static constexpr uint8_t separator = '1';
static const char encode_table[] = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";
static const uint8_t decode_table[] =
{
    null, null, null, null, null, null, null, null,
    null, null, null, null, null, null, null, null,
    null, null, null, null, null, null, null, null,
    null, null, null, null, null, null, null, null,
    null, null, null, null, null, null, null, null,
    null, null, null, null, null, null, null, null,
    15,   null, 10,   17,   21,   20,   26,   30,
    7,    5,    null, null, null, null, null, null,
    null, 29,   null, 24,   13,   25,   9,    8,
    23,   null, 18,   22,   31,   27,   19,   null,
    1,    0,    3,    16,   11,   28,   12,   14,
    6,    4,    2,    null, null, null, null, null,
    null, 29,   null, 24,   13,   25,   9,    8,
    23,   null, 18,   22,   31,   27,   19,   null,
    1,    0,    3,    16,   11,   28,   12,   14,
    6,    4,    2,    null, null, null, null, null
};

inline char ascii_to_lowercase(char character)
{
    return character + ('a' - 'A');
}

// Expand the prefix for checksum computation.
data_chunk expand(const std::string& prefix)
{
    data_chunk result(2 * prefix.size() + 1);
    auto iterator = result.begin();

    for (const auto character: prefix)
    {
        *iterator = static_cast<uint8_t>(character >> 5);
        ++iterator;
    }

    // Current position is initialized to 0x00 so skip it.
    ++iterator;

    for (const auto character: prefix)
    {
        *iterator = static_cast<uint8_t>(character & 31);
        ++iterator;
    }

    return result;
}

// Do the checksum math.
uint32_t polymod(const data_chunk& values)
{
    static const uint32_t magic_numbers[] =
    {
        0x3b6a57b2, 0x26508e6d, 0x1ea119fa, 0x3d4233dd, 0x2a1462b3
    };

    uint32_t result = 1;
    for (const auto value: values)
    {
        const auto shift = (result >> 25);
        result = (result & 0x1ffffff) << 5 ^ value;

        for (auto index = 0; index < 5; ++index)
            result ^= (((shift >> index) & 1) != 0 ? magic_numbers[index] : 0);
    }

    return result;
}

// Compute the checksum.
data_chunk checksum(const base32& value)
{
    static const data_chunk empty_checksum
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    const auto expanded = build_chunk(
    {
        expand(value.prefix),
        value.payload,
        empty_checksum
    });

    data_chunk checksum(checksum_size);
    const auto modified = polymod(expanded) ^ 1;

    for (size_t index = 0; index < checksum.size(); ++index)
        checksum[index] = (modified >> (5 * (5 - index))) & 31;

    return checksum;
}

// Normalize and validate input characters.
static bool normalize(data_chunk& out, const std::string& in)
{
    out.clear();
    out.reserve(in.length());
    auto uppercase = false;
    auto lowercase = false;

    for (auto character: in)
    {
        if (character >= 'A' && character <= 'Z')
        {
            uppercase = true;
            character = ascii_to_lowercase(character);
        }
        else if (character >= 'a' && character <= 'z')
        {
            lowercase = true;
        }
        else if (character < '!' || character > '~')
        {
            return false;
        }

        out.push_back(static_cast<uint8_t>(character));
    }

    // Must not accept mixed case strings.
    return !(uppercase && lowercase);
}

// Split the prefix from the payload and validate sizes.
static bool split(base32& out, const data_chunk& in)
{
    static const auto separator_size = sizeof(separator);
    static const auto payload_min_size = checksum_size;
    static const auto prefix_max_size = combined_max_size - separator_size -
        payload_min_size;

    if (in.size() > combined_max_size)
        return false;

    // Find the last instance of the separator byte.
    const auto reverse = std::find(in.rbegin(), in.rend(), separator);

    if (reverse == in.rend())
        return false;

    // Convert separator iterator from reverse to forward (min distance is 1).
    const auto offset = std::distance(reverse, in.rend()) - 1;

    // Clang 3.4 cannot handle iterator variable here, so this is a bit ugly.
    out.prefix = { in.begin(), std::next(in.begin(), offset) };
    out.payload = { std::next(in.begin(), offset + 1), in.end() };

    return
        out.prefix.size() >= prefix_min_size &&
        out.prefix.size() <= prefix_max_size &&
        out.payload.size() >= payload_min_size;
}

// Verify the checksummed payload.
bool verify(const base32& value)
{
    const auto expanded = build_chunk(
    {
        expand(value.prefix),
        value.payload
    });

    return polymod(expanded) == 1;
}

// public
//-----------------------------------------------------------------------------

// TODO: add guard against invalid input.
// There is no guard in the BIP for invalid sizes or prefix characters here,
// and as a result it is possible to encode a value that cannot be decoded.
// TODO: guard against uppercase prefix.
// An uppercase prefix is valid but the BIP reference code does not normalize
// it. The result is invalid encoded value due to mixed case. There is no tool
// to produce uppercase encodings, though the values may be simply mapped.
std::string encode_base32(const base32& unencoded)
{
    std::string encoded;
    encoded.reserve(unencoded.prefix.size() + sizeof(separator) +
        unencoded.payload.size() + checksum_size);

    // Copy the prefix and add the separator.
    encoded = unencoded.prefix;
    encoded += separator;

    // Encode and add the payload.
    for (const auto value: unencoded.payload)
        encoded += encode_table[value];

    // Compute, encode and add the checksum.
    for (const auto value: checksum(unencoded))
        encoded += encode_table[value];

    return encoded;
}

bool decode_base32(base32& out, const std::string& in)
{
    static const auto check = checksum_size;
    data_chunk normal;

    // Normalize and validate input characters.
    if (!normalize(normal, in))
        return false;

    // Set prefix and payload with checksum.
    if (!split(out, normal))
        return false;

    // Decode payload/checksum in place.
    for (auto it = out.payload.begin(); it != out.payload.end(); ++it)
        if (((*it = decode_table[*it])) == null)
            return false;

    // Verify checksummed payload.
    if (!verify(out))
        return false;

    // Truncate checksum from payload.
    out.payload.resize(out.payload.size() - check);
    out.payload.shrink_to_fit();
    return true;
}

} // namespace system
} // namespace libbitcoin

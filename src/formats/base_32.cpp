/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/string.hpp>

namespace libbitcoin {
namespace system {
    
constexpr uint8_t null = 0xff;
constexpr char encode_map[] = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";
constexpr uint8_t decode_map[] =
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

std::string encode_base32(const data_slice& data)
{
    const auto expanded = base32_expand(data);

    ////// This should never fail as we just created it.
    ////// 31 is the last index of the 32 element encode_map.
    ////BITCOIN_ASSERT(std::all_of(expanded.begin(), expanded.end(),
    ////    [](uint8_t byte) { return byte < 32; }));

    size_t index = 0;
    std::string out(data.size(), '\0');

    // The map is 32 elements, all expanded byte indexes are valid.
    for (const auto byte: expanded)
        out[index++] = encode_map[byte];

    return out;
}

data_chunk decode_base32(const std::string& data)
{
    if (is_ascii_mixed_case(data))
        return {};

    size_t index = 0;
    data_chunk expanded(data.size());

    // The map is 256 elements, all char indexes are valid.
    for (const auto character: ascii_to_lower(data))
        if (((expanded[index++] = decode_map[character])) == null)
            return {};

    ////// This should never fail since the expansion was successful.
    ////BITCOIN_ASSERT(base32_compress(expanded).empty() == in.empty());

    return base32_compress(expanded);
}

// compression/expansion
// ----------------------------------------------------------------------------

static bool transform(data_chunk& out, const data_slice& data,
    uint32_t from_bits, uint32_t to_bits, bool pad)
{
    uint32_t bits = 0;
    uint32_t accumulator = 0;
    const uint32_t max_value = (1 << to_bits) - 1;
    const uint32_t max_accumulator = (1 << (from_bits + to_bits - 1)) - 1;

    for (const auto value: data)
    {
        accumulator = ((accumulator << from_bits) | value) & max_accumulator;
        bits += from_bits;

        while (bits >= to_bits)
        {
            bits -= to_bits;
            out.push_back((accumulator >> bits) & max_value);
        }
    }

    if (pad)
    {
        if (bits > 0)
        {
            out.push_back((accumulator << (to_bits - bits)) & max_value);
            return true;
        }
    }
    else
    {
        if (bits >= from_bits || ((accumulator << (to_bits - bits)) & max_value))
        {
            ////// Expansion may never fail as any data is encodable.
            ////BITCOIN_ASSERT(from_bits < to_bits);
            out.clear();
            return false;
        }
    }

    return true;
}

static bool transform(data_chunk& out, const data_slice& data, bool compress)
{
    const auto pad = !compress;
    const auto bits = [](bool compress) { return compress ? 5 : 8; };
    return transform(out, data, bits(compress), bits(!compress), pad);
}

// Failure cannot happen for expansion as any data is encodable.
data_chunk base32_expand(const data_slice& data)
{
    data_chunk out;

    // The boolean return is a consequence of symmetrical conversion.
    /* bool */ transform(out, data, false);

    return out;
}

// Expanded data may be invalid, returns empty.
data_chunk base32_compress(const data_slice& expanded)
{
    data_chunk out;

    // Pass the failure as an empty result (for non-empty input).
    // This allows caller who generates expansion to flow the return.
    if (!transform(out, expanded, true))
        out.clear();

    return out;
}

} // namespace system
} // namespace libbitcoin

/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/math/checksum.hpp>

#include <utility>
#include <bitcoin/system/formats/base_32.hpp>
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/data_slice.hpp>
#include <bitcoin/system/utility/endian.hpp>
#include <bitcoin/system/utility/string.hpp>

namespace libbitcoin {
namespace system {

// bitcoin checksum.
// ----------------------------------------------------------------------------

// checksum encoding is a private data format, do not expose.
typedef uint32_t checksum;

static checksum bitcoin_checksum(const data_slice& data)
{
    const auto hash = bitcoin_hash(data);
    return from_little_endian_unsafe<checksum>(hash.begin());
}

data_chunk append_checksum(const data_loaf& slices)
{
    auto out = build_chunk(slices, checksum_default_size);
    append_checksum(out);
    return out;
}

void append_checksum(data_chunk& data)
{
    const auto check = bitcoin_checksum(data);
    extend_data(data, to_little_endian<checksum>(check));
    BITCOIN_ASSERT(verify_checksum(data));
}

bool verify_checksum(const data_slice& data)
{
    if (data.size() < sizeof(checksum))
        return false;

    const auto position = std::prev(data.end(), sizeof(checksum));
    const auto check = from_little_endian<checksum>(position, data.end());
    return check == bitcoin_checksum({ data.begin(), position });
}

// bech32 checksum
// ----------------------------------------------------------------------------

static const size_t bech32_checksum_size = 6;

static size_t bech32_expanded_prefix_size(const std::string& prefix)
{
    return 2u * prefix.size() + 1u;
}

static void bech32_expand_prefix(base32_chunk& out, const std::string& prefix)
{
    const auto size = prefix.size();
    out[size] = 0x00;

    for (size_t index = 0; index < size; ++index)
    {
        char character = prefix[index];
        out[index] = character >> 5;
        out[size + 1u + index] = character;
    }
}

static void bech32_expand_checksum(base32_chunk& out, uint32_t checksum)
{
    auto index = out.size() - bech32_checksum_size;

    // The top two bits of the 32 bit checksum are discarded.
    out[index++] = (checksum >> 25);
    out[index++] = (checksum >> 20);
    out[index++] = (checksum >> 15);
    out[index++] = (checksum >> 10);
    out[index++] = (checksum >> 5);
    out[index++] = (checksum >> 0);
}

static uint32_t bech32_checksum(const base32_chunk& data)
{
    static const uint32_t generator[] =
    {
        0x3b6a57b2, 0x26508e6d, 0x1ea119fa, 0x3d4233dd, 0x2a1462b3
    };

    uint32_t checksum = 1;

    for (const auto& value: data)
    {
        const uint32_t coeficient = (checksum >> 25);
        checksum = ((checksum & 0x1ffffff) << 5) ^ value.convert_to<uint8_t>();

        if (coeficient & 0x01) checksum ^= generator[0];
        if (coeficient & 0x02) checksum ^= generator[1];
        if (coeficient & 0x04) checksum ^= generator[2];
        if (coeficient & 0x08) checksum ^= generator[3];
        if (coeficient & 0x10) checksum ^= generator[4];
    }

    return checksum;
}

// Version zero implies bech32, otherwise bech32m.
static uint32_t constant(uint8_t version)
{
    return version > 0 ? 0x2bc830a3 : 0x00000001;
}

static void bech32_insert_checksum(base32_chunk& data, uint8_t version)
{
    // Compute the checksum for the given version.
    const auto checksum = bech32_checksum(data) ^ constant(version);

    // Expand the checksum into the checksum buffer.
    bech32_expand_checksum(data, checksum);
}

static bool bech32_verify_checksum(const base32_chunk& data, uint8_t version)
{
    // Compute the checksum and compare to versioned expectation.
    return bech32_checksum(data) == constant(version);
}

base32_chunk bech32_build_checked(uint8_t version, const std::string& prefix,
    const data_chunk& program)
{
    // Get the expanded prefix size.
    const auto prefix_size = bech32_expanded_prefix_size(prefix);

    // Expand the program first to obtain its size.
    const auto expanded_program = base32_expand(program);

    // Create a checksum buffer with null checksum.
    base32_chunk buffer(prefix_size + 1u + expanded_program.size() +
        bech32_checksum_size, 0x00);

    // Expand and copy the prefix.
    bech32_expand_prefix(buffer, prefix);

    // Copy the version (top three bits truncated).
    buffer[prefix_size] = static_cast<uint5_t>(version);

    // Copy the program.
    std::copy(expanded_program.begin(), expanded_program.end(),
        std::next(buffer.begin(), prefix_size + 1u));

    // Compute and copy the checksum.
    bech32_insert_checksum(buffer, version);

    // [version|program|checksum]
    // Return the checked data.
    return{ std::next(buffer.begin(), prefix_size), buffer.end() };
}

bool bech32_verify_checked(uint8_t& out_version, data_chunk& out_program,
    const std::string& prefix, const base32_chunk& checked)
{
    if (prefix.empty() || checked.empty())
        return false;

    // Extract version byte (first).
    out_version = checked.front().convert_to<uint8_t>();

    // Extract and compact the program (between version and checksum).
    out_program = base32_compact(
    {
        std::next(checked.begin()),
        std::prev(checked.end(), bech32_checksum_size)
    });

    // Get the expanded prefix size.
    const auto prefix_size  = bech32_expanded_prefix_size(prefix);

    // Create a checksum buffer with null prefix.
    base32_chunk buffer(prefix_size, 0x00);

    // Expand and copy the prefix.
    bech32_expand_prefix(buffer, prefix);

    // Append [version|program|checksum] to checksum buffer.
    buffer.insert(buffer.end(), checked.begin(), checked.end());

    // Verify the checksum.
    return bech32_verify_checksum(buffer, out_version);
}

} // namespace system
} // namespace libbitcoin

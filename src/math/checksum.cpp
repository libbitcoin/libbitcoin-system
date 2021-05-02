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
#include "external/bech32.h"

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

data_chunk bech32_build_checked(uint8_t version, const data_chunk& data,
    const std::string& prefix)
{
    // Build the expanded data with prefix buffer, version, and null checksum.
    auto expanded = build_chunk(
    {
        data_chunk(bech32_expanded_prefix_size(prefix.length())),
        to_array(version),
        base32_expand(data),
        data_chunk(bech32_checksum_size)
    });

    // Expanded the prefix into the prefix buffer.
    bech32_expand_prefix(expanded.data(), prefix.c_str(), prefix.length());

    // Insert the checksum into the null checksum.
    bech32_insert_checksum(expanded.data(), expanded.size(), version);

    // Compact the data.
    // Cannot fail because the expansion was performed here.
    data_chunk out;
    base32_compact(out, expanded);
    return out;
}

bool bech32_verify_checked(uint8_t& out_version, data_chunk& out_program,
    const data_chunk& data, const std::string& prefix)
{
    if (data.empty() || prefix.empty())
        return false;

    // Get the size of the expanded prefix.
    const auto prefix_size  = bech32_expanded_prefix_size(prefix.length());

    // Prepend the expanded, versioned/checked data with prefix buffer.
    auto expanded = build_chunk(
    {
        data_chunk(prefix_size),
        base32_expand(data)
    });

    // Expanded the prefix into the prefix buffer.
    bech32_expand_prefix(expanded.data(), prefix.c_str(), prefix.length());

    // Read version byte from the expanded data (after the prefix).
    out_version = expanded[prefix_size];

    // Extract program from expanded data (after the version) and recompact.
    // Cannot fail because the expansion was performed here.
    base32_compact(out_program, data_chunk
    (
        std::next(expanded.begin(), prefix_size + sizeof(out_version)),
        std::prev(expanded.end(), bech32_checksum_size)
    ));

    // Verify the checksum.
    return bech32_verify_checksum(expanded.data(), expanded.size(),
        out_version) == 0;
}

} // namespace system
} // namespace libbitcoin

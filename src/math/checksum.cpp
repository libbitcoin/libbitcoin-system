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
#include <bitcoin/system/utility/endian.hpp>
#include <bitcoin/system/utility/string.hpp>

namespace libbitcoin {
namespace system {

// bitcoin checksum.
// ----------------------------------------------------------------------------

// checksum encoding is a private data format, do not expose.
// Endianness is arbitrary here. All that is required is consistency.
typedef uint32_t checksum;

static checksum bitcoin_checksum(const data_slice& data)
{
    const auto hash = bitcoin_hash(data);
    return from_little_endian_unsafe<checksum>(hash.begin());
}

data_chunk append_checksum(const loaf& slices)
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

// bech32 checksum.
// ----------------------------------------------------------------------------

// TODO: bech32::checksum
// TODO: bech32::checksum_size
using bech32_checksum = byte_array<6>;
constexpr auto null_extension = bech32_checksum{ 0 };

static data_chunk bech32_expand_prefix(const std::string& prefix)
{
    const auto size = prefix.size();

    data_chunk out;
    out.resize(2 * size + 1);

    for (size_t i = 0; i < size; ++i)
    {
        const auto c = prefix[i];

        out[i           ] = c >> 5;
        out[i + size + 1] = c & 31;
    }

    out[size] = 0;
    return out;
}

static bech32_checksum bech32_expand_checksum(uint32_t polymod)
{
    return bech32_checksum
    {
        (polymod >> 25) & 31,
        (polymod >> 20) & 31,
        (polymod >> 15) & 31,
        (polymod >> 10) & 31,
        (polymod >>  5) & 31,
        (polymod >>  0) & 31
    };
}

static uint32_t bech32_polymod(const data_slice& expanded)
{
    constexpr uint32_t g[]
    {
        0x3b6a57b2, 0x26508e6d, 0x1ea119fa, 0x3d4233dd, 0x2a1462b3
    };

    uint32_t c = 1;

    for (const auto v: expanded)
    {
        const auto c0 = (c >> 25);
        c = ((c & 0x1ffffff) << 5) ^ v;

        if (c0 & 0x01) c ^= g[0];
        if (c0 & 0x02) c ^= g[1];
        if (c0 & 0x04) c ^= g[2];
        if (c0 & 0x08) c ^= g[3];
        if (c0 & 0x10) c ^= g[4];
    }

    return c;
}

static const uint32_t constant(bool bech32m)
{
    return bech32m ? 0x2bc830a3 : 0x00000001;
}

// published

data_chunk bech32_build_checked(uint8_t version, const data_chunk& data,
    const std::string& prefix)
{
    auto expanded = build_chunk(
    {
        bech32_expand_prefix(prefix),
        data_chunk{ version },
        base32_expand(data),
        null_extension
    });

    const auto polymod = bech32_polymod(expanded) ^ constant(version > 0u);

    // Strip extension, append checksum and recompress.
    expanded.resize(expanded.size() - null_extension.size());
    extend_data(expanded, bech32_expand_checksum(polymod));
    return base32_compress(expanded);
}

bool bech32_verify_checked(uint8_t& out_version, data_chunk& in_out_data,
    const std::string& prefix)
{
    if (in_out_data.empty() || prefix.empty())
        return false;

    auto expanded = bech32_expand_prefix(prefix);
    const auto expanded_data = base32_expand(in_out_data);
    extend_data(expanded, expanded_data);

    // Strip version from expanded data and return with compressed program.
    out_version = expanded_data.front();
    data_chunk program{ std::next(expanded_data.begin()), expanded_data.end() };
    in_out_data = base32_compress(program);

    return bech32_polymod(expanded) == constant(out_version > 0u);
}

} // namespace system
} // namespace libbitcoin

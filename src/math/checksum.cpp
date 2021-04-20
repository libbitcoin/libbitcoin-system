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

#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/endian.hpp>

namespace libbitcoin {
namespace system {

// checksum encoding is a private data format, do not expose.
// Endianness is arbitrary here. All that is required is consistency.
typedef uint32_t checksum;

static checksum bitcoin_checksum(const data_slice& data)
{
    const auto hash = bitcoin_hash(data);

    // Read first four bytes from the bitcoin hash of the data.
    return from_little_endian_unsafe<checksum>(hash.begin());
}

data_chunk append_checksum(const loaf& slices)
{
    auto out = build_chunk(slices, checksum_default);
    append_checksum(out);
    return out;
}

void append_checksum(data_chunk& data)
{
    const auto check = bitcoin_checksum(data);

    // Append the checksum to the stretched data chunk.
    extend_data(data, to_little_endian<checksum>(check));
    BITCOIN_ASSERT(verify_checksum(data));
}

bool verify_checksum(const data_slice& data)
{
    if (data.size() < sizeof(checksum))
        return false;

    // Read the appended checksum from the end of the buffer.
    const auto position = std::prev(data.end(), sizeof(checksum));
    const auto check = from_little_endian<checksum>(position, data.end());

    // Compare the appended checksum to the checksum of the payload.
    return check == bitcoin_checksum({ data.begin(), position });
}

} // namespace system
} // namespace libbitcoin

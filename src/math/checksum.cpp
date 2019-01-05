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
#include <bitcoin/system/math/checksum.hpp>

#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/utility/endian.hpp>
#include <bitcoin/system/utility/deserializer.hpp>

namespace libbitcoin {
namespace system {

void append_checksum(data_chunk& data)
{
    const auto checksum = bitcoin_checksum(data);
    extend_data(data, to_little_endian(checksum));
}

uint32_t bitcoin_checksum(data_slice data)
{
    const auto hash = bitcoin_hash(data);
    return from_little_endian_unsafe<uint32_t>(hash.begin());
}

bool verify_checksum(data_slice data)
{
    if (data.size() < checksum_size)
        return false;

    // TODO: create a bitcoin_checksum overload that can accept begin/end.
    const auto checksum_begin = data.end() - checksum_size;
    data_slice slice(data.begin(), checksum_begin);
    auto checksum = from_little_endian_unsafe<uint32_t>(checksum_begin);
    return bitcoin_checksum(slice) == checksum;
}

} // namespace system
} // namespace libbitcoin


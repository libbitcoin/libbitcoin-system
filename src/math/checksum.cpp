/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/math/checksum.hpp>

#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>
#include <bitcoin/bitcoin/utility/deserializer.hpp>

namespace libbitcoin {

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

    data_slice body(data.begin(), data.end() - checksum_size);
    auto checksum = from_little_endian_unsafe<uint32_t>(data.end() - checksum_size);
    return bitcoin_checksum(body) == checksum;
}

} // namespace libbitcoin


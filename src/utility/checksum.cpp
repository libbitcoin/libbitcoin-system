/*
 * Copyright (c) 2011-2014 libbitcoin developers (see AUTHORS)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/utility/checksum.hpp>

#include <bitcoin/format.hpp>
#include <bitcoin/utility/hash.hpp>

namespace libbitcoin {

BC_API void append_checksum(data_chunk& data)
{
    uint32_t checksum = bitcoin_checksum(data);
    extend_data(data, to_little_endian(checksum));
}

BC_API bool verify_checksum(const data_chunk& data)
{
    data_chunk body(data.begin(), data.end() - 4);
    auto checksum = from_little_endian<uint32_t>(data.end() - 4);
    return bitcoin_checksum(body) == checksum;
}

} // namespace libbitcoin


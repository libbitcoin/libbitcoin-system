/*
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/message/network_address.hpp>

#include <bitcoin/bitcoin/utility/deserializer.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace message {

network_address::network_address()
{
}

network_address::network_address(const data_chunk& value)
{
    auto deserializer = make_deserializer(value.begin(), value.end());
    services = deserializer.read_8_bytes();
    // Read IP address
    ip = deserializer.read_bytes<16>();
    port = deserializer.read_big_endian<uint16_t>();
}

network_address::operator const data_chunk() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());
    serial.write_8_bytes(services);
    serial.write_data(ip);
    serial.write_big_endian<uint16_t>(port);
    return result;
}

size_t network_address::satoshi_size() const
{
    return network_address::satoshi_fixed_size();
}

size_t network_address::satoshi_fixed_size()
{
    return 26;
}

} // end message
} // end libbitcoin

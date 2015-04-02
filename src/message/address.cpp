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
#include <bitcoin/bitcoin/message/address.hpp>

namespace libbitcoin {
namespace message {

address::address()
{
}

address::address(const data_chunk& value)
: address(value.begin(), value.end())
{
}

address::operator const data_chunk() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    serial.write_variable_uint(addresses.size());

    for (const network_address& net_address: addresses)
    {
        serial.write_4_bytes(net_address.timestamp);
        data_chunk raw_address = net_address;
        serial.write_data(raw_address);
    }

    return result;
}

size_t address::satoshi_size() const
{
    return variable_uint_size(addresses.size())
        + addresses.size() * network_address::satoshi_fixed_size();
}

} // end message
} // end libbitcoin

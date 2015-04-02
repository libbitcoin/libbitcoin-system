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
#include <bitcoin/bitcoin/message/announce_version.hpp>

#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace message {

announce_version::announce_version()
{
}

announce_version::announce_version(const data_chunk& value)
: announce_version(value.begin(), value.end())
{
}

announce_version::operator const data_chunk() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());
    serial.write_4_bytes(version);
    serial.write_8_bytes(services);
    serial.write_8_bytes(timestamp);
    serial.write_network_address(address_me);
    serial.write_network_address(address_you);
    serial.write_8_bytes(nonce);
    serial.write_string(user_agent);
    serial.write_4_bytes(start_height);
    return result;
}

size_t announce_version::satoshi_size() const
{
    return 84 +
        variable_uint_size(user_agent.size()) + user_agent.size();
}

} // end message
} // end libbitcoin

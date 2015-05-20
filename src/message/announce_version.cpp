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
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/data_source.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace message {

announce_version announce_version::factory_from_data(const data_chunk& data)
{
    announce_version instance;
    instance.from_data(data);
    return instance;
}

announce_version announce_version::factory_from_data(std::istream& stream)
{
    announce_version instance;
    instance.from_data(stream);
    return instance;
}

bool announce_version::is_valid() const
{
    return (version != 0) ||
        (services != 0) ||
        (timestamp != 0) ||
        address_me.is_valid() ||
        address_you.is_valid() ||
        (nonce != 0) ||
        !user_agent.empty();
}

void announce_version::reset()
{
    version = 0;
    services = 0;
    timestamp = 0;
    address_me.reset();
    address_you.reset();
    nonce = 0;
    user_agent.clear();
}

bool announce_version::from_data(const data_chunk& data)
{
    byte_source<data_chunk> source(data);
    boost::iostreams::stream<byte_source<data_chunk>> istream(source);
    return from_data(istream);
}

bool announce_version::from_data(std::istream& stream)
{
    bool result = 0;

    reset();

    version = read_4_bytes(stream);
    services = read_8_bytes(stream);
    timestamp = read_8_bytes(stream);
    result = stream;

    if (result)
        result = address_me.from_data(stream, false);

    if (result && (version >= 106))
    {
        result = address_you.from_data(stream, false);
        nonce = read_8_bytes(stream);
        user_agent = read_string(stream);

        if (version >= 209)
            start_height = read_4_bytes(stream);

        result &= !stream.fail();
    }

    if (!result)
        reset();

    return result;
}

data_chunk announce_version::to_data() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());
    serial.write_4_bytes(version);
    serial.write_8_bytes(services);
    serial.write_8_bytes(timestamp);
    serial.write_data(address_me.to_data(false));
    serial.write_data(address_you.to_data(false));
    serial.write_8_bytes(nonce);
    serial.write_string(user_agent);
    serial.write_4_bytes(start_height);
    return result;
}

uint64_t announce_version::satoshi_size() const
{
    return 32 + (2 * network_address::satoshi_fixed_size(false)) +
        variable_uint_size(user_agent.size()) + user_agent.size();
}

} // end message
} // end libbitcoin

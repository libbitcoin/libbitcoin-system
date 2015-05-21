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
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>
#include <bitcoin/bitcoin/utility/ostream.hpp>

namespace libbitcoin {
namespace message {

ip_address null_address = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

network_address network_address::factory_from_data(const data_chunk& data,
    bool with_timestamp /*= false*/)
{
    network_address instance;
    instance.from_data(data, with_timestamp);
    return instance;
}

network_address network_address::factory_from_data(std::istream& stream,
    bool with_timestamp /*= false*/)
{
    network_address instance;
    instance.from_data(stream, with_timestamp);
    return instance;
}

bool network_address::is_valid() const
{
    return (timestamp != 0) ||
        (services != 0) ||
        (port != 0) ||
        (ip != null_address);
}

void network_address::reset()
{
    timestamp = 0;
    services = 0;
    ip.fill(0);
    port = 0;
}

bool network_address::from_data(const data_chunk& data, bool with_timestamp)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(istream, with_timestamp);
}

bool network_address::from_data(std::istream& stream, bool with_timestamp)
{
    reset();

    if (with_timestamp)
        timestamp = read_4_bytes(stream);

    services = read_8_bytes(stream);
    ip= read_bytes<16>(stream);
    port = read_big_endian<uint16_t>(stream);

    if (!stream)
        reset();

    return stream;
}

data_chunk network_address::to_data(bool with_timestamp) const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(ostream, with_timestamp);
    BOOST_ASSERT(data.size() == satoshi_size());
    return data;
}

void network_address::to_data(std::ostream& stream, bool with_timestamp) const
{
    if (with_timestamp)
        write_4_bytes(stream, timestamp);

    write_8_bytes(stream, services);
    write_data(stream, ip);
    write_big_endian<uint16_t>(stream, port);
}

uint64_t network_address::satoshi_size(bool with_timestamp) const
{
    return network_address::satoshi_fixed_size(with_timestamp);
}

uint64_t network_address::satoshi_fixed_size(bool with_timestamp)
{
    uint64_t result = 26;

    if (with_timestamp)
        result += 4;

    return result;
}

} // end message
} // end libbitcoin

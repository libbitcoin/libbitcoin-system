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
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

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

announce_version announce_version::factory_from_data(reader& source)
{
    announce_version instance;
    instance.from_data(source);
    return instance;
}

bool announce_version::is_valid() const
{
    return (version != 0)
        || (services != 0)
        || (timestamp != 0)
        || address_me.is_valid()
        || address_you.is_valid()
        || (nonce != 0)
        || !user_agent.empty()
        || (relay != 0);
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
    relay = 0;
}

bool announce_version::from_data(const data_chunk& data)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(istream);
}

bool announce_version::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool announce_version::from_data(reader& source)
{
    bool result = 0;

    reset();

    version = source.read_4_bytes_little_endian();
    services = source.read_8_bytes_little_endian();
    timestamp = source.read_8_bytes_little_endian();
    result = source;

    if (result)
        result = address_me.from_data(source, false);

    if (result && (version >= 106))
    {
        result = address_you.from_data(source, false);
        nonce = source.read_8_bytes_little_endian();
        user_agent = source.read_string();

        if (version >= 209)
            start_height = source.read_4_bytes_little_endian();

        // The satoshi client treats 209 as the "initial protocol version"
        // and disconnects peers below 31800 (for getheaders support).
        if (version >= 70001)
            relay = (source.read_byte() != 0);

        result &= source;
    }

    if (!result)
        reset();

    return result;
}

data_chunk announce_version::to_data() const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == satoshi_size());
    return data;
}

void announce_version::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void announce_version::to_data(writer& sink) const
{
    sink.write_4_bytes_little_endian(version);
    sink.write_8_bytes_little_endian(services);
    sink.write_8_bytes_little_endian(timestamp);
    address_me.to_data(sink, false);

    if (version >= 106)
    {
        address_you.to_data(sink, false);
        sink.write_8_bytes_little_endian(nonce);
        sink.write_string(user_agent);
    }

    if (version >= 209)
        sink.write_4_bytes_little_endian(start_height);

    if (version >= 70001)
        sink.write_byte(relay ? 1 : 0);
}

uint64_t announce_version::satoshi_size() const
{
    uint64_t size = 4 + 8 + 8 + address_me.satoshi_size(false);

    if (version >= 106)
    {
        size += address_you.satoshi_size(false) + 8 +
            variable_uint_size(user_agent.size()) + user_agent.size();
    }

    if (version >= 209)
        size += 4;

    if (version >= 70001)
        size += 1;

    return size;
}

} // end message
} // end libbitcoin

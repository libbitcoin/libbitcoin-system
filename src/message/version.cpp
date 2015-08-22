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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/message/version.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

version version::factory_from_data(const data_chunk& data)
{
    version instance;
    instance.from_data(data);
    return instance;
}

version version::factory_from_data(std::istream& stream)
{
    version instance;
    instance.from_data(stream);
    return instance;
}

version version::factory_from_data(reader& source)
{
    version instance;
    instance.from_data(source);
    return instance;
}

bool version::is_valid() const
{
    return (value != 0)
        || (services != 0)
        || (timestamp != 0)
        || address_me.is_valid()
        || address_you.is_valid()
        || (nonce != 0)
        || !user_agent.empty()
        || (relay != 0);
}

void version::reset()
{
    value = 0;
    services = 0;
    timestamp = 0;
    address_me.reset();
    address_you.reset();
    nonce = 0;
    user_agent.clear();
    relay = 0;
}

bool version::from_data(const data_chunk& data)
{
    data_source istream(data);
    return from_data(istream);
}

bool version::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool version::from_data(reader& source)
{
    auto result = false;
    reset();
    value = source.read_4_bytes_little_endian();
    services = source.read_8_bytes_little_endian();
    timestamp = source.read_8_bytes_little_endian();
    result = source;
    if (result)
        result = address_me.from_data(source, false);

    if (result && (value >= 106))
    {
        result = address_you.from_data(source, false);
        nonce = source.read_8_bytes_little_endian();
        user_agent = source.read_string();
        if (value >= 209)
            start_height = source.read_4_bytes_little_endian();

        // The satoshi client treats 209 as the "initial protocol version"
        // and disconnects peers below 31800 (for getheaders support).
        if (value >= 70001)
            relay = (source.read_byte() != 0);

        result &= source;
    }

    if (!result)
        reset();

    return result;
}

data_chunk version::to_data() const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == satoshi_size());
    return data;
}

void version::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void version::to_data(writer& sink) const
{
    sink.write_4_bytes_little_endian(value);
    sink.write_8_bytes_little_endian(services);
    sink.write_8_bytes_little_endian(timestamp);
    address_me.to_data(sink, false);
    if (value >= 106)
    {
        address_you.to_data(sink, false);
        sink.write_8_bytes_little_endian(nonce);
        sink.write_string(user_agent);
    }

    if (value >= 209)
        sink.write_4_bytes_little_endian(start_height);

    if (value >= 70001)
        sink.write_byte(relay ? 1 : 0);
}

uint64_t version::satoshi_size() const
{
    auto size = 4 + 8 + 8 + address_me.satoshi_size(false);
    if (value >= 106)
    {
        size += address_you.satoshi_size(false) + 8 +
            variable_uint_size(user_agent.size()) + user_agent.size();
    }

    if (value >= 209)
        size += 4;

    if (value >= 70001)
        size += 1;

    return size;
}

} // namspace message
} // namspace libbitcoin

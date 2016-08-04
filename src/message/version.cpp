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
#include <bitcoin/bitcoin/message/version.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string message::version::command = "version";

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
        || (services_sender != 0)
        || (timestamp != 0)
        ////|| (services_recevier != 0)
        || address_recevier.is_valid()
        || address_sender.is_valid()
        || (nonce != 0)
        || !user_agent.empty()
        || (start_height != 0)
        || (relay != 0);
}

void version::reset()
{
    value = 0;
    services_sender = 0;
    timestamp = 0;
    ////services_recevier = 0;
    address_recevier.reset();
    address_sender.reset();
    nonce = 0;
    user_agent.clear();
    start_height = 0;
    relay = false;
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
    reset();

    value = source.read_4_bytes_little_endian();
    services_sender = source.read_8_bytes_little_endian();
    timestamp = source.read_8_bytes_little_endian();
    auto result = static_cast<bool>(source);

    if (result && (value >= 106))
    {
        /*services_recevier = source.read_8_bytes_little_endian();*/
        result = address_recevier.from_data(source, false);
    }

    /*const auto services_sender_copy = source.read_8_bytes_little_endian();*/
    result = result && address_sender.from_data(source, false);
    nonce = source.read_8_bytes_little_endian();
    user_agent = source.read_string();
    start_height = source.read_4_bytes_little_endian();

    if (value >= 70001)
        relay = (source.read_byte() != 0);

    // The protocol requires duplication of the sender's services.
    result &= (source /*&& services_sender == services_sender_copy*/);

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
    BITCOIN_ASSERT(data.size() == serialized_size());
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
    sink.write_8_bytes_little_endian(services_sender);
    sink.write_8_bytes_little_endian(timestamp);

    if (value >= 106)
    {
        ////sink.write_8_bytes_little_endian(services_recevier);
        address_recevier.to_data(sink, false);
    }

    ////sink.write_8_bytes_little_endian(services_sender);
    address_sender.to_data(sink, false);
    sink.write_8_bytes_little_endian(nonce);
    sink.write_string(user_agent);
    sink.write_4_bytes_little_endian(start_height);

    if (value >= 70001)
        sink.write_byte(relay ? 1 : 0);
}

uint64_t version::serialized_size() const
{
    auto size = sizeof(value) + sizeof(services_sender) + sizeof(timestamp);

    if (value >= 106)
        size += /*sizeof(services_recevier) +*/  
            address_recevier.serialized_size(false);

    size += /*sizeof(services_sender) +*/
        address_sender.serialized_size(false) +
        sizeof(nonce) +
        variable_uint_size(user_agent.size()) + user_agent.size() +
        sizeof(start_height);

    if (value >= 70001)
        size += sizeof(uint8_t);

    return size;
}

} // namspace message
} // namspace libbitcoin

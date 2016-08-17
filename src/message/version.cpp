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

#include <algorithm>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string version::command = "version";
const uint32_t message::version::version_minimum = level::minimum;
const uint32_t message::version::version_maximum = level::maximum;

version version::factory_from_data(uint32_t version,
    const data_chunk& data)
{
    message::version instance;
    instance.from_data(version, data);
    return instance;
}

version version::factory_from_data(uint32_t version,
    std::istream& stream)
{
    message::version instance;
    instance.from_data(version, stream);
    return instance;
}

version version::factory_from_data(uint32_t version,
    reader& source)
{
    message::version instance;
    instance.from_data(version, source);
    return instance;
}

bool version::is_valid() const
{
    return (value != 0)
        || (services != 0)
        || (timestamp != 0)
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
    services = 0;
    timestamp = 0;
    address_recevier.reset();
    address_sender.reset();
    nonce = 0;
    user_agent.clear();
    user_agent.shrink_to_fit();
    start_height = 0;
    relay = false;
}

bool version::from_data(uint32_t version, const data_chunk& data)
{
    data_source istream(data);
    return from_data(version, istream);
}

bool version::from_data(uint32_t version, std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool version::from_data(uint32_t version, reader& source)
{
    reset();

    value = source.read_4_bytes_little_endian();
    const uint32_t effective_version = std::min(version, value);
    services = source.read_8_bytes_little_endian();
    timestamp = source.read_8_bytes_little_endian();
    auto result = static_cast<bool>(source);
    result = address_recevier.from_data(version, source, false);
    result = result && address_sender.from_data(version, source, false);
    nonce = source.read_8_bytes_little_endian();
    user_agent = source.read_string();
    start_height = source.read_4_bytes_little_endian();

    // HACK: due to the partial implementation of Bitcoin Core BIP37.
    if (effective_version >= level::bip61)
        relay = (source.read_byte() != 0);

    // HACK: disabled check due to inconsistent node implementation.
    // The protocol expects duplication of the sender's services.
    result &= (source /*&& services == address_sender.services*/);

    if (!result)
        reset();

    return result;
}

data_chunk version::to_data(uint32_t version) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version));
    return data;
}

void version::to_data(uint32_t version, std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void version::to_data(uint32_t version, writer& sink) const
{
    sink.write_4_bytes_little_endian(value);
    const uint32_t effective_version = std::min(version, value);
    sink.write_8_bytes_little_endian(services);
    sink.write_8_bytes_little_endian(timestamp);
    address_recevier.to_data(version, sink, false);
    address_sender.to_data(version, sink, false);
    sink.write_8_bytes_little_endian(nonce);
    sink.write_string(user_agent);
    sink.write_4_bytes_little_endian(start_height);

    if (effective_version >= level::bip37)
        sink.write_byte(relay ? 1 : 0);
}

uint64_t version::serialized_size(uint32_t version) const
{
    auto size = 
        sizeof(value) +
        sizeof(services) +
        sizeof(timestamp) +
        address_recevier.serialized_size(version, false) +
        address_sender.serialized_size(version, false) +
        sizeof(nonce) +
        variable_uint_size(user_agent.size()) + user_agent.size() +
        sizeof(start_height);

    if (value >= level::bip37)
        size += sizeof(uint8_t);

    return size;
}

} // namspace message
} // namspace libbitcoin

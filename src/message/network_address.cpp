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
#include <bitcoin/bitcoin/message/network_address.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

ip_address null_address =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

network_address network_address::factory_from_data(uint32_t version,
    const data_chunk& data, bool with_timestamp)
{
    network_address instance;
    instance.from_data(version, data, with_timestamp);
    return instance;
}

network_address network_address::factory_from_data(uint32_t version,
    std::istream& stream, bool with_timestamp)
{
    network_address instance;
    instance.from_data(version, stream, with_timestamp);
    return instance;
}

network_address network_address::factory_from_data(uint32_t version,
    reader& source, bool with_timestamp)
{
    network_address instance;
    instance.from_data(version, source, with_timestamp);
    return instance;
}

bool network_address::is_valid() const
{
    return (timestamp != 0)
        || (services != 0)
        || (port != 0)
        || (ip != null_address);
}

void network_address::reset()
{
    timestamp = 0;
    services = 0;
    ip.fill(0);
    port = 0;
}

bool network_address::from_data(uint32_t version,
    const data_chunk& data, bool with_timestamp)
{
    data_source istream(data);
    return from_data(version, istream, with_timestamp);
}

bool network_address::from_data(uint32_t version,
    std::istream& stream, bool with_timestamp)
{
    istream_reader source(stream);
    return from_data(version, source, with_timestamp);
}

bool network_address::from_data(uint32_t version,
    reader& source, bool with_timestamp)
{
    auto result = false;

    reset();

    if (with_timestamp)
        timestamp = source.read_4_bytes_little_endian();

    services = source.read_8_bytes_little_endian();
    const auto ip_size = source.read_data(ip.data(), ip.size());
    port = source.read_2_bytes_big_endian();
    result = source && (ip.size() == ip_size);

    if (!result)
        reset();

    return result;
}

data_chunk network_address::to_data(uint32_t version,
    bool with_timestamp) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(version, ostream, with_timestamp);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version, with_timestamp));
    return data;
}

void network_address::to_data(uint32_t version,
    std::ostream& stream, bool with_timestamp) const
{
    ostream_writer sink(stream);
    to_data(version, sink, with_timestamp);
}

void network_address::to_data(uint32_t version,
    writer& sink, bool with_timestamp) const
{
    if (with_timestamp)
        sink.write_4_bytes_little_endian(timestamp);

    sink.write_8_bytes_little_endian(services);
    sink.write_data(ip.data(), ip.size());
    sink.write_2_bytes_big_endian(port);
}

uint64_t network_address::serialized_size(uint32_t version,
    bool with_timestamp) const
{
    return network_address::satoshi_fixed_size(version, with_timestamp);
}

uint64_t network_address::satoshi_fixed_size(uint32_t version,
    bool with_timestamp)
{
    uint64_t result = 26;

    if (with_timestamp)
        result += 4;

    return result;
}

} // namspace message
} // namspace libbitcoin

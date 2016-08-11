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
#include <bitcoin/bitcoin/message/send_compact_blocks.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string send_compact_blocks::command = "sendcmpct";
const uint32_t send_compact_blocks::version_minimum = version::level::bip152;
const uint32_t send_compact_blocks::version_maximum = version::level::bip152;

send_compact_blocks send_compact_blocks::factory_from_data(
    const uint32_t version, const data_chunk& data)
{
    send_compact_blocks instance;
    instance.from_data(version, data);
    return instance;
}

send_compact_blocks send_compact_blocks::factory_from_data(
    const uint32_t version, std::istream& stream)
{
    send_compact_blocks instance;
    instance.from_data(version, stream);
    return instance;
}

send_compact_blocks send_compact_blocks::factory_from_data(
    const uint32_t version, reader& source)
{
    send_compact_blocks instance;
    instance.from_data(version, source);
    return instance;
}

bool send_compact_blocks::is_valid() const
{
    return true;
}

void send_compact_blocks::reset()
{
    high_bandwidth_mode = false;
    version = 0;
}

bool send_compact_blocks::from_data(uint32_t version,
    const data_chunk& data)
{
    data_source istream(data);
    return from_data(version, istream);
}

bool send_compact_blocks::from_data(uint32_t version,
    std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool send_compact_blocks::from_data(uint32_t version,
    reader& source)
{
    reset();
    const auto insufficient_version = (version < send_compact_blocks::version_minimum);

    const auto mode = source.read_byte();
    auto result = static_cast<bool>(source);

    if (mode > 1)
        result &= false;

    high_bandwidth_mode = (mode == 1);

    this->version = source.read_8_bytes_little_endian();
    result &= static_cast<bool>(source);

    if (!result || insufficient_version)
        reset();

    return result && !insufficient_version;
}

data_chunk send_compact_blocks::to_data(uint32_t version) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version));
    return data;
}

void send_compact_blocks::to_data(uint32_t version,
    std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void send_compact_blocks::to_data(uint32_t version,
    writer& sink) const
{
    sink.write_byte(high_bandwidth_mode ? 1 : 0);
    sink.write_8_bytes_little_endian(this->version);
}

uint64_t send_compact_blocks::serialized_size(uint32_t version) const
{
    return send_compact_blocks::satoshi_fixed_size(version);
}


uint64_t send_compact_blocks::satoshi_fixed_size(uint32_t version)
{
    return 9;
}

} // namspace message
} // namspace libbitcoin

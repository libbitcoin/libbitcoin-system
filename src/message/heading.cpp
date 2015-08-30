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
#include <bitcoin/bitcoin/message/heading.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

heading heading::factory_from_data(const data_chunk& data)
{
    heading instance;
    instance.from_data(data);
    return instance;
}

heading heading::factory_from_data(std::istream& stream)
{
    heading instance;
    instance.from_data(stream);
    return instance;
}

heading heading::factory_from_data(reader& source)
{
    heading instance;
    instance.from_data(source);
    return instance;
}

bool heading::is_valid() const
{
    return (magic != 0)
        || (payload_size != 0)
        || (checksum != 0)
        || !command.empty();
}

void heading::reset()
{
    magic = 0;
    command.clear();
    payload_size = 0;
    checksum = 0;
}

bool heading::from_data(const data_chunk& data)
{
    data_source istream(data);
    return from_data(istream);
}

bool heading::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool heading::from_data(reader& source)
{
    reset();
    magic = source.read_4_bytes_little_endian();
    command = source.read_fixed_string(command_size);
    payload_size = source.read_4_bytes_little_endian();
    checksum = source.read_4_bytes_little_endian();
    if (!source)
        reset();

    return source;
}

data_chunk heading::to_data() const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == heading::serialized_size);
    return data;
}

void heading::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void heading::to_data(writer& sink) const
{
    sink.write_4_bytes_little_endian(magic);
    sink.write_fixed_string(command, command_size);
    sink.write_4_bytes_little_endian(payload_size);
    sink.write_4_bytes_little_endian(checksum);
}

bool operator==(const heading& left, const heading& right)
{
    return (left.magic == right.magic)
        && (left.command == right.command)
        && (left.payload_size == right.payload_size)
        && (left.checksum == right.checksum);
}

bool operator!=(const heading& left, const heading& right)
{
    return !(left == right);
}

} // namspace message
} // namspace libbitcoin

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
#include <bitcoin/bitcoin/message/header.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>

namespace libbitcoin {
namespace message {

header header::factory_from_data(const data_chunk& data)
{
    header instance;
    instance.from_data(data);
    return instance;
}

header header::factory_from_data(std::istream& stream)
{
    header instance;
    instance.from_data(stream);
    return instance;
}

bool header::is_valid() const
{
    return (magic != 0) ||
        (payload_length != 0) ||
        (checksum != 0) ||
        !command.empty();
}

void header::reset()
{
    magic = 0;
    command.clear();
    payload_length = 0;
    checksum = 0;
}

bool header::from_data(const data_chunk& data)
{
    byte_source<data_chunk> source(data);
    boost::iostreams::stream<byte_source<data_chunk>> istream(source);
    return from_data(istream);
}

bool header::from_data(std::istream& stream)
{
    reset();

    magic = read_4_bytes(stream);
    command = read_fixed_string(stream, command_size);
    payload_length = read_4_bytes(stream);
    checksum = 0;

    if (stream)
        reset();

    return stream;
}

data_chunk header::to_data() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());
    serial.write_4_bytes(magic);
    serial.write_fixed_string(command, command_size);
    serial.write_4_bytes(payload_length);

    if (checksum != 0)
        serial.write_4_bytes(checksum);

    return result;
}

uint64_t header::satoshi_size() const
{
    return 20 + (checksum == 0 ? 0 : 4);
}

} // end message
} // end libbitcoin

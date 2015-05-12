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
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/data_stream_host.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>
namespace libbitcoin {
namespace message {

uint32_t header::magic() const
{
    return magic_;
}

void header::magic(const uint32_t value)
{
    magic_ = value;
}

std::string& header::command()
{
    return command_;
}

const std::string& header::command() const
{
    return command_;
}

void header::command(const std::string& value)
{
    command_ = value;
}

uint32_t header::payload_length() const
{
    return payload_length_;
}

void header::payload_length(const uint32_t value)
{
    payload_length_ = value;
}

uint32_t header::checksum() const
{
    return checksum_;
}

void header::checksum(const uint32_t value)
{
    checksum_ = value;
}

void header::reset()
{
    magic_ = 0;
    command_.clear();
    payload_length_ = 0;
    checksum_ = 0;
}

bool header::from_data(const data_chunk& data)
{
    data_chunk_stream_host host(data);
    return from_data(host.stream);
}

bool header::from_data(std::istream& stream)
{
    bool result = true;

    reset();

    magic_ = read_4_bytes(stream);
    command_ = read_fixed_string(stream, command_size);
    payload_length_ = read_4_bytes(stream);
    checksum_ = 0;
    result = !stream.fail();

    if (!result)
        reset();

    return result;
}

data_chunk header::to_data() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());
    serial.write_4_bytes(magic_);
    serial.write_fixed_string(command_, command_size);
    serial.write_4_bytes(payload_length_);
    if (checksum_ != 0)
        serial.write_4_bytes(checksum_);
    return result;
}

size_t header::satoshi_size() const
{
    return 20 + (checksum_ == 0 ? 0 : 4);
}

} // end message
} // end libbitcoin

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

namespace libbitcoin {
namespace message {

header::header()
    : magic_(), command_(), payload_length_(), checksum_()
{
}

header::header(uint32_t magic, std::string command, uint32_t payload_length,
    uint32_t checksum)
    : magic_(magic), command_(command), payload_length_(payload_length),
      checksum_(checksum)
{
}

header::header(const data_chunk& value)
: header(value.begin(), value.end())
{
}

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

header::operator const data_chunk() const
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

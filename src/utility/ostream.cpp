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
#include <bitcoin/bitcoin/utility/ostream.hpp>

namespace libbitcoin {

void write_byte(std::ostream& stream, uint8_t value)
{
    stream.put(value);
}

void write_2_bytes(std::ostream& stream, uint16_t value)
{
    write_little_endian<uint16_t>(stream, value);
}

void write_4_bytes(std::ostream& stream, uint32_t value)
{
    write_little_endian<uint32_t>(stream, value);
}

void write_8_bytes(std::ostream& stream, uint64_t value)
{
    write_little_endian<uint64_t>(stream, value);
}

void write_variable_uint(std::ostream& stream, uint64_t value)
{
    if (value < 0xfd)
    {
        write_byte(stream, (uint8_t)value);
    }
    else if (value <= 0xffff)
    {
        write_byte(stream, 0xfd);
        write_2_bytes(stream, (uint16_t)value);
    }
    else if (value <= 0xffffffff)
    {
        write_byte(stream, 0xfe);
        write_4_bytes(stream, (uint32_t)value);
    }
    else
    {
        write_byte(stream, 0xff);
        write_8_bytes(stream, value);
    }
}

//void write_data(std::ostream& stream, const data_chunk& value)
//{
//    stream.write(reinterpret_cast<const char*>(value.data()), value.size());
//}

void write_hash(std::ostream& stream, const hash_digest& value)
{
    stream.write(reinterpret_cast<const char*>(value.data()), value.size());
}

void write_short_hash(std::ostream& stream, const short_hash& value)
{
    stream.write(reinterpret_cast<const char*>(value.data()), value.size());
}

void write_fixed_string(std::ostream& stream, const std::string& value, size_t string_size)
{
    size_t max_size = std::max(string_size, value.size());
    data_chunk raw_string(string_size, 0);

    std::copy_n(value.begin(), max_size, raw_string.begin());

    // conditionally truncate
    if (max_size > string_size)
        raw_string.resize(string_size);

    write_data(stream, raw_string);
}

void write_string(std::ostream& stream, const std::string& value)
{
    write_variable_uint(stream, value.size());
    stream.write(value.data(), value.size());
}

}

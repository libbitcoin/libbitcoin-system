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
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {

ostream_writer::ostream_writer(std::ostream& stream)
  : stream_(stream)
{
}

ostream_writer::operator bool() const
{
    return (bool)stream_;
}

bool ostream_writer::operator!() const
{
    return !stream_;
}

void ostream_writer::write_byte(uint8_t value)
{
    stream_.put(value);
}

void ostream_writer::write_2_bytes_little_endian(uint16_t value)
{
    write_little_endian<uint16_t>(value);
}

void ostream_writer::write_4_bytes_little_endian(uint32_t value)
{
    write_little_endian<uint32_t>(value);
}

void ostream_writer::write_8_bytes_little_endian(uint64_t value)
{
    write_little_endian<uint64_t>(value);
}

void ostream_writer::write_variable_uint_little_endian(uint64_t value)
{
    if (value < 0xfd)
    {
        write_byte((uint8_t)value);
    }
    else if (value <= 0xffff)
    {
        write_byte(0xfd);
        write_2_bytes_little_endian((uint16_t)value);
    }
    else if (value <= 0xffffffff)
    {
        write_byte(0xfe);
        write_4_bytes_little_endian((uint32_t)value);
    }
    else
    {
        write_byte(0xff);
        write_8_bytes_little_endian(value);
    }
}

void ostream_writer::write_2_bytes_big_endian(uint16_t value)
{
    write_big_endian<uint16_t>(value);
}

void ostream_writer::write_4_bytes_big_endian(uint32_t value)
{
    write_big_endian<uint32_t>(value);
}

void ostream_writer::write_8_bytes_big_endian(uint64_t value)
{
    write_big_endian<uint64_t>(value);
}

void ostream_writer::write_variable_uint_big_endian(uint64_t value)
{
    if (value < 0xfd)
    {
        write_byte((uint8_t)value);
    }
    else if (value <= 0xffff)
    {
        write_byte(0xfd);
        write_2_bytes_big_endian((uint16_t)value);
    }
    else if (value <= 0xffffffff)
    {
        write_byte(0xfe);
        write_4_bytes_big_endian((uint32_t)value);
    }
    else
    {
        write_byte(0xff);
        write_8_bytes_big_endian(value);
    }
}

void ostream_writer::write_data(const data_chunk& data)
{
    write_data<const data_chunk>(data);
}

void ostream_writer::write_data(const uint8_t* data, size_t size)
{
    stream_.write(reinterpret_cast<const char*>(data), size);
}

void ostream_writer::write_hash(const hash_digest& value)
{
    stream_.write(reinterpret_cast<const char*>(value.data()), value.size());
}

void ostream_writer::write_short_hash(const short_hash& value)
{
    stream_.write(reinterpret_cast<const char*>(value.data()), value.size());
}

void ostream_writer::write_mini_hash(const mini_hash& value)
{
    stream_.write(reinterpret_cast<const char*>(value.data()), value.size());
}

void ostream_writer::write_fixed_string(const std::string& value, size_t size)
{
    const auto min_size = std::min(size, value.size());
    data_chunk raw_string(size, 0);
    std::copy_n(value.begin(), min_size, raw_string.begin());
    write_data(raw_string);
}

void ostream_writer::write_string(const std::string& value)
{
    write_variable_uint_little_endian(value.size());
    stream_.write(value.data(), value.size());
}

} // namespace libbitcoin

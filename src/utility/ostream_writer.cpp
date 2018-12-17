/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/system/utility/ostream_writer.hpp>

#include <algorithm>
#include <iostream>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/utility/endian.hpp>
#include <bitcoin/system/utility/reader.hpp>

namespace libbitcoin {
namespace system {

ostream_writer::ostream_writer(std::ostream& stream)
  : stream_(stream)
{
}

// Context.
//-----------------------------------------------------------------------------

ostream_writer::operator bool() const
{
    return (bool)stream_;
}

bool ostream_writer::operator!() const
{
    return !stream_;
}

// Hashes.
//-----------------------------------------------------------------------------

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

// Big Endian Integers.
//-----------------------------------------------------------------------------

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

void ostream_writer::write_variable_big_endian(uint64_t value)
{
    if (value < varint_two_bytes)
    {
        write_byte(static_cast<uint8_t>(value));
    }
    else if (value <= max_uint16)
    {
        write_byte(varint_two_bytes);
        write_2_bytes_big_endian(static_cast<uint16_t>(value));
    }
    else if (value <= max_uint32)
    {
        write_byte(varint_four_bytes);
        write_4_bytes_big_endian(static_cast<uint32_t>(value));
    }
    else
    {
        write_byte(varint_eight_bytes);
        write_8_bytes_big_endian(value);
    }
}

void ostream_writer::write_size_big_endian(size_t value)
{
    write_variable_big_endian(value);
}

// Little Endian Integers.
//-----------------------------------------------------------------------------

void ostream_writer::write_error_code(const code& ec)
{
    write_4_bytes_little_endian(static_cast<uint32_t>(ec.value()));
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

void ostream_writer::write_variable_little_endian(uint64_t value)
{
    if (value < varint_two_bytes)
    {
        write_byte(static_cast<uint8_t>(value));
    }
    else if (value <= max_uint16)
    {
        write_byte(varint_two_bytes);
        write_2_bytes_little_endian(static_cast<uint16_t>(value));
    }
    else if (value <= max_uint32)
    {
        write_byte(varint_four_bytes);
        write_4_bytes_little_endian(static_cast<uint32_t>(value));
    }
    else
    {
        write_byte(varint_eight_bytes);
        write_8_bytes_little_endian(value);
    }
}

void ostream_writer::write_size_little_endian(size_t value)
{
    write_variable_little_endian(value);
}

// Bytes.
//-----------------------------------------------------------------------------

void ostream_writer::write_byte(uint8_t value)
{
    stream_.put(value);
}

void ostream_writer::write_bytes(const data_slice data)
{
    const auto size = data.size();

    if (size > 0)
        stream_.write(reinterpret_cast<const char*>(data.data()), size);
}

void ostream_writer::write_bytes(const uint8_t* data, size_t size)
{
    auto buffer = reinterpret_cast<const char*>(data);
    stream_.write(buffer, size);
}

void ostream_writer::write_string(const std::string& value, size_t size)
{
    const auto length = std::min(size, value.size());
    write_bytes(reinterpret_cast<const uint8_t*>(value.data()), length);
    data_chunk padding(floor_subtract(size, length), string_terminator);
    write_bytes(padding);
}

void ostream_writer::write_string(const std::string& value)
{
    write_variable_little_endian(value.size());
    stream_.write(value.data(), value.size());
}

void ostream_writer::skip(size_t size)
{
    // TODO: verify.
    stream_.seekp(size, std::ios_base::cur);
}

} // namespace system
} // namespace libbitcoin

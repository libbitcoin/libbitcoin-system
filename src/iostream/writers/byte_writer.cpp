/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/iostream/writers/byte_writer.hpp>

#include <algorithm>
#include <cstdint>
#include <cstddef>
#include <ostream>
#include <string>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/error.hpp>
#include <bitcoin/system/serialization/endian.hpp>

namespace libbitcoin {
namespace system {

// constructors
//-----------------------------------------------------------------------------

byte_writer::byte_writer(std::ostream& stream) noexcept
  : stream_(stream)
{
}

byte_writer::~byte_writer() noexcept
{
    do_flush();
}

// big endian
//-----------------------------------------------------------------------------

void byte_writer::write_2_bytes_big_endian(uint16_t value) noexcept
{
    write_big_endian<uint16_t>(value);
}

void byte_writer::write_4_bytes_big_endian(uint32_t value) noexcept
{
    write_big_endian<uint32_t>(value);
}

void byte_writer::write_8_bytes_big_endian(uint64_t value) noexcept
{
    write_big_endian<uint64_t>(value);
}

// little endian
//-----------------------------------------------------------------------------

void byte_writer::write_2_bytes_little_endian(uint16_t value) noexcept
{
    write_little_endian<uint16_t>(value);
}

void byte_writer::write_4_bytes_little_endian(uint32_t value) noexcept
{
    write_little_endian<uint32_t>(value);
}

void byte_writer::write_8_bytes_little_endian(uint64_t value) noexcept
{
    write_little_endian<uint64_t>(value);
}

void byte_writer::write_variable(uint64_t value) noexcept
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

void byte_writer::write_error_code(const code& ec) noexcept
{
    write_4_bytes_little_endian(static_cast<uint32_t>(ec.value()));
}

// bytes
//-----------------------------------------------------------------------------

void byte_writer::write(byte_reader& in) noexcept
{
    while (!in.is_exhausted())
        write_byte(in.read_byte());
}

void byte_writer::write_byte(uint8_t value) noexcept
{
    do_write(value);
}

void byte_writer::write_bytes(const data_slice& data) noexcept
{
    const auto size = data.size();
    if (is_zero(size))
        return;

    write_bytes(data.data(), size);
}

void byte_writer::write_bytes(const uint8_t* data, size_t size) noexcept
{
    do_write(data, size);
}

// strings
//-----------------------------------------------------------------------------

void byte_writer::write_string(const std::string& value, size_t size) noexcept
{
    const auto length = std::min(size, value.size());
    write_bytes(reinterpret_cast<const uint8_t*>(value.data()), length);
    write_bytes(data_chunk(size - length, string_terminator));
}

void byte_writer::write_string(const std::string& value) noexcept
{
    write_variable(value.size());
    write_string(value.data(), value.size());
}

void byte_writer::skip(size_t size) noexcept
{
    do_skip(size);
}

// context
//-----------------------------------------------------------------------------

void byte_writer::flush() noexcept
{
    do_flush();
}

byte_writer::operator bool() const noexcept
{
    return get_valid();
}

bool byte_writer::operator!() const noexcept
{
    return !get_valid();
}

// protected virtual
//-----------------------------------------------------------------------------

void byte_writer::do_write(uint8_t byte) noexcept
{
    stream_.put(byte);
}

void byte_writer::do_write(const uint8_t* data, size_t size) noexcept
{
    if (is_zero(size))
        return;

    // It is not generally more efficient to call stream_.put() for one byte.
    stream_.write(reinterpret_cast<const char*>(data), size);
}

// TODO: implement forward seek.
void byte_writer::do_skip(size_t size) noexcept
{
    ////// TODO: verify this behavior.
    ////// TODO: this cannot work with the stream::out::push.
    ////stream_.seekp(size, std::ios_base::cur);
    while (!is_zero(size--))
        write_byte(0x00);
}

void byte_writer::do_flush() noexcept
{
    stream_.flush();
}

bool byte_writer::get_valid() const noexcept
{
    return !!stream_;
}

} // namespace system
} // namespace libbitcoin

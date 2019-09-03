/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

// Sponsored in part by Digital Contract Design, LLC

#include <bitcoin/system/utility/ostream_bit_writer.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/utility/endian.hpp>
#include <bitcoin/system/utility/reader.hpp>

namespace libbitcoin {
namespace system {

BC_CONSTEXPR uint8_t bit_mask = 0x01;
BC_CONSTEXPR uint64_t low_byte_mask_64 = 0x00000000000000FF;
BC_CONSTEXPR uint64_t low_bit_mask_64 = 0x0000000000000001;

ostream_bit_writer::ostream_bit_writer(writer& writer)
  : buffer_(0x00), offset_(0), writer_(writer)
{
}

ostream_bit_writer::~ostream_bit_writer()
{
    flush();
}

void ostream_bit_writer::buffered_write(data_chunk& data)
{
    if (data.size() > 0)
    {
        uint8_t next_buffer = (data.back() << (byte_bits - offset_));

        for (auto index = (data.size() - 1); index > 0; index--)
            data[index] = ((data[index] << (byte_bits - offset_))
                | (data[index - 1] >> offset_));

        data[0] = (buffer_ | (data[0] >> offset_));

        writer_.write_bytes(data);
        buffer_ = next_buffer;
    }
}

void ostream_bit_writer::flush()
{
    if (offset_ > 0)
    {
        writer_.write_byte(buffer_);
        buffer_ = 0x00;
        offset_ = 0;
    }
}

// Context.
//-----------------------------------------------------------------------------

ostream_bit_writer::operator bool() const
{
    return (bool)writer_;
}

bool ostream_bit_writer::operator!() const
{
    return !writer_;
}

// Hashes.
//-----------------------------------------------------------------------------

void ostream_bit_writer::write_hash(const hash_digest& value)
{
    write_bytes(value.data(), value.size());
}

void ostream_bit_writer::write_short_hash(const short_hash& value)
{
    write_bytes(value.data(), value.size());
}

void ostream_bit_writer::write_mini_hash(const mini_hash& value)
{
    write_bytes(value.data(), value.size());
}

// Big Endian Integers.
//-----------------------------------------------------------------------------

void ostream_bit_writer::write_2_bytes_big_endian(uint16_t value)
{
    write_big_endian<uint16_t>(value);
}

void ostream_bit_writer::write_4_bytes_big_endian(uint32_t value)
{
    write_big_endian<uint32_t>(value);
}

void ostream_bit_writer::write_8_bytes_big_endian(uint64_t value)
{
    write_big_endian<uint64_t>(value);
}

void ostream_bit_writer::write_variable_big_endian(uint64_t value)
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

void ostream_bit_writer::write_variable_bits_big_endian(uint64_t value,
    uint8_t least_significant_bits)
{
    // TODO: should be an exception
    auto remaining_bits = std::min(static_cast<uint64_t>(least_significant_bits),
        static_cast<uint64_t>(byte_bits * sizeof(uint64_t)));

    while (remaining_bits > byte_bits)
    {
        uint8_t current = ((value >> (remaining_bits - byte_bits))
            & low_byte_mask_64);

        write_byte(current);
        remaining_bits -= byte_bits;
    }

    for (uint8_t index = 1; index <= remaining_bits; index++)
        write_bit((value >> (remaining_bits - index)) & low_bit_mask_64);
}

void ostream_bit_writer::write_size_big_endian(size_t value)
{
    write_variable_big_endian(value);
}

// Little Endian Integers.
//-----------------------------------------------------------------------------

void ostream_bit_writer::write_error_code(const code& ec)
{
    write_4_bytes_little_endian(static_cast<uint32_t>(ec.value()));
}

void ostream_bit_writer::write_2_bytes_little_endian(uint16_t value)
{
    write_little_endian<uint16_t>(value);
}

void ostream_bit_writer::write_4_bytes_little_endian(uint32_t value)
{
    write_little_endian<uint32_t>(value);
}

void ostream_bit_writer::write_8_bytes_little_endian(uint64_t value)
{
    write_little_endian<uint64_t>(value);
}

void ostream_bit_writer::write_variable_little_endian(uint64_t value)
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

void ostream_bit_writer::write_size_little_endian(size_t value)
{
    write_variable_little_endian(value);
}

// Bytes.
//-----------------------------------------------------------------------------

void ostream_bit_writer::write_bit(bool value)
{
    uint8_t byte_value = value ? 0x80 : 0x00;
    buffer_ |= (byte_value >> offset_);
    offset_++;

    if (offset_ >= byte_bits)
        flush();
}

void ostream_bit_writer::write_byte(uint8_t value)
{
    uint8_t next_byte = value;

    if (offset_ > 0)
    {
        next_byte = buffer_ | (value >> offset_);
        buffer_ = value << (byte_bits - offset_);
    }

    writer_.write_byte(next_byte);
}

void ostream_bit_writer::write_bytes(const data_slice& data)
{
    const auto size = data.size();

    if ((offset_ > 0) && (size > 0))
    {
        data_chunk duplicate(data.begin(), data.end());
        buffered_write(duplicate);
    }
    else
        writer_.write_bytes(data);
}

void ostream_bit_writer::write_bytes(const uint8_t* data, size_t size)
{
    if ((offset_ > 0) && (size > 0))
    {
        data_chunk duplicate;
        duplicate.reserve(size);

        for (size_t i = 0; i < size; i++)
            duplicate[i] = data[i];

        buffered_write(duplicate);
    }
    else
        writer_.write_bytes(data, size);
}

void ostream_bit_writer::write_string(const std::string& value, size_t size)
{
    const auto length = std::min(size, value.size());
    write_bytes(reinterpret_cast<const uint8_t*>(value.data()), length);
    data_chunk padding(floor_subtract(size, length), string_terminator);
    write_bytes(padding);
}

void ostream_bit_writer::write_string(const std::string& value)
{
    write_variable_little_endian(value.size());
    write_string(value, value.size());
}

void ostream_bit_writer::skip(size_t size)
{
    // save offset, flush and restore offset to perform logical skip
    uint8_t prev_offset = offset_;
    flush();
    offset_ = prev_offset;

    // skip
    writer_.skip(size);
}

} // namespace system
} // namespace libbitcoin

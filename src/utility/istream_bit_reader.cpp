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

#include <bitcoin/system/utility/istream_bit_reader.hpp>

#include <algorithm>
#include <cstddef>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/endian.hpp>

namespace libbitcoin {
namespace system {

BC_CONSTEXPR uint8_t bit_mask = 0x80;
BC_CONSTEXPR uint8_t unsigned_eof = static_cast<uint8_t>(
    std::istream::traits_type::eof());

BC_CONSTEXPR uint64_t low_bit_mask_64 = 0x01;
BC_CONSTEXPR uint64_t uint64_bits =
    static_cast<uint64_t>(byte_bits) *
    static_cast<uint64_t>(sizeof(uint64_t));

istream_bit_reader::istream_bit_reader(reader& reader)
  : buffer_(0x00), offset_(byte_bits), reader_(reader)
{
}

void istream_bit_reader::feed()
{
    if (offset_ == byte_bits)
    {
        buffer_ = reader_.read_byte();

        if (reader_)
            offset_ = 0;
    }
}

// Context.
//-----------------------------------------------------------------------------

istream_bit_reader::operator bool() const
{
    return (bool)reader_;
}

bool istream_bit_reader::operator!() const
{
    return !reader_;
}

bool istream_bit_reader::is_exhausted() const
{
    return reader_.is_exhausted() && (offset_ == byte_bits);
}

void istream_bit_reader::invalidate()
{
    reader_.invalidate();
    offset_ = byte_bits;
}

// Hashes.
//-----------------------------------------------------------------------------

hash_digest istream_bit_reader::read_hash()
{
    return read_forward<hash_size>();
}

short_hash istream_bit_reader::read_short_hash()
{
    return read_forward<short_hash_size>();
}

mini_hash istream_bit_reader::read_mini_hash()
{
    return read_forward<mini_hash_size>();
}

// Big Endian Integers.
//-----------------------------------------------------------------------------

uint16_t istream_bit_reader::read_2_bytes_big_endian()
{
    return read_big_endian<uint16_t>();
}

uint32_t istream_bit_reader::read_4_bytes_big_endian()
{
    return read_big_endian<uint32_t>();
}

uint64_t istream_bit_reader::read_8_bytes_big_endian()
{
    return read_big_endian<uint64_t>();
}

uint64_t istream_bit_reader::read_variable_big_endian()
{
    const auto value = read_byte();

    switch (value)
    {
        case varint_eight_bytes:
            return read_8_bytes_big_endian();
        case varint_four_bytes:
            return read_4_bytes_big_endian();
        case varint_two_bytes:
            return read_2_bytes_big_endian();
        default:
            return value;
    }
}

uint64_t istream_bit_reader::read_bits(uint8_t bits)
{
    auto remaining_bits = std::min(uint64_bits, static_cast<uint64_t>(bits));

    if (remaining_bits == 0)
    {
        invalidate();
        return 0;
    }

    uint64_t result = 0;

    while (remaining_bits > byte_bits)
    {
        const auto current = read_byte();
        result |= (static_cast<uint64_t>(current) << (remaining_bits - byte_bits));
        remaining_bits -= byte_bits;
    }

    for (uint8_t index = 1; index <= remaining_bits; index++)
        if (read_bit())
            result |= (low_bit_mask_64 << (remaining_bits - index));

    return result;
}

size_t istream_bit_reader::read_size_big_endian()
{
    const auto size = read_variable_big_endian();

    // This facilitates safely passing the size into a follow-on reader.
    // Return zero allows follow-on use before testing reader state.
    if (size <= max_size_t)
        return static_cast<size_t>(size);

    invalidate();
    return 0;
}

// Little Endian Integers.
//-----------------------------------------------------------------------------

code istream_bit_reader::read_error_code()
{
    const auto value = read_little_endian<uint32_t>();
    return code(static_cast<error::error_code_t>(value));
}

uint16_t istream_bit_reader::read_2_bytes_little_endian()
{
    return read_little_endian<uint16_t>();
}

uint32_t istream_bit_reader::read_4_bytes_little_endian()
{
    return read_little_endian<uint32_t>();
}

uint64_t istream_bit_reader::read_8_bytes_little_endian()
{
    return read_little_endian<uint64_t>();
}

uint64_t istream_bit_reader::read_variable_little_endian()
{
    const auto value = read_byte();

    switch (value)
    {
        case varint_eight_bytes:
            return read_8_bytes_little_endian();
        case varint_four_bytes:
            return read_4_bytes_little_endian();
        case varint_two_bytes:
            return read_2_bytes_little_endian();
        default:
            return value;
    }
}

size_t istream_bit_reader::read_size_little_endian()
{
    const auto size = read_variable_little_endian();

    // This facilitates safely passing the size into a follow-on reader.
    // Return zero allows follow-on use before testing reader state.
    if (size <= max_size_t)
        return static_cast<size_t>(size);

    invalidate();
    return 0;
}

// Bits.
//-----------------------------------------------------------------------------

bool istream_bit_reader::read_bit()
{
    feed();
    return ((buffer_ << offset_++) & bit_mask) != 0;
}

// Bytes.
//-----------------------------------------------------------------------------

uint8_t istream_bit_reader::peek_byte()
{
    if (offset_ == byte_bits)
        return reader_.peek_byte();

    if (offset_ > 0)
    {
        const uint8_t hi = (buffer_ << offset_);
        const uint8_t low = reader_ ?
            (reader_.peek_byte() >> (byte_bits - offset_)) : 0x00;

        return hi | low;
    }

    return buffer_;
}

uint8_t istream_bit_reader::read_byte()
{
    if (offset_ == byte_bits)
        return reader_.read_byte();

    if (offset_ > 0)
    {
        const uint8_t hi = (buffer_ << offset_);

        if (!reader_.is_exhausted())
        {
            buffer_ = reader_.read_byte();
        }
        else
        {
            buffer_ = 0x00;
            offset_ = byte_bits;
        }

        const uint8_t low = (buffer_ >> (byte_bits - offset_));
        return hi | low;
    }

    offset_ = byte_bits;
    return buffer_;
}

data_chunk istream_bit_reader::read_bytes()
{
    data_chunk out;

    while (!is_exhausted())
        out.push_back(read_byte());

    return out;
}

// Return size is guaranteed.
// This is a memory exhaustion risk if caller does not control size.
data_chunk istream_bit_reader::read_bytes(size_t size)
{
    if (size == 0)
        return {};

    const size_t raw_size = (offset_ == 0) ? size - 1 : size;
    auto out = reader_.read_bytes(raw_size);

    if (offset_ == 0)
        out.push_back(0x00);

    if (offset_ < byte_bits)
    {
        const auto next_buffer = out.back();

        for (auto index = out.size() - 1; index > 0; index--)
            out[index] = ((out[index] >> (byte_bits - offset_)) |
                (out[index - 1] << offset_));

        out[0] = ((buffer_ << offset_) | (out[0] >> (byte_bits - offset_)));

        if (offset_ == 0)
        {
            buffer_ = next_buffer;
            offset_ = byte_bits;
        }
    }

    return out;
}

std::string istream_bit_reader::read_string()
{
    return read_string(read_size_little_endian());
}

// Removes trailing zeros, required for bitcoin string comparisons.
std::string istream_bit_reader::read_string(size_t size)
{
    std::string out;
    out.reserve(size);
    auto terminated = false;

    // Read all size characters, pushing all non-null (may be many).
    for (size_t index = 0; index < size && !empty(); ++index)
    {
        const auto character = read_byte();
        terminated |= (character == string_terminator);

        // Stop pushing characters at the first null.
        if (!terminated)
            out.push_back(character);
    }

    // Reduce the allocation to the number of characters pushed.
    out.shrink_to_fit();
    return out;
}

void istream_bit_reader::skip(size_t size)
{
    read_bytes(size);
}

// protected

bool istream_bit_reader::empty() const
{
    return (offset_ == byte_bits) && (reader_.peek_byte() == unsigned_eof);
}

} // namespace system
} // namespace libbitcoin

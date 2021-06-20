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
#include <bitcoin/system/iostream/byte_reader.hpp>

#include <cstddef>
#include <cstdint>
#include <istream>
#include <string>
#include <utility>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/error.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/serialization/endian.hpp>

namespace libbitcoin {
namespace system {

// constructors
//-----------------------------------------------------------------------------

byte_reader::byte_reader(std::istream& source) noexcept
  : stream_(source)
{
}

byte_reader::~byte_reader() noexcept
{
}

// big endian
//-----------------------------------------------------------------------------

uint16_t byte_reader::read_2_bytes_big_endian() noexcept
{
    return read_big_endian<uint16_t>();
}

uint32_t byte_reader::read_4_bytes_big_endian() noexcept
{
    return read_big_endian<uint32_t>();
}

uint64_t byte_reader::read_8_bytes_big_endian() noexcept
{
    return read_big_endian<uint64_t>();
}

// little endian
//-----------------------------------------------------------------------------

uint16_t byte_reader::read_2_bytes_little_endian() noexcept
{
    return read_little_endian<uint16_t>();
}

uint32_t byte_reader::read_4_bytes_little_endian() noexcept
{
    return read_little_endian<uint32_t>();
}

uint64_t byte_reader::read_8_bytes_little_endian() noexcept
{
    return read_little_endian<uint64_t>();
}

uint64_t byte_reader::read_variable() noexcept
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

size_t byte_reader::read_size() noexcept
{
    const auto size = read_variable();

    // This facilitates safely passing the size into a follow-on reader.
    // Return zero allows follow-on use before testing reader state.
    if (size > max_size_t)
    {
        invalidate();
        return zero;
    }

    return static_cast<size_t>(size);
}

code byte_reader::read_error_code() noexcept
{
    const auto value = read_little_endian<uint32_t>();
    return code(static_cast<error::error_code_t>(value));
}

// bytes
//-----------------------------------------------------------------------------

mini_hash byte_reader::read_mini_hash() noexcept
{
    return read_forward<mini_hash_size>();
}

short_hash byte_reader::read_short_hash() noexcept
{
    return read_forward<short_hash_size>();
}

hash_digest byte_reader::read_hash() noexcept
{
    return read_forward<hash_size>();
}

long_hash byte_reader::read_long_hash() noexcept
{
    return read_forward<long_hash_size>();
}

uint8_t byte_reader::peek_byte() noexcept
{
    return do_peek();
}

uint8_t byte_reader::read_byte() noexcept
{
    return do_read();
}

data_chunk byte_reader::read_bytes() noexcept
{
    data_chunk out;
    while (!is_exhausted())
        out.push_back(read_byte());

    return std::move(out);
}

data_chunk byte_reader::read_bytes(size_t size) noexcept
{
    if (is_zero(size))
        return {};

    data_chunk out(no_fill_allocator);
    out.resize(size);
    do_read(out.data(), size);
    return std::move(out);
}

void byte_reader::read_bytes(uint8_t* buffer, size_t size) noexcept
{
    do_read(buffer, size);
}

// strings
//-----------------------------------------------------------------------------

std::string byte_reader::read_string() noexcept
{
    return read_string(read_size());
}

// Removes trailing zeros, required for bitcoin string comparisons.
std::string byte_reader::read_string(size_t size) noexcept
{
    std::string out;
    out.reserve(size);
    auto terminated = false;

    while (!is_zero(size--) && !is_exhausted())
    {
        const auto byte = read_byte();
        terminated |= (byte == string_terminator);

        // Stop pushing characters at the first null.
        if (!terminated)
            out.push_back(byte);
    }

    out.shrink_to_fit();
    return out;
}

void byte_reader::skip(size_t size) noexcept
{
    // Stream source is presumed to not be seekable.
    read_bytes(size);
}

// context
//-----------------------------------------------------------------------------

bool byte_reader::is_exhausted() const noexcept
{
    return get_exhausted();
}

void byte_reader::invalidate() noexcept
{
    set_invalid();
}

byte_reader::operator bool() const noexcept
{
    return get_valid();
}

bool byte_reader::operator!() const noexcept
{
    return !get_valid();
}

// protected virtual
//-----------------------------------------------------------------------------
// Stream state is not checked on reads, stream should return 0x00 if failed.

uint8_t byte_reader::do_peek() noexcept
{
    // This invalidates the stream if empty (invalid read).
    return stream_.peek();
}

uint8_t byte_reader::do_read() noexcept
{
    return stream_.get();
}

void byte_reader::do_read(uint8_t* buffer, size_t size) noexcept
{
    if (is_zero(size))
        return;

    // It is not generally more efficient to call stream_.get() for one byte.
    stream_.read(reinterpret_cast<char*>(buffer), size);
}

bool byte_reader::get_valid() const noexcept
{
    return !!stream_;
}

bool byte_reader::get_exhausted() const noexcept
{
    // This will invalidate an empty stream. stream.peek is non-const but this
    // compiles because the stream_ member is a reference. The behavior can be
    // unexpected as the state of the stream, reader and bit reader can change
    // as a result of the peek, which reads and then restores a byte to stream.
    return !stream_ || stream_.peek() == std::istream::traits_type::eof();
}

void byte_reader::set_invalid() noexcept
{
    stream_.setstate(std::istream::failbit);
}

} // namespace system
} // namespace libbitcoin

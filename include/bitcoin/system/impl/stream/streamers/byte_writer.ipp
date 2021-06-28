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
#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BYTE_WRITER_IPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BYTE_WRITER_IPP

#include <algorithm>
#include <cstdint>
#include <cstddef>
#include <ostream>
#include <string>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/error.hpp>
#include <bitcoin/system/serialization/endian.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {
    
// All public methods must rely on protected for stream state except validity.

// constructors
//-----------------------------------------------------------------------------
    
template <typename OStream>
byte_writer<OStream>::byte_writer(OStream& sink) noexcept
  : stream_(sink)
{
}

template <typename OStream>
byte_writer<OStream>::~byte_writer() noexcept
{
    do_flush();
}

// big endian
//-----------------------------------------------------------------------------

template <typename OStream>
template <typename Integer, if_integer<Integer>>
void byte_writer<OStream>::write_big_endian(Integer value) noexcept
{
    // TODO: reimplement on derived classes and change to this here:
    ////to_big_endian<Integer>(stream_, value);

    // Call into virtual writer (vs. stream) so derived class can reuse.
    // This creates a copy that could be avoided by using stream overload.
    write_bytes(to_big_endian<Integer>(value));
}

template <typename OStream>
void byte_writer<OStream>::write_2_bytes_big_endian(uint16_t value) noexcept
{
    write_big_endian<uint16_t>(value);
}

template <typename OStream>
void byte_writer<OStream>::write_4_bytes_big_endian(uint32_t value) noexcept
{
    write_big_endian<uint32_t>(value);
}

template <typename OStream>
void byte_writer<OStream>::write_8_bytes_big_endian(uint64_t value) noexcept
{
    write_big_endian<uint64_t>(value);
}

// little endian
//-----------------------------------------------------------------------------

template <typename OStream>
template <typename Integer, if_integer<Integer>>
void byte_writer<OStream>::write_little_endian(Integer value) noexcept
{
    // TODO: reimplement on derived classes and change to this here:
    ////to_little_endian<Integer>(stream_, value);

    // Call into virtual writer (vs. stream) so derived class can reuse.
    // This creates a copy that could be avoided by using stream overload.
    write_bytes(to_little_endian<Integer>(value));
}

template <typename OStream>
void byte_writer<OStream>::write_2_bytes_little_endian(uint16_t value) noexcept
{
    write_little_endian<uint16_t>(value);
}

template <typename OStream>
void byte_writer<OStream>::write_4_bytes_little_endian(uint32_t value) noexcept
{
    write_little_endian<uint32_t>(value);
}

template <typename OStream>
void byte_writer<OStream>::write_8_bytes_little_endian(uint64_t value) noexcept
{
    write_little_endian<uint64_t>(value);
}

template <typename OStream>
void byte_writer<OStream>::write_variable(uint64_t value) noexcept
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

template <typename OStream>
void byte_writer<OStream>::write_error_code(const code& ec) noexcept
{
    write_4_bytes_little_endian(static_cast<uint32_t>(ec.value()));
}

// bytes
//-----------------------------------------------------------------------------

template <typename OStream>
void byte_writer<OStream>::write(std::istream& in) noexcept
{
    while (in)
        write_byte(in.get());
}

template <typename OStream>
void byte_writer<OStream>::write_byte(uint8_t value) noexcept
{
    do_write(value);
}

template <typename OStream>
void byte_writer<OStream>::write_bytes(const data_slice& data) noexcept
{
    const auto size = data.size();
    if (is_zero(size))
        return;

    write_bytes(data.data(), size);
}

template <typename OStream>
void byte_writer<OStream>::write_bytes(const uint8_t* data,
    size_t size) noexcept
{
    do_write(data, size);
}

// strings
//-----------------------------------------------------------------------------

template <typename OStream>
void byte_writer<OStream>::write_string(const std::string& value,
    size_t size) noexcept
{
    const auto length = std::min(size, value.size());
    write_bytes(reinterpret_cast<const uint8_t*>(value.data()), length);
    write_bytes(data_chunk(size - length, '\0'));
}

template <typename OStream>
void byte_writer<OStream>::write_string(const std::string& value) noexcept
{
    write_variable(value.size());
    write_string(value.data(), value.size());
}

// context
//-----------------------------------------------------------------------------

template <typename OStream>
void byte_writer<OStream>::flush() noexcept
{
    do_flush();
}

template <typename OStream>
byte_writer<OStream>::operator bool() const noexcept
{
    return valid();
}

// This should not be necessary with bool() defined, but it is.
template <typename OStream>
bool byte_writer<OStream>::operator!() const noexcept
{
    return !valid();
}

// protected virtual
//-----------------------------------------------------------------------------
// These may only call non-virtual (private) methods (due to overriding).

template <typename OStream>
void byte_writer<OStream>::do_write(uint8_t byte) noexcept
{
    stream_.put(byte);
    validate();
}

template <typename OStream>
void byte_writer<OStream>::do_write(const uint8_t* data, size_t size) noexcept
{
    // It is not generally more efficient to call stream_.put() for one byte.
    stream_.write(reinterpret_cast<const char*>(data), size);
    validate();
}

template <typename OStream>
void byte_writer<OStream>::do_flush() noexcept
{
    stream_.flush();
    validate();
}

// private
//-----------------------------------------------------------------------------
// These may only call other private methods (due to overriding).

template <typename OStream>
bool byte_writer<OStream>::valid() const noexcept
{
    // zero is the istream documented flag for no error.
    return stream_.rdstate() == OStream::goodbit;
}

template <typename OStream>
void byte_writer<OStream>::invalid() noexcept
{
    // If eofbit is set, failbit is generally set on all operations.
    // badbit is unrecoverable, set the others to ensure consistency.
    stream_.setstate(OStream::eofbit | OStream::failbit | OStream::badbit);
}

template <typename OStream>
void byte_writer<OStream>::validate() noexcept
{
    // Ensure that any failure in the call fully invalidates the stream/writer.
    // Some errors are recoverable, so a sequence of operations without testing
    // for validity could miss an error on intervening operations.
    if (!valid())
        invalid();
}

} // namespace system
} // namespace libbitcoin

#endif

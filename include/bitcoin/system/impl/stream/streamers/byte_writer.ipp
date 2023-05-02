/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include <ios>
#include <ostream>
#include <string>
#include <utility>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/streamers/byte_reader.hpp>

namespace libbitcoin {
namespace system {

// All public methods must rely on protected for stream state except validity.

// constructors
// ----------------------------------------------------------------------------

template <typename OStream>
byte_writer<OStream>::byte_writer(OStream& sink) NOEXCEPT
  : stream_(sink)
{
    ////BC_ASSERT_MSG(stream_.exceptions() == OStream::goodbit,
    ////    "Output stream must not be configured to throw exceptions.");
}

template <typename OStream>
byte_writer<OStream>::~byte_writer() NOEXCEPT
{
    flusher();
}

// big endian
// ----------------------------------------------------------------------------

template <typename OStream>
template <typename Integer, size_t Size,
    if_integer<Integer>, if_not_greater<Size, sizeof(Integer)>>
void byte_writer<OStream>::write_big_endian(Integer value) NOEXCEPT
{
    const auto bytes = byte_cast(native_to_big_end(value));

    // Call into virtual writer (vs. stream) so derived class can reuse.
    write_bytes(std::next(bytes.data(), sizeof(Integer) - Size), Size);
}

template <typename OStream>
void byte_writer<OStream>::write_2_bytes_big_endian(uint16_t value) NOEXCEPT
{
    write_big_endian<uint16_t>(value);
}

template <typename OStream>
void byte_writer<OStream>::write_3_bytes_big_endian(uint32_t value) NOEXCEPT
{
    write_big_endian<uint32_t, 3>(value);
}

template <typename OStream>
void byte_writer<OStream>::write_4_bytes_big_endian(uint32_t value) NOEXCEPT
{
    write_big_endian<uint32_t>(value);
}

template <typename OStream>
void byte_writer<OStream>::write_5_bytes_big_endian(uint64_t value) NOEXCEPT
{
    write_big_endian<uint64_t, 5>(value);
}

template <typename OStream>
void byte_writer<OStream>::write_6_bytes_big_endian(uint64_t value) NOEXCEPT
{
    write_big_endian<uint64_t, 6>(value);
}

template <typename OStream>
void byte_writer<OStream>::write_7_bytes_big_endian(uint64_t value) NOEXCEPT
{
    write_big_endian<uint64_t, 7>(value);
}

template <typename OStream>
void byte_writer<OStream>::write_8_bytes_big_endian(uint64_t value) NOEXCEPT
{
    write_big_endian<uint64_t>(value);
}

// little endian
// ----------------------------------------------------------------------------

template <typename OStream>
template <typename Integer, size_t Size,
    if_integer<Integer>, if_not_greater<Size, sizeof(Integer)>>
void byte_writer<OStream>::write_little_endian(Integer value) NOEXCEPT
{
    const auto bytes = byte_cast(native_to_little_end(value));

    // Call into virtual writer (vs. stream) so derived class can reuse.
    write_bytes(bytes.data(), Size);
}

template <typename OStream>
void byte_writer<OStream>::write_2_bytes_little_endian(uint16_t value) NOEXCEPT
{
    write_little_endian<uint16_t>(value);
}

template <typename OStream>
void byte_writer<OStream>::write_3_bytes_little_endian(uint32_t value) NOEXCEPT
{
    write_little_endian<uint32_t, 3>(value);
}

template <typename OStream>
void byte_writer<OStream>::write_4_bytes_little_endian(uint32_t value) NOEXCEPT
{
    write_little_endian<uint32_t>(value);
}

template <typename OStream>
void byte_writer<OStream>::write_5_bytes_little_endian(uint64_t value) NOEXCEPT
{
    write_little_endian<uint64_t, 5>(value);
}

template <typename OStream>
void byte_writer<OStream>::write_6_bytes_little_endian(uint64_t value) NOEXCEPT
{
    write_little_endian<uint64_t, 6>(value);
}

template <typename OStream>
void byte_writer<OStream>::write_7_bytes_little_endian(uint64_t value) NOEXCEPT
{
    write_little_endian<uint64_t, 7>(value);
}

template <typename OStream>
void byte_writer<OStream>::write_8_bytes_little_endian(uint64_t value) NOEXCEPT
{
    write_little_endian<uint64_t>(value);
}

// Normal consensus form.
// There is exactly one representation for any number in the domain.
template <typename OStream>
void byte_writer<OStream>::write_variable(uint64_t value) NOEXCEPT
{
    if (value < varint_two_bytes)
    {
        write_byte(narrow_cast<uint8_t>(value));
    }
    else if (value <= max_uint16)
    {
        write_byte(varint_two_bytes);
        write_2_bytes_little_endian(narrow_cast<uint16_t>(value));
    }
    else if (value <= max_uint32)
    {
        write_byte(varint_four_bytes);
        write_4_bytes_little_endian(narrow_cast<uint32_t>(value));
    }
    else
    {
        write_byte(varint_eight_bytes);
        write_8_bytes_little_endian(value);
    }
}

// Normal client-server form.
template <typename OStream>
void byte_writer<OStream>::write_error_code(const code& ec) NOEXCEPT
{
    write_4_bytes_little_endian(sign_cast<uint32_t>(ec.value()));
}

// bytes
// ----------------------------------------------------------------------------

template <typename OStream>
std::istream& byte_writer<OStream>::write(std::istream& in) NOEXCEPT
{
    // This creates an intermediate buffer the size of the stream.
    // This is presumed to be more optimal than looping individual bytes.
    write_bytes(byte_reader<std::istream>(in).read_bytes());
    return in;
}

template <typename OStream>
void byte_writer<OStream>::write_byte(uint8_t value) NOEXCEPT
{
    do_write_bytes(&value, one);
}

template <typename OStream>
void byte_writer<OStream>::write_bytes(const data_slice& data) NOEXCEPT
{
    do_write_bytes(data.data(), data.size());
}

template <typename OStream>
void byte_writer<OStream>::write_bytes(const uint8_t* data,
    size_t size) NOEXCEPT
{
    do_write_bytes(data, size);
}

// strings
// ----------------------------------------------------------------------------
// Normal p2p form (consensus hash no strings).

template <typename OStream>
void byte_writer<OStream>::write_string(const std::string& value) NOEXCEPT
{
    write_variable(value.size());
    write_string_buffer(value, value.size());
}

template <typename OStream>
void byte_writer<OStream>::write_string_buffer(const std::string& value,
    size_t size) NOEXCEPT
{
    const auto length = std::min(size, value.size());

    // Write the substring without creating a copy.
    write_bytes(data_slice(value).data(), length);

    // Pad to length with zeros, required for Bitcoin heading serialization.
    write_bytes(data_chunk(size - length, pad()));
}

// context
// ----------------------------------------------------------------------------

template <typename OStream>
void byte_writer<OStream>::flush() NOEXCEPT
{
    do_flush();
}

// control
// ----------------------------------------------------------------------------
// These only call non-virtual (private) methods.

template <typename OStream>
size_t byte_writer<OStream>::get_write_position() NOEXCEPT
{
    return getter();
}

template <typename OStream>
byte_writer<OStream>::operator bool() const NOEXCEPT
{
    return valid();
}

// This should not be necessary with bool() defined, but it is.
template <typename OStream>
bool byte_writer<OStream>::operator!() const NOEXCEPT
{
    return !valid();
}

// protected virtual
// ----------------------------------------------------------------------------
// These may only call non-virtual (private) methods (due to overriding).

// Suppress ostream members may throw inside NOEXCEPT.
// The intended behavior in this case is program abort.
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

template <typename OStream>
void byte_writer<OStream>::do_write_bytes(const uint8_t* data,
    size_t size) NOEXCEPT
{
    // It is not generally more efficient to call stream_.put() for one byte.

    // Write past stream start invalidates stream unless size exceeds maximum.
    BC_ASSERT(size <= maximum);
    stream_.write(pointer_cast<const char>(data),
        possible_narrow_and_sign_cast<typename OStream::pos_type>(size));

    validate();
}

template <typename OStream>
void byte_writer<OStream>::do_flush() NOEXCEPT
{
    flusher();
}

// private
// ----------------------------------------------------------------------------
// These may only call other private methods (due to overriding).

template <typename OStream>
bool byte_writer<OStream>::valid() const NOEXCEPT
{
    // zero is the istream documented flag for no error.
    return stream_.rdstate() == OStream::goodbit;
}

template <typename OStream>
void byte_writer<OStream>::invalid() NOEXCEPT
{
    // If eofbit is set, failbit is generally set on all operations.
    // badbit is unrecoverable, set the others to ensure consistency.
    stream_.setstate(OStream::eofbit | OStream::failbit | OStream::badbit);
}

template <typename OStream>
void byte_writer<OStream>::validate() NOEXCEPT
{
    // Ensure that any failure in the call fully invalidates the stream/writer.
    // Some errors are recoverable, so a sequence of operations without testing
    // for validity could miss an error on intervening operations.
    if (!valid())
        invalid();
}

template <typename OStream>
void byte_writer<OStream>::flusher() NOEXCEPT
{
    stream_.flush();
    validate();
}

template <typename OStream>
size_t byte_writer<OStream>::getter() NOEXCEPT
{
    static const auto failure = typename OStream::pos_type(-1);
    typename OStream::pos_type position;

    // Force these to be consistent, and avoid propagating exceptions.
    // Assuming behavior is consistent with seekg (as documented).
    // Returns current position on success and pos_type(-1) on failure.
    try
    {
        // This does not honor BOOST_EXCEPTION_DISABLE.
        position = stream_.tellp();
        validate();
    }
    catch (const typename OStream::failure&)
    {
        position = failure;
        invalid();
    }

    // Max size_t is presumed to exceed max OStream::pos_type.
    return position == failure ? zero :
        possible_narrow_and_sign_cast<size_t>(position);
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif

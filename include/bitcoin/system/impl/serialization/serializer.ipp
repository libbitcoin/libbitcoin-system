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
#ifndef LIBBITCOIN_SYSTEM_SERIALIZATION_SERIALIZER_IPP
#define LIBBITCOIN_SYSTEM_SERIALIZATION_SERIALIZER_IPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/error.hpp>
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/serialization/endian.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

template <typename Iterator>
serializer<Iterator>::serializer(const Iterator begin)
  : valid_(true), iterator_(begin)
{
}

// Context.
//-----------------------------------------------------------------------------

template <typename Iterator>
serializer<Iterator>::operator bool() const
{
    return valid_;
}

template <typename Iterator>
bool serializer<Iterator>::operator!() const
{
    return !valid_;
}

// Hashes.
//-----------------------------------------------------------------------------

template <typename Iterator>
void serializer<Iterator>::write_hash(const hash_digest& hash)
{
    write_forward(hash);
}

template <typename Iterator>
void serializer<Iterator>::write_short_hash(const short_hash& hash)
{
    write_forward(hash);
}

template <typename Iterator>
void serializer<Iterator>::write_mini_hash(const mini_hash& hash)
{
    write_forward(hash);
}

// Big Endian Integers.
//-----------------------------------------------------------------------------

template <typename Iterator>
void serializer<Iterator>::write_2_bytes_big_endian(uint16_t value)
{
    write_big_endian(value);
}

template <typename Iterator>
void serializer<Iterator>::write_4_bytes_big_endian(uint32_t value)
{
    write_big_endian(value);
}

template <typename Iterator>
void serializer<Iterator>::write_8_bytes_big_endian(uint64_t value)
{
    write_big_endian(value);
}

template <typename Iterator>
void serializer<Iterator>::write_variable_big_endian(uint64_t value)
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

template <typename Iterator>
void serializer<Iterator>::write_size_big_endian(size_t value)
{
    write_variable_big_endian(value);
}

// Little Endian Integers.
//-----------------------------------------------------------------------------

template <typename Iterator>
void serializer<Iterator>::write_error_code(const code& ec)
{
    write_4_bytes_little_endian(static_cast<uint32_t>(ec.value()));
}

template <typename Iterator>
void serializer<Iterator>::write_2_bytes_little_endian(uint16_t value)
{
    write_little_endian(value);
}

template <typename Iterator>
void serializer<Iterator>::write_4_bytes_little_endian(uint32_t value)
{
    write_little_endian(value);
}

template <typename Iterator>
void serializer<Iterator>::write_8_bytes_little_endian(uint64_t value)
{
    write_little_endian(value);
}

template <typename Iterator>
void serializer<Iterator>::write_variable_little_endian(uint64_t value)
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

template <typename Iterator>
void serializer<Iterator>::write_size_little_endian(size_t value)
{
    write_variable_little_endian(value);
}

// Bytes (unchecked).
//-----------------------------------------------------------------------------

template <typename Iterator>
void serializer<Iterator>::write(reader& in)
{
    while (!in.is_exhausted())
        write_byte(in.read_byte());
}

template <typename Iterator>
void serializer<Iterator>::write_byte(uint8_t value)
{
    *iterator_++ = value;
}

template <typename Iterator>
void serializer<Iterator>::write_bytes(const data_slice& data)
{
    write_forward(data);
}

template <typename Iterator>
void serializer<Iterator>::write_bytes(const uint8_t* data, size_t size)
{
    iterator_ = std::copy_n(data, size, iterator_);
}

template <typename Iterator>
void serializer<Iterator>::write_string(const std::string& value)
{
    write_variable_little_endian(value.size());
    write_forward(value);
}

template <typename Iterator>
void serializer<Iterator>::write_string(const std::string& value, size_t size)
{
    const auto length = std::min(size, value.size());
    write_bytes(reinterpret_cast<const uint8_t*>(value.data()), length);
    data_chunk padding(floor_subtract(size, length), string_terminator);
    write_bytes(padding);
}

template <typename Iterator>
void serializer<Iterator>::skip(size_t size)
{
    iterator_ += size;
}

// non-interface
//-------------------------------------------------------------------------

template <typename Iterator>
void serializer<Iterator>::write_delegated(functor write)
{
    write(*this);
}

template <typename Iterator>
template <typename Tuple>
void serializer<Iterator>::write_forward(const Tuple& data)
{
    iterator_ = std::copy(data.begin(), data.end(), iterator_);
}

template <typename Iterator>
template <typename Tuple>
void serializer<Iterator>::write_reverse(const Tuple& data)
{
    iterator_ = std::reverse_copy(data.begin(), data.end(), iterator_);
}

template <typename Iterator>
template <typename Integer, if_integer<Integer>>
void serializer<Iterator>::write_big_endian(Integer value)
{
    return write_forward(to_big_endian(value));
}

template <typename Iterator>
template <typename Integer, if_integer<Integer>>
void serializer<Iterator>::write_little_endian(Integer value)
{
    return write_forward(to_little_endian(value));
}

template <typename Iterator>
size_t serializer<Iterator>::read_size_big_endian()
{
    static_assert(sizeof(size_t) >= sizeof(uint32_t), "unexpected size");
    const auto prefix = *iterator_++;
    const auto begin = iterator_;
    uint64_t size;

    switch (prefix)
    {
        case varint_eight_bytes:
            iterator_ += sizeof(uint64_t);
            size = from_big_endian_unsafe<uint64_t>(begin);
        case varint_four_bytes:
            iterator_ += sizeof(uint32_t);
            return from_big_endian_unsafe<uint32_t>(begin);
        case varint_two_bytes:
            iterator_ += sizeof(uint16_t);
            return from_big_endian_unsafe<uint16_t>(begin);
        default:
            return prefix;
    }

    // This facilitates safely passing the size into a follow-on writer.
    // Return zero allows follow-on use before testing reader state.
    if (size <= max_size_t)
        return static_cast<size_t>(size);

    valid_ = false;
    return 0;
}

template <typename Iterator>
size_t serializer<Iterator>::read_size_little_endian()
{
    static_assert(sizeof(size_t) >= sizeof(uint32_t), "unexpected size");
    const auto prefix = *iterator_++;
    const auto begin = iterator_;
    uint64_t size;

    switch (prefix)
    {
        case varint_eight_bytes:
            iterator_ += sizeof(uint64_t);
            size = from_little_endian_unsafe<uint64_t>(begin);
        case varint_four_bytes:
            iterator_ += sizeof(uint32_t);
            return from_little_endian_unsafe<uint32_t>(begin);
        case varint_two_bytes:
            iterator_ += sizeof(uint16_t);
            return from_little_endian_unsafe<uint16_t>(begin);
        default:
            return prefix;
    }

    // This facilitates safely passing the size into a follow-on writer.
    // Return zero allows follow-on use before testing reader state.
    if (size <= max_size_t)
        return static_cast<size_t>(size);

    valid_ = false;
    return 0;
}

// Factories.
//-----------------------------------------------------------------------------

template <typename Iterator>
serializer<Iterator> make_unsafe_serializer(Iterator begin)
{
    return serializer<Iterator>(begin);
}

} // namespace system
} // namespace libbitcoin

#endif

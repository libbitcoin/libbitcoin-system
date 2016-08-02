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
#ifndef LIBBITCOIN_DESERIALIZER_IPP
#define LIBBITCOIN_DESERIALIZER_IPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <boost/asio/streambuf.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>
#include <bitcoin/bitcoin/utility/exceptions.hpp>

namespace libbitcoin {

// Macro used so that compiler will optimise out function calls to
// check_distance() if SafeCheckLast is false.
#define SAFE_CHECK_DISTANCE(N) \
    if (SafeCheckLast) \
        check_distance(iterator_, end_, N);

template <typename Iterator, bool SafeCheckLast>
deserializer<Iterator, SafeCheckLast>::deserializer(const Iterator begin,
    const Iterator end)
  : iterator_(begin), end_(end)
{
}

template <typename Iterator, bool SafeCheckLast>
deserializer<Iterator, SafeCheckLast>::operator bool() const
{
    return true;
}

template <typename Iterator, bool SafeCheckLast>
bool deserializer<Iterator, SafeCheckLast>::operator!() const
{
    return false;
}

template <typename Iterator, bool SafeCheckLast>
bool deserializer<Iterator, SafeCheckLast>::is_exhausted() const
{
    return (iterator_ == end_);
}

template <typename Iterator, bool SafeCheckLast>
uint8_t deserializer<Iterator, SafeCheckLast>::read_byte()
{
    SAFE_CHECK_DISTANCE(1);
    return *(iterator_++);
}

template <typename Iterator, bool SafeCheckLast>
uint16_t deserializer<Iterator, SafeCheckLast>::read_2_bytes_little_endian()
{
    return read_little_endian<uint16_t>();
}

template <typename Iterator, bool SafeCheckLast>
uint32_t deserializer<Iterator, SafeCheckLast>::read_4_bytes_little_endian()
{
    return read_little_endian<uint32_t>();
}

template <typename Iterator, bool SafeCheckLast>
uint64_t deserializer<Iterator, SafeCheckLast>::read_8_bytes_little_endian()
{
    return read_little_endian<uint64_t>();
}

template <typename Iterator, bool SafeCheckLast>
uint16_t deserializer<Iterator, SafeCheckLast>::read_2_bytes_big_endian()
{
    return read_big_endian<uint16_t>();
}

template <typename Iterator, bool SafeCheckLast>
uint32_t deserializer<Iterator, SafeCheckLast>::read_4_bytes_big_endian()
{
    return read_big_endian<uint32_t>();
}

template <typename Iterator, bool SafeCheckLast>
uint64_t deserializer<Iterator, SafeCheckLast>::read_8_bytes_big_endian()
{
    return read_big_endian<uint64_t>();
}

template <typename Iterator, bool SafeCheckLast>
template <typename T>
T deserializer<Iterator, SafeCheckLast>::read_big_endian()
{
    const auto begin = iterator_;
    SAFE_CHECK_DISTANCE(sizeof(T));
    iterator_ += sizeof(T);
    return from_big_endian_unsafe<T>(begin);
}
template <typename Iterator, bool SafeCheckLast>
template <typename T>
T deserializer<Iterator, SafeCheckLast>::read_little_endian()
{
    const auto begin = iterator_;
    SAFE_CHECK_DISTANCE(sizeof(T));
    iterator_ += sizeof(T);
    return from_little_endian_unsafe<T>(begin);
}

template <typename Iterator, bool SafeCheckLast>
uint64_t deserializer<Iterator,
    SafeCheckLast>::read_variable_uint_little_endian()
{
    const auto length = read_byte();
    if (length < 0xfd)
        return length;
    else if (length == 0xfd)
        return read_2_bytes_little_endian();
    else if (length == 0xfe)
        return read_4_bytes_little_endian();

    // length should be 0xff
    return read_8_bytes_little_endian();
}

template <typename Iterator, bool SafeCheckLast>
uint64_t deserializer<Iterator, SafeCheckLast>::read_variable_uint_big_endian()
{
    const auto length = read_byte();
    if (length < 0xfd)
        return length;
    else if (length == 0xfd)
        return read_2_bytes_big_endian();
    else if (length == 0xfe)
        return read_4_bytes_big_endian();

    // length should be 0xff
    return read_8_bytes_big_endian();
}

template <typename Iterator, bool SafeCheckLast>
data_chunk deserializer<Iterator, SafeCheckLast>::read_data(size_t size)
{
    SAFE_CHECK_DISTANCE(size);
    data_chunk raw_bytes(size);
    for (size_t i = 0; i < size; ++i)
        raw_bytes[i] = read_byte();

    return raw_bytes;
}

template <typename Iterator, bool SafeCheckLast>
size_t deserializer<Iterator, SafeCheckLast>::read_data(uint8_t* data,
    size_t size)
{
    SAFE_CHECK_DISTANCE(size);
    for (size_t i = 0; i < size; ++i)
        data[i] = read_byte();
    return size;
}

template <typename Iterator, bool SafeCheckLast>
code deserializer<Iterator, SafeCheckLast>::read_error_code()
{
    const auto value = read_little_endian<uint32_t>();
    return code(static_cast<error::error_code_t>(value));
}

template <typename Iterator, bool SafeCheckLast>
data_chunk deserializer<Iterator, SafeCheckLast>::read_data_to_eof()
{
    data_chunk raw_bytes;
    while (iterator_ != end_)
        raw_bytes.push_back(read_byte());

    return raw_bytes;
}

template <typename Iterator, bool SafeCheckLast>
hash_digest deserializer<Iterator, SafeCheckLast>::read_hash()
{
    return read_bytes<hash_size>();
}

template <typename Iterator, bool SafeCheckLast>
short_hash deserializer<Iterator, SafeCheckLast>::read_short_hash()
{
    return read_bytes<short_hash_size>();
}

template <typename Iterator, bool SafeCheckLast>
mini_hash deserializer<Iterator, SafeCheckLast>::read_mini_hash()
{
    return read_bytes<mini_hash_size>();
}

template <typename Iterator, bool SafeCheckLast>
std::string deserializer<Iterator, SafeCheckLast>::read_fixed_string(
    size_t length)
{
    data_chunk string_bytes = read_data(length);
    std::string result(string_bytes.begin(), string_bytes.end());

    // Removes trailing 0s... Needed for string comparisons
    return result.c_str();
}

template <typename Iterator, bool SafeCheckLast>
std::string deserializer<Iterator, SafeCheckLast>::read_string()
{
    const auto size = read_variable_uint_little_endian();

    // Warning: conversion from uint64_t to size_t, possible loss of data.
    return read_fixed_string(static_cast<size_t>(size));
}

template <typename Iterator, bool SafeCheckLast>
template <unsigned N>
byte_array<N> deserializer<Iterator, SafeCheckLast>::read_bytes()
{
    SAFE_CHECK_DISTANCE(N);
    byte_array<N> out;
    std::copy(iterator_, iterator_ + N, out.begin());
    iterator_ += N;
    return out;
}

template <typename Iterator, bool SafeCheckLast>
template <unsigned N>
byte_array<N> deserializer<Iterator, SafeCheckLast>::read_bytes_reverse()
{
    SAFE_CHECK_DISTANCE(N);
    byte_array<N> out;
    std::reverse_copy(iterator_, iterator_ + N, out.begin());
    iterator_ += N;
    return out;
}

/**
 * Returns underlying iterator.
 */
template <typename Iterator, bool SafeCheckLast>
Iterator deserializer<Iterator, SafeCheckLast>::iterator() const
{
    return iterator_;
}

/**
 * Useful if you advance the iterator using other serialization
 * methods or objects.
 */
template <typename Iterator, bool SafeCheckLast>
void deserializer<Iterator, SafeCheckLast>::set_iterator(
    const Iterator iterator)
{
    iterator_ = iterator;
}

// Try to advance iterator 'distance' increments forwards.
// Throw if we prematurely reach the end.
template <typename Iterator, bool SafeCheckLast>
void deserializer<Iterator, SafeCheckLast>::check_distance(
    Iterator it, const Iterator end, size_t distance)
{
    BITCOIN_ASSERT(SafeCheckLast);
    for (size_t i = 0; i < distance; ++i)
    {
        // Is this a valid byte?
        if (it == end)
            throw end_of_stream();

        // If so move to next value.
        ++it;
    }
}

#undef SAFE_CHECK_DISTANCE

template <typename Iterator>
deserializer<Iterator, true> make_deserializer(
    const Iterator begin, const Iterator end)
{
    return deserializer<Iterator, true>(begin, end);
}

template <typename Iterator>
deserializer<Iterator, false> make_deserializer_unsafe(
    const Iterator begin)
{
    // end argument isn't used so just reuse begin here.
    return deserializer<Iterator, false>(begin, begin);
}

} // libbitcoin

#endif


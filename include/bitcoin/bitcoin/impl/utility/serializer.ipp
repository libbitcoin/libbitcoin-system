/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SERIALIZER_IPP
#define LIBBITCOIN_SERIALIZER_IPP

#include <algorithm>
#include <boost/asio/streambuf.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>

namespace libbitcoin {

template <typename Iterator>
serializer<Iterator>::serializer(const Iterator begin)
  : iter_(begin)
{
}

template <typename Iterator>
void serializer<Iterator>::write_byte(uint8_t value)
{
    *iter_ = value;
    ++iter_;
}
template <typename Iterator>
void serializer<Iterator>::write_2_bytes(uint16_t value)
{
    write_little_endian(value);
}
template <typename Iterator>
void serializer<Iterator>::write_4_bytes(uint32_t value)
{
    write_little_endian(value);
}
template <typename Iterator>
void serializer<Iterator>::write_8_bytes(uint64_t value)
{
    write_little_endian(value);
}

template <typename Iterator>
template <typename T>
void serializer<Iterator>::write_big_endian(T n)
{
    return write_data(to_big_endian(n));
}
template <typename Iterator>
template <typename T>
void serializer<Iterator>::write_little_endian(T n)
{
    return write_data(to_little_endian(n));
}

template <typename Iterator>
void serializer<Iterator>::write_variable_uint(uint64_t value)
{
    if (value < 0xfd)
    {
        write_byte((uint8_t)value);
    }
    else if (value <= 0xffff)
    {
        write_byte(0xfd);
        write_2_bytes((uint16_t)value);
    }
    else if (value <= 0xffffffff)
    {
        write_byte(0xfe);
        write_4_bytes((uint32_t)value);
    }
    else
    {
        write_byte(0xff);
        write_8_bytes(value);
    }
}

template <typename Iterator>
template <typename T>
void serializer<Iterator>::write_data(const T& data)
{
    iter_ = std::copy(data.begin(), data.end(), iter_);
}

template <typename Iterator>
void serializer<Iterator>::write_network_address(network_address_type addr)
{
    write_8_bytes(addr.services);
    write_data(addr.ip);
    write_big_endian<uint16_t>(addr.port);
}

template <typename Iterator>
void serializer<Iterator>::write_hash(const hash_digest& hash)
{
    write_data(hash);
}

template <typename Iterator>
void serializer<Iterator>::write_short_hash(const short_hash& hash)
{
    write_data(hash);
}

template <typename Iterator>
void serializer<Iterator>::write_fixed_string(
    const std::string& command, size_t string_size)
{
    BITCOIN_ASSERT(command.size() <= string_size);
    data_chunk raw_string(string_size, 0);
    std::copy(command.begin(), command.end(), raw_string.begin());
    write_data(raw_string);
}

template <typename Iterator>
void serializer<Iterator>::write_string(const std::string& str)
{
    write_variable_uint(str.size());
    write_data(str);
}

/**
 * Returns underlying iterator.
 */
template <typename Iterator>
Iterator serializer<Iterator>::iterator()
{
    return iter_;
}

/**
 * Useful if you want to serialize some data using another
 * routine and then continue with this serializer.
 */
template <typename Iterator>
void serializer<Iterator>::set_iterator(Iterator iter)
{
    iter_ = iter;
}

template <typename Iterator>
template <typename T>
void serializer<Iterator>::write_data_reverse(const T& data)
{
    iter_ = std::reverse_copy(data.begin(), data.end(), iter_);
}

template <typename Iterator>
serializer<Iterator> make_serializer(Iterator begin)
{
    return serializer<Iterator>(begin);
}

// Macro used so that compiler will optimise out function calls to
// check_distance() if SafeCheckLast is false.
#define SAFE_CHECK_DISTANCE(N) \
    if (SafeCheckLast) \
        check_distance(iter_, end_, N);

template <typename Iterator, bool SafeCheckLast>
deserializer<Iterator, SafeCheckLast>::deserializer(
    const Iterator begin, const Iterator end)
  : iter_(begin), end_(end)
{
}

template <typename Iterator, bool SafeCheckLast>
uint8_t deserializer<Iterator, SafeCheckLast>::read_byte()
{
    SAFE_CHECK_DISTANCE(1);
    return *(iter_++);
}
template <typename Iterator, bool SafeCheckLast>
uint16_t deserializer<Iterator, SafeCheckLast>::read_2_bytes()
{
    return read_little_endian<uint16_t>();
}
template <typename Iterator, bool SafeCheckLast>
uint32_t deserializer<Iterator, SafeCheckLast>::read_4_bytes()
{
    return read_little_endian<uint32_t>();
}
template <typename Iterator, bool SafeCheckLast>
uint64_t deserializer<Iterator, SafeCheckLast>::read_8_bytes()
{
    return read_little_endian<uint64_t>();
}

template <typename Iterator, bool SafeCheckLast>
template <typename T>
T deserializer<Iterator, SafeCheckLast>::read_big_endian()
{
    const auto begin = iter_;
    SAFE_CHECK_DISTANCE(sizeof(T));
    iter_ += sizeof(T);
    return from_big_endian_unsafe<T>(begin);
}
template <typename Iterator, bool SafeCheckLast>
template <typename T>
T deserializer<Iterator, SafeCheckLast>::read_little_endian()
{
    const auto begin = iter_;
    SAFE_CHECK_DISTANCE(sizeof(T));
    iter_ += sizeof(T);
    return from_little_endian_unsafe<T>(begin);
}

template <typename Iterator, bool SafeCheckLast>
uint64_t deserializer<Iterator, SafeCheckLast>::read_variable_uint()
{
    uint8_t length = read_byte();
    if (length < 0xfd)
        return length;
    else if (length == 0xfd)
        return read_2_bytes();
    else if (length == 0xfe)
        return read_4_bytes();
    // length should be 0xff
    return read_8_bytes();
}

template <typename Iterator, bool SafeCheckLast>
data_chunk deserializer<
    Iterator, SafeCheckLast>::read_data(size_t n_bytes)
{
    SAFE_CHECK_DISTANCE(n_bytes);
    data_chunk raw_bytes(n_bytes);
    for (size_t i = 0; i < n_bytes; ++i)
        raw_bytes[i] = read_byte();
    return raw_bytes;
}

template <typename Iterator, bool SafeCheckLast>
network_address_type deserializer<
    Iterator, SafeCheckLast>::read_network_address()
{
    network_address_type addr;
    addr.services = read_8_bytes();
    // Read IP address
    addr.ip = read_bytes<16>();
    addr.port = read_big_endian<uint16_t>();
    return addr;
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
std::string deserializer<
    Iterator, SafeCheckLast>::read_fixed_string(size_t len)
{
    data_chunk string_bytes = read_data(len);
    std::string result(string_bytes.begin(), string_bytes.end());
    // Removes trailing 0s... Needed for string comparisons
    return result.c_str();
}

template <typename Iterator, bool SafeCheckLast>
std::string deserializer<Iterator, SafeCheckLast>::read_string()
{
    uint64_t string_size = read_variable_uint();
    // Warning: conversion from uint64_t to size_t, possible loss of data.
    return read_fixed_string((size_t)string_size);
}

template <typename Iterator, bool SafeCheckLast>
template<unsigned N>
byte_array<N> deserializer<Iterator, SafeCheckLast>::read_bytes()
{
    SAFE_CHECK_DISTANCE(N);
    byte_array<N> out;
    std::copy(iter_, iter_ + N, out.begin());
    iter_ += N;
    return out;
}

template <typename Iterator, bool SafeCheckLast>
template<unsigned N>
byte_array<N> deserializer<Iterator, SafeCheckLast>::read_bytes_reverse()
{
    SAFE_CHECK_DISTANCE(N);
    byte_array<N> out;
    std::reverse_copy(iter_, iter_ + N, out.begin());
    iter_ += N;
    return out;
}

/**
 * Returns underlying iterator.
 */
template <typename Iterator, bool SafeCheckLast>
Iterator deserializer<Iterator, SafeCheckLast>::iterator() const
{
    return iter_;
}

/**
 * Useful if you advance the iterator using other serialization
 * methods or objects.
 */
template <typename Iterator, bool SafeCheckLast>
void deserializer<Iterator, SafeCheckLast>::set_iterator(const Iterator iter)
{
    iter_ = iter;
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


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

namespace libbitcoin {

template <typename Iterator>
serializer<Iterator>::serializer(const Iterator begin)
  : iter_(begin)
{
}

template <typename Iterator>
template <typename T>
void serializer<Iterator>::write_uint_auto(T value)
{
    write_data(uncast_type(value));
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
    write_uint_auto(value);
}
template <typename Iterator>
void serializer<Iterator>::write_4_bytes(uint32_t value)
{
    write_uint_auto(value);
}
template <typename Iterator>
void serializer<Iterator>::write_8_bytes(uint64_t value)
{
    write_uint_auto(value);
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
    internal_copy(data.begin(), data.end());
}

template <typename Iterator>
void serializer<Iterator>::write_network_address(network_address_type addr)
{
    write_8_bytes(addr.services);
    write_data(addr.ip);
    write_data(uncast_type(addr.port, true));
}

template <typename Iterator>
void serializer<Iterator>::write_hash(const hash_digest& hash)
{
    write_data_reverse(hash);
}

template <typename Iterator>
void serializer<Iterator>::write_short_hash(const short_hash& hash)
{
    write_data_reverse(hash);
}

template <typename Iterator>
void serializer<Iterator>::write_fixed_string(
    const std::string& command, size_t string_size)
{
    BITCOIN_ASSERT(command.size() <= string_size);
    data_chunk raw_string(string_size);
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
    internal_copy(data.rbegin(), data.rend());
}

// We need to advance the internal iterator.
// std::copy gives no info on length of the data copied.
template <typename Iterator>
template <typename InputIterator>
void serializer<Iterator>::internal_copy(
    InputIterator first, InputIterator last)
{
    while (first != last)
    {
        *iter_ = *first;
        ++first;
        ++iter_;
    }
}

template <typename Iterator>
serializer<Iterator> make_serializer(Iterator begin)
{
    return serializer<Iterator>(begin);
}

template <typename Iterator>
deserializer<Iterator>::deserializer(const Iterator begin, const Iterator end)
  : iter_(begin), end_(end)
{
}

template <typename Iterator>
template <typename T>
const T deserializer<Iterator>::read_uint_auto()
{
    return read_data_impl<T>(iter_, end_);
}

template <typename Iterator>
uint8_t deserializer<Iterator>::read_byte()
{
    check_distance(iter_, end_, 1);
    return *(iter_++);
}
template <typename Iterator>
uint16_t deserializer<Iterator>::read_2_bytes()
{
    return read_uint_auto<uint16_t>();
}
template <typename Iterator>
uint32_t deserializer<Iterator>::read_4_bytes()
{
    return read_uint_auto<uint32_t>();
}
template <typename Iterator>
uint64_t deserializer<Iterator>::read_8_bytes()
{
    return read_uint_auto<uint64_t>();
}

template <typename Iterator>
uint64_t deserializer<Iterator>::read_variable_uint()
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

// NOTE: n_bytes changed to uint32_t to prevent array overflow.
template <typename Iterator>
data_chunk deserializer<Iterator>::read_data(uint32_t n_bytes)
{
    check_distance(iter_, end_, n_bytes);
    data_chunk raw_bytes(n_bytes);
    for (uint32_t i = 0; i < n_bytes; ++i)
        raw_bytes[i] = read_byte();
    return raw_bytes;
}

template <typename Iterator>
network_address_type deserializer<Iterator>::read_network_address()
{
    network_address_type addr;
    addr.services = read_8_bytes();
    // Read IP address
    read_bytes<16>(iter_, end_, addr.ip);
    addr.port = read_data_impl<uint16_t>(iter_, end_, true);
    return addr;
}

template <typename Iterator>
hash_digest deserializer<Iterator>::read_hash()
{
    hash_digest hash;
    read_bytes<32>(iter_, end_, hash, true);
    return hash;
}

template <typename Iterator>
short_hash deserializer<Iterator>::read_short_hash()
{
    short_hash hash;
    read_bytes<20>(iter_, end_, hash, true);
    return hash;
}

template <typename Iterator>
std::string deserializer<Iterator>::read_fixed_string(size_t len)
{
    data_chunk string_bytes = read_data(len);
    std::string result(string_bytes.begin(), string_bytes.end());
    // Removes trailing 0s... Needed for string comparisons
    return result.c_str();
}

template <typename Iterator>
std::string deserializer<Iterator>::read_string()
{
    uint64_t string_size = read_variable_uint();
    // Warning: conversion from uint64_t to size_t, possible loss of data.
    return read_fixed_string((size_t)string_size);
}

/**
 * Returns underlying iterator.
 */
template <typename Iterator>
Iterator deserializer<Iterator>::iterator() const
{
    return iter_;
}

/**
 * Useful if you advance the iterator using other serialization
 * methods or objects.
 */
template <typename Iterator>
void deserializer<Iterator>::set_iterator(const Iterator iter)
{
    iter_ = iter;
}

// Try to advance iterator 'distance' incremenets forwards.
// Throw if we prematurely reach the end.
template <typename Iterator>
void deserializer<Iterator>::check_distance(
    Iterator it, const Iterator end, size_t distance)
{
    for (size_t i = 0; i < distance; ++i)
    {
        // Is this a valid byte?
        if (it == end)
            throw end_of_stream();
        // If so move to next value.
        ++it;
    }
}

template <typename Iterator>
template <typename T>
T deserializer<Iterator>::read_data_impl(
    Iterator& begin, const Iterator end, bool reverse)
{
    check_distance(begin, end, sizeof(T));
    data_chunk chunk(begin, begin + sizeof(T));
    T val = cast_chunk<T>(chunk, reverse);
    begin += sizeof(T);
    return val;
}

template <typename Iterator>
template <unsigned int N>
void deserializer<Iterator>::read_bytes(
    Iterator& begin, const Iterator& end,
    std::array<uint8_t, N>& byte_array, bool reverse)
{
    check_distance(begin, end, byte_array.size());
#ifdef BOOST_LITTLE_ENDIAN
    // do nothing
#elif BOOST_BIG_ENDIAN
    reverse = !reverse;
#else
    #error "Endian isn't defined!"
#endif

    if (reverse)
        std::reverse_copy(
            begin, begin + byte_array.size(), byte_array.begin());
    else
        std::copy(begin, begin + byte_array.size(), byte_array.begin());
    begin += byte_array.size();
}

template <typename Iterator>
deserializer<Iterator> make_deserializer(
    const Iterator begin, const Iterator end)
{
    return deserializer<Iterator>(begin, end);
}

} // libbitcoin

#endif


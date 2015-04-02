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
void serializer<Iterator>::write_network_address(
    const message::network_address& addr)
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

} // libbitcoin

#endif


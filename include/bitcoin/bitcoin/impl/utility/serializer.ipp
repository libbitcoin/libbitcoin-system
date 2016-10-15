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
#ifndef LIBBITCOIN_SERIALIZER_IPP
#define LIBBITCOIN_SERIALIZER_IPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>

namespace libbitcoin {

template <typename Iterator>
serializer<Iterator>::serializer(const Iterator begin)
  : iterator_(begin)
{
}

// Context.
//-----------------------------------------------------------------------------

template <typename Iterator>
serializer<Iterator>::operator bool() const
{
    return true;
}

template <typename Iterator>
bool serializer<Iterator>::operator!() const
{
    return false;
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
    if (value < two_bytes)
    {
        write_byte(static_cast<uint8_t>(value));
    }
    else if (value <= max_uint16)
    {
        write_byte(two_bytes);
        write_2_bytes_big_endian(static_cast<uint16_t>(value));
    }
    else if (value <= max_uint32)
    {
        write_byte(four_bytes);
        write_4_bytes_big_endian(static_cast<uint32_t>(value));
    }
    else
    {
        write_byte(eight_bytes);
        write_8_bytes_big_endian(value);
    }
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
    if (value < two_bytes)
    {
        write_byte(static_cast<uint8_t>(value));
    }
    else if (value <= max_uint16)
    {
        write_byte(two_bytes);
        write_2_bytes_little_endian(static_cast<uint16_t>(value));
    }
    else if (value <= max_uint32)
    {
        write_byte(four_bytes);
        write_4_bytes_little_endian(static_cast<uint32_t>(value));
    }
    else
    {
        write_byte(eight_bytes);
        write_8_bytes_little_endian(value);
    }
}

// Bytes (unchecked).
//-----------------------------------------------------------------------------

template <typename Iterator>
void serializer<Iterator>::write_byte(uint8_t value)
{
    *iterator_ = value;
    skip(1);
}

template <typename Iterator>
void serializer<Iterator>::write_bytes(const data_chunk& data)
{
    write_forward(data);
}

template <typename Iterator>
void serializer<Iterator>::write_bytes(const uint8_t* data, size_t size)
{
    std::copy_n(data, size, iterator_);
    skip(size);
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
    data_chunk padding(floor_subtract(size, length), terminator);
    write_bytes(padding);
}

template <typename Iterator>
void serializer<Iterator>::skip(size_t size)
{
    iterator_ += size;
}

template <typename Iterator>
template <typename Buffer>
void serializer<Iterator>::write_forward(const Buffer& data)
{
    iterator_ = std::copy(data.begin(), data.end(), iterator_);
}

template <typename Iterator>
template <typename Buffer>
void serializer<Iterator>::write_reverse(const Buffer& data)
{
    iterator_ = std::reverse_copy(data.begin(), data.end(), iterator_);
}

template <typename Iterator>
template <typename Integer>
void serializer<Iterator>::write_big_endian(Integer value)
{
    return write_forward(to_big_endian(value));
}

template <typename Iterator>
template <typename Integer>
void serializer<Iterator>::write_little_endian(Integer value)
{
    return write_forward(to_little_endian(value));
}

// Factories.
//-----------------------------------------------------------------------------

template <typename Iterator>
serializer<Iterator> make_unsafe_serializer(Iterator begin)
{
    return serializer<Iterator>(begin);
}

} // namespace libbitcoin

#endif

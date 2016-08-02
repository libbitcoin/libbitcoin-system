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
#ifndef LIBBITCOIN_SERIALIZER_HPP
#define LIBBITCOIN_SERIALIZER_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {

/**
 * Serializer that uses iterators and is oblivious to the underlying
 * container type. Is not threadsafe.
 *
 * Use the helper make_serializer() to construct a serializer without
 * needing to specify the Iterator type.
 *
 * Makes no assumptions about the size of the underlying container type.
 * User is responsible for allocating enough space prior to serialization.
 *
 * @code
 *  data_chunk buffer(8);
 *  auto serial = make_serializer(buffer.begin());
 *  serial.write_8_bytes(110);
 * @endcode
 */
template <typename Iterator>
class serializer
  : public writer
{
public:
    serializer(const Iterator begin);

    operator bool() const;
    bool operator!() const;
    void write_byte(uint8_t value);
    void write_data(const data_chunk& data);
    void write_data(const uint8_t* data, size_t size);
    void write_error_code(const code& ec);
    void write_hash(const hash_digest& hash);
    void write_short_hash(const short_hash& hash);
    void write_mini_hash(const mini_hash& hash);

    // These write data in little endian format:
    void write_2_bytes_little_endian(uint16_t value);
    void write_4_bytes_little_endian(uint32_t value);
    void write_8_bytes_little_endian(uint64_t value);

    /**
     * Variable uints are usually used for sizes.
     * They're encoded using fewer bytes depending on the value itself.
     */
    void write_variable_uint_little_endian(uint64_t value);

    // These write data in big endian format:
    void write_2_bytes_big_endian(uint16_t value);
    void write_4_bytes_big_endian(uint32_t value);
    void write_8_bytes_big_endian(uint64_t value);

    /**
     * Variable uints are usually used for sizes.
     * They're encoded using fewer bytes depending on the value itself.
     */
    void write_variable_uint_big_endian(uint64_t value);

    /**
     * Encodes an unsigned integer in big-endian format.
     */
    template <typename T>
    void write_big_endian(T n);

    /**
     * Encodes an unsigned integer in little-endian format.
     */
    template <typename T>
    void write_little_endian(T n);
    
    /**
     * Encodes a list of bytes.
     */
    template <typename T>
    void write_data(const T& data);

    /**
     * Write a fixed size string padded with zeroes.
     */
    void write_fixed_string(const std::string& value, size_t size);

    /**
     * Write a variable length string.
     */
    void write_string(const std::string& value);

    /**
     * Returns underlying iterator.
     */
    Iterator iterator();

    /**
     * Useful if you want to serialize some data using another
     * routine and then continue with this serializer.
     */
    void set_iterator(Iterator iterator);

private:
    template <typename T>
    void write_data_reverse(const T& data);

    Iterator iterator_;
};

template <typename Iterator>
serializer<Iterator> make_serializer(Iterator begin);

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/serializer.ipp>

#endif

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
#ifndef LIBBITCOIN_SERIALIZER_HPP
#define LIBBITCOIN_SERIALIZER_HPP

#include <boost/asio/streambuf.hpp>
#include <string>
#include <vector>

#include <bitcoin/format.hpp>
#include <bitcoin/primitives.hpp>
#include <bitcoin/types.hpp>

namespace libbitcoin {

size_t variable_uint_size(uint64_t v);

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
{
public:
    serializer(const Iterator begin);

    /**
     * Equivalent to calling write_byte/write_N_bytes depending
     * on the size of the passed value.
     * For instance a uint32_t value is equivalent to write_4_bytes(value).
     */
    template <typename T>
    void write_uint_auto(T value);

    void write_byte(uint8_t value);
    void write_2_bytes(uint16_t value);
    void write_4_bytes(uint32_t value);
    void write_8_bytes(uint64_t value);

    /**
     * Variable uints are usually used for sizes.
     * They're encoded using fewer bytes depending on the value itself.
     */
    void write_variable_uint(uint64_t value);

    template <typename T>
    void write_data(const T& data);
    void write_hash(const hash_digest& hash);
    void write_short_hash(const short_hash& hash);

    void write_network_address(network_address_type addr);

    /**
     * Write a fixed size string padded with zeroes.
     */
    void write_fixed_string(const std::string& command, size_t string_size);

    /**
     * Write a variable length string.
     */
    void write_string(const std::string& str);

    /**
     * Returns underlying iterator.
     */
    Iterator iterator();

    /**
     * Useful if you want to serialize some data using another
     * routine and then continue with this serializer.
     */
    void set_iterator(Iterator iter);

private:
    template <typename T>
    void write_data_reverse(const T& data);

    // We need to advance the internal iterator.
    // std::copy gives no info on length of the data copied.
    template <typename InputIterator>
    void internal_copy(InputIterator first, InputIterator last);

    Iterator iter_;
};

template <typename Iterator>
serializer<Iterator> make_serializer(Iterator begin);

class end_of_stream
  : std::exception {};

/**
 * Deserializer that uses iterators and is oblivious to the underlying
 * container type. Is not threadsafe.
 *
 * Use the helper make_deserializer() to construct a deserializer without
 * needing to specify the Iterator type.
 *
 * Throws end_of_stream exception upon early termination during deserialize.
 * For example, calling read_8_bytes() with only 5 bytes remaining will throw.
 *
 * @code
 *  auto deserial = make_deserializer(data.begin(), data.end());
 *  try {
 *    uint64_t value = deserial.read_8_bytes();
 *  } catch (end_of_stream) {
 *    // ...
 *  }
 * @endcode
 */
template <typename Iterator>
class deserializer
{
public:
    deserializer(const Iterator begin, const Iterator end);

    /**
     * Equivalent to calling read_byte/read_N_bytes depending
     * on the size of the template parameter.
     * For instance a uint32_t parameter is equivalent to read_4_bytes().
     */
    template <typename T>
    const T read_uint_auto();

    uint8_t read_byte();
    uint16_t read_2_bytes();
    uint32_t read_4_bytes();
    uint64_t read_8_bytes();

    /**
     * Variable uints are usually used for sizes.
     * They're encoded using fewer bytes depending on the value itself.
     */
    uint64_t read_variable_uint();

    data_chunk read_data(uint32_t n_bytes);
    hash_digest read_hash();
    short_hash read_short_hash();

    network_address_type read_network_address();

    /**
     * Read a fixed size string padded with zeroes.
     */
    std::string read_fixed_string(size_t len);

    /**
     * Read a variable length string.
     */
    std::string read_string();

    /**
     * Returns underlying iterator.
     */
    Iterator iterator() const;

    /**
     * Useful if you advance the iterator using other serialization
     * methods or objects.
     */
    void set_iterator(const Iterator iter);

private:
    // Try to advance iterator 'distance' incremenets forwards.
    // Throw if we prematurely reach the end.
    static void check_distance(
        Iterator it, const Iterator end, size_t distance);

    template <typename T>
    static T read_data_impl(
        Iterator& begin, const Iterator end, bool reverse=false);

    template <unsigned int N>
    static void read_bytes(Iterator& begin, const Iterator& end,
        std::array<uint8_t, N>& byte_array, bool reverse=false);

    Iterator iter_;
    const Iterator end_;
};

template <typename Iterator>
deserializer<Iterator> make_deserializer(
    const Iterator begin, const Iterator end);

} // namespace libbitcoin

#include <bitcoin/impl/utility/serializer.ipp>

#endif


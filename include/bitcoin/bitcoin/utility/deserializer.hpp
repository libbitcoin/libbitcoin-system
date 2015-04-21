/*
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_DESERIALIZER_HPP
#define LIBBITCOIN_DESERIALIZER_HPP

#include <string>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {

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
template <typename Iterator, bool SafeCheckLast>
class deserializer
{
public:
    deserializer(const Iterator begin, const Iterator end);

    /* These read data in little endian format: */
    uint8_t read_byte();
    uint16_t read_2_bytes();
    uint32_t read_4_bytes();
    uint64_t read_8_bytes();

    /**
     * Reads an unsigned integer that has been encoded in big endian format.
     */
    template <typename T>
    T read_big_endian();

    /**
     * Reads an unsigned integer that has been encoded in little endian format.
     */
    template <typename T>
    T read_little_endian();

    /**
     * Variable uints are usually used for sizes.
     * They're encoded using fewer bytes depending on the value itself.
     */
    uint64_t read_variable_uint();

    data_chunk read_data(size_t n_bytes);
    hash_digest read_hash();
    short_hash read_short_hash();

    /**
     * Read a fixed size string padded with zeroes.
     */
    std::string read_fixed_string(size_t len);

    /**
     * Read a variable length string.
     */
    std::string read_string();

    /**
     * Read a fixed-length data block.
     */
    template<unsigned N>
    byte_array<N> read_bytes();

    template<unsigned N>
    byte_array<N> read_bytes_reverse();

    /**
     * Returns underlying iterator.
     */
    Iterator iterator() const;

    /**
     * Useful if you advance the iterator using other serialization
     * methods or objects.
     */
    void set_iterator(const Iterator iter);

    /**
     * Returns underlying iterator end.
     */
    Iterator end() const
    {
        return end_;
    }

private:
    // The compiler will optimise out all calls to this function
    // if SafeCheckLast is false.
    static void check_distance(
        Iterator it, const Iterator end, size_t distance);

    Iterator iter_;
    const Iterator end_;
};

/**
 * Deserializer which performs bounds checking and throws end_of_stream
 * if the iterator exceeds 'end'.
 */
template <typename Iterator>
deserializer<Iterator, true> make_deserializer(
    const Iterator begin, const Iterator end);

/**
 * Faster deserializer with no bounds checking.
 */
template <typename Iterator>
deserializer<Iterator, false> make_deserializer_unsafe(
    const Iterator begin);

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/deserializer.ipp>

#endif


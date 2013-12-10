/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
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
    serializer(const Iterator begin)
      : iter_(begin) {}

    void write_byte(uint8_t value)
    {
        *iter_ = value;
        ++iter_;
    }
    void write_2_bytes(uint16_t value)
    {
        write_data_impl(value);
    }
    void write_4_bytes(uint32_t value)
    {
        write_data_impl(value);
    }
    void write_8_bytes(uint64_t value)
    {
        write_data_impl(value);
    }

    void write_variable_uint(uint64_t value)
    {
        if (value < 0xfd)
        {
            write_byte(value);
        }
        else if (value <= 0xffff)
        {
            write_byte(0xfd);
            write_2_bytes(value);
        }
        else if (value <= 0xffffffff)
        {
            write_byte(0xfe);
            write_4_bytes(value);
        }
        else
        {
            write_byte(0xff);
            write_8_bytes(value);
        }
    }

    template <typename T>
    void write_data(const T& data)
    {
        copy(data.begin(), data.end());
    }

    void write_network_address(network_address_type addr)
    {
        write_8_bytes(addr.services);
        write_data(addr.ip);
        write_data(uncast_type(addr.port, true));
    }

    void write_hash(const hash_digest& hash)
    {
        write_data_reverse(hash);
    }

    void write_short_hash(const short_hash& hash)
    {
        write_data_reverse(hash);
    }

    void write_fixed_string(const std::string& command, size_t string_size)
    {
        BITCOIN_ASSERT(command.size() <= string_size);
        data_chunk raw_string(string_size);
        std::copy(command.begin(), command.end(), raw_string.begin());
        write_data(raw_string);
    }

    void write_string(const std::string& str)
    {
        write_variable_uint(str.size());
        write_data(str);
    }

    /**
     * Returns underlying iterator.
     */
    Iterator iterator()
    {
        return iter_;
    }

    /**
     * Useful if you want to serialize some data using another
     * routine and then continue with this serializer.
     */
    void set_iterator(Iterator iter)
    {
        iter_ = iter;
    }

private:
    template <typename T>
    void write_data_impl(T value)
    {
        write_data(uncast_type(value));
    }

    template <typename T>
    void write_data_reverse(const T& data)
    {
        copy(data.rbegin(), data.rend());
    }

    template <typename InputIterator>
    void copy(InputIterator first, InputIterator last)
    {
        while (first != last)
        {
            *iter_ = *first;
            ++first;
            ++iter_;
        }
    }

    Iterator iter_;
};

template <typename Iterator>
serializer<Iterator> make_serializer(Iterator begin)
{
    return serializer<Iterator>(begin);
}

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
    deserializer(const Iterator begin, const Iterator end)
      : iter_(begin), end_(end) {}

    uint8_t read_byte()
    {
        check_distance(iter_, end_, 1);
        return *(iter_++);
    }
    uint16_t read_2_bytes()
    {
        return read_data_impl<uint16_t>(iter_, end_);
    }
    uint32_t read_4_bytes()
    {
        return read_data_impl<uint32_t>(iter_, end_);
    }
    uint64_t read_8_bytes()
    {
        return read_data_impl<uint64_t>(iter_, end_);
    }

    uint64_t read_variable_uint()
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

    data_chunk read_data(uint64_t n_bytes)
    {
        check_distance(iter_, end_, n_bytes);
        data_chunk raw_bytes(n_bytes);
        for (uint64_t i = 0; i < n_bytes; ++i)
            raw_bytes[i] = read_byte();
        return raw_bytes;
    }

    network_address_type read_network_address()
    {
        network_address_type addr;
        addr.services = read_8_bytes();
        // Read IP address
        read_bytes<16>(iter_, end_, addr.ip);
        addr.port = read_data_impl<uint16_t>(iter_, end_, true);
        return addr;
    }

    hash_digest read_hash()
    {
        hash_digest hash;
        read_bytes<32>(iter_, end_, hash, true);
        return hash;
    }

    short_hash read_short_hash()
    {
        short_hash hash;
        read_bytes<20>(iter_, end_, hash, true);
        return hash;
    }

    std::string read_fixed_string(size_t len)
    {
        data_chunk string_bytes = read_data(len);
        std::string result(string_bytes.begin(), string_bytes.end());
        // Removes trailing 0s... Needed for string comparisons
        return result.c_str();
    }

    std::string read_string()
    {
        uint64_t string_size = read_variable_uint();
        return read_fixed_string(string_size);
    }

    /**
     * Returns underlying iterator.
     */
    Iterator iterator() const
    {
        return iter_;
    }

    /**
     * Useful if you advance the iterator using other serialization
     * methods or objects.
     */
    void set_iterator(const Iterator iter)
    {
        iter_ = iter;
    }

private:
    // Try to advance iterator 'distance' incremenets forwards.
    // Throw if we prematurely reach the end.
    static void check_distance(
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

    template <typename T>
    static T read_data_impl(
        Iterator& begin, const Iterator end, bool reverse=false)
    {
        check_distance(begin, end, sizeof(T));
        data_chunk chunk(begin, begin + sizeof(T));
        T val = cast_chunk<T>(chunk, reverse);
        begin += sizeof(T);
        return val;
    }

    template <unsigned int N>
    static void read_bytes(Iterator& begin, const Iterator& end,
        std::array<uint8_t, N>& byte_array, bool reverse=false)
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

    Iterator iter_;
    const Iterator end_;
};

template <typename Iterator>
deserializer<Iterator> make_deserializer(
    const Iterator begin, const Iterator end)
{
    return deserializer<Iterator>(begin, end);
}

} // namespace libbitcoin

#endif


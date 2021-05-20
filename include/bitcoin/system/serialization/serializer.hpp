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
#ifndef LIBBITCOIN_SYSTEM_SERIALIZATION_SERIALIZER_HPP
#define LIBBITCOIN_SYSTEM_SERIALIZATION_SERIALIZER_HPP

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <bitcoin/system/error.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/utility/data.hpp>
////#include <bitcoin/system/utility/noncopyable.hpp>
#include <bitcoin/system/utility/reader.hpp>
#include <bitcoin/system/utility/writer.hpp>

namespace libbitcoin {
namespace system {

/// Writer to wrap arbitrary iterator.
template <typename Iterator>
class serializer
  : public writer/*, noncopyable*/
{
public:
    typedef std::function<void(serializer<Iterator>&)> functor;

    serializer(const Iterator begin);

    template <typename Tuple>
    void write_forward(const Tuple& data);

    template <typename Tuple>
    void write_reverse(const Tuple& data);

    template <typename Integer>
    void write_big_endian(Integer value);

    template <typename Integer>
    void write_little_endian(Integer value);

    /// Context.
    operator bool() const;
    bool operator!() const;

    /// Write hashes.
    void write_hash(const hash_digest& hash);
    void write_short_hash(const short_hash& hash);
    void write_mini_hash(const mini_hash& hash);

    /// Write big endian integers.
    void write_2_bytes_big_endian(uint16_t value);
    void write_4_bytes_big_endian(uint32_t value);
    void write_8_bytes_big_endian(uint64_t value);
    void write_variable_big_endian(uint64_t value);
    void write_size_big_endian(size_t value);

    /// Write little endian integers.
    void write_error_code(const code& ec);
    void write_2_bytes_little_endian(uint16_t value);
    void write_4_bytes_little_endian(uint32_t value);
    void write_8_bytes_little_endian(uint64_t value);
    void write_variable_little_endian(uint64_t value);
    void write_size_little_endian(size_t value);

    /// Write until reader is exhausted.
    void write(reader& in);

    /// Write one byte.
    void write_byte(uint8_t value);

    /// Write all bytes.
    void write_bytes(const data_slice& data);

    /// Write required size buffer.
    void write_bytes(const uint8_t* data, size_t size);

    /// Write variable length string.
    void write_string(const std::string& value);

    /// Write required length string, padded with nulls.
    void write_string(const std::string& value, size_t size);

    /// Advance iterator without writing.
    void skip(size_t size);

    // non-interface
    //-------------------------------------------------------------------------

    /// Delegate write to a write function.
    void write_delegated(functor write);

    /// Utility for variable skipping of writer.
    size_t read_size_big_endian();

    /// Utility for variable skipping of writer.
    size_t read_size_little_endian();

private:
    bool valid_;
    Iterator iterator_;
};

// Factories.
//-----------------------------------------------------------------------------

template <typename Iterator>
serializer<Iterator> make_unsafe_serializer(Iterator begin);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/utility/serializer.ipp>

#endif

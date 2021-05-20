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
#ifndef LIBBITCOIN_SYSTEM_SERIALIZATION_DESERIALIZER_HPP
#define LIBBITCOIN_SYSTEM_SERIALIZATION_DESERIALIZER_HPP

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <bitcoin/system/error.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/data/data.hpp>
////#include <bitcoin/system/noncopyable.hpp>
#include <bitcoin/system/iostream/reader.hpp>

namespace libbitcoin {
namespace system {

/// Reader to wrap arbitrary iterator.
template <typename Iterator, bool CheckSafe>
class deserializer
  : public reader/*, noncopyable*/
{
public:
    typedef std::function<void(deserializer<Iterator, CheckSafe>&)> functor;

    deserializer(const Iterator begin);
    deserializer(const Iterator begin, const Iterator end);

    ////// TODO: enable complex object store key types.
    ////template <typename OutputIterator>
    ////OutputIterator read();

    template <typename Tuple>
    Tuple read_forward();

    template <typename Tuple>
    Tuple read_reverse();

    template <typename Integer>
    Integer read_big_endian();

    template <typename Integer>
    Integer read_little_endian();

    /// Context.
    operator bool() const;
    bool operator!() const;
    bool is_exhausted() const;
    void invalidate();

    /// Read hashes.
    hash_digest read_hash();
    short_hash read_short_hash();
    mini_hash read_mini_hash();

    /// Read big endian integers.
    uint16_t read_2_bytes_big_endian();
    uint32_t read_4_bytes_big_endian();
    uint64_t read_8_bytes_big_endian();
    uint64_t read_variable_big_endian();
    size_t read_size_big_endian();

    /// Read little endian integers.
    code read_error_code();
    uint16_t read_2_bytes_little_endian();
    uint32_t read_4_bytes_little_endian();
    uint64_t read_8_bytes_little_endian();
    uint64_t read_variable_little_endian();
    size_t read_size_little_endian();

    /// Read/peek one byte.
    uint8_t peek_byte();
    uint8_t read_byte();

    /// Read all remaining bytes (always safe).
    data_chunk read_bytes();

    /// Read required size buffer.
    data_chunk read_bytes(size_t size);

    /// Read variable length string.
    std::string read_string();

    /// Read required size string and trim nulls.
    std::string read_string(size_t size);

    /// Advance iterator without reading.
    void skip(size_t size);

private:
    // True if is a safe deserializer and size does not exceed remaining bytes.
    bool safe(size_t size) const;

    // The number of bytes remaining in the buffer.
    size_t remaining() const;

    bool valid_;
    Iterator iterator_;
    const Iterator end_;
};

// Factories.
//-----------------------------------------------------------------------------

/// Slower deserializer (with bounds checking).
/// Safe for use with public data, caller should check object state.
template <typename Iterator>
deserializer<Iterator, true> make_safe_deserializer(const Iterator begin,
    const Iterator end);

/// Faster deserializer (without bounds checking).
/// Intended for use with internal/protected buffers only.
template <typename Iterator>
deserializer<Iterator, false> make_unsafe_deserializer(const Iterator begin);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/serialization/deserializer.ipp>

#endif

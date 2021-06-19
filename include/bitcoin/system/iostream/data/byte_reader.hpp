/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_DATA_BYTE_READER_HPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_DATA_BYTE_READER_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <string>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/data/data_slice.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {
    
/// A byte reader that accepts an istream.
class BC_API byte_reader
{
public:
    /// Constructors.
    byte_reader(std::istream& source);
    virtual ~byte_reader();

    /// Read integer, size determined from paramter type.
    template <typename Integer, if_integer<Integer> = true>
    Integer read_big_endian();
    template <typename Integer, if_integer<Integer> = true>
    Integer read_little_endian();

    /// Read big endian (explicit specializations of read_big_endian).
    virtual uint16_t read_2_bytes_big_endian();
    virtual uint32_t read_4_bytes_big_endian();
    virtual uint64_t read_8_bytes_big_endian();

    /// Little endian integer readers (specializations of read_little_endian).
    virtual uint16_t read_2_bytes_little_endian();
    virtual uint32_t read_4_bytes_little_endian();
    virtual uint64_t read_8_bytes_little_endian();

    /// Read Bitcoin variable integer (3, 5, or 9 bytes, little-endian).
    virtual uint64_t read_variable();

    /// Cast read_variable to size_t, facilitates read_bytes(read_size()).
    /// Returns zero and invalidates stream if would overflow size_t.
    virtual size_t read_size();

    /// Convert read_4_bytes_little_endian to an error code.
    virtual code read_error_code();

    /// Read size bytes into array.
    template <size_t Size>
    byte_array<Size> read_forward();
    template <size_t Size>
    byte_array<Size> read_reverse();

    /// Read hash (explicit specializations of read_forward).
    virtual mini_hash read_mini_hash();
    virtual short_hash read_short_hash();
    virtual hash_digest read_hash();
    virtual long_hash read_long_hash();

    /// Read/peek one byte (invalidates an empty stream).
    virtual uint8_t peek_byte();
    virtual uint8_t read_byte();

    /// Read all remaining bytes.
    virtual data_chunk read_bytes();

    /// Read size bytes, return size is guaranteed.
    virtual data_chunk read_bytes(size_t size);
    virtual void read_bytes(uint8_t* buffer, size_t size);

    /// Read Bitcoin length-prefixed string, same as read_string(read_size()).
    virtual std::string read_string();

    /// Read string, truncated at at size or first null.
    virtual std::string read_string(size_t size);

    /// Advance the iterator.
    virtual void skip(size_t size);

    /// The stream is empty (or invalid).
    virtual bool is_exhausted() const;

    /// Invalidate the stream.
    virtual void invalidate();

    /// The stream is valid.
    virtual operator bool() const;

    /// The stream is invalid.
    virtual bool operator!() const;

protected:
    virtual uint8_t do_peek();
    virtual uint8_t do_read();
    virtual void do_read(uint8_t* buffer, size_t size);
    virtual bool get_valid() const;
    virtual bool get_exhausted() const;
    virtual void set_invalid();

private:
    std::istream& stream_;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/iostream/data/byte_reader.ipp>

#endif

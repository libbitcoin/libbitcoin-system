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
#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_DATA_BYTE_WRITER_HPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_DATA_BYTE_WRITER_HPP

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/data/data_slice.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error.hpp>
#include <bitcoin/system/iostream/data/byte_reader.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

/// A byte writer that accepts an ostream.
class BC_API byte_writer
{
public:
    /// Constructors.
    byte_writer(std::ostream& stream);
    virtual ~byte_writer();

    /// Type-inferenced integer writers.
    template <typename Integer, if_integer<Integer> = true>
    void write_big_endian(Integer value);
    template <typename Integer, if_integer<Integer> = true>
    void write_little_endian(Integer value);

    /// Write big endian integers.
    virtual void write_2_bytes_big_endian(uint16_t value);
    virtual void write_4_bytes_big_endian(uint32_t value);
    virtual void write_8_bytes_big_endian(uint64_t value);

    /// Write little endian integers.
    virtual void write_2_bytes_little_endian(uint16_t value);
    virtual void write_4_bytes_little_endian(uint32_t value);
    virtual void write_8_bytes_little_endian(uint64_t value);

    /// Write Bitcoin variable integer (3, 5, or 9 bytes, little-endian).
    virtual void write_variable(uint64_t value);

    /// Call write_4_bytes_little_endian with integer value of error code.
    virtual void write_error_code(const code& ec);

    /// Write until reader is exhausted.
    virtual void write(byte_reader& in);

    /// Write one byte.
    virtual void write_byte(uint8_t value);

    /// Write all bytes.
    virtual void write_bytes(const data_slice& data);

    /// Write size bytes.
    virtual void write_bytes(const uint8_t* data, size_t size);

    /// Write Bitcoin length-prefixed string (prefixed by write_variable).
    virtual void write_string(const std::string& value, size_t size);

    /// Write string to specified length, padded with nulls as required.
    virtual void write_string(const std::string& value);

    /// Advance iterator by writing nulls.
    virtual void skip(size_t size);

    /// Flush the buffer.
    virtual void flush();

    /// The stream is valid.
    virtual operator bool() const;

    /// The stream is invalid.
    virtual bool operator!() const;

protected:
    virtual void do_write(uint8_t byte);
    virtual void do_write(const uint8_t* data, size_t size);
    virtual void do_skip(size_t size);
    virtual void do_flush();
    virtual bool get_valid() const;

private:
    std::ostream& stream_;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/iostream/data/byte_writer.ipp>

#endif

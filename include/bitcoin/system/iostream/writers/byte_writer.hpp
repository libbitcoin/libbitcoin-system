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
#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_WRITERS_BYTE_WRITER_HPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_WRITERS_BYTE_WRITER_HPP

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error.hpp>
#include <bitcoin/system/iostream/readers/byte_reader.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

/// A byte writer that accepts an ostream.
class BC_API byte_writer
{
public:
    /// Constructors.
    byte_writer(std::ostream& stream) noexcept;
    virtual ~byte_writer() noexcept;

    /// Type-inferenced integer writers.
    template <typename Integer, if_integer<Integer> = true>
    void write_big_endian(Integer value) noexcept;
    template <typename Integer, if_integer<Integer> = true>
    void write_little_endian(Integer value) noexcept;

    /// Write big endian integers.
    virtual void write_2_bytes_big_endian(uint16_t value) noexcept;
    virtual void write_4_bytes_big_endian(uint32_t value) noexcept;
    virtual void write_8_bytes_big_endian(uint64_t value) noexcept;

    /// Write little endian integers.
    virtual void write_2_bytes_little_endian(uint16_t value) noexcept;
    virtual void write_4_bytes_little_endian(uint32_t value) noexcept;
    virtual void write_8_bytes_little_endian(uint64_t value) noexcept;

    /// Write Bitcoin variable integer (3, 5, or 9 bytes, little-endian).
    virtual void write_variable(uint64_t value) noexcept;

    /// Call write_4_bytes_little_endian with integer value of error code.
    virtual void write_error_code(const code& ec) noexcept;

    /// Write until reader is exhausted.
    virtual void write(byte_reader& in) noexcept;

    /// Write one byte.
    virtual void write_byte(uint8_t value) noexcept;

    /// Write all bytes.
    virtual void write_bytes(const data_slice& data) noexcept;

    /// Write size bytes.
    virtual void write_bytes(const uint8_t* data, size_t size) noexcept;

    /// Write Bitcoin length-prefixed string (prefixed by write_variable).
    virtual void write_string(const std::string& value, size_t size) noexcept;

    /// Write string to specified length, padded with nulls as required.
    virtual void write_string(const std::string& value) noexcept;

    /// Advance iterator by writing nulls.
    virtual void skip(size_t size) noexcept;

    /// Flush the buffer.
    virtual void flush() noexcept;

    /// The stream is valid.
    virtual operator bool() const noexcept;

    /// The stream is invalid.
    virtual bool operator!() const noexcept;

protected:
    virtual void do_write(uint8_t byte) noexcept;
    virtual void do_write(const uint8_t* data, size_t size) noexcept;
    virtual void do_skip(size_t size) noexcept;
    virtual void do_flush() noexcept;
    virtual bool get_valid() const noexcept;

private:
    std::ostream& stream_;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/iostream/writers/byte_writer.ipp>

#endif

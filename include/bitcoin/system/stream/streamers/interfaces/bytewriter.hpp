/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_INTERFACES_BYTEWRITER_HPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_INTERFACES_BYTEWRITER_HPP

#include <cstddef>
#include <cstdint>
#include <string>
// DELETEMENOW
// DELETEMENOW
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>

namespace libbitcoin {
namespace system {
    
/// A byte writer interface.
class bytewriter
{
public:
    /// Write big endian integers.
    virtual void write_2_bytes_big_endian(uint16_t value) NOEXCEPT = 0;
    virtual void write_4_bytes_big_endian(uint32_t value) NOEXCEPT = 0;
    virtual void write_8_bytes_big_endian(uint64_t value) NOEXCEPT = 0;

    /// Write little endian integers.
    virtual void write_2_bytes_little_endian(uint16_t value) NOEXCEPT = 0;
    virtual void write_4_bytes_little_endian(uint32_t value) NOEXCEPT = 0;
    virtual void write_8_bytes_little_endian(uint64_t value) NOEXCEPT = 0;

    /// Write Bitcoin variable integer (1, 3, 5, or 9 bytes, little-endian).
    virtual void write_variable(uint64_t value) NOEXCEPT = 0;

    /// Call write_4_bytes_little_endian with integer value of error code.
    virtual void write_error_code(const code& ec) NOEXCEPT = 0;

    /// Write until reader is exhausted.
    virtual std::istream& write(std::istream& in) NOEXCEPT = 0;

    /// Write one byte.
    virtual void write_byte(uint8_t value) NOEXCEPT = 0;

    /// Write all bytes.
    virtual void write_bytes(const data_slice& data) NOEXCEPT = 0;

    /// Write size bytes.
    virtual void write_bytes(const uint8_t* data, size_t size) NOEXCEPT = 0;

    /// Write Bitcoin length-prefixed string (prefixed by write_variable).
    virtual void write_string(const std::string& value) NOEXCEPT = 0;

    /// This is only used for writing Bitcoin command command text.
    /// Write string to specified length, padded with nulls as required.
    virtual void write_string_buffer(const std::string& value, size_t size)
        NOEXCEPT = 0;

    /// Flush the buffer.
    virtual void flush() NOEXCEPT = 0;

    /// Get the current absolute position (fails with push_sink).
    virtual size_t get_position() NOEXCEPT = 0;

    /// The stream is valid.
    virtual operator bool() const NOEXCEPT = 0;

    /// The stream is invalid.
    virtual bool operator!() const NOEXCEPT = 0;
};

} // namespace system
} // namespace libbitcoin

#endif

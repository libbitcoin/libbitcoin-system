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
#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BYTE_WRITER_HPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BYTE_WRITER_HPP

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/error.hpp>
#include <bitcoin/system/stream/streamers/interfaces/bytewriter.hpp>

namespace libbitcoin {
namespace system {
    
/// A byte writer that accepts an ostream.
template <typename OStream = std::ostream>
class byte_writer
  : public virtual bytewriter
{
public:
    /// Constructors.
    byte_writer(OStream& sink) noexcept;
    virtual ~byte_writer() noexcept;

    /// Type-inferenced integer writers.
    template <typename Integer, if_integer<Integer> = true>
    void write_big_endian(Integer value) noexcept;
    template <typename Integer, if_integer<Integer> = true>
    void write_little_endian(Integer value) noexcept;

    /// Write big endian integers.
    void write_2_bytes_big_endian(uint16_t value) noexcept override;
    void write_4_bytes_big_endian(uint32_t value) noexcept override;
    void write_8_bytes_big_endian(uint64_t value) noexcept override;

    /// Write little endian integers.
    void write_2_bytes_little_endian(uint16_t value) noexcept override;
    void write_4_bytes_little_endian(uint32_t value) noexcept override;
    void write_8_bytes_little_endian(uint64_t value) noexcept override;

    /// Write Bitcoin variable integer (3, 5, or 9 bytes, little-endian).
    void write_variable(uint64_t value) noexcept override;

    /// Call write_4_bytes_little_endian with integer value of error code.
    void write_error_code(const code& ec) noexcept override;

    /// Write into buffer until stream is exhausted.
    std::istream& write(std::istream& in) noexcept override;

    /// Write one byte.
    void write_byte(uint8_t value) noexcept override;

    /// Write all bytes.
    void write_bytes(const data_slice& data) noexcept override;

    /// Write size bytes.
    void write_bytes(const uint8_t* data, size_t size) noexcept override;

    /// Write string to specified length, padded with nulls as required.
    void write_string(const std::string& value) noexcept override;

    /// Write Bitcoin length-prefixed string (prefixed by write_variable).
    void write_string(const std::string& value, size_t size) noexcept override;

    /// Flush the buffer.
    void flush() noexcept override;

    /// The stream is valid.
    operator bool() const noexcept override;

    /// The stream is invalid.
    bool operator!() const noexcept override;

protected:
    virtual void do_write_bytes(const uint8_t* data, size_t size) noexcept;
    virtual void do_flush() noexcept;

private:
    static const uint8_t pad;

    bool valid() const noexcept;
    void invalid() noexcept;
    void validate() noexcept;
    void flusher() noexcept;

    OStream& stream_;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/stream/streamers/byte_writer.ipp>

#endif

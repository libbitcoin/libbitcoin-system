/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/stream/streamers/interfaces/bytewriter.hpp>

namespace libbitcoin {
namespace system {
    
/// A byte writer that accepts an ostream.
template <typename OStream = std::ostream>
class byte_writer
  : public virtual bytewriter
{
public:
    DEFAULT_COPY_MOVE(byte_writer);

    /// Constructors.
    byte_writer(OStream& sink) NOEXCEPT;

    /// Flush on destruct.
    virtual ~byte_writer() NOEXCEPT;

    /// Integrals.
    /// -----------------------------------------------------------------------

    /// Type-inferenced integer writers.
    template <typename Integer, size_t Size = sizeof(Integer),
        if_integer<Integer> = true,
        if_not_greater<Size, sizeof(Integer)> = true>
    void write_big_endian(Integer value) NOEXCEPT;
    template <typename Integer, size_t Size = sizeof(Integer),
        if_integer<Integer> = true,
        if_not_greater<Size, sizeof(Integer)> = true>
    void write_little_endian(Integer value) NOEXCEPT;

    /// Write big endian integers.
    void write_2_bytes_big_endian(uint16_t value) NOEXCEPT override;
    void write_3_bytes_big_endian(uint32_t value) NOEXCEPT override;
    void write_4_bytes_big_endian(uint32_t value) NOEXCEPT override;
    void write_5_bytes_big_endian(uint64_t value) NOEXCEPT override;
    void write_6_bytes_big_endian(uint64_t value) NOEXCEPT override;
    void write_7_bytes_big_endian(uint64_t value) NOEXCEPT override;
    void write_8_bytes_big_endian(uint64_t value) NOEXCEPT override;

    /// Write little endian integers.
    void write_2_bytes_little_endian(uint16_t value) NOEXCEPT override;
    void write_3_bytes_little_endian(uint32_t value) NOEXCEPT override;
    void write_4_bytes_little_endian(uint32_t value) NOEXCEPT override;
    void write_5_bytes_little_endian(uint64_t value) NOEXCEPT override;
    void write_6_bytes_little_endian(uint64_t value) NOEXCEPT override;
    void write_7_bytes_little_endian(uint64_t value) NOEXCEPT override;
    void write_8_bytes_little_endian(uint64_t value) NOEXCEPT override;

    /// Write Bitcoin variable integer (1, 3, 5, or 9 bytes, little-endian).
    void write_variable(uint64_t value) NOEXCEPT override;

    /// Call write_4_bytes_little_endian with integer value of error code.
    void write_error_code(const code& ec) NOEXCEPT override;

    /// Write one byte.
    void write_byte(uint8_t value) NOEXCEPT override;

    /// Buffers.
    /// -----------------------------------------------------------------------

    /// Write all bytes.
    void write_bytes(const data_slice& data) NOEXCEPT override;

    /// Write size bytes.
    void write_bytes(const uint8_t* data, size_t size) NOEXCEPT override;

    /// Strings.
    /// -----------------------------------------------------------------------

    /// Write Bitcoin length-prefixed string (prefixed by write_variable).
    void write_string(const std::string& value) NOEXCEPT override;

    /// This is only used for writing Bitcoin command command text.
    /// Write string to specified length, padded with nulls as required.
    void write_string_buffer(const std::string& value, size_t size) NOEXCEPT
        override;

    /// Write value and specified terminator.
    void write_line(const std::string& value="",
        const std::string& end="\r\n") NOEXCEPT override;

    /// Streams.
    /// -----------------------------------------------------------------------

    /// Write into buffer until stream is exhausted.
    std::istream& write(std::istream& in) NOEXCEPT override;

    /// Control.
    /// -----------------------------------------------------------------------

    /// Flush the buffer.
    void flush() NOEXCEPT override;

    /// Get the current absolute position (fails with push_sink).
    size_t get_write_position() NOEXCEPT override;

    /// The stream is valid.
    operator bool() const NOEXCEPT override;

    /// The stream is invalid.
    bool operator!() const NOEXCEPT override;

protected:
    static constexpr uint8_t pad() { return 0x00; };

    // The maximum addressable stream position.
    // Should be defined on OStream::pos_type, however that is implementation
    // defined and is not an integer domain. Could rely on std::streamsize,
    // however this exceeds max_size_t in 32 bit, so limit to max_size_t.
    static constexpr size_t maximum = system::maximum<size_t>;

    virtual void do_write_bytes(const uint8_t* data, size_t size) NOEXCEPT;
    virtual void do_flush() NOEXCEPT;

private:
    bool valid() const NOEXCEPT;
    void invalid() NOEXCEPT;
    void validate() NOEXCEPT;
    void flusher() NOEXCEPT;
    size_t getter() NOEXCEPT;

    OStream& stream_;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/stream/streamers/byte_writer.ipp>

#endif

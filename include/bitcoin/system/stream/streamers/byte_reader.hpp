/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BYTE_READER_HPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BYTE_READER_HPP

#include <iostream>
#include <limits>
#include <string>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/streamers/interfaces/bytereader.hpp>

namespace libbitcoin {
namespace system {
    
/// A byte reader that accepts an istream.
template <typename IStream = std::istream>
class byte_reader
  : public virtual bytereader
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(byte_reader);

    /// Constructors.
    byte_reader(IStream& source) NOEXCEPT;

    /// Read integer, size determined from parameter type.
    template <typename Integer, size_t Size = sizeof(Integer),
        if_integer<Integer> = true,
        if_not_greater<Size, sizeof(Integer)> = true>
    Integer read_big_endian() NOEXCEPT;
    template <typename Integer, size_t Size = sizeof(Integer),
        if_integer<Integer> = true,
        if_not_greater<Size, sizeof(Integer)> = true>
    Integer read_little_endian() NOEXCEPT;

    /// Read big endian (explicit specializations of read_big_endian).
    uint16_t read_2_bytes_big_endian() NOEXCEPT override;
    uint32_t read_3_bytes_big_endian() NOEXCEPT override;
    uint32_t read_4_bytes_big_endian() NOEXCEPT override;
    uint64_t read_5_bytes_big_endian() NOEXCEPT override;
    uint64_t read_6_bytes_big_endian() NOEXCEPT override;
    uint64_t read_7_bytes_big_endian() NOEXCEPT override;
    uint64_t read_8_bytes_big_endian() NOEXCEPT override;

    /// Little endian integer readers (specializations of read_little_endian).
    uint16_t read_2_bytes_little_endian() NOEXCEPT override;
    uint32_t read_3_bytes_little_endian() NOEXCEPT override;
    uint32_t read_4_bytes_little_endian() NOEXCEPT override;
    uint64_t read_5_bytes_little_endian() NOEXCEPT override;
    uint64_t read_6_bytes_little_endian() NOEXCEPT override;
    uint64_t read_7_bytes_little_endian() NOEXCEPT override;
    uint64_t read_8_bytes_little_endian() NOEXCEPT override;

    /// Read Bitcoin variable integer (1, 3, 5, or 9 bytes, little-endian).
    uint64_t read_variable() NOEXCEPT override;

    /// Cast read_variable to size_t, facilitates read_bytes(read_size()).
    /// Returns zero and invalidates stream if would exceed read limit.
    size_t read_size(size_t limit=max_size_t) NOEXCEPT override;

    /// Convert read_4_bytes_little_endian to an error code.
    code read_error_code() NOEXCEPT override;

    /// Read size bytes into array.
    template <size_t Size>
    data_array<Size> read_forward() NOEXCEPT;
    template <size_t Size>
    data_array<Size> read_reverse() NOEXCEPT;

    /// Read into stream until buffer is exhausted.
    std::ostream& read(std::ostream& out) NOEXCEPT override;

    /// Read hash (explicit specializations of read_forward).
    mini_hash read_mini_hash() NOEXCEPT override;
    short_hash read_short_hash() NOEXCEPT override;
    hash_digest read_hash() NOEXCEPT override;
    long_hash read_long_hash() NOEXCEPT override;

    /// Read/peek one byte (invalidates an empty stream).
    uint8_t peek_byte() NOEXCEPT override;
    uint8_t read_byte() NOEXCEPT override;

    /// Read all remaining bytes.
    data_chunk read_bytes() NOEXCEPT override;

    /// Read size bytes, return size is guaranteed.
    data_chunk read_bytes(size_t size) NOEXCEPT override;
    void read_bytes(uint8_t* buffer, size_t size) NOEXCEPT override;

    /// Read Bitcoin length-prefixed string.
    /// Returns empty and invalidates stream if would exceed read limit.
    std::string read_string(size_t limit=max_size_t) NOEXCEPT override;

    /// Read string, truncated at size or first null.
    /// This is only used for reading Bitcoin heading command text.
    std::string read_string_buffer(size_t size) NOEXCEPT override;

    /// Advance the iterator.
    void skip_byte() NOEXCEPT override;
    void skip_bytes(size_t size) NOEXCEPT override;

    /// Read one byte and advance the iterator accordingly.
    void skip_variable() NOEXCEPT override;

    /// Rewind the iterator.
    void rewind_byte() NOEXCEPT override;
    void rewind_bytes(size_t size) NOEXCEPT override;

    /// The stream is empty (or invalid).
    bool is_exhausted() const NOEXCEPT override;

    /// Get the current absolute position.
    size_t get_read_position() NOEXCEPT override;

    /// Clear invalid state and set absolute position.
    void set_position(size_t absolute) NOEXCEPT override;

    /// Limit stream upper bound to current position plus size (default resets).
    void set_limit(size_t size=max_size_t) NOEXCEPT override;

    /// Invalidate the stream.
    void invalidate() NOEXCEPT override;

    /// The stream is valid.
    operator bool() const NOEXCEPT override;

    /// The stream is invalid.
    bool operator!() const NOEXCEPT override;

protected:
    static constexpr uint8_t pad() { return 0x00; };

    // The maximum addressable stream position.
    // Should be defined on IStream::pos_type, however that is implementation
    // defined and is not an integer domain. Could rely on std::streamsize,
    // however this exceeds max_size_t in 32 bit, so limit to max_size_t.
    static constexpr size_t maximum = system::maximum<size_t>;

    virtual uint8_t do_peek_byte() NOEXCEPT;
    virtual void do_read_bytes(uint8_t* buffer, size_t size) NOEXCEPT;
    virtual void do_skip_bytes(size_t size) NOEXCEPT;
    virtual void do_rewind_bytes(size_t size) NOEXCEPT;
    virtual bool get_exhausted() const NOEXCEPT;

private:
    bool valid() const NOEXCEPT;
    void invalid() NOEXCEPT;
    void validate() NOEXCEPT;
    void clear() NOEXCEPT;
    size_t getter() NOEXCEPT;
    void limit(size_t size) NOEXCEPT;
    bool limiter(size_t size) NOEXCEPT;
    void seeker(typename IStream::pos_type offset) NOEXCEPT;

    IStream& stream_;
    size_t remaining_;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/stream/streamers/byte_reader.ipp>

#endif

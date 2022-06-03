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
#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BYTE_READER_HPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BYTE_READER_HPP

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/stream/streamers/interfaces/bytereader.hpp>

namespace libbitcoin {
namespace system {
    
/// A byte reader that accepts an istream.
template <typename IStream = std::istream>
class byte_reader
  : public virtual bytereader
{
public:
    /// Constructors.
    byte_reader(IStream& source) noexcept;

    /// Defaults.
    byte_reader(byte_reader&&) = default;
    byte_reader(const byte_reader&) = default;
    byte_reader& operator=(byte_reader&&) = default;
    byte_reader& operator=(const byte_reader&) = default;
    virtual ~byte_reader() = default;

    /// Read integer, size determined from parameter type.
    template <typename Integer, if_integer<Integer> = true>
    Integer read_big_endian() noexcept;
    template <typename Integer, if_integer<Integer> = true>
    Integer read_little_endian() noexcept;

    /// Read big endian (explicit specializations of read_big_endian).
    uint16_t read_2_bytes_big_endian() noexcept override;
    uint32_t read_4_bytes_big_endian() noexcept override;
    uint64_t read_8_bytes_big_endian() noexcept override;

    /// Little endian integer readers (specializations of read_little_endian).
    uint16_t read_2_bytes_little_endian() noexcept override;
    uint32_t read_4_bytes_little_endian() noexcept override;
    uint64_t read_8_bytes_little_endian() noexcept override;

    /// Read Bitcoin variable integer (1, 3, 5, or 9 bytes, little-endian).
    uint64_t read_variable() noexcept override;

    /// Cast read_variable to size_t, facilitates read_bytes(read_size()).
    /// Returns zero and invalidates stream if would exceed read limit.
    size_t read_size(size_t limit=max_size_t) noexcept override;

    /// Convert read_4_bytes_little_endian to an error code.
    code read_error_code() noexcept override;

    /// Read size bytes into array.
    template <size_t Size>
    data_array<Size> read_forward() noexcept;
    template <size_t Size>
    data_array<Size> read_reverse() noexcept;

    /// Read into stream until buffer is exhausted.
    std::ostream& read(std::ostream& out) noexcept override;

    /// Read hash (explicit specializations of read_forward).
    mini_hash read_mini_hash() noexcept override;
    short_hash read_short_hash() noexcept override;
    hash_digest read_hash() noexcept override;
    long_hash read_long_hash() noexcept override;

    /// Read/peek one byte (invalidates an empty stream).
    uint8_t peek_byte() noexcept override;
    uint8_t read_byte() noexcept override;

    /// Read all remaining bytes.
    data_chunk read_bytes() noexcept override;

    /// Read size bytes, return size is guaranteed.
    data_chunk read_bytes(size_t size) noexcept override;
    void read_bytes(uint8_t* buffer, size_t size) noexcept override;

    /// Read Bitcoin length-prefixed string.
    /// Returns empty and invalidates stream if would exceed read limit.
    std::string read_string(size_t limit=max_size_t) noexcept override;

    /// Read string, truncated at size or first null.
    /// This is only used for reading Bitcoin heading command text.
    std::string read_string_buffer(size_t size) noexcept override;

    /// Advance the iterator.
    void skip_byte() noexcept override;
    void skip_bytes(size_t size) noexcept override;

    /// Rewind the iterator.
    void rewind_byte() noexcept override;
    void rewind_bytes(size_t size) noexcept override;

    /// The stream is empty (or invalid).
    bool is_exhausted() const noexcept override;

    /// Get the current absolute position.
    size_t get_position() noexcept override;

    /// Clear invalid state and set absolute position.
    void set_position(size_t absolute) noexcept override;

    /// Limit stream upper bound to current position plus size (default resets).
    void set_limit(size_t size=max_size_t) noexcept override;

    /// Invalidate the stream.
    void invalidate() noexcept override;

    /// The stream is valid.
    operator bool() const noexcept override;

    /// The stream is invalid.
    bool operator!() const noexcept override;

protected:
    static constexpr uint8_t pad() { return 0x00; };

    // The maximum addressable stream position.
    // Should be defined on IStream::pos_type, however that is implementation
    // defined and is not an integer domain. Could rely on std::streamsize,
    // however this exceeds max_size_t in 32 bit, so limit to max_size_t.
    static constexpr size_t maximum() noexcept
    {
        return std::numeric_limits<size_t>::max();
    };

    virtual uint8_t do_peek_byte() noexcept;
    virtual void do_read_bytes(uint8_t* buffer, size_t size) noexcept;
    virtual void do_skip_bytes(size_t size) noexcept;
    virtual void do_rewind_bytes(size_t size) noexcept;
    virtual bool get_exhausted() const noexcept;

private:
    bool valid() const noexcept;
    void invalid() noexcept;
    void validate() noexcept;
    void clear() noexcept;
    size_t getter() noexcept;
    void limit(size_t size) noexcept;
    bool limiter(size_t size) noexcept;
    void seeker(typename IStream::pos_type offset) noexcept;

    IStream& stream_;
    size_t remaining_;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/stream/streamers/byte_reader.ipp>

#endif

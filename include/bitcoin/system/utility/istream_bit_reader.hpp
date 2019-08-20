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
#ifndef LIBBITCOIN_SYSTEM_ISTREAM_BIT_READER_HPP
#define LIBBITCOIN_SYSTEM_ISTREAM_BIT_READER_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <bitcoin/system/utility/bit_reader.hpp>
#include <bitcoin/system/utility/istream_reader.hpp>

namespace libbitcoin {
namespace system {

class BC_API istream_bit_reader
  : public bit_reader
{
public:
    istream_bit_reader(reader& reader);

    template <size_t Size>
    byte_array<Size> read_forward();

    template <size_t Size>
    byte_array<Size> read_reverse();

    template <typename Integer>
    Integer read_big_endian();

    template <typename Integer>
    Integer read_little_endian();

    /// Context.
    operator bool() const;
    bool operator!() const;
    bool is_exhausted() const;
    void invalidate();

    /// Read bit.
    bool read_bit();

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

    /// Read all remaining bytes.
    data_chunk read_bytes();

    /// Read required size buffer.
    data_chunk read_bytes(size_t size);

    /// Read variable length string.
    std::string read_string();

    /// Read required size string and trim nulls.
    std::string read_string(size_t size);

    /// Advance iterator without reading.
    void skip(size_t size);

protected:
    // The buffer is faulted or at eof.
    bool empty() const;

    // guarded buffer population
    void feed();

private:
    uint8_t buffer_;
    uint8_t offset_;
    reader& reader_;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/utility/istream_bit_reader.ipp>

#endif

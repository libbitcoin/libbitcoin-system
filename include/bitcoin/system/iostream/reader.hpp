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
#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_READER_HPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_READER_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <bitcoin/system/error.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/math/hash.hpp>

namespace libbitcoin {
namespace system {

/// Reader interface.
class BC_API reader
{
public:
    /// Context.
    virtual operator bool() const = 0;
    virtual bool operator!() const = 0;
    virtual bool is_exhausted() const = 0;
    virtual void invalidate() = 0;

    /// Read hashes.
    virtual hash_digest read_hash() = 0;
    virtual short_hash read_short_hash() = 0;
    virtual mini_hash read_mini_hash() = 0;

    /// Read big endian integers.
    virtual uint16_t read_2_bytes_big_endian() = 0;
    virtual uint32_t read_4_bytes_big_endian() = 0;
    virtual uint64_t read_8_bytes_big_endian() = 0;
    virtual uint64_t read_variable_big_endian() = 0;
    virtual size_t read_size_big_endian() = 0;

    /// Read little endian integers.
    virtual code read_error_code() = 0;
    virtual uint16_t read_2_bytes_little_endian() = 0;
    virtual uint32_t read_4_bytes_little_endian() = 0;
    virtual uint64_t read_8_bytes_little_endian() = 0;
    virtual uint64_t read_variable_little_endian() = 0;
    virtual size_t read_size_little_endian() = 0;

    /// Read/peek one byte.
    virtual uint8_t peek_byte() = 0;
    virtual uint8_t read_byte() = 0;

    /// Read all remaining bytes.
    virtual data_chunk read_bytes() = 0;

    /// Read required size buffer.
    virtual data_chunk read_bytes(size_t size) = 0;

    /// Read variable length string.
    virtual std::string read_string() = 0;

    /// Read required length string and trim nulls.
    virtual std::string read_string(size_t size) = 0;

    /// Advance iterator without reading.
    virtual void skip(size_t size) = 0;
};

} // namespace system
} // namespace libbitcoin

#endif

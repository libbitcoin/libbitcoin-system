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
#ifndef LIBBITCOIN_SYSTEM_OSTREAM_WRITER_HPP
#define LIBBITCOIN_SYSTEM_OSTREAM_WRITER_HPP

#include <ostream>
#include <bitcoin/system/error.hpp>
#include <bitcoin/system/utility/writer.hpp>

namespace libbitcoin {
namespace system {

class BC_API ostream_writer
  : public writer
{
public:
    ostream_writer(std::ostream& stream);

    template <unsigned Size>
    void write_forward(const byte_array<Size>& value);

    template <unsigned Size>
    void write_reverse(const byte_array<Size>& value);

    template <typename Integer>
    void write_big_endian(Integer value);

    template <typename Integer>
    void write_little_endian(Integer value);

    /// Context.
    operator bool() const;
    bool operator!() const;

    /// Write hashes.
    void write_hash(const hash_digest& value);
    void write_short_hash(const short_hash& value);
    void write_mini_hash(const mini_hash& value);

    /// Write big endian integers.
    void write_2_bytes_big_endian(uint16_t value);
    void write_4_bytes_big_endian(uint32_t value);
    void write_8_bytes_big_endian(uint64_t value);
    void write_variable_big_endian(uint64_t value);
    void write_size_big_endian(size_t value);

    /// Write little endian integers.
    void write_error_code(const code& ec);
    void write_2_bytes_little_endian(uint16_t value);
    void write_4_bytes_little_endian(uint32_t value);
    void write_8_bytes_little_endian(uint64_t value);
    void write_variable_little_endian(uint64_t value);
    void write_size_little_endian(size_t value);

    /// Write one byte.
    void write_byte(uint8_t value);

    /// Write all bytes.
    void write_bytes(const data_slice data);

    /// Write required size buffer.
    void write_bytes(const uint8_t* data, size_t size);

    /// Write variable length string.
    void write_string(const std::string& value, size_t size);

    /// Write required length string, padded with nulls.
    void write_string(const std::string& value);

    /// Advance iterator without writing.
    void skip(size_t size);

private:
    std::ostream& stream_;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/utility/ostream_writer.ipp>

#endif

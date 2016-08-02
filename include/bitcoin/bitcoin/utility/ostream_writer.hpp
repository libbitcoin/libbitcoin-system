/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_OSTREAM_WRITER_HPP
#define LIBBITCOIN_OSTREAM_WRITER_HPP

#include <ostream>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {

class BC_API ostream_writer
  : public writer
{
public:
    ostream_writer(std::ostream& stream);

    operator bool() const;
    bool operator!() const;

    void write_byte(uint8_t value);
    void write_data(const data_chunk& data);
    void write_data(const uint8_t* data, size_t size);
    void write_hash(const hash_digest& value);
    void write_short_hash(const short_hash& value);
    void write_mini_hash(const mini_hash& value);

    // These write data in little endian format:
    void write_2_bytes_little_endian(uint16_t value);
    void write_4_bytes_little_endian(uint32_t value);
    void write_8_bytes_little_endian(uint64_t value);
    void write_variable_uint_little_endian(uint64_t value);

    // These write data in big endian format:
    void write_2_bytes_big_endian(uint16_t value);
    void write_4_bytes_big_endian(uint32_t value);
    void write_8_bytes_big_endian(uint64_t value);
    void write_variable_uint_big_endian(uint64_t value);

    /**
     * Write a fixed size string padded with zeroes.
     */
    void write_fixed_string(const std::string& value, size_t size);

    /**
     * Write a variable length string.
     */
    void write_string(const std::string& value);

    /**
     * Writes an unsigned integer that has been encoded in big endian format.
     */
    template <typename T>
    void write_big_endian(T value);

    /**
     * Reads an unsigned integer that has been encoded in little endian format.
     */
    template <typename T>
    void write_little_endian(T value);

    template <typename T>
    void write_data(T& value);

    /**
     * Write a fixed-length data block.
     */
    template <unsigned Size>
    void write_bytes(const byte_array<Size>& value);

    template <unsigned Size>
    void write_bytes_reverse(const byte_array<Size>& value);

private:

    std::ostream& stream_;
};

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/ostream_writer.ipp>

#endif

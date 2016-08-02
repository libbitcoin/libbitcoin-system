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
#ifndef LIBBITCOIN_ISTREAM_READER_HPP
#define LIBBITCOIN_ISTREAM_READER_HPP

#include <cstdint>
#include <istream>
#include <bitcoin/bitcoin/utility/reader.hpp>

namespace libbitcoin {

class BC_API istream_reader
  : public reader
{
public:
    istream_reader(std::istream& stream);

    operator bool() const;
    bool operator!() const;

    bool is_exhausted() const;
    uint8_t read_byte();
    data_chunk read_data(size_t size);
    size_t read_data(uint8_t* data, size_t size);
    data_chunk read_data_to_eof();
    hash_digest read_hash();
    short_hash read_short_hash();
    mini_hash read_mini_hash();

    // These read data in little endian format: 
    uint16_t read_2_bytes_little_endian();
    uint32_t read_4_bytes_little_endian();
    uint64_t read_8_bytes_little_endian();
    uint64_t read_variable_uint_little_endian();

    // These read data in big endian format:
    uint16_t read_2_bytes_big_endian();
    uint32_t read_4_bytes_big_endian();
    uint64_t read_8_bytes_big_endian();
    uint64_t read_variable_uint_big_endian();

    /**
     * Read a fixed size string padded with zeroes.
     */
    std::string read_fixed_string(size_t length);

    /**
     * Read a variable length string.
     */
    std::string read_string();

    /**
     * Reads an unsigned integer that has been encoded in big endian format.
     */
    template <typename T>
    T read_big_endian();

    /**
     * Reads an unsigned integer that has been encoded in little endian format.
     */
    template <typename T>
    T read_little_endian();

    /**
     * Read a fixed-length data block.
     */
    template <unsigned Size>
    byte_array<Size> read_bytes();

    template <unsigned Size>
    byte_array<Size> read_bytes_reverse();

private:
    std::istream& stream_;
};

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/istream_reader.ipp>

#endif

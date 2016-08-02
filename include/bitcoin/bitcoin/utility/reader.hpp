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
#ifndef LIBBITCOIN_READER_HPP
#define LIBBITCOIN_READER_HPP

#include <string>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {

class BC_API reader
{
public:
    virtual operator bool() const = 0;
    virtual bool operator!() const = 0;

    virtual bool is_exhausted() const = 0;
    virtual uint8_t read_byte() = 0;
    virtual data_chunk read_data(size_t size) = 0;
    virtual size_t read_data(uint8_t* data, size_t size) = 0;
    virtual data_chunk read_data_to_eof() = 0;
    virtual hash_digest read_hash() = 0;
    virtual short_hash read_short_hash() = 0;
    virtual mini_hash read_mini_hash() = 0;

    // These read data in little endian format:
    virtual uint16_t read_2_bytes_little_endian() = 0;
    virtual uint32_t read_4_bytes_little_endian() = 0;
    virtual uint64_t read_8_bytes_little_endian() = 0;
    virtual uint64_t read_variable_uint_little_endian() = 0;

    // These read data in big endian format:
    virtual uint16_t read_2_bytes_big_endian() = 0;
    virtual uint32_t read_4_bytes_big_endian() = 0;
    virtual uint64_t read_8_bytes_big_endian() = 0;
    virtual uint64_t read_variable_uint_big_endian() = 0;

    /**
     * Read a fixed size string padded with zeroes.
     */
    virtual std::string read_fixed_string(size_t length) = 0;

    /**
     * Read a variable length string.
     */
    virtual std::string read_string() = 0;
};

} // namespace libbitcoin

#endif

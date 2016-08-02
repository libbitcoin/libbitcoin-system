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
#ifndef LIBBITCOIN_WRITER_HPP
#define LIBBITCOIN_WRITER_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/variable_uint_size.hpp>

namespace libbitcoin {

class BC_API writer
{
public:
    virtual operator bool() const = 0;
    virtual bool operator!() const = 0;

    virtual void write_byte(uint8_t value) = 0;
    virtual void write_data(const data_chunk& data) = 0;
    virtual void write_data(const uint8_t* data, size_t size) = 0;
    virtual void write_hash(const hash_digest& value) = 0;
    virtual void write_short_hash(const short_hash& value) = 0;
    virtual void write_mini_hash(const mini_hash& value) = 0;

    // These write data in little endian format:
    virtual void write_2_bytes_little_endian(uint16_t value) = 0;
    virtual void write_4_bytes_little_endian(uint32_t value) = 0;
    virtual void write_8_bytes_little_endian(uint64_t value) = 0;
    virtual void write_variable_uint_little_endian(uint64_t value) = 0;

    // These write data in big endian format:
    virtual void write_2_bytes_big_endian(uint16_t value) = 0;
    virtual void write_4_bytes_big_endian(uint32_t value) = 0;
    virtual void write_8_bytes_big_endian(uint64_t value) = 0;
    virtual void write_variable_uint_big_endian(uint64_t value) = 0;

    /**
     * Write a fixed size string padded with zeroes.
     */
    virtual void write_fixed_string(const std::string& value, size_t size) = 0;

    /**
     * Write a variable length string.
     */
    virtual void write_string(const std::string& value) = 0;
};

} // namespace libbitcoin

#endif

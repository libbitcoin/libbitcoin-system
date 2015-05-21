/*
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_OSTREAM_HPP
#define LIBBITCOIN_OSTREAM_HPP

#include <ostream>
#include <string>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/variable_uint_size.hpp>

namespace libbitcoin {

/* These write data in little endian format: */
void write_byte(std::ostream& stream, uint8_t value);

void write_2_bytes(std::ostream& stream, uint16_t value);

void write_4_bytes(std::ostream& stream, uint32_t value);

void write_8_bytes(std::ostream& stream, uint64_t value);

/**
 * Writes an unsigned integer that has been encoded in big endian format.
 */
template <typename T>
void write_big_endian(std::ostream& stream, T value);

/**
 * Reads an unsigned integer that has been encoded in little endian format.
 */
template <typename T>
void write_little_endian(std::ostream& stream, T value);

void write_variable_uint(std::ostream& stream, uint64_t value);

template <typename T>
void write_data(std::ostream& stream, T& value);

void write_hash(std::ostream& stream, const hash_digest& value);

void write_short_hash(std::ostream& stream, const short_hash& value);

/**
 * Write a fixed size string padded with zeroes.
 */
void write_fixed_string(std::ostream& stream, const std::string& value, size_t string_size);

/**
 * Write a variable length string.
 */
void write_string(std::ostream& stream, const std::string& value);

/**
 * Write a fixed-length data block.
 */
template<unsigned N>
void write_bytes(std::ostream& stream, const byte_array<N>& value);

template<unsigned N>
void write_bytes_reverse(std::ostream& stream, const byte_array<N>& value);

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/ostream.ipp>

#endif


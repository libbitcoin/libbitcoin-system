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
#ifndef LIBBITCOIN_ISTREAM_HPP
#define LIBBITCOIN_ISTREAM_HPP

#include <istream>
#include <string>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {

/* These read data in little endian format: */
uint8_t read_byte(std::istream& stream);

uint16_t read_2_bytes(std::istream& stream);

uint32_t read_4_bytes(std::istream& stream);

uint64_t read_8_bytes(std::istream& stream);

/**
 * Reads an unsigned integer that has been encoded in big endian format.
 */
template <typename T>
T read_big_endian(std::istream& stream);

/**
 * Reads an unsigned integer that has been encoded in little endian format.
 */
template <typename T>
T read_little_endian(std::istream& stream);

uint64_t read_variable_uint(std::istream& stream);

data_chunk read_data(std::istream& stream, size_t n_bytes);

hash_digest read_hash(std::istream& stream);

short_hash read_short_hash(std::istream& stream);

/**
 * Read a fixed size string padded with zeroes.
 */
std::string read_fixed_string(std::istream& stream, size_t len);

/**
 * Read a variable length string.
 */
std::string read_string(std::istream& stream);

/**
 * Read a fixed-length data block.
 */
template<unsigned N>
byte_array<N> read_bytes(std::istream& stream);

template<unsigned N>
byte_array<N> read_bytes_reverse(std::istream& stream);

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/istream.ipp>

#endif


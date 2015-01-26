/*
 * Copyright (c) 2011-2014 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_BASE16_HPP
#define LIBBITCOIN_BASE16_HPP

#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {

/**
 * Convert data into a user-readable hex string.
 */
BC_API std::string encode_base16(data_slice data);

/**
 * Convert a hex string into bytes.
 * @return false if the input is malformed.
 */
BC_API bool decode_base16(data_chunk& out, const std::string &in);

/**
 * Converts a hex string to a number of bytes.
 * @return false if the input is malformed, or the wrong length.
 */
template <size_t Size>
bool decode_base16(byte_array<Size>& out, const std::string &in);

/**
 * Converts a hex string literal to a data array.
 * This would be better as a C++11 user-defined literal,
 * but MSVC doesn't support those.
 */
template <size_t Size>
byte_array<(Size - 1) / 2> base16_literal(const char (&string)[Size]);

/**
 * Converts a bitcoin_hash to a string.
 * The bitcoin_hash format is like base16, but with the bytes reversed.
 */
BC_API std::string encode_hash(hash_digest hash);

/**
 * Convert a string into a bitcoin_hash.
 * The bitcoin_hash format is like base16, but with the bytes reversed.
 * @return false if the input is malformed.
 */
BC_API bool decode_hash(hash_digest& out, const std::string& in);

/**
 * Convert a hex string literal into a bitcoin_hash.
 * The bitcoin_hash format is like base16, but with the bytes reversed.
 */
BC_API hash_digest hash_literal(const char (&string)[2*hash_size + 1]);

// Old names:
std::string encode_hex(data_slice in);
data_chunk decode_hex(std::string in);

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/formats/base16.ipp>

#endif


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

BC_API bool is_base16(const char c);

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
 * Converts a hex string literal to a data array.
 * This would be better as a C++11 user-defined literal,
 * but MSVC doesn't support those.
 */
template <size_t Size>
byte_array<(Size - 1) / 2> base16_literal(const char (&string)[Size]);

/**
 * Convert a hex string into hash bytes.
 * On error, returns null_hash.
 */
BC_API hash_digest decode_hash(const std::string& hex);

// Old names:
std::string encode_hex(data_slice in);
data_chunk decode_hex(std::string in);

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/base16.ipp>

#endif


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
#ifndef LIBBITCOIN_SYSTEM_BASE_32_HPP
#define LIBBITCOIN_SYSTEM_BASE_32_HPP

#include <string>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/utility/data.hpp>

namespace libbitcoin {
namespace system {

/**
* Convert bytes to a base32 string.
* @return the base32 encoded string.
*/
BC_API std::string encode_base32(const data_chunk& data);

/**
 * Convert a base32 string to a byte vector.
 * The input is invalid if mixed case, any character is not from base32
 * character set, or expanded.size % 8 != 0.
 * @return false if invalid input.
 */
BC_API bool decode_base32(data_chunk& out, const std::string& in);

/**
 * Expand any data (8 bit bytes) to a vector of 5 bit bytes.
 * @return the expanded data. This is an internal intermediate base32 encoding
 * step, exposed only because it is required to implement the bech32 checksum
 * fuctions. If you are working with addresses you probably don't need it.
 */
BC_API data_chunk base32_expand(const data_chunk& data);

/**
 * Compact bytes with 5 bit values to vector of 8 bit bytes.
 * The input is invalid if any byte has a value greater than (2^5)-1 or if
 * expanded.size % 8 != 0. This is an internal intermediate base32 encoding
 * step, exposed only because it is required to implement the bech32 checksum
 * fuctions. If you are working with addresses you probably don't need it.
 * @return false if invalid input.
 */
BC_API bool base32_compact(data_chunk& out, const data_chunk& expanded);

// TODO: en.cppreference.com/w/cpp/language/user_literal

} // namespace system
} // namespace libbitcoin

#endif

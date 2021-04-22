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

/// Base32 encoding conforms to "bech32" requirements but isolates the encoding
/// facility from the checksum facility. In this way it behaves in the same
/// manner as other data-string transforms, such as base 16, 58, 64 and 85.
/// This approach also brings address manipulation in line with expectations.
/// A witness address may be checksummed and encoded like a payment addresses.

/**
* Convert a byte vector to a base32 string.
* Do not include the prefix:separator when encoding a witness address.
* @return the base32 encoded string.
*/
BC_API std::string encode_base32(const data_slice& data);

/**
 * Convert a base32 string to a byte vector.
 * Do not include the prefix:separator when decoding a witness address.
 * @return false if the input is malformed.
 */
BC_API bool decode_base32(data_chunk& out, const std::string& in);

/**
 * These aren't the functions you're looking for.
 */
BC_API data_chunk base32_expand(const data_slice& data);
BC_API data_chunk base32_compress(const data_slice& expanded);

// TODO: en.cppreference.com/w/cpp/language/user_literal

} // namespace system
} // namespace libbitcoin

#endif

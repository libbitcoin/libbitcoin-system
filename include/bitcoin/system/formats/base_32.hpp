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

// This is NOT an implementation of RFC 4648: tools.ietf.org/html/rfc4648
// This is a generic byte-aligned data encoder fully compliant with the data
// conversion aspect of BIP713. However, unlike BIP173 this is entirely
// decoupled from bitcoin semantics. BIP173 requires version and checksum
// incorporation, fed into an internal stage of encoding. This makes unusable
// for general purpose encoding (and complex to work with even in bitcoin).

// BIP173 actually specifies two mappings. One is this base32 and the other
// is a numeric expansion with that can be mapped to and from base32 characters
// by holding the index offset of those characters. This is also base 32 but it
// is not a texual encoding. Instead it is a simple expansion of the original
// data to 5 bits per byte. This would be inconsequential but for the BIP173
// specification that bitcoin segregated witness script version and checksum
// be written directly to the expanded encoding with purposeful non-conformance
// with its own base32 data mapping. These elements must be injected into and
// extracted from the intermediate stage of the base32 encoding.

// For these reasons we provide bech32_build_checked and bech32_verify_checked
// functions as a bridge for base32 for those working with witness addresses.
// These are implemented using the base32_expand and base32_compact functions
// here. These expose the internal expand/contract stages of base32
// encode/decode, but are not necessary given the bech32 checked functions.
// The expand/contract functions here are exposed for implementation of the
// bech32 checked functions and for testing and are otherwise unnecessary.

// BIP173 address test vectors do not round trip here, even with the prefix and
// separator charactors removed. The prefix and checksum are injected into the
// expanded representation, which is internal to this base32 encoding. The
// injection appears as base32, but is non-standard base32 encoding. The BIP173
// data value must be compressed from the checked expansion before encoding
// as base32, and must be unchecked to parse data values after base32 decoding.
// In this implementation any encoded value that was created by encode_base32
// will be returned by decode_base32. Arbitrary encoding characters will decode
// but may not round trip (as is the case with BIP173 address encoding).

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

/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_RADIX_BASE_32B_HPP
#define LIBBITCOIN_SYSTEM_RADIX_BASE_32B_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

typedef std_vector<uint5_t> base32b_chunk;

// This is NOT an implementation of RFC 4648: tools.ietf.org/html/rfc4648
// Generic data-text encoder fully compliant with data conversion aspect of
// BIP713. Unlike BIP173 it is entirely decoupled from bitcoin semantics.
// BIP173 requires version and checksum incorporation, fed into internal
// stage of encoding, making it unusable for general purpose encoding (and
// complex to work with even in bitcoin).

// BIP173 specifies two mappings. One is this base32 and other is numeric
// expansion that can be mapped to and from base32 characters by holding index
// offset of those characters. This is also base 32 but not textual encoding,
// rather simple expansion of original data to 5 bits per byte. This would be
// inconsequential but for BIP173 specification that bitcoin segregated
// witness script version and checksum be written directly to expanded
// encoding. These elements must be injected into and extracted from
// intermediate stage of base32 encoding.

// For these reasons bech32_build_checked and bech32_verify_checked functions
// bridge base32 for those working with witness addresses. These are
// implemented using base32b_unpack and base32b_pack functions here, which
// expose internal expand/contract stages of base32 encode/decode, but are not
// necessary given bech32 checked functions. Expand/contract functions here
// are exposed for implementation of bech32 checked functions and for testing
// and are otherwise unnecessary.

// BIP173 address test vectors do not round trip here, even with prefix and
// separator characters removed. Prefix and checksum are injected into expanded
// representation, internal to this base32 encoding. Injection appears as
// base32, but is non-standard base32 encoding. BIP173 data value must be
// compressed from checked expansion before encoding as base32, and must be
// unchecked to parse data values after base32 decoding. Any encoded value
// created by encode_base32 is returned by decode_base32. Arbitrary encoding
// characters decode but may not round trip (as with BIP173 address encoding).

/// Convert bytes to a base32 string.
BC_API std::string encode_base32b(const data_chunk& data) NOEXCEPT;

/// Convert base32 string to bytes.
/// False if mixed case or any character is not from base32 character set.
BC_API bool decode_base32b(data_chunk& out, const std::string& in) NOEXCEPT;

/// Convert base32 data to a base32 string.
BC_API std::string encode_base32b(const base32b_chunk& data) NOEXCEPT;

/// Convert base32 string to base32 data.
/// False if mixed case or any character is not from base32 character set.
BC_API bool decode_base32b(base32b_chunk& out, const std::string& in) NOEXCEPT;

/// Pack any vector of 5 bit bytes to vector of 8 bit bytes.
BC_API data_chunk base32b_pack(const base32b_chunk& unpacked) NOEXCEPT;

/// Unpack any vector of 8 bit bytes to a vector of 5 bit bytes.
BC_API base32b_chunk base32b_unpack(const data_chunk& packed) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#endif

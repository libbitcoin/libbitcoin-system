/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHECKSUM_HPP
#define LIBBITCOIN_SYSTEM_CHECKSUM_HPP

#include <cstddef>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/utility/data.hpp>

namespace libbitcoin {
namespace system {

// checksum, used by payment_address, wif, hd and ek.
// p2p message checksum is implemented independently.
// ----------------------------------------------------------------------------

static const size_t checksum_default_size = sizeof(uint32_t);

/// These utilities are used for bitcoin payment addresses and other standards
/// that leverage the same technique. A bitcoin checksum is the leading bytes
/// of a double sha256 hash of any data. Bitcoin checksums are appended to the
/// data, typically later base58 encoded. Bitcoin standards typically use a four
/// byte checksum, though any length up to min(Size, 256) is allowed here.
/// Bitcoin payment addresses prefix data with a version before checksumming.
/// Accessing Checksum (length) parameter requires explicitly specifying both.

/// Append the bitcoin checksum of slices to end of new Size array.
/// Underfill is padded with 0x00, excess is truncated.
template <size_t Size, size_t Checksum = checksum_default_size>
byte_array<Size> insert_checksum(const loaf& slices);

/// Append bitcoin checksum of preceding data to end of existing Size array.
template <size_t Size, size_t Checksum = checksum_default_size>
void insert_checksum(byte_array<Size>& data);

/// Verify the last bytes are a bitcoin checksum of the preceding bytes.
template <size_t Size, size_t Checksum = checksum_default_size>
bool verify_checksum(const byte_array<Size>& data);

/// Append slices and a four byte bitcoin checksum.
BC_API data_chunk append_checksum(const loaf& slices);

/// Append a four byte bitcoin checksum of data to itself.
BC_API void append_checksum(data_chunk& data);

/// Verify the last four bytes are a bitcoin checksum of the preceding bytes.
BC_API bool verify_checksum(const data_slice& data);

// bech32 checksum, used by witness_address.
// ----------------------------------------------------------------------------

/// These utilities are used for witness payment addresses and other standards
/// that leverage the same technique. Bitcoin witness addresses prefix data
/// with version bytes before checking. Checksum length is six bytes.
/// The prefix is use for checksumming and not incorporate into output.
/// Standard prefixes are provided in the witness_address implementation.

/// Base32 encoding conforms to "bech32" requirements but isolates the encoding
/// facility from the checksum facility. In this way it behaves in the same
/// manner as other data-string transforms, such as base 16, 58, 64 and 85.
/// This approach also brings address manipulation in line with expectations.
/// A witness address may be checksummed and encoded like a payment addresses.
/// The only visible distinction is the additional 'prefix' parameter. One may
/// think of this as a checksum "salt".

/// Combine version, data and checksum.
BC_API data_chunk bech32_build_checked(uint8_t version, const data_chunk& data,
    const std::string& prefix);

/// Verify the bech32 checksum and extract version and payload.
BC_API bool bech32_verify_checked(uint8_t& out_version, data_chunk& in_out_data,
    const std::string& prefix);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/checksum.ipp>

#endif

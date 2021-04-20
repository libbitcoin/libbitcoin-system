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

static const size_t checksum_default = sizeof(uint32_t);

/// These utilities are used for bitcoin payment addresses and other standards
/// that leverage the same technique. A bitcoin checksum is the leading bytes
/// of a double sha256 hash of any data. Bitcoin checksums are appended to the
/// data, typically later ascii encoded. Bitcoin standards typically use a four
/// byte checksum, though any length up to min(Size, 256) is allowed here.

// arrays
/// Accessing Checksum (length) parameter requires explicitly specifying both.

/// Append the bitcoin checksum of slices to end of new Size array.
/// Underfill is padded with 0x00, excess is truncated.
template <size_t Size, size_t Checksum = checksum_default>
byte_array<Size> insert_checksum(const loaf& slices);

/// Append bitcoin checksum of preceding data to end of existing Size array.
template <size_t Size, size_t Checksum = checksum_default>
void insert_checksum(byte_array<Size>& data);

/// Verify the last bytes are a bitcoin checksum of the preceding bytes.
template <size_t Size, size_t Checksum = checksum_default>
bool verify_checksum(const byte_array<Size>& data);

// chunks

/// Append slices and a four byte bitcoin checksum.
BC_API data_chunk append_checksum(const loaf& slices);

/// Append a four byte bitcoin checksum of data to itself.
BC_API void append_checksum(data_chunk& data);

/// Verify the last four bytes are a bitcoin checksum of the preceding bytes.
BC_API bool verify_checksum(const data_slice& data);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/checksum.ipp>

#endif

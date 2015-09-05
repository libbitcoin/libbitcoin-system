/**
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
#ifndef LIBBITCOIN_CHECKSUM_HPP
#define LIBBITCOIN_CHECKSUM_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {

BC_CONSTEXPR size_t checksum_size = sizeof(uint32_t);

/**
 * Concatenate several data slices into a single fixed size array and append a
 * checksum.
 */
template <size_t Size>
bool build_checked_array(byte_array<Size>& out,
    std::initializer_list<data_slice> slices);

/**
 * Appends a four-byte checksum into the end of an array.
 * Returns false if the array is too small to contain the checksum.
 */
template<size_t Size>
bool insert_checksum(byte_array<Size>& out);

/**
 * Appends a four-byte checksum of a datachunk to itself.
 */
BC_API void append_checksum(data_chunk& data);

/**
 * Generate a bitcoin hash checksum. Last 4 bytes of sha256(sha256(data))
 *
 * int(sha256(sha256(data))[-4:])
 */
BC_API uint32_t bitcoin_checksum(data_slice data);

/**
 * Verifies the last four bytes of a data chunk are a valid checksum of the
 * earlier bytes. This is typically used to verify base58 data.
 */
BC_API bool verify_checksum(data_slice data);

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/math/checksum.ipp>

#endif


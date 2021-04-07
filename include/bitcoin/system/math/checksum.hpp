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
#ifndef LIBBITCOIN_SYSTEM_CHECKSUM_HPP
#define LIBBITCOIN_SYSTEM_CHECKSUM_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <bitcoin/system/compat.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/utility/data.hpp>

namespace libbitcoin {
namespace system {

static BC_CONSTEXPR size_t checksum_size = sizeof(uint32_t);

#define WRAP_SIZE(payload_size) (payload_size + checksum_size + 1u)
#define UNWRAP_SIZE(payload_size) (payload_size - checksum_size - 1u)

/**
 * Obtain the integer log2 of a given value.
 * @param[in] value   The value from which to derive the log.
 * @return            The log2(value), or zero if value is zero.
 */
inline size_t log2(size_t value)
{
    if (value == 0u)
        return 0u;

    size_t power = 0;
    while (((value >>= 1u)) > 0u) ++power;
    return power;
}

/**
 * Concatenate several data slices into a single fixed size array and append a
 * checksum.
 */
template <size_t Size>
byte_array<Size> build_checked_array(
    const std::initializer_list<data_slice>& slices);

/**
 * Appends a four-byte checksum into the end of an array.
 * Array must be longer than bc::checksum_size.
 */
template<size_t Size>
void insert_checksum(byte_array<Size>& out);

/**
 * Unwrap a wrapped payload.
 * @param[out] out_version   The version byte of the wrapped data.
 * @param[out] out_payload   The payload of the wrapped data.
 * @param[in]  wrapped       The wrapped data to unwrap.
 * @return                   True if input checksum validates.
 */
template <size_t Size>
bool unwrap(uint8_t& out_version, byte_array<UNWRAP_SIZE(Size)>& out_payload,
    byte_array<Size>& wrapped);

/**
 * Unwrap a wrapped payload and return the checksum.
 * @param[out] out_version   The version byte of the wrapped data.
 * @param[out] out_payload   The payload of the wrapped data.
 * @param[out] out_checksum  The validated checksum of the wrapped data.
 * @param[in]  wrapped       The wrapped data to unwrap.
 * @return                   True if input checksum validates.
 */
template <size_t Size>
bool unwrap(uint8_t& out_version,
    byte_array<UNWRAP_SIZE(Size)>& out_payload, uint32_t& out_checksum,
    byte_array<Size>& wrapped);

/**
 * Wrap arbitrary data.
 * @param[in]  version  The version byte for the wrapped data.
 * @param[out] payload  The payload to wrap.
 * @return              The wrapped data.
 */
template <size_t Size>
byte_array<WRAP_SIZE(Size)> wrap(uint8_t version, byte_array<Size>& payload);

/**
 * Appends a four-byte checksum of a data chunk to itself.
 */
BC_API void append_checksum(data_chunk& data);

/**
 * Generate a bitcoin hash checksum. Last 4 bytes of sha256(sha256(data))
 *
 * int(sha256(sha256(data))[-4:])
 */
BC_API uint32_t bitcoin_checksum(const data_slice& data);

/**
 * Verifies the last four bytes of a data chunk are a valid checksum of the
 * earlier bytes. This is typically used to verify base58 data.
 */
BC_API bool verify_checksum(const data_slice& data);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/checksum.ipp>

#endif


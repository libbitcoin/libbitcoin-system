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
#ifndef LIBBITCOIN_SYSTEM_SERIAL_ENDIAN_HPP
#define LIBBITCOIN_SYSTEM_SERIAL_ENDIAN_HPP

#include <cstddef>
#include <iostream>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/data/data.hpp>

namespace libbitcoin {
namespace system {

/// integer (inferred type) -> data (value-sized) or array (explicit size)
/// ---------------------------------------------------------------------------

template <size_t Size, typename Integer, if_integer<Integer> = true>
data_array<Size> to_big_endian_array(Integer value) noexcept;

template <size_t Size, typename Integer, if_integer<Integer> = true>
data_array<Size> to_little_endian_array(Integer value) noexcept;

template <typename Integer, if_integer<Integer> = true>
data_chunk to_big_endian_chunk(Integer value) noexcept;

template <typename Integer, if_integer<Integer> = true>
data_chunk to_little_endian_chunk(Integer value) noexcept;

/// data -> integral (inferred type), integral -> array (implicit size)
/// ---------------------------------------------------------------------------

template <typename Integer, if_integral_integer<Integer> = true>
Integer from_big_endian(const data_slice& data) noexcept;

template <typename Integer, if_integral_integer<Integer> = true>
Integer from_little_endian(const data_slice& data) noexcept;

template <typename Integer, if_integral_integer<Integer> = true>
data_array<sizeof(Integer)> to_big_endian(Integer value) noexcept;

template <typename Integer, if_integral_integer<Integer> = true>
data_array<sizeof(Integer)> to_little_endian(Integer value) noexcept;

/// data -> uintx (inferred size), uintx -> data_chunk (inferred size)
/// ---------------------------------------------------------------------------

template <typename Integer, if_base_of<Integer, uintx> = true>
Integer from_big_endian(const data_slice& data) noexcept;

template <typename Integer, if_base_of<Integer, uintx> = true>
Integer from_little_endian(const data_slice& data) noexcept;

template <typename Integer, if_base_of<Integer, uintx> = true>
data_chunk to_big_endian(const Integer& value) noexcept;

template <typename Integer, if_base_of<Integer, uintx> = true>
data_chunk to_little_endian(const Integer& value) noexcept;

/// data -> uintx_t (explicit size), uintx_t/integer -> array (explicit size)
/// ---------------------------------------------------------------------------
/// Bytes is not required to match data size or Integer type implicit size.
/// Integer may be any integral, uintx or other integer type.

template <size_t Bytes>
uintx_t<to_bits(Bytes)> from_big_endian(const data_slice& data) noexcept;

template <size_t Bytes>
uintx_t<to_bits(Bytes)> from_little_endian(const data_slice& data) noexcept;

template <size_t Bytes, typename Integer, if_integer<Integer> = true>
data_array<Bytes> to_big_endian(const Integer& value) noexcept;

template <size_t Bytes, typename Integer, if_integer<Integer> = true>
data_array<Bytes> to_little_endian(const Integer& value) noexcept;

/// stream -> integral (explicit type), integral -> stream (implicit size)
/// ---------------------------------------------------------------------------

template <typename Integer, if_integral_integer<Integer> = true>
Integer from_big_endian(std::istream& stream) noexcept;

template <typename Integer, if_integral_integer<Integer> = true>
Integer from_little_endian(std::istream& stream) noexcept;

template <typename Integer, if_integral_integer<Integer> = true>
void to_big_endian(std::ostream& stream, Integer value) noexcept;

template <typename Integer, if_integral_integer<Integer> = true>
void to_little_endian(std::ostream& stream, Integer value) noexcept;

/// iterator (unknown size) -> integral (implicit size)
/// ---------------------------------------------------------------------------
/// Must be guarded by the caller, use when passing end would be redundant.

template <typename Integer, typename Iterator,
    if_integral_integer<Integer> = true>
Integer from_big_endian_unchecked(const Iterator& data) noexcept;

template <typename Integer, typename Iterator,
    if_integral_integer<Integer> = true>
Integer from_little_endian_unchecked(const Iterator& data) noexcept;

/// Specialized functions.
/// ---------------------------------------------------------------------------
/// Slight performance advantage to loop unroll.

inline uint32_t from_big_endian(const uint8_t data[sizeof(uint32_t)]) noexcept
{
    return
        (static_cast<uint32_t>(data[3]) <<  0) |
        (static_cast<uint32_t>(data[2]) <<  8) |
        (static_cast<uint32_t>(data[1]) << 16) |
        (static_cast<uint32_t>(data[0]) << 24);
}

inline uint32_t from_little_endian(const uint8_t data[sizeof(uint32_t)]) noexcept
{
    return
        (static_cast<uint32_t>(data[0]) <<  0) |
        (static_cast<uint32_t>(data[1]) <<  8) |
        (static_cast<uint32_t>(data[2]) << 16) |
        (static_cast<uint32_t>(data[3]) << 24);
}

inline void to_big_endian(uint8_t data[sizeof(uint32_t)],
    uint32_t value) noexcept
{
    data[3] = (value >>  0) & 0xff;
    data[2] = (value >>  8) & 0xff;
    data[1] = (value >> 16) & 0xff;
    data[0] = (value >> 24) & 0xff;
}

inline void to_little_endian(uint8_t data[sizeof(uint32_t)],
    uint32_t value) noexcept
{
    data[0] = (value >>  0) & 0xff;
    data[1] = (value >>  8) & 0xff;
    data[2] = (value >> 16) & 0xff;
    data[3] = (value >> 24) & 0xff;
}

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/serial/endian.ipp>

#endif

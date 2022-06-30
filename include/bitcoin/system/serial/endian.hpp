/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// stream -> integral (explicit type), integral -> stream (implicit size)
/// ---------------------------------------------------------------------------

// integral from_big|little_endian(istream)
// void       to_big|little_endian(ostream)

template <typename Integer, if_integral_integer<Integer> = true>
inline Integer from_big_endian(std::istream& stream) NOEXCEPT;

template <typename Integer, if_integral_integer<Integer> = true>
inline Integer from_little_endian(std::istream& stream) NOEXCEPT;

template <typename Integer, if_integral_integer<Integer> = true>
inline void to_big_endian(std::ostream& stream, Integer value) NOEXCEPT;

template <typename Integer, if_integral_integer<Integer> = true>
inline void to_little_endian(std::ostream& stream, Integer value) NOEXCEPT;

/// integer (inferred type) -> data (value-sized) or array (explicit size)
/// ---------------------------------------------------------------------------
/// Use excess to reserve the specified number of bytes beyond return size.

// data_array to_big|little_endian_size<Size>(Integer)
// data_chunk to_big|little_endian_size(Integer, Size)

template <size_t Size, typename Integer, if_integer<Integer> = true>
constexpr data_array<Size> to_big_endian_size(Integer value) NOEXCEPT;

template <size_t Size, typename Integer, if_integer<Integer> = true>
constexpr data_array<Size> to_little_endian_size(Integer value) NOEXCEPT;

template <typename Integer, if_integer<Integer> = true>
inline data_chunk to_big_endian_size(Integer value, size_t excess=zero) NOEXCEPT;

template <typename Integer, if_integer<Integer> = true>
inline data_chunk to_little_endian_size(Integer value, size_t excess=zero) NOEXCEPT;

/// data -> integral (inferred type), integral -> array (implicit size)
/// ---------------------------------------------------------------------------

// integral   from_big|little_endian(data_slice)
// data_array   to_big|little_endian(integral)

template <typename Integer, if_integral_integer<Integer> = true>
inline Integer from_big_endian(const data_slice& data) NOEXCEPT;

template <typename Integer, if_integral_integer<Integer> = true>
inline Integer from_little_endian(const data_slice& data) NOEXCEPT;

template <typename Integer, if_integral_integer<Integer> = true>
constexpr data_array<sizeof(Integer)> to_big_endian(Integer value) NOEXCEPT;

template <typename Integer, if_integral_integer<Integer> = true>
constexpr data_array<sizeof(Integer)> to_little_endian(Integer value) NOEXCEPT;

/// data -> uintx (inferred size), uintx -> data_chunk (inferred size)
/// ---------------------------------------------------------------------------

// uintx      from_big|little_endian(data_slice)
// data_chunk   to_big|little_endian(uintx)

template <typename Integer, if_base_of<Integer, uintx> = true>
inline Integer from_big_endian(const data_slice& data) NOEXCEPT;

template <typename Integer, if_base_of<Integer, uintx> = true>
inline Integer from_little_endian(const data_slice& data) NOEXCEPT;

template <typename Integer, if_base_of<Integer, uintx> = true>
inline data_chunk to_big_endian(const Integer& value) NOEXCEPT;

template <typename Integer, if_base_of<Integer, uintx> = true>
inline data_chunk to_little_endian(const Integer& value) NOEXCEPT;

/// data -> uintx_t (explicit size), uintx_t/integer -> array (explicit size)
/// ---------------------------------------------------------------------------
/// Bytes is not required to match data size or Integer type implicit size.
/// Integer may be any integral, uintx or other integer type.

// uintx_t    from_big|little_endian<Size>(data_slice)
// uintx_t    from_big|little_endian_array(data_array)
// data_array   to_big|little_endian<Size>(uintx_t/integer)

template <size_t Bytes>
inline uintx_t<to_bits<uint32_t>(Bytes)>
uintx_from_big_endian_chunk(const data_slice& data) NOEXCEPT;

template <size_t Bytes>
inline uintx_t<to_bits<uint32_t>(Bytes)>
uintx_from_little_endian_chunk(const data_slice& data) NOEXCEPT;

template <size_t Bytes>
constexpr uintx_t<to_bits<uint32_t>(Bytes)>
uintx_from_big_endian_array(const data_array<Bytes>& data) NOEXCEPT;

template <size_t Bytes>
constexpr uintx_t<to_bits<uint32_t>(Bytes)>
uintx_from_little_endian_array(const data_array<Bytes>& data) NOEXCEPT;

template <size_t Bytes, typename Integer, if_integer<Integer> = true>
constexpr data_array<Bytes> to_big_endian(const Integer& value) NOEXCEPT;

template <size_t Bytes, typename Integer, if_integer<Integer> = true>
constexpr data_array<Bytes> to_little_endian(const Integer& value) NOEXCEPT;

/// iterator (unknown size) -> integral (implicit size)
/// ---------------------------------------------------------------------------
/// Must be guarded by the caller, use when passing end would be redundant.

// Integer from_big|little_endian_unchecked(Iterator)
// Integer   to_big|little_endian_unchecked(Iterator)

template <typename Integer, typename Iterator,
    if_integral_integer<Integer> = true>
constexpr Integer from_big_endian_unchecked(const Iterator& data) NOEXCEPT;

template <typename Integer, typename Iterator,
    if_integral_integer<Integer> = true>
constexpr Integer from_little_endian_unchecked(const Iterator& data) NOEXCEPT;

// data[] -> integral[] (explicit size), integral[] -> data[] (explicit size)
/// ---------------------------------------------------------------------------

// void from_big|little_endian(integral[])
// void   to_big|little_endian(integral[])

template <size_t Count, typename Integer, if_integral_integer<Integer> = true>
constexpr void from_big_endian(Integer to[Count],
    const uint8_t from[Count * sizeof(Integer)]) NOEXCEPT;

template <size_t Count, typename Integer, if_integral_integer<Integer> = true>
constexpr void from_little_endian(Integer to[Count],
    const uint8_t from[Count * sizeof(Integer)]) NOEXCEPT;

template <size_t Count, typename Integer, if_integral_integer<Integer> = true>
constexpr void to_big_endian(uint8_t to[Count * sizeof(Integer)],
    const Integer from[Count]) NOEXCEPT;

template <size_t Count, typename Integer, if_integral_integer<Integer> = true>
constexpr void to_little_endian(uint8_t to[Count * sizeof(Integer)],
    const Integer from[Count]) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/serial/endian.ipp>

#endif

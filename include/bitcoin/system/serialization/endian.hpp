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
#ifndef LIBBITCOIN_SYSTEM_SERIALIZATION_ENDIAN_HPP
#define LIBBITCOIN_SYSTEM_SERIALIZATION_ENDIAN_HPP

#include <cstddef>
#include <iostream>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/data/uintx.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

/// data -> integral (explicit type), integral -> array (implicit size)
/// ---------------------------------------------------------------------------

template <typename Integer, if_integral_integer<Integer> = true>
Integer from_big_endian(const data_slice& data) noexcept;

template <typename Integer, if_integral_integer<Integer> = true>
Integer from_little_endian(const data_slice& data) noexcept;

template <typename Integer, if_integral_integer<Integer> = true>
byte_array<sizeof(Integer)> to_big_endian(Integer value) noexcept;

template <typename Integer, if_integral_integer<Integer> = true>
byte_array<sizeof(Integer)> to_little_endian(Integer value) noexcept;

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

/// data -> uintx (inferred size), uintx -> vector (inferred size)
/// ---------------------------------------------------------------------------

inline uintx from_big_endian(const data_slice& data) noexcept;

inline uintx from_little_endian(const data_slice& data) noexcept;

template <typename Integer, if_non_integral_integer<Integer> = true>
data_chunk to_big_endian(const Integer& value) noexcept;

template <typename Integer, if_non_integral_integer<Integer> = true>
data_chunk to_little_endian(const Integer& value) noexcept;

/// data -> uintx (explicit size), integer -> array (explicit size)
//// ---------------------------------------------------------------------------
/// Bytes is not required to match data size or Integer type implicit size.
/// Integer may be any integral, uintx or other integer type.

template <size_t Bytes>
uintx_t<to_bits(Bytes)> from_big_endian(const data_slice& data) noexcept;

template <size_t Bytes>
uintx_t<to_bits(Bytes)> from_little_endian(const data_slice& data) noexcept;

template <size_t Bytes, typename Integer, if_integer<Integer> = true>
byte_array<Bytes> to_big_endian(const Integer& value) noexcept;

template <size_t Bytes, typename Integer, if_integer<Integer> = true>
byte_array<Bytes> to_little_endian(const Integer& value) noexcept;

/// iterator (unknown size) -> integral (implicit size)
/// ---------------------------------------------------------------------------
/// Must be guarded by the caller, use when passing end would be redundant.

template <typename Integer, typename Iterator,
    if_integral_integer<Integer> = true>
Integer from_big_endian_unsafe(const Iterator& data) noexcept;

template <typename Integer, typename Iterator,
    if_integral_integer<Integer> = true>
Integer from_little_endian_unsafe(const Iterator& data) noexcept;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/serialization/endian.ipp>

#endif

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
#ifndef LIBBITCOIN_SYSTEM_SERIALIZATION_ENDIAN_HPP
#define LIBBITCOIN_SYSTEM_SERIALIZATION_ENDIAN_HPP

#include <cstddef>
#include <iostream>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/data/integer.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

/// data - integral integer (e.g. int32_t, uint64_t)
/// ---------------------------------------------------------------------------
/// Integer is an explicitly-required template parameter for "from" conversion.

template <typename Integer, if_integer<Integer> = true>
Integer from_big_endian(const data_slice& data);

template <typename Integer, if_integer<Integer> = true>
Integer from_little_endian(const data_slice& data);

template <typename Integer, if_integer<Integer> = true>
byte_array<sizeof(Integer)> to_big_endian(Integer value);

template <typename Integer, if_integer<Integer> = true>
byte_array<sizeof(Integer)> to_little_endian(Integer value);

/// stream - integral integer (e.g. int32_t, uint64_t)
/// ---------------------------------------------------------------------------
/// Integer is an explicitly-required template parameter for "from" conversion.

template <typename Integer, if_integer<Integer> = true>
Integer from_big_endian(std::istream& stream);

template <typename Integer, if_integer<Integer> = true>
Integer from_little_endian(std::istream& stream);

template <typename Integer, if_integer<Integer> = true>
void to_big_endian(std::ostream& stream, Integer value);

template <typename Integer, if_integer<Integer> = true>
void to_little_endian(std::ostream& stream, Integer value);

/// byte_array - uintx_t/integer (e.g. uint128_t, uint256_t)
/// ---------------------------------------------------------------------------
/// Bits is an explicitly-required template parameter for all conversion.
/// Bits determines the return value size (uintx<Bits> or byte_array<Bits/8>).
/// Bits is required to be byte aligned but not to match the Integer parameter.
/// Integer may be any copy-constructable type, including integral and uintx_t,
/// that implements bit shift 'operator>>=' and cast 'operator uint8_t()'.

template <size_t Bits, if_byte_aligned<Bits> = true>
uintx_t<Bits> from_big_endian(const data_slice& data);

template <size_t Bits, if_byte_aligned<Bits> = true>
uintx_t<Bits> from_little_endian(const data_slice& data);

template <size_t Bits, typename Integer, if_byte_aligned<Bits> = true>
byte_array<to_bytes(Bits)> to_big_endian(const Integer& value);

template <size_t Bits, typename Integer, if_byte_aligned<Bits> = true>
byte_array<to_bytes(Bits)> to_little_endian(const Integer& value);

/// iterator - integer
/// ---------------------------------------------------------------------------
/// Integer is an explicitly-required template parameter.

/// Must be guarded by the caller, use when passing end would be redundant.
template <typename Integer, typename Iterator, if_integer<Integer> = true>
Integer from_big_endian_unsafe(const Iterator& data);

/// Must be guarded by the caller, use when passing end would be redundant.
template <typename Integer, typename Iterator, if_integer<Integer> = true>
Integer from_little_endian_unsafe(const Iterator& data);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/serialization/endian.ipp>

#endif

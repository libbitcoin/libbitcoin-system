/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_DATA_BYTE_CAST_HPP
#define LIBBITCOIN_SYSTEM_DATA_BYTE_CAST_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

/// C++20: std::bit_cast is a copy, not a cast, these are true casts.
/// True casts (not projections) of bytes arrays to/from integrals with size
/// constraints to preclude referencing fractional elements. A const reference
/// parameter returns a const reference, non-const returns a non-const
/// reference, and an rvalue parameter returns a cast instance. These enable
/// C/pointer-style performance (no copies) with full type safety at no cost
/// (reinterpret_cast does not compile to any CPU instructions).

namespace libbitcoin {
namespace system {
    
/// Cast Integral& to same-sized array& of Byte.
/// ---------------------------------------------------------------------------
/// Size mismatch can be resolved using array_cast prior to byte_cast.

/// Cast integral& to a byte array& with byte length of the integral.
template <typename Byte = uint8_t, typename Integral,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true>
inline std_array<Byte, sizeof(Integral)>&
byte_cast(Integral& value) NOEXCEPT;

/// Cast const integral& to const byte array& with byte length of the integral.
template <typename Byte = uint8_t, typename Integral,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true>
inline const std_array<Byte, sizeof(Integral)>&
byte_cast(const Integral& value) NOEXCEPT;

/// Cast integral&& to byte array with byte length of the integral.
template <typename Byte = uint8_t, typename Integral,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true>
inline std_array<Byte, sizeof(Integral)>
byte_cast(Integral&& value) NOEXCEPT;

/// Cast array& of Byte to same-sized Integral&.
/// ---------------------------------------------------------------------------
/// Size mismatch can be resolved using array_cast prior to byte_cast.

/// Cast byte array& to unsigned integral& of same byte length.
template <typename Byte, size_t Size,
    if_one_byte<Byte> = true,
    if_integral_size<Size> = true>
inline unsigned_type<Size>&
byte_cast(std_array<Byte, Size>& bytes) NOEXCEPT;

/// Cast const byte array& to const unsigned integral& of same byte length.
template <typename Byte, size_t Size,
    if_one_byte<Byte> = true,
    if_integral_size<Size> = true>
inline const unsigned_type<Size>&
byte_cast(const std_array<Byte, Size>& bytes) NOEXCEPT;

/// Cast byte array&& to unsigned integral of same byte length.
template <typename Byte, size_t Size,
    if_one_byte<Byte> = true,
    if_integral_size<Size> = true>
inline unsigned_type<Size>
byte_cast(std_array<Byte, Size>&& bytes) NOEXCEPT;

/// Cast Byte* to Integral&.
/// ---------------------------------------------------------------------------
/// Safe if Byte count matches Integral size.

template <typename Integral, typename Byte,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true>
inline Integral& unsafe_byte_cast(Byte* bytes) NOEXCEPT;

template <typename Integral, typename Byte,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true>
inline const Integral& unsafe_byte_cast(const Byte* bytes) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/byte_cast.ipp>

#endif

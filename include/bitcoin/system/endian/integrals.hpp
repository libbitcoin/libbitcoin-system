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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_INTEGRALS_HPP
#define LIBBITCOIN_SYSTEM_ENDIAN_INTEGRALS_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

/// These constexpr implementations are more efficient than the integer
/// functions, as these are limited to integrals. However swaps are more
/// efficient than these and should be used for non-constexpr evaluation.
/// These are unique in that they allow for offsetting into the byte array.
/// This enables translation to/from an existing data array without needing
/// to array cast (which is not constexpr).

namespace libbitcoin {
namespace system {

/// to_big/to_little (by reference, implicit, offsettable).
/// ---------------------------------------------------------------------------

template <size_t Offset = zero, typename Byte, size_t Size, typename Integral,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true>
constexpr void to_big(std_array<Byte, Size>& data, Integral value) NOEXCEPT;

template <size_t Offset = zero, typename Byte, size_t Size, typename Integral,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true>
constexpr void to_little(std_array<Byte, Size>& data, Integral value) NOEXCEPT;

/// from_big/from_little (by reference, implicit, offsettable).
/// ---------------------------------------------------------------------------

template <size_t Offset = zero, typename Integral, typename Byte, size_t Size,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true>
constexpr void from_big(Integral& value,
    const std_array<Byte, Size>& data) NOEXCEPT;

template <size_t Offset = zero, typename Integral, typename Byte, size_t Size,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true>
constexpr void from_little(Integral& value,
    const std_array<Byte, Size>& data) NOEXCEPT;

/// from_big/from_little (by value, explicit, offsettable).
/// ---------------------------------------------------------------------------

template<typename Integral, size_t Offset = zero, typename Byte, size_t Size,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true>
constexpr Integral from_big(const std_array<Byte, Size>& data) NOEXCEPT;

template<typename Integral, size_t Offset = zero, typename Byte, size_t Size,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true>
constexpr Integral from_little(const std_array<Byte, Size>& data) NOEXCEPT;

/// to_big/to_little (by value, implicit).
/// ---------------------------------------------------------------------------

template <typename Integral,
    if_integral_integer<Integral> = true>
constexpr data_array<sizeof(Integral)> to_big(Integral value) NOEXCEPT;

template <typename Integral,
    if_integral_integer<Integral> = true>
constexpr data_array<sizeof(Integral)> to_little(Integral value) NOEXCEPT;

/// from_big/from_little (by value, implicit).
/// ---------------------------------------------------------------------------

template<place1 = place1{}, typename Byte, size_t Size,
    if_one_byte<Byte> = true>
constexpr unsigned_type<Size> from_big(
    const std_array<Byte, Size>& data) NOEXCEPT;

template<place1 = place1{}, typename Byte, size_t Size,
    if_one_byte<Byte> = true>
constexpr unsigned_type<Size> from_little(
    const std_array<Byte, Size>& data) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/endian/integrals.ipp>

#endif

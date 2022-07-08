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
#ifndef LIBBITCOIN_SYSTEM_MATH_BYTES_HPP
#define LIBBITCOIN_SYSTEM_MATH_BYTES_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Byte widths.
/// ---------------------------------------------------------------------------

/// The number of bytes required to store value (zero returns zero).
template <typename Integer, if_unsigned_integer<Integer> = true>
constexpr size_t byte_width(Integer value) NOEXCEPT;

/// The number of bytes required to store value.
/// Negative value always returns sizeof Value (zero returns zero).
template <typename Integer, if_signed_integer<Integer> = true>
constexpr size_t byte_width(Integer value) NOEXCEPT;

/// Endianness.
/// ---------------------------------------------------------------------------

/// Convert a native integral integer to big-endian.
template <typename Integral, if_integral_integer<Integral> = true>
constexpr Integral native_to_big_end(Integral big) NOEXCEPT;

/// Convert a native integral integer to little-endian.
template <typename Integral, if_integral_integer<Integral> = true>
constexpr Integral native_to_little_end(Integral big) NOEXCEPT;

/// Convert a big-endian integral integer to native.
template <typename Integral, if_integral_integer<Integral> = true>
constexpr Integral native_from_big_end(Integral big) NOEXCEPT;

/// Convert a little-endian integral integer to native.
template <typename Integral, if_integral_integer<Integral> = true>
constexpr Integral native_from_little_end(Integral little) NOEXCEPT;

/// Byteswap (platform independent byte reversal).
/// ---------------------------------------------------------------------------

template <typename Integral,
    if_integral_integer<Integral> = true,
    if_size_of<Integral, sizeof(uint8_t)> = true,
    if_unique_object_representations<Integral> = true>
constexpr Integral byteswap(Integral value) NOEXCEPT;

template <typename Integral,
    if_integral_integer<Integral> = true,
    if_size_of<Integral, sizeof(uint16_t)> = true,
    if_unique_object_representations<Integral> = true>
constexpr Integral byteswap(Integral value) NOEXCEPT;

template <typename Integral,
    if_integral_integer<Integral> = true,
    if_size_of<Integral, sizeof(uint32_t)> = true,
    if_unique_object_representations<Integral> = true>
constexpr Integral byteswap(Integral value) NOEXCEPT;

template <typename Integral,
    if_integral_integer<Integral> = true,
    if_size_of<Integral, sizeof(uint64_t)> = true,
    if_unique_object_representations<Integral> = true>
constexpr Integral byteswap(Integral value) NOEXCEPT;

/// Bits to bytes utilities.
/// ---------------------------------------------------------------------------

template <typename Integer, if_unsigned_integer<Integer> = true>
constexpr Integer to_ceilinged_bytes(Integer bits) NOEXCEPT;

template <typename Integer, if_unsigned_integer<Integer> = true>
constexpr Integer to_floored_bytes(Integer bits) NOEXCEPT;

/// Byte Negation.
/// ---------------------------------------------------------------------------

/// Get the high order bit of high order non-zero byte.
template <typename Integer,
    if_integer<Integer> = true>
constexpr bool is_negated(Integer value) NOEXCEPT;

/// Set high order bit.
template <typename Integer,
    if_integer<Integer> = true>
constexpr Integer to_negated(Integer value) NOEXCEPT;

/// Clear high order bit of high order non-zero byte.
template <typename Integer,
    if_signed_integer<Integer> = true>
constexpr Integer to_unnegated(Integer value) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/bytes.ipp>

#endif

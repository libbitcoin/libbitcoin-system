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
#ifndef LIBBITCOIN_SYSTEM_MATH_BYTES_HPP
#define LIBBITCOIN_SYSTEM_MATH_BYTES_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Byte width.
/// ---------------------------------------------------------------------------

/// The number of bytes required to store value (zero returns zero).
template <typename Integer, if_unsigned_integer<Integer> = true>
constexpr size_t byte_width(Integer value) NOEXCEPT;

/// The number of bytes required to store value.
/// Negative value always returns sizeof Value (zero returns zero).
template <typename Integer, if_signed_integer<Integer> = true>
constexpr size_t byte_width(Integer value) NOEXCEPT;

/// Bit count to byte count.
/// ---------------------------------------------------------------------------

template <typename Integer, if_unsigned_integer<Integer> = true>
constexpr Integer to_ceilinged_bytes(Integer bits) NOEXCEPT;

template <typename Integer, if_unsigned_integer<Integer> = true>
constexpr Integer to_floored_bytes(Integer bits) NOEXCEPT;

/// Byte of integral by logical index (index zero obtains low order byte).
/// ---------------------------------------------------------------------------

template <size_t Index, typename Byte = uint8_t, typename Integral,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true,
    if_lesser<Index, sizeof(Integral)> = true>
constexpr Byte byte(Integral value) NOEXCEPT;

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

/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_MATH_MULTIPLICATION_HPP
#define LIBBITCOIN_SYSTEM_MATH_MULTIPLICATION_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// left * right after cast of each to Explicit type.
/// Result defaults to greater of operand types.
/// Result size must not be lesser than either operand.
/// Use is_multiply_overflow<Explicit>(left, right) to guard.
template <place1 = place1{}, typename Integer,
    if_integral_integer<Integer> = true>
constexpr Integer multiply(Integer left, Integer right) NOEXCEPT;
template <typename Explicit, typename Integer,
    if_integral_integer<Integer> = true>
constexpr Explicit multiply(Integer left, Integer right) NOEXCEPT;
template <place2 = place2{}, typename Left, typename Right,
    if_not_same<Left, Right> = true,
    if_integral_integer<Left> = true,
    if_integral_integer<Right> = true>
constexpr to_greater_type<Left, Right> multiply(Left left, Right right) NOEXCEPT;
template <typename Explicit, typename Left, typename Right,
    if_not_same<Left, Right> = true,
    if_integral_integer<Left> = true,
    if_integral_integer<Right> = true>
constexpr Explicit multiply(Left left, Right right) NOEXCEPT;

/// Safely determine whether product is a whole multiple of value.
template <typename Integer,
    if_integer<Integer> = true>
constexpr bool is_multiple(Integer product, Integer value) NOEXCEPT;

/// Safely determine whether product is left * right.
template <typename Integer,
    if_integer<Integer> = true>
constexpr bool is_product(Integer product, Integer left,
    Integer right) NOEXCEPT;

/// Integer maximum if would overflow, otherwise the product.
template <typename Unsigned,
    if_unsigned_integer<Unsigned> = true>
constexpr Unsigned ceilinged_multiply(Unsigned left, Unsigned right) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/multiplication.ipp>

#endif

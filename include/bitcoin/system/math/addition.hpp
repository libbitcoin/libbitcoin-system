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
#ifndef LIBBITCOIN_SYSTEM_MATH_ADDITION_HPP
#define LIBBITCOIN_SYSTEM_MATH_ADDITION_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// left + right after cast of each to Explicit type.
/// Result defaults to greater of operand types.
/// Result size must not be lesser than either operand.
/// Use overflows<Result>(left, right) to guard.
template <place1 = place1{}, typename Integer,
    if_integral_integer<Integer> = true>
constexpr Integer add(Integer left, Integer right) NOEXCEPT;
template <typename Explicit, typename Integer,
    if_integral_integer<Integer> = true>
constexpr Explicit add(Integer left, Integer right) NOEXCEPT;
template <place2 = place2{}, typename Left, typename Right,
    if_not_same<Left, Right> = true,
    if_integral_integer<Left> = true,
    if_integral_integer<Right> = true>
constexpr to_greater_type<Left, Right> add(Left left, Right right) NOEXCEPT;
template <typename Explicit, typename Left, typename Right,
    if_not_same<Left, Right> = true,
    if_integral_integer<Left> = true,
    if_integral_integer<Right> = true>
constexpr Explicit add(Left left, Right right) NOEXCEPT;

/// left - right after cast of each to Explicit type.
/// Result defaults to greater of operand types.
/// Result size must not be lesser than either operand.
/// Use underflows<Result>(left, right) to guard.
template <place1 = place1{}, typename Integer,
    if_integral_integer<Integer> = true >
constexpr Integer subtract(Integer left, Integer right) NOEXCEPT;
template <typename Explicit, typename Integer,
    if_integral_integer<Integer> = true>
constexpr Explicit subtract(Integer left, Integer right) NOEXCEPT;
template <place2 = place2{}, typename Left, typename Right,
    if_not_same<Left, Right> = true,
    if_integral_integer<Left> = true,
    if_integral_integer<Right> = true>
constexpr to_greater_type<Left, Right> subtract(Left left, Right right) NOEXCEPT;
template <typename Explicit, typename Left, typename Right,
    if_not_same<Left, Right> = true,
    if_integral_integer<Left> = true,
    if_integral_integer<Right> = true>
constexpr Explicit subtract(Left left, Right right) NOEXCEPT;

/// Integer maximum if would overflow, otherwise the sum.
template <place1 = place1{}, typename Integer,
    if_signed_integral_integer<Integer> = true >
constexpr Integer ceilinged_add(Integer left, Integer right) NOEXCEPT;
template <place1 = place1{}, typename Integer,
    if_unsigned_integral_integer<Integer> = true >
constexpr Integer ceilinged_add(Integer left, Integer right) NOEXCEPT;
template <typename Explicit, typename Integer,
    if_integral_integer<Integer> = true>
constexpr Explicit ceilinged_add(Integer left, Integer right) NOEXCEPT;
template <place2 = place2{}, typename Left, typename Right,
    if_not_same<Left, Right> = true,
    if_integral_integer<Left> = true,
    if_integral_integer<Right> = true>
constexpr to_greater_type<Left, Right> ceilinged_add(Left left,
    Right right) NOEXCEPT;
template <typename Explicit, typename Left, typename Right,
    if_not_same<Left, Right> = true,
    if_integral_integer<Left> = true,
    if_integral_integer<Right> = true>
constexpr Explicit ceilinged_add(Left left, Right right) NOEXCEPT;

/// Integer minimum if would underflow, otherwise the difference.
template <place1 = place1{}, typename Integer,
    if_signed_integral_integer<Integer> = true >
constexpr Integer floored_subtract(Integer left, Integer right) NOEXCEPT;
template <place1 = place1{}, typename Integer,
    if_unsigned_integral_integer<Integer> = true >
constexpr Integer floored_subtract(Integer left, Integer right) NOEXCEPT;
template <typename Explicit, typename Integer,
    if_integral_integer<Integer> = true>
constexpr Explicit floored_subtract(Integer left, Integer right) NOEXCEPT;
template <place2 = place2{}, typename Left, typename Right,
    if_not_same<Left, Right> = true,
    if_integral_integer<Left> = true,
    if_integral_integer<Right> = true >
constexpr to_greater_type<Left, Right> floored_subtract(Left left,
    Right right) NOEXCEPT;
template <typename Explicit, typename Left, typename Right,
    if_not_same<Left, Right> = true,
    if_integral_integer<Left> = true,
    if_integral_integer<Right> = true>
constexpr Explicit floored_subtract(Left left, Right right) NOEXCEPT;

/// The term 'overflow' is used here to describe a sum that is above the
/// maximum or below the minimum of the integer domain. Similarly, 'underflow'
/// is used here to describe a difference that is above the maximum or below
/// the minimum of the integer domain. In other words, add overflows and
/// subtract underflows.

/// True if add would overflow.
template <typename Integer,
    if_signed_integral_integer<Integer> = true>
constexpr bool overflows(Integer left, Integer right) NOEXCEPT;

/// True if add would overflow.
template <typename Integer,
    if_unsigned_integral_integer<Integer> = true>
constexpr bool overflows(Integer left, Integer right) NOEXCEPT;

/// True if subtract would underflow.
template <typename Integer,
    if_signed_integral_integer<Integer> = true>
constexpr bool underflows(Integer left, Integer right) NOEXCEPT;

/// True if subtract would underflow.
template <typename Integer,
    if_unsigned_integral_integer<Integer> = true>
constexpr bool underflows(Integer left, Integer right) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/addition.ipp>

#endif

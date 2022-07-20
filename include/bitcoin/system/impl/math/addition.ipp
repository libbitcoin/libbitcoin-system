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
#ifndef LIBBITCOIN_SYSTEM_MATH_ADDITION_IPP
#define LIBBITCOIN_SYSTEM_MATH_ADDITION_IPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/math/overflow.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {
    
// add
// ----------------------------------------------------------------------------

// Single type.
// Prevents left type alone from being cast by first argument.
template <place1, typename Integer,
    if_integral_integer<Integer>>
constexpr Integer add(Integer left, Integer right) NOEXCEPT
{
    // is_add_overflow() can be used to guard this addition.
    return depromote<Integer>(promote(left) + promote(right));
}

// Single type explicit, possibly narrowing and/or sign changing.
template <typename Explicit, typename Integer,
    if_integral_integer<Integer>>
constexpr Explicit add(Integer left, Integer right) NOEXCEPT
{
    return add<place1{}, Explicit>(left, right);
}

// Implicit type (defaulted to larger operand, same signs).
template <place2, typename Left, typename Right,
    if_not_same<Left, Right>,
    if_integral_integer<Left>,
    if_integral_integer<Right>>
constexpr to_greater_type<Left, Right> add(Left left, Right right) NOEXCEPT
{
    using greater = to_greater_type<Left, Right>;
    return add<place1{}, greater>(
        possible_wide_cast<greater>(left),
        possible_wide_cast<greater>(right));
}

// Explicit type, possibly narrowing and/or sign changing.
template <typename Explicit, typename Left, typename Right,
    if_not_same<Left, Right>,
    if_integral_integer<Left>,
    if_integral_integer<Right>>
constexpr Explicit add(Left left, Right right) NOEXCEPT
{
    return add<place1{}, Explicit>(
        possible_narrow_and_sign_cast<Explicit>(left),
        possible_narrow_and_sign_cast<Explicit>(right));
}

// subtract
// ----------------------------------------------------------------------------

// Single type.
template <place1, typename Integer,
    if_integral_integer<Integer>>
constexpr Integer subtract(Integer left, Integer right) NOEXCEPT
{
    // is_underflow() can be used to guard this subtraction.
    return depromote<Integer>(promote(left) - promote(right));
}

// Single type explicit, possibly narrowing and/or sign changing.
template <typename Explicit, typename Integer,
    if_integral_integer<Integer>>
constexpr Explicit subtract(Integer left, Integer right) NOEXCEPT
{
    return subtract<place1{}, Explicit>(left, right);
}

// Implicit type (defaulted to larger operand, same signs).
template <place2, typename Left, typename Right,
    if_not_same<Left, Right>,
    if_integral_integer<Left>,
    if_integral_integer<Right>>
constexpr to_greater_type<Left, Right> subtract(Left left, Right right) NOEXCEPT
{
    using greater = to_greater_type<Left, Right>;
    return subtract<place1{}, greater>(
        possible_wide_cast<greater>(left),
        possible_wide_cast<greater>(right));
}

// Explicit type <Result>, possibly narrowing and/or sign changing.
template <typename Explicit, typename Left, typename Right,
    if_not_same<Left, Right>,
    if_integral_integer<Left>,
    if_integral_integer<Right>>
constexpr Explicit subtract(Left left, Right right) NOEXCEPT
{
    return subtract<place1{}, Explicit>(
        possible_narrow_and_sign_cast<Explicit>(left),
        possible_narrow_and_sign_cast<Explicit>(right));
}

// ceilinged_add
// ----------------------------------------------------------------------------

template <place1, typename Integer,
    if_signed_integral_integer<Integer>>
constexpr Integer ceilinged_add(Integer left, Integer right) NOEXCEPT
{
    return is_add_overflow(left, right) ? (is_negative(right) ?
        minimum<Integer> : maximum<Integer>) : add(left, right);
}

template <place1, typename Integer,
    if_unsigned_integral_integer<Integer>>
constexpr Integer ceilinged_add(Integer left, Integer right) NOEXCEPT
{
    return is_add_overflow(left, right) ? maximum<Integer> : add(left, right);
}

template <typename Explicit, typename Integer,
    if_integral_integer<Integer>>
constexpr Explicit ceilinged_add(Integer left, Integer right) NOEXCEPT
{
    return ceilinged_add<place1{}, Explicit>(left, right);
}

template <place2, typename Left, typename Right,
    if_not_same<Left, Right>,
    if_integral_integer<Left>,
    if_integral_integer<Right>>
constexpr to_greater_type<Left, Right> ceilinged_add(Left left,
    Right right) NOEXCEPT
{
    using greater = to_greater_type<Left, Right>;
    return ceilinged_add<place1{}, greater>(
        possible_wide_cast<greater>(left),
        possible_wide_cast<greater>(right));
}

template <typename Explicit, typename Left, typename Right,
    if_not_same<Left, Right>,
    if_integral_integer<Left>,
    if_integral_integer<Right>>
constexpr Explicit ceilinged_add(Left left, Right right) NOEXCEPT
{
    return ceilinged_add<place1{}, Explicit>(
        possible_narrow_and_sign_cast<Explicit>(left),
        possible_narrow_and_sign_cast<Explicit>(right));
}

// floored_subtract
// ----------------------------------------------------------------------------

template <place1, typename Integer,
    if_signed_integral_integer<Integer>>
constexpr Integer floored_subtract(Integer left, Integer right) NOEXCEPT
{
    return is_subtract_overflow(left, right) ? (is_negative(right) ?
        maximum<Integer> : minimum<Integer>) : subtract(left, right);
}

template <place1, typename Integer,
    if_unsigned_integral_integer<Integer>>
constexpr Integer floored_subtract(Integer left, Integer right) NOEXCEPT
{
    return is_subtract_overflow(left, right) ? minimum<Integer> :
        subtract(left, right);
}

template <typename Explicit, typename Integer,
    if_integral_integer<Integer>>
constexpr Explicit floored_subtract(Integer left, Integer right) NOEXCEPT
{
    return floored_subtract<place1{}, Explicit>(left, right);
}

template <place2, typename Left, typename Right,
    if_not_same<Left, Right>,
    if_integral_integer<Left>,
    if_integral_integer<Right>>
constexpr to_greater_type<Left, Right> floored_subtract(Left left,
    Right right) NOEXCEPT
{
    using greater = to_greater_type<Left, Right>;
    return floored_subtract<place1{}, greater>(
        possible_wide_cast<greater>(left),
        possible_wide_cast<greater>(right));
}

template <typename Explicit, typename Left, typename Right,
    if_not_same<Left, Right>,
    if_integral_integer<Left>,
    if_integral_integer<Right>>
constexpr Explicit floored_subtract(Left left, Right right) NOEXCEPT
{
    return floored_subtract<place1{}, Explicit>(
        possible_narrow_and_sign_cast<Explicit>(left),
        possible_narrow_and_sign_cast<Explicit>(right));
}

} // namespace system
} // namespace libbitcoin

#endif

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
#ifndef LIBBITCOIN_SYSTEM_MATH_MULTIPLICATION_IPP
#define LIBBITCOIN_SYSTEM_MATH_MULTIPLICATION_IPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/cast.hpp>
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/math/overflow.hpp>

namespace libbitcoin {
namespace system {

// multiply
// ----------------------------------------------------------------------------

// Single type.
// Prevents left type alone from being cast by first argument.
template <place1, typename Integer,
    if_integral_integer<Integer>>
constexpr Integer multiply(Integer left, Integer right) NOEXCEPT
{
    // is_multiply_overflow() can be used to guard this multiplication.
    return depromote<Integer>(promote(left) * promote(right));
}

// Single type explicit, possibly narrowing and/or sign changing.
template <typename Explicit, typename Integer,
    if_integral_integer<Integer>>
constexpr Explicit multiply(Integer left, Integer right) NOEXCEPT
{
    return multiply<place1{}, Explicit>(left, right);
}

// Implicit type (defaulted to larger operand, same signs).
template <place2, typename Left, typename Right,
    if_not_same<Left, Right>,
    if_integral_integer<Left>,
    if_integral_integer<Right>>
constexpr to_greater_type<Left, Right> multiply(Left left, Right right) NOEXCEPT
{
    using greater = to_greater_type<Left, Right>;
    return multiply<place1{}, greater>(
        possible_wide_cast<greater>(left),
        possible_wide_cast<greater>(right));
}

// Explicit type, possibly narrowing and/or sign changing.
template <typename Explicit, typename Left, typename Right,
    if_not_same<Left, Right>,
    if_integral_integer<Left>,
    if_integral_integer<Right>>
constexpr Explicit multiply(Left left, Right right) NOEXCEPT
{
    return multiply<place1{}, Explicit>(
        possible_narrow_and_sign_cast<Explicit>(left),
        possible_narrow_and_sign_cast<Explicit>(right));
}

// other
// ----------------------------------------------------------------------------

template <typename Integer, if_integer<Integer>>
constexpr bool is_multiple(Integer product, Integer value) NOEXCEPT
{
    return is_nonzero(value) ? is_zero(product % value) : is_zero(product);
}

template <typename Integer, if_integer<Integer>>
constexpr bool is_product(Integer product, Integer left,
    Integer right) NOEXCEPT
{
    return (is_zero(left) || is_zero(right)) ? is_zero(product) :
        is_zero(product % left) && ((product / left) == right);
}

template <typename Unsigned, if_unsigned_integer<Unsigned>>
constexpr Unsigned ceilinged_multiply(Unsigned left, Unsigned right) NOEXCEPT
{
    return is_multiply_overflow(left, right) ? maximum<Unsigned> :
        depromote<Unsigned>(left * right);
}

} // namespace system
} // namespace libbitcoin

#endif

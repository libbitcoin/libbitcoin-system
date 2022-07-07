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
#ifndef LIBBITCOIN_SYSTEM_MATH_SIGN_IPP
#define LIBBITCOIN_SYSTEM_MATH_SIGN_IPP

#include <limits>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/cast.hpp>

namespace libbitcoin {
namespace system {

// Integer Representations:
//
// Unsigned to signed is of the same size is non-narrowing, but signed to
// unsigned of the same size is narrowing. The minimum signed value cannot be
// represented in the unsigned domain of the same bit width. Simply put, this
// is because there is not a negative zero, allowing space for one more value.
//
// This is the nature of twos complement vs. ones complement. In ones
// complement, all bits in the positive domain are inverted to obtained the
// corresponding negative value. This results in a negative zero. By adding one
// to the ones complement, twos complement consumes this negative zero value,
// and therefore has a negative range of one more than its positive range
// (which retains the zero representation).

// Coversions.
// ----------------------------------------------------------------------------

// absolute(minimum<Signed>) overflows to add1(maximum<Signed>) for all types.
template <typename Signed, if_signed_integer<Signed>>
constexpr to_unsigned_type<Signed> absolute(Signed value) NOEXCEPT
{
    // std::abs is signed types only and not constexpr until C++23 (avoid).
    return to_unsigned(is_negative(value) ? negate(value) : value);
}

template <typename Unsigned, if_unsigned_integer<Unsigned>>
constexpr Unsigned absolute(Unsigned value) NOEXCEPT
{
    return value;
}

// negate(minimum<Signed>) overflows to add1(maximum<Signed>) for all types.
template <typename Signed, if_signed_integer<Signed>>
constexpr Signed negate(Signed value) NOEXCEPT
{
    return possible_narrow_and_sign_cast<Signed>(-maximal_cast(value));
}

// C++20: requires twos complement integer representation.
// So negate<unsigned> is now the same as twos_complement.
template <typename Unsigned, if_unsigned_integer<Unsigned>>
constexpr Unsigned negate(Unsigned value) NOEXCEPT
{
    return twos_complement(value);
}

template <typename Value, if_integer<Value>>
constexpr Value twos_complement(Value value) NOEXCEPT
{
    // Flows from minimum to ~minimum (-1) to +1 => zero.
    return add1(ones_complement(value));
}

template <typename Value, if_integer<Value>>
constexpr Value ones_complement(Value value) NOEXCEPT
{
    // Alias for bit_not.
    return possible_narrow_and_sign_cast<Value>(~maximal_cast(value));
}

// Comparisons.
// ----------------------------------------------------------------------------

template <typename Integer,
    if_signed_integer<Integer>>
constexpr bool is_negative(Integer value) NOEXCEPT
{
    return value < 0;
}

template <typename Integer,
    if_unsigned_integer<Integer>>
constexpr bool is_negative(Integer) NOEXCEPT
{
    return false;
}

template <typename Left, typename Right,
    if_same_signed_integer<Left, Right>>
constexpr bool is_greater(Left left, Right right) NOEXCEPT
{
    return left > right;
}

template <typename Left, typename Right,
    if_unsigned_integer<Left>, if_signed_integer<Right>>
constexpr bool is_greater(Left left, Right right) NOEXCEPT
{
    return is_negative(right) || (left > to_unsigned(right));
}

template <typename Left, typename Right,
    if_signed_integer<Left>, if_unsigned_integer<Right>>
constexpr bool is_greater(Left left, Right right) NOEXCEPT
{
     return !is_negative(left) && (right < to_unsigned(left));
}

template <typename Left, typename Right,
    if_same_signed_integer<Left, Right>>
constexpr bool is_lesser(Left left, Right right) NOEXCEPT
{
    return left < right;
}

template <typename Left, typename Right,
    if_signed_integer<Left>, if_unsigned_integer<Right>>
constexpr bool is_lesser(Left left, Right right) NOEXCEPT
{
    return is_negative(left) || (to_unsigned(left) < right);
}

template <typename Left, typename Right,
    if_unsigned_integer<Left>, if_signed_integer<Right>>
constexpr bool is_lesser(Left left, Right right) NOEXCEPT
{
    return !is_negative(right) && (to_unsigned(right) > left);
}

// Types cannot be cast by first argument.
template <place1, typename Left, typename Right,
    if_same_signed_integer<Left, Right>>
constexpr to_greater_type<Left, Right> greater(Left left, Right right) NOEXCEPT
{
    return is_greater(left, right) ? left : right;
}

// Types cannot be cast by first argument.
template <place1, typename Left, typename Right,
    if_same_signed_integer<Left, Right>>
constexpr to_greater_type<Left, Right> lesser(Left left, Right right) NOEXCEPT
{
    return is_lesser(left, right) ? left : right;
}

template <typename Result, typename Left, typename Right,
    if_not_lesser_size<Result, Left>,
    if_not_lesser_size<Result, Right>,
    if_not_same_signed_integer<Left, Right>>
constexpr Result greater(Left left, Right right) NOEXCEPT
{
    return possible_sign_cast<Result>(is_greater(left, right) ? left : right);
}

template <typename Result, typename Left, typename Right,
    if_not_lesser_size<Result, Left>,
    if_not_lesser_size<Result, Right>,
    if_not_same_signed_integer<Left, Right>>
constexpr Result lesser(Left left, Right right) NOEXCEPT
{
    return possible_sign_cast<Result>(is_lesser(left, right) ? left : right);
}

} // namespace system
} // namespace libbitcoin

#endif

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

// DELETEMENOW
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/safe.hpp>

namespace libbitcoin {
namespace system {

// Integer Representations:
//
// Unsigned to signed is of the same size is non-narrowing, but signed to
// unsigned of the same size is narrowing. The minimum signed value cannot be
// represented in the unsigned domain of the same bit width. Fundamentally this
// is because there is not a negative zero, allowing space for one more value.
//
// This is the nature of twos complement vs. ones complement. In ones
// complement, all bits in the positive domain are inverted to obtained the
// corresponding negative value. This results in a negative zero. By adding one
// to the ones complement, twos complement consumes this negative zero value,
// and therefore has a magnitude of one more than the unsigned domain (which
// retains the zero representation).

// C++20: requires twos complement integer representation.
// This makes negate<signed> twos_complement, and negate<unsigned> is defined
// as twos_complement. So these are conceptually identical at C++20 (required).
// This library implements twos complement with safe overflow behavior.

// if negate or absolute is called with the domain minimum value, this aborts
// the process, as this is undefined behavior. terminate_minimum is elided as a
// no-op in non-constexpr and non-debug builds.

// std::abs is signed types only and not constexpr until C++23 (avoid).
// All operations below support signed and unsigned integer parameters.
// Use depromote only for integral constrained functions.
// Unary operators do not promote sign.

// Coversions.
// ----------------------------------------------------------------------------

// unsafe
template <typename Integer, typename Result, if_signed_integer<Integer>>
DEPRECATED constexpr Result absolute(Integer value) NOEXCEPT
{
    return to_unsigned(is_negative(value) ? negate(value) : value);
}

template <typename Integer, if_unsigned_integer<Integer>>
constexpr Integer absolute(Integer value) NOEXCEPT
{
    return value;
}

// unsafe
template <typename Integer, if_signed_integer<Integer>>
DEPRECATED constexpr Integer negate(Integer value) NOEXCEPT
{
    return safe_negate(value);
}

template <typename Integer, if_unsigned_integer<Integer>>
constexpr Integer negate(Integer value) NOEXCEPT
{
    return add1(ones_complement(value));
}

template <typename Value, if_integer<Value>>
constexpr Value twos_complement(Value value) NOEXCEPT
{
    // Overflows from minimum to ~minimum (-1) to +1 => zero.
    return add1(ones_complement(value));
}

template <typename Value, if_integer<Value>>
constexpr Value ones_complement(Value value) NOEXCEPT
{
    // Alias for bit_not.
    return possible_narrow_and_sign_cast<Value>(~value);
}

template <typename Integer, typename Signed, if_integer<Integer>>
constexpr Signed to_signed(Integer value) NOEXCEPT
{
    return possible_sign_cast<Signed>(value);
}

template <typename Integer, typename Unsigned, if_integer<Integer>>
constexpr Unsigned to_unsigned(Integer value) NOEXCEPT
{
    return possible_sign_cast<Unsigned>(value);
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

template<typename Result, typename Left, typename Right,
    if_integer<Left>, if_integer<Right>>
constexpr Result greater(Left left, Right right) NOEXCEPT
{
    // Precludes Result narrower than either operand.
    return possible_sign_cast<Result>(is_greater(left, right) ? left : right);
}

template<typename Result, typename Left, typename Right,
    if_integer<Left>, if_integer<Right>>
constexpr Result lesser(Left left, Right right) NOEXCEPT
{
    // Precludes Result narrower than either operand.
    return possible_sign_cast<Result>(is_lesser(left, right) ? left : right);
}

} // namespace system
} // namespace libbitcoin

#endif

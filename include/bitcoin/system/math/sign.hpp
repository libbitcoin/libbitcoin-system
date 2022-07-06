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
#ifndef LIBBITCOIN_SYSTEM_MATH_SIGN_HPP
#define LIBBITCOIN_SYSTEM_MATH_SIGN_HPP

#include <type_traits>
/// DELETEMENOW
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// If negate/absolute is called with the domain minimum value, this aborts the
/// process, as this is undefinable behavior in twos complement representation.

/// Conversions.
/// ---------------------------------------------------------------------------

/// Signed absolute value (unsafe), converts to unsigned.
template <typename Signed, if_signed_integer<Signed> = true>
constexpr to_unsigned_type<Signed> absolute(Signed value) NOEXCEPT;

/// Unsigned absolute value (no-op).
template <typename Unsigned, if_unsigned_integer<Unsigned> = true>
constexpr Unsigned absolute(Unsigned value) NOEXCEPT;

/// Signed integer negation (unsafe), does not invert sign.
template <typename Signed, if_signed_integer<Signed> = true>
constexpr Signed negate(Signed value) NOEXCEPT;

/// Unsigned integer two's complement (safe).
template <typename Unsigned, if_unsigned_integer<Unsigned> = true>
constexpr Unsigned negate(Unsigned value) NOEXCEPT;

/// Ones complement, alias for bit_not (safe).
template <typename Value, if_integer<Value> = true>
constexpr Value ones_complement(Value value) NOEXCEPT;

/// Twos complement, overflows from minimum to zero (safe).
template <typename Value, if_integer<Value> = true>
constexpr Value twos_complement(Value value) NOEXCEPT;

/// Comparisons.
/// ---------------------------------------------------------------------------

/// Determine whether the integer is negative.
template <typename Integer,
    if_signed_integer<Integer> = true>
constexpr bool is_negative(Integer value) NOEXCEPT;
template <typename Integer,
    if_unsigned_integer<Integer> = true>
constexpr bool is_negative(Integer) NOEXCEPT;

/// Use !is_lesser(left, right) for (left >= right).
/// Determine whether the left integer is greater than the right.
template <typename Left, typename Right,
    if_same_signed_integer<Left, Right> = true>
constexpr bool is_greater(Left left, Right right) NOEXCEPT;
template <typename Left, typename Right,
    if_unsigned_integer<Left> = true,
    if_signed_integer<Right> = true>
constexpr bool is_greater(Left left, Right right) NOEXCEPT;
template <typename Left, typename Right,
    if_signed_integer<Left> = true,
    if_unsigned_integer<Right> = true>
constexpr bool is_greater(Left left, Right right) NOEXCEPT;

/// Use !is_greater(left, right) for (left <= right).
/// Determine whether the left integer is less than the right.
template <typename Left, typename Right,
    if_same_signed_integer<Left, Right> = true>
constexpr bool is_lesser(Left left, Right right) NOEXCEPT;
template <typename Left, typename Right,
    if_signed_integer<Left> = true,
    if_unsigned_integer<Right> = true>
constexpr bool is_lesser(Left left, Right right) NOEXCEPT;
template <typename Left, typename Right,
    if_unsigned_integer<Left> = true,
    if_signed_integer<Right> = true>
constexpr bool is_lesser(Left left, Right right) NOEXCEPT;

/// Return greater/lesser of two same-signed integrals, as the wider type.
template <place1 = place1{}, typename Left, typename Right,
    if_same_signed_integer<Left, Right> = true>
constexpr to_greater_type<Left, Right> greater(Left left, Right right) NOEXCEPT;
template <place1 = place1{}, typename Left, typename Right,
    if_same_signed_integer<Left, Right> = true>
constexpr to_greater_type<Left, Right> lesser(Left left, Right right) NOEXCEPT;

/// Return greater/lesser of two opposite-sign integers, cast to the Result.
/// Result type may not be of lesser size than either operand (non-narrowing).
template<typename Result, typename Left, typename Right,
    if_not_lesser_size<Result, Left> = true,
    if_not_lesser_size<Result, Right> = true,
    if_not_same_signed_integer<Left, Right> = true>
constexpr Result greater(Left left, Right right) NOEXCEPT;
template<typename Result, typename Left, typename Right,
    if_not_lesser_size<Result, Left> = true,
    if_not_lesser_size<Result, Right> = true,
    if_not_same_signed_integer<Left, Right> = true>
constexpr Result lesser(Left left, Right right) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/sign.ipp>

#endif


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
#ifndef LIBBITCOIN_SYSTEM_MATH_SAFE_HPP
#define LIBBITCOIN_SYSTEM_MATH_SAFE_HPP

#include <bitcoin/system/constraints.hpp>

namespace libbitcoin {
namespace system {

/// Explicit integral casts.

/// Sign may be changed on promotion by binary operators.
/// Sign will not be changed by unary ops (-) unless size is also promoted.
/// This is a narrowing cast to restore Restored type from any native operation
/// (type of Common) resulting in at least decltype(+Restored{}) size.
/// This should not be used except in the case of possible integral promotion.
template <typename Restored, typename Common,
    if_not_lesser_width<to_common_sized_type<Restored>, Restored> = true,
    if_integral_integer<Restored> = true,
    if_integral_integer<Common> = true>
constexpr Restored depromote(Common value) noexcept;

/// Cast integral to integral of narrower bit width.
template <typename To, typename From,
    if_lesser_width<To, From> = true,
    if_integral_integer<To> = true,
    if_integral_integer<From> = true,
    if_same_signed_integer<To, From> = true>
constexpr To narrow_cast(From value) noexcept;

/// Cast integral sign (non-narrowing).
template <typename To, typename From,
    if_not_lesser_width<To, From> = true,
    if_integral_integer<To> = true,
    if_integral_integer<From> = true,
    if_not_same_signed_integer<To, From> = true>
constexpr To sign_cast(From value) noexcept;

/// Cast integral sign to integral of narrower bit width.
template <typename To, typename From,
    if_lesser_width<To, From> = true,
    if_integral_integer<To> = true,
    if_integral_integer<From> = true,
    if_not_same_signed_integer<To, From> = true>
constexpr To narrow_sign_cast(From value) noexcept;

/// Cast integral to integral of wider bit width (precludes reassignment).
template <typename To, typename From,
    if_lesser_width<From, To> = true,
    if_same_signed_integer<To, From> = true>
constexpr To wide_cast(From value) noexcept;

/// Possible integer casts.

/// Possible narrowing without sign cast.
template <typename To, typename From,
    if_same_signed_integer<To, From> = true>
constexpr To possible_narrow_cast(From value) noexcept;

/// Possible sign cast without narrowing.
template <typename To, typename From,
    if_not_lesser_width<To, From> = true>
constexpr To possible_sign_cast(From value) noexcept;

/// Possible narrowing with sign cast.
template <typename To, typename From,
    if_not_same_signed_integer<To, From> = true>
constexpr To possible_narrow_sign_cast(From value) noexcept;

/// Possible sign cast with narrowing.
template <typename To, typename From,
    if_lesser_width<To, From> = true>
constexpr To possible_sign_narrow_cast(From value) noexcept;

/// Possible narrowing and possible sign cast (unrestricted).
template <typename To, typename From>
constexpr To possible_narrow_and_sign_cast(From value) noexcept;

/// Possible widening without sign cast (precludes reassignment).
template <typename To, typename From, if_same_signed_integer<To, From> = true>
constexpr To possible_wide_cast(From value) noexcept;

/// Explicit pointer casts.

/// Cast of pointer.
template <typename To, typename From>
constexpr To* pointer_cast(From* value) noexcept;

/// Possible cast of pointer.
template <typename To, typename From>
constexpr To* possible_pointer_cast(From* value) noexcept;

/// Cast integer to pointer.
template <typename To, typename From>
constexpr To* integer_pointer_cast(From value) noexcept;

/// Mathematical operations that throw on overflow.
/// Avoid structured exception handling, use only to cause program abort.

/// Throws overflow_exception on overflow (2 uses in libbitcoin).
template <typename Integer, if_unsigned_integer<Integer> = true>
Integer safe_multiply(Integer left, Integer right) noexcept(false);

/// Throws overflow_exception on overflow (1 use in libbitcoin).
template <typename Integer, if_unsigned_integer<Integer> = true>
Integer safe_add(Integer left, Integer right) noexcept(false);

template <typename Integer,
    if_signed_integer<Integer> = true>
constexpr void terminate_minimum(Integer value) noexcept;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/safe.ipp>

#endif

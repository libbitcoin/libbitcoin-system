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
#ifndef LIBBITCOIN_SYSTEM_MATH_CAST_HPP
#define LIBBITCOIN_SYSTEM_MATH_CAST_HPP

#include <array>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Explicit integral casts.
/// ---------------------------------------------------------------------------

/// Sign may be changed on promotion by binary operators.
/// Sign will not (?) be changed by unary ops (-) unless size is also promoted.
/// This is a narrowing cast to restore Restored type after a native operation
template <typename Restored, typename Common,
    if_common<Common> = true,
    if_integral_integer<Restored> = true,
    if_integral_integer<Common> = true>
constexpr Restored depromote(Common value) NOEXCEPT;

/// Cast integral to integral of narrower bit width.
template <typename To, typename From,
    if_lesser_size<To, From> = true,
    if_same_signed_integral_integer<To, From> = true>
constexpr To narrow_cast(From value) NOEXCEPT;

/// Cast integral sign (non-narrowing).
template <typename To, typename From,
    if_not_lesser_size<To, From> = true,
    if_not_same_signed_integral_integer<To, From> = true>
constexpr To sign_cast(From value) NOEXCEPT;

/// Cast integral sign to integral of narrower bit width.
template <typename To, typename From,
    if_lesser_size<To, From> = true,
    if_not_same_signed_integral_integer<To, From> = true>
constexpr To narrow_sign_cast(From value) NOEXCEPT;

/// Promote integral to integral of wider bit width.
template <typename To, typename From,
    if_greater_size<To, From> = true,
    if_same_signed_integral_integer<To, From> = true>
constexpr To wide_cast(From value) NOEXCEPT;

/// Promote integral to integral of widest bit width.
template <typename From,
    if_integral_integer<From> = true>
constexpr to_maximal_type<From> maximal_cast(From value) NOEXCEPT;

/// Possible integer casts.
/// ---------------------------------------------------------------------------

/// Possible narrowing without sign cast.
template <typename To, typename From,
    if_same_signed_integer<To, From> = true>
constexpr To possible_narrow_cast(From value) NOEXCEPT;

/// Possible sign cast without narrowing.
template <typename To, typename From,
    if_not_lesser_size<To, From> = true>
constexpr To possible_sign_cast(From value) NOEXCEPT;

/// Possible narrowing with sign cast.
template <typename To, typename From,
    if_not_same_signed_integer<To, From> = true>
constexpr To possible_narrow_sign_cast(From value) NOEXCEPT;

/// Possible sign cast with narrowing.
template <typename To, typename From,
    if_lesser_size<To, From> = true>
constexpr To possible_sign_narrow_cast(From value) NOEXCEPT;

/// Possible narrowing and possible sign cast (unrestricted).
template <typename To, typename From>
constexpr To possible_narrow_and_sign_cast(From value) NOEXCEPT;

/// Possible promotion (without sign change).
template <typename To, typename From,
    if_not_lesser_size<To, From> = true,
    if_same_signed_integer<To, From> = true>
constexpr To possible_wide_cast(From value) NOEXCEPT;

/// Explicit pointer casts.
/// ---------------------------------------------------------------------------

/// Cast of pointer.
template <typename To, typename From>
inline To* pointer_cast(From* value) NOEXCEPT;

/// Possible cast of pointer.
template <typename To, typename From>
inline To* possible_pointer_cast(From* value) NOEXCEPT;

/// Sign casts.
/// ---------------------------------------------------------------------------

/// Cast signed/unsigned to signed type of corresponding size.
template <typename Signed, if_signed_integer<Signed> = true>
constexpr Signed to_signed(Signed value) NOEXCEPT;
template <typename Unsigned, if_unsigned_integer<Unsigned> = true>
constexpr to_signed_type<Unsigned> to_signed(Unsigned value) NOEXCEPT;

/// Cast signed/unsigned to unsigned type of corresponding size.
template <typename Signed, if_signed_integer<Signed> = true>
constexpr to_unsigned_type<Signed> to_unsigned(Signed value) NOEXCEPT;
template <typename Unsigned, if_unsigned_integer<Unsigned> = true>
constexpr Unsigned to_unsigned(Unsigned value) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/cast.ipp>

#endif

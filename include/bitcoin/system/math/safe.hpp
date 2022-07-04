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

#include <array>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Explicit integral casts.
/// ---------------------------------------------------------------------------

/// Sign may be changed on promotion by binary operators.
/// Sign will not (?) be changed by unary ops (-) unless size is also promoted.
/// This is a narrowing cast to restore Restored type from any native operation
/// (type of Common) resulting in at least decltype(+Restored{}) size.
/// This should not be used except in the case of integral only promotion as
/// its use will preclude non-integral template matching.
template <typename Restored, typename Common,
    if_not_lesser_size<to_common_sized_type<Restored>, Restored> = true,
    if_integral_integer<Restored> = true,
    if_integral_integer<Common> = true>
constexpr Restored depromote(Common value) NOEXCEPT;

/// Cast integral to integral of narrower bit width.
template <typename To, typename From,
    if_lesser_size<To, From> = true,
    if_integral_integer<To> = true,
    if_integral_integer<From> = true,
    if_same_signed_integer<To, From> = true>
constexpr To narrow_cast(From value) NOEXCEPT;

/// Cast integral sign (non-narrowing).
template <typename To, typename From,
    if_not_lesser_size<To, From> = true,
    if_integral_integer<To> = true,
    if_integral_integer<From> = true,
    if_not_same_signed_integer<To, From> = true>
constexpr To sign_cast(From value) NOEXCEPT;

/// Cast integral sign to integral of narrower bit width.
template <typename To, typename From,
    if_lesser_size<To, From> = true,
    if_integral_integer<To> = true,
    if_integral_integer<From> = true,
    if_not_same_signed_integer<To, From> = true>
constexpr To narrow_sign_cast(From value) NOEXCEPT;

/// Cast integral to integral of wider bit width (precludes reassignment).
template <typename To, typename From,
    if_lesser_size<From, To> = true,
    if_same_signed_integer<To, From> = true>
constexpr To wide_cast(From value) NOEXCEPT;

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

/// Possible widening without sign cast (precludes reassignment).
template <typename To, typename From, if_same_signed_integer<To, From> = true>
constexpr To possible_wide_cast(From value) NOEXCEPT;

/// Explicit pointer casts (reiterpret_cast is not constexpr).
/// ---------------------------------------------------------------------------

/// Cast of pointer.
template <typename To, typename From>
inline To* pointer_cast(From* value) NOEXCEPT;

/// Possible cast of pointer.
template <typename To, typename From>
inline To* possible_pointer_cast(From* value) NOEXCEPT;

/// Byte casts (require pointer_cast, not constexpr).
/// ---------------------------------------------------------------------------
/// C++20: std::bit_cast is a copy, not a cast, these are true casts.

/// Cast integral& to a data array& with byte length of the integral.
template <typename Integral,
    if_integral_integer<Integral> = true>
constexpr std::array<uint8_t, sizeof(Integral)>&
byte_cast(Integral& value) NOEXCEPT;

/// Cast const integral& to a const data array& with byte length of the integral.
template <typename Integral,
    if_integral_integer<Integral> = true>
constexpr const std::array<uint8_t, sizeof(Integral)>&
byte_cast(const Integral& value) NOEXCEPT;

/// Cast data array& to unsigned integral& of same byte length.
template <size_t Size,
    if_integral_size<Size> = true>
constexpr unsigned_type<Size>&
byte_cast(std::array<uint8_t, Size>& value) NOEXCEPT;

/// Cast const data array& to const unsigned integral& of same byte length.
template <size_t Size,
    if_integral_size<Size> = true>
constexpr const unsigned_type<Size>&
byte_cast(const std::array<uint8_t, Size>& value) NOEXCEPT;

/// Array casts (require pointer_cast, not constexpr).
/// ---------------------------------------------------------------------------

/// Cast array& of integrals to same-sized array& of integrals.
template <typename To, size_t Size, typename From,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true,
    if_not_lesser<max_size_t / sizeof(From), Size> = true>
constexpr std::array<To, (Size * sizeof(From)) / sizeof(To)>&
array_cast(std::array<From, Size>& values) NOEXCEPT;

/// Cast const array& of integrals to same-sized const array& of integrals.
template <typename To, size_t Size, typename From,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true,
    if_not_lesser<max_size_t / sizeof(From), Size> = true>
constexpr const std::array<To, (Size * sizeof(From)) / sizeof(To)>&
array_cast(const std::array<From, Size>& values) NOEXCEPT;

/// Cast array& of integrals to lesser-sized array& of integrals.
/// Use array_cast for non-narrowing array cast.
template <typename To, size_t ToSize, typename From, size_t FromSize,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true,
    if_not_lesser<max_size_t / sizeof(To), ToSize> = true,
    if_not_lesser<max_size_t / sizeof(From), FromSize> = true,
    if_lesser<ToSize * sizeof(To), FromSize * sizeof(From)> = true>
constexpr std::array<To, ToSize>&
narrowing_array_cast(std::array<From, FromSize>& values) NOEXCEPT;

/// Cast const array& of integrals to lesser-sized const array& of integrals.
/// Use array_cast for non-narrowing array cast.
template <typename To, size_t ToSize, typename From, size_t FromSize,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true,
    if_not_lesser<max_size_t / sizeof(To), ToSize> = true,
    if_not_lesser<max_size_t / sizeof(From), FromSize> = true,
    if_lesser<ToSize * sizeof(To), FromSize * sizeof(From)> = true>
constexpr const std::array<To, ToSize>&
narrowing_array_cast(const std::array<From, FromSize>& values) NOEXCEPT;

/// Cast contiguous integral buffer to array& of integrals.
/// Safe if byte count (size) is correct.
template <typename To, size_t Size, typename From,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true>
constexpr std::array<To, Size>&
unsafe_array_cast(From bytes[]) NOEXCEPT;

/// Cast contiguous const integral buffer to const array& of integrals.
/// Safe if byte count (size) is correct.
template <typename To, size_t Size, typename From,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true>
constexpr const std::array<To, Size>&
unsafe_array_cast(const From bytes[]) NOEXCEPT;

/// Overflows.
/// ---------------------------------------------------------------------------
/// Avoid structured exception handling, use only to cause program abort.

/// Throws overflow_exception on overflow (2 uses in libbitcoin).
template <typename Integer, if_unsigned_integer<Integer> = true>
DEPRECATED constexpr Integer safe_multiply(Integer left, Integer right) THROWS;

/// Throws overflow_exception on overflow (1 use in libbitcoin).
template <typename Integer, if_unsigned_integer<Integer> = true>
DEPRECATED constexpr Integer safe_add(Integer left, Integer right) THROWS;

/// Throws overflow_exception on negate signed minimum (several uses).
template <typename Integer, if_signed_integer<Integer> = true>
DEPRECATED constexpr Integer safe_negate(Integer value) THROWS;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/safe.ipp>

#endif

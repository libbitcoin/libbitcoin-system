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
#ifndef LIBBITCOIN_SYSTEM_MATH_OVERFLOW_HPP
#define LIBBITCOIN_SYSTEM_MATH_OVERFLOW_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
    
/// add/subtract.
/// ---------------------------------------------------------------------------
// TODO: generalize is_overflow() and is_underflow() argument types.

template <typename Signed,
    if_signed_integral_integer<Signed> = true>
constexpr bool is_negate_overflow(Signed value) NOEXCEPT;

/// True if add would overflow Integer domain.
template <typename Signed,
    if_signed_integral_integer<Signed> = true>
constexpr bool is_add_overflow(Signed left, Signed right) NOEXCEPT;

/// True if add would overflow Integer domain.
template <typename Unsigned,
    if_unsigned_integral_integer<Unsigned> = true>
constexpr bool is_add_overflow(Unsigned left, Unsigned right) NOEXCEPT;

/// True if subtract would underflow Integer domain.
template <typename Signed,
    if_signed_integral_integer<Signed> = true>
constexpr bool is_subtract_overflow(Signed left, Signed right) NOEXCEPT;

/// True if subtract would underflow Integer domain.
template <typename Unsigned,
    if_unsigned_integral_integer<Unsigned> = true>
constexpr bool is_subtract_overflow(Unsigned left, Unsigned right) NOEXCEPT;

/// multiply/divide.
/// ---------------------------------------------------------------------------
/// Multiply is limited to integrals, as it requires domain bounds. 

/// True if multiply would overflow Integer domain.
template <typename Unsigned,
    if_unsigned_integral_integer<Unsigned> = true>
constexpr bool is_multiply_overflow(Unsigned left, Unsigned right) NOEXCEPT;

/// True if divide (or modulo) implies division-by-zero.
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true, if_integer<Divisor> = true>
constexpr bool is_divide_overflow(Dividend dividend, Divisor divisor) NOEXCEPT;

/// Safely determine whether product is a whole mutiple of value.
template <typename Integer,
    if_integer<Integer> = true>
constexpr bool is_multiple(Integer product, Integer value) NOEXCEPT;

/// Safely determine whether product is left * right.
template <typename Integer,
    if_integer<Integer> = true>
constexpr bool is_product(Integer product, Integer left,
    Integer right) NOEXCEPT;

/// power/log.
/// ---------------------------------------------------------------------------
/// These are not limited to integral as they do not require domain bounds.

/// True if 0^0.
template <size_t Base, typename Exponent,
    if_integer<Exponent> = true>
constexpr bool is_power_overflow(Exponent exponent) NOEXCEPT;
template <typename Base, typename Exponent,
    if_integer<Base> = true,
    if_integer<Exponent> = true>
constexpr bool is_power_overflow(Base base, Exponent exponent) NOEXCEPT;

/// True if base < 2 or value < 1.
template <size_t Base, typename Value,
    if_integer<Value> = true>
constexpr bool is_log_overflow(Value value) NOEXCEPT;
template <typename Base, typename Value,
    if_integer<Base> = true,
    if_integer<Value> = true>
constexpr bool is_log_overflow(Base base, Value value) NOEXCEPT;

/// Throwing.
/// ---------------------------------------------------------------------------
/// Avoid structured exception handling, use only to cause program abort.
/// Exceptions can be used in constexpr to invalidate constexpr evaluation,
/// which includes template matching during compilation. So these are safe and
/// simplifying to use in template argument validation. Otherwise they should
/// generally be avoided and use only to abort program execution (not caught).

// TODO: generalize safe_add/subtract/multiply argument types.

/// Throws overflow_exception on negate signed minimum (1 use in bc).
template <typename Signed, if_signed_integral_integer<Signed> = true>
DEPRECATED constexpr Signed safe_negate(Signed value) THROWS;

/// Throws overflow_exception on sum overflow (1 use in bc).
template <typename Integral, if_integral_integer<Integral> = true>
DEPRECATED constexpr Integral safe_add(Integral left,
    Integral right) THROWS;

/// Throws overflow_exception on difference underflow (0 use in bc).
template <typename Integral, if_integral_integer<Integral> = true>
DEPRECATED constexpr Integral safe_subtract(Integral left,
    Integral right) THROWS;

/// Throws overflow_exception on product overflow (2 uses in bc (settings)).
template <typename Unsigned, if_unsigned_integral_integer<Unsigned> = true>
DEPRECATED constexpr Unsigned safe_multiply(Unsigned left,
    Unsigned right) THROWS;

/// Throws overflow_exception on /0, returns Dividend type (0 uses in bc).
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true,
    if_integer<Divisor> = true>
DEPRECATED constexpr Dividend safe_divide(Dividend dividend,
    Divisor divisor) THROWS;

/// Throws overflow_exception on %0, returns Dividend type (0 uses in bc).
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true,
    if_integer<Divisor> = true>
DEPRECATED constexpr Dividend safe_modulo(Dividend dividend,
    Divisor divisor) THROWS;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/overflow.ipp>

#endif

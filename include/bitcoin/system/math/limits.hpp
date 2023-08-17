/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_MATH_LIMITS_HPP
#define LIBBITCOIN_SYSTEM_MATH_LIMITS_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/power.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

/// Conditionals.
/// ---------------------------------------------------------------------------

/// The value is outside of the type bounds.
template <typename By, typename Integer,
    if_integer<By> = true, if_integer<Integer> = true>
constexpr bool is_limited(Integer value) NOEXCEPT;

/// The value exceeds the type bound or upper limit.
template <typename By, typename Integer,
    if_integer<By> = true, if_integer<Integer> = true>
constexpr bool is_limited(Integer value, By maximum) NOEXCEPT;

/// The value exceeds the lower or upper limit.
template <typename By, typename Integer,
    if_integer<By> = true, if_integer<Integer> = true>
constexpr bool is_limited(Integer value, By minimum, By maximum) NOEXCEPT;

/// Cast a value to Result, constrained to the limits of both types.
template <typename Result, typename Integer,
    if_integer<Result> = true, if_integer<Integer> = true>
constexpr Result limit(Integer value) NOEXCEPT;

/// Cast a value to Result, constrained to the type and upper limit.
/// Casting positive to/from negative may change the sign of the result.
template <typename Result, typename Integer,
    if_integer<Result> = true, if_integer<Integer> = true>
constexpr Result limit(Integer value, Result maximum) NOEXCEPT;

/// Cast a value to Result, constrained to the specified limits.
/// Casting positive to/from negative may change the sign of the result.
template <typename Result, typename Integer,
    if_integer<Result> = true, if_integer<Integer> = true>
constexpr Result limit(Integer value, Result minimum, Result maximum) NOEXCEPT;

/// Integral bounds.
/// ---------------------------------------------------------------------------

/// The maximum value of an integer.
template <typename Integer, if_integer<Integer> = true>
constexpr Integer maximum = std::numeric_limits<Integer>::max();

/// The minimum value of a integral integer.
template <typename Integer, if_integer<Integer> = true>
constexpr Integer minimum = std::numeric_limits<Integer>::min();

// dispatch for absolute_min/unsigned_max
template <typename Signed, if_signed_integral_integer<Signed> = true>
constexpr to_unsigned_type<Signed> absolute_minimum() NOEXCEPT;
template <typename Signed, if_signed_integral_integer<Signed> = true>
constexpr to_unsigned_type<Signed> unsigned_maximum() NOEXCEPT;
template <typename Unsigned, if_unsigned_integral_integer<Unsigned> = true>
constexpr Unsigned absolute_minimum() NOEXCEPT;
template <typename Unsigned, if_unsigned_integral_integer<Unsigned> = true>
constexpr Unsigned unsigned_maximum() NOEXCEPT;

/// The maximum value of a integral integer.
template <typename Integral, if_integral_integer<Integral> = true>
constexpr auto unsigned_max = unsigned_maximum<Integral>();

/// The absolute value of minimum value of a integral integer.
template <typename Integral, if_integral_integer<Integral> = true>
constexpr auto absolute_min = absolute_minimum<Integral>();

/// The maximum value of a byte-sized domain (1-8 bytes).
template <size_t Bytes>
constexpr signed_type<Bytes> domain_max = to_signed(
    sub1(power2<unsigned_type<Bytes>>(sub1(to_bits(Bytes)))));

/// The minimum value of a byte-sized domain (1-8 bytes).
template <size_t Bytes>
constexpr signed_type<Bytes> domain_min = ones_complement(domain_max<Bytes>);

/// The maximum value of a bitcoin integer by byte size (1-8 bytes).
template <size_t Bytes>
constexpr auto bitcoin_max = domain_max<Bytes>;

/// The minimum value of a bitcoin integer by byte size (1-8 bytes).
template <size_t Bytes>
constexpr auto bitcoin_min = twos_complement(bitcoin_max<Bytes>);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/limits.ipp>

#endif

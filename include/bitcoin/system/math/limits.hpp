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
#ifndef LIBBITCOIN_SYSTEM_MATH_LIMITS_HPP
#define LIBBITCOIN_SYSTEM_MATH_LIMITS_HPP

/// DELETECSTDDEF
/// DELETEMENOW
#include <bitcoin/system/define.hpp>

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

/// The minimum value of a integral integer type (std::numeric_limits).
template <typename Integer, if_integral_integer<Integer> = true>
constexpr Integer minimum() NOEXCEPT;

/// The maximum value of a integral integer type (std::numeric_limits).
template <typename Integer, if_integral_integer<Integer> = true>
constexpr Integer maximum() NOEXCEPT;

/// The absolute value of minimum value of a integral integer type.
template <typename Integer,
    typename Absolute = to_unsigned_type<Integer>,
    if_signed_integral_integer<Integer> = true>
constexpr Absolute absolute_minimum() NOEXCEPT;
template <typename Integer,
    if_unsigned_integral_integer<Integer> = true>
constexpr Integer absolute_minimum() NOEXCEPT;

/// The maximum value of a integral integer type (unsigned).
template <typename Integer,
    typename Unsigned = to_unsigned_type<Integer>,
    if_signed_integral_integer<Integer> = true>
constexpr Unsigned unsigned_maximum() NOEXCEPT;
template <typename Integer,
    if_unsigned_integral_integer<Integer> = true>
constexpr Integer unsigned_maximum() NOEXCEPT;

/// The minimum value of a type by byte size (1-8 bytes).
template <size_t Bytes, typename Return = signed_type<Bytes>>
constexpr Return minimum() NOEXCEPT;

/// The maximum value of a type by byte size (1-8 bytes).
template <size_t Bytes, typename Return = signed_type<Bytes>>
constexpr Return maximum() NOEXCEPT;

/// The minimum value of a bitcoin integer by byte size (1-8 bytes).
template <size_t Bytes, typename Return = signed_type<Bytes>>
constexpr Return bitcoin_min() NOEXCEPT;

/// The maximum value of a bitcoin integer by byte size (1-8 bytes).
template <size_t Bytes, typename Return = signed_type<Bytes>>
constexpr Return bitcoin_max() NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/limits.ipp>

#endif

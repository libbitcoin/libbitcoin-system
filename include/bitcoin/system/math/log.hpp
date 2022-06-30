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
#ifndef LIBBITCOIN_SYSTEM_MATH_LOG_HPP
#define LIBBITCOIN_SYSTEM_MATH_LOG_HPP

#include <cstddef>
#include <bitcoin/system/boost.hpp>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Ceilinged logarithms.

/// Obtain the ceilinged (rounded up) integer logarithm of given value and base.
/// Returns 0 for undefined (base < 2 or value < 1).
template <size_t Base, typename Exponent = size_t, typename Value>
constexpr Exponent ceilinged_log(Value value) NOEXCEPT;
template <typename Exponent = size_t, typename Base, typename Value,
    if_integer<Exponent> = true,
    if_integer<Base> = true,
    if_integer<Value> = true>
constexpr Exponent ceilinged_log(Base base, Value value) NOEXCEPT;

/// Optimizations for ceilinged_log(2, Value).
template <typename Exponent = size_t, typename Value,
    if_integer<Exponent> = true,
    if_integral_integer<Value> = true>
constexpr Exponent ceilinged_log2(Value value) NOEXCEPT;
template <typename Exponent = size_t, typename Value,
    if_integer<Exponent> = true,
    if_non_integral_integer<Value> = true>
constexpr Exponent ceilinged_log2(Value value) NOEXCEPT;

/// Optimizations for ceilinged_log(256, integral).
template <typename Exponent = size_t, typename Value,
    if_integer<Exponent> = true,
    if_integral_integer<Value> = true>
constexpr Exponent ceilinged_log256(Value value) NOEXCEPT;
template <typename Exponent = size_t, typename Value,
    if_integer<Exponent> = true,
    if_non_integral_integer<Value> = true>
constexpr Exponent ceilinged_log256(Value value) NOEXCEPT;

/// Floored logarithms.

/// Obtain the floored (rounded down) integer logarithm of given value and base.
/// Returns 0 for undefined (base < 2 or value < 1).
template <size_t Base, typename Exponent = size_t, typename Value>
constexpr Exponent floored_log(Value value) NOEXCEPT;
template <typename Exponent = size_t, typename Base, typename Value,
    if_integer<Exponent> = true,
    if_integer<Base> = true,
    if_integer<Value> = true>
constexpr Exponent floored_log(Base base, Value value) NOEXCEPT;

/// Optimizations for floored_log(2, Value).
template <typename Exponent = size_t, typename Value,
    if_integer<Exponent> = true,
    if_integral_integer<Value> = true>
constexpr Exponent floored_log2(Value value) NOEXCEPT;
template <typename Exponent = size_t, typename Value,
    if_integer<Exponent> = true,
    if_non_integral_integer<Value> = true>
constexpr Exponent floored_log2(Value value) NOEXCEPT;

/// Optimizations for floored_log(256, Value).
template <typename Exponent = size_t, typename Value,
    if_integer<Exponent> = true,
    if_integral_integer<Value> = true>
constexpr Exponent floored_log256(Value value) NOEXCEPT;
template <typename Exponent = size_t, typename Value,
    if_integer<Exponent> = true,
    if_non_integral_integer<Value> = true>
constexpr Exponent floored_log256(Value value) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/log.ipp>

#endif

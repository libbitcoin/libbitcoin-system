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
#ifndef LIBBITCOIN_SYSTEM_MATH_POWER_HPP
#define LIBBITCOIN_SYSTEM_MATH_POWER_HPP

#include <cstddef>
#include <bitcoin/system/constraints.hpp>

namespace libbitcoin {
namespace system {

/// Obtain the integer power of given base for given exponent.
/// Returns zero if all undefined operations (base < 2 or value < 1).

// Constexpr dispatches to optimal overload.
template <size_t Base, typename Value = size_t, typename Exponent>
constexpr Value power(Exponent exponent) noexcept;

// Normal form, for all integer types.
template <typename Value = size_t, typename Base, typename Exponent,
    if_integer<Value> = true,
    if_integer<Base> = true,
    if_integer<Exponent> = true>
constexpr Value power(Base base, Exponent exponent) noexcept;

/// Optimizations for power(2, Exponent).
template <typename Value = size_t, typename Exponent,
    if_integral_integer<Value> = true,
    if_integer<Exponent> = true>
constexpr Value power2(Exponent exponent) noexcept;
template <typename Value = size_t, typename Exponent,
    if_non_integral_integer<Value> = true,
    if_integer<Exponent> = true>
constexpr Value power2(Exponent exponent) noexcept;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/power.ipp>

#endif

/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

#include <bitcoin/system/define.hpp>

/// These functions manage type promotion but do not manage overflow.

namespace libbitcoin {
namespace system {
    
/// Obtain the integer power of given base for given unsigned exponent.
/// Returns 0 if all undefined operations (base < 2 or value < 1).

/// power(Base, Exponent).
template <size_t Base, typename Value = size_t, typename Exponent,
    if_unsigned_integer<Exponent> = true>
constexpr Value power(Exponent exponent) NOEXCEPT;
template <typename Value = size_t, typename Base, typename Exponent,
    if_integer<Value> = true,
    if_unsigned_integer<Base> = true,
    if_unsigned_integer<Exponent> = true>
constexpr Value power(Base base, Exponent exponent) NOEXCEPT;
template <typename Value = signed_size_t, typename Base, typename Exponent,
    if_integer<Value> = true,
    if_signed_integer<Base> = true,
    if_unsigned_integer<Exponent> = true>
constexpr Value power(Base base, Exponent exponent) NOEXCEPT;

/// Optimizations for power(2, Exponent).
template <typename Value = size_t, typename Exponent,
    if_integral_integer<Value> = true,
    if_unsigned_integer<Exponent> = true>
constexpr Value power2(Exponent exponent) NOEXCEPT;
template <typename Value = size_t, typename Exponent,
    if_non_integral_integer<Value> = true,
    if_unsigned_integer<Exponent> = true>
constexpr Value power2(Exponent exponent) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/power.ipp>

#endif

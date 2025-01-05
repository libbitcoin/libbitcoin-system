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
#ifndef LIBBITCOIN_SYSTEM_MATH_ROTATE_HPP
#define LIBBITCOIN_SYSTEM_MATH_ROTATE_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// rotl/rotr (platform independent intrinsic bitwise rotation).
/// ---------------------------------------------------------------------------
/// This avoids default to std::rotl/rotr in favor of bitwise implementation.

/// shift is unguarded.
template <typename Integral,
    if_integral_integer<Integral> = true>
INLINE constexpr Integral rotl(Integral value, size_t shift) NOEXCEPT;

/// shift is unguarded.
template <typename Integral,
    if_integral_integer<Integral> = true>
INLINE constexpr Integral rotr(Integral value, size_t shift) NOEXCEPT;

template <size_t Shift, typename Integral,
    if_integral_integer<Integral> = true>
INLINE constexpr Integral rotl(Integral value) NOEXCEPT;

template <size_t Shift, typename Integral,
    if_integral_integer<Integral> = true>
INLINE constexpr Integral rotr(Integral value) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/rotate.ipp>

#endif

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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_INTRINSICS_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_INTRINSICS_HPP

#include <bitcoin/system/intrinsics/arm/arm.hpp>
#include <bitcoin/system/intrinsics/xcpu/xcpu.hpp>
#include <bitcoin/system/intrinsics/haves.hpp>

#include <limits>
#include <type_traits>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// These are unconditionally defined, though xint types may be mocked.
/// Intel/AMD is always little-endian, and SHA is always big-endian (swap).
/// Primitives implement rotr/rotl because these are not available in SIMD.

/// This is false for any of the extended types not defined.
template <typename Type>
constexpr auto is_extended =
    is_same_type<Type, xint128_t> ||
    is_same_type<Type, xint256_t> ||
    is_same_type<Type, xint512_t>;

/// This fails for any of the extended types not defined.
template <typename Type>
using if_extended = bool_if<is_extended<Type>>;

/// Define lane-expanded 32/64 bit types.
template <typename Word, size_t Lanes,
    bool_if<Lanes == 16 || Lanes == 8 || Lanes == 4 || Lanes == 2> = true>
using to_extended = iif<is_same_size<Word, uint32_t>,
    iif<Lanes == 16, xint512_t, iif<Lanes == 8, xint256_t, xint128_t>>,
    iif<Lanes ==  8, xint512_t, iif<Lanes == 4, xint256_t, xint128_t>>>;

// Integral overload for vectorization coexistence.
template <typename To, unsigned int Lane, typename From,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true>
INLINE constexpr To get(From a) NOEXCEPT
{
    static_assert(Lane == one);
    return a;
}

} // namespace system
} // namespace libbitcoin

#endif

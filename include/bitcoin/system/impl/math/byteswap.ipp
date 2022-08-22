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
#ifndef LIBBITCOIN_SYSTEM_MATH_BYTESWAP_IPP
#define LIBBITCOIN_SYSTEM_MATH_BYTESWAP_IPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/cast.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>

namespace libbitcoin {
namespace system {

// Byteswap (platform independent byte reversal).
// ----------------------------------------------------------------------------
// If wrong overload is selected (such as for a literal) result is unexpected.
// C++23 std::byteswap does not support signed integrals.

template <typename Integral,
    if_integral_integer<Integral>,
    if_same_size<Integral, uint8_t>,
    if_unique_object_representations<Integral>>
INLINE constexpr Integral byteswap(Integral value) NOEXCEPT
{
    // no-op for calling consistency across all integral integer types.
    return value;
}

template <typename Integral,
    if_integral_integer<Integral>,
    if_same_size<Integral, uint16_t>,
    if_unique_object_representations<Integral>>
INLINE constexpr Integral byteswap(Integral value) NOEXCEPT
{
    // Compiles away to intrinsic call for non-constexpr.
    return std::is_constant_evaluated() ?
        possible_sign_cast<Integral>(byte_swap16_native(to_unsigned(value))) :
        possible_sign_cast<Integral>(byte_swap16(to_unsigned(value)));
}

template <typename Integral,
    if_integral_integer<Integral>,
    if_same_size<Integral, uint32_t>,
    if_unique_object_representations<Integral>>
INLINE constexpr Integral byteswap(Integral value) NOEXCEPT
{
    // Compiles away to intrinsic call for non-constexpr.
    return std::is_constant_evaluated() ?
        possible_sign_cast<Integral>(byte_swap32_native(to_unsigned(value))) :
        possible_sign_cast<Integral>(byte_swap32(to_unsigned(value)));
}

template <typename Integral,
    if_integral_integer<Integral>,
    if_same_size<Integral, uint64_t>,
    if_unique_object_representations<Integral>>
INLINE constexpr Integral byteswap(Integral value) NOEXCEPT
{
    // Compiles away to intrinsic call for non-constexpr.
    return std::is_constant_evaluated() ?
        possible_sign_cast<Integral>(byte_swap64_native(to_unsigned(value))) :
        possible_sign_cast<Integral>(byte_swap64(to_unsigned(value)));
}

} // namespace system
} // namespace libbitcoin

#endif

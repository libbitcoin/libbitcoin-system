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
#ifndef LIBBITCOIN_SYSTEM_MATH_ROTATE_IPP
#define LIBBITCOIN_SYSTEM_MATH_ROTATE_IPP

#include <type_traits>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/cast.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>

// Shift left is undefined for negative value.
// Shift right is implementation-defined for negative value.
// So these are unsigned though caller may cast from/to signed.

// C++ standard: "The behavior is undefined if the right operand is negative,
// or greater than or equal to the width of the promoted left operand."
// So use of parameter-shifted rotl_native/rotr_native should be guarded.

namespace libbitcoin {
namespace system {

template <typename Integral,
    if_integral_integer<Integral>>
INLINE constexpr Integral rotl(Integral value, size_t shift) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        return rotl_native(to_unsigned(value), shift);
    }
    else if constexpr (is_same_size<Integral, uint8_t>)
    {
        return possible_narrow_and_sign_cast<Integral>(rotl8(to_unsigned(value),
            possible_narrow_cast<unsigned char>(shift)));
    }
    else if constexpr (is_same_size<Integral, uint16_t>)
    {
        return possible_narrow_and_sign_cast<Integral>(rotl16(to_unsigned(value),
            possible_narrow_cast<unsigned char>(shift)));
    }
    else if constexpr (is_same_size<Integral, uint32_t>)
    {
        return possible_narrow_and_sign_cast<Integral>(rotl32(to_unsigned(value),
            possible_narrow_cast<unsigned char>(shift)));
    }
    else if constexpr (is_same_size<Integral, uint64_t>)
    {
        return possible_narrow_and_sign_cast<Integral>(rotl64(to_unsigned(value),
            possible_narrow_cast<unsigned char>(shift)));
    }
}

template <typename Integral,
    if_integral_integer<Integral>>
INLINE constexpr Integral rotr(Integral value, size_t shift) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        return rotr_native(to_unsigned(value), shift);
    }
    else if constexpr (is_same_size<Integral, uint8_t>)
    {
        return possible_narrow_and_sign_cast<Integral>(rotr8(to_unsigned(value),
            possible_narrow_cast<unsigned char>(shift)));
    }
    else if constexpr (is_same_size<Integral, uint16_t>)
    {
        return possible_narrow_and_sign_cast<Integral>(rotr16(to_unsigned(value),
            possible_narrow_cast<unsigned char>(shift)));
    }
    else if constexpr (is_same_size<Integral, uint32_t>)
    {
        return possible_narrow_and_sign_cast<Integral>(rotr32(to_unsigned(value),
            possible_narrow_cast<unsigned char>(shift)));
    }
    else if constexpr (is_same_size<Integral, uint64_t>)
    {
        return possible_narrow_and_sign_cast<Integral>(rotr64(to_unsigned(value),
            possible_narrow_cast<unsigned char>(shift)));
    }
}

template <size_t Shift, typename Integral,
    if_integral_integer<Integral>>
INLINE constexpr Integral rotl(Integral value) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        return rotl_native<Shift>(to_unsigned(value));
    }
    else
    {
        constexpr auto cycle = Shift % bits<Integral>;
        if constexpr (is_zero(cycle)) return value;
        else return rotl(to_unsigned(value), cycle);
    }
}

template <size_t Shift, typename Integral,
    if_integral_integer<Integral>>
INLINE constexpr Integral rotr(Integral value) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        return rotr_native<Shift>(to_unsigned(value));
    }
    else
    {
        constexpr auto cycle = Shift % bits<Integral>;
        if constexpr (is_zero(cycle)) return value;
        else return rotr(to_unsigned(value), cycle);
    }
}

} // namespace system
} // namespace libbitcoin

#endif

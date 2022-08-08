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
#ifndef LIBBITCOIN_SYSTEM_MATH_ROTATE_IPP
#define LIBBITCOIN_SYSTEM_MATH_ROTATE_IPP

#include <type_traits>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/cast.hpp>

// Shift left is undefined for negative value.
// Shift right is implementation-defined for negative value.
// So these are unsigned though caller may cast from/to signed.

// C++ standard: "The behavior is undefined if the right operand is negative,
// or greater than or equal to the width of the promoted left operand."
// So use of parameter-shifted rotl_native/rotr_native should be guarded.

template <typename Unsigned,
    bc::if_unsigned_integral_integer<Unsigned> = true>
INLINE constexpr Unsigned rotl_native(Unsigned value, size_t shift) NOEXCEPT
{
    // unguarded.
    constexpr auto span = bc::bits<Unsigned>;
    return bc::system::depromote<Unsigned>(
        (value << shift) | (value >> (span - shift)));
}

template <typename Unsigned,
    bc::if_unsigned_integral_integer<Unsigned> = true>
INLINE constexpr Unsigned rotr_native(Unsigned value, size_t shift) NOEXCEPT
{
    // unguarded.
    constexpr auto span = bc::bits<Unsigned>;
    return bc::system::depromote<Unsigned>(
        (value >> shift) | (value << (span - shift)));
}

template <size_t Shift, typename Unsigned,
    bc::if_unsigned_integral_integer<Unsigned> = true>
INLINE constexpr Unsigned rotl_native(Unsigned value) NOEXCEPT
{
    constexpr auto span = bc::bits<Unsigned>;
    constexpr auto cycle = Shift % span;
    if constexpr (bc::is_zero(cycle)) return value;
    else return bc::system::depromote<Unsigned>(
        (value << cycle) | (value >> (span - cycle)));
}

template <size_t Shift, typename Unsigned,
    bc::if_unsigned_integral_integer<Unsigned> = true>
INLINE constexpr Unsigned rotr_native(Unsigned value) NOEXCEPT
{
    constexpr auto span = bc::bits<Unsigned>;
    constexpr auto cycle = Shift % span;
    if constexpr (bc::is_zero(cycle)) return value;
    else return bc::system::depromote<Unsigned>(
        (value >> cycle) | (value << (span - cycle)));
}

// Use intrinsics if available (portable).
//#if defined(HAVE_LINUX)
//#elif defined(HAVE_APPLE)
//#elif defined(HAVE_FREEBSD)
//#elif defined(HAVE_OPENBSD)
//#elif defined(HAVE_NETBSD)
#if defined(HAVE_MSC)
    // docs.microsoft.com/en-us/cpp/intrinsics/rotl8-rotl16
    // docs.microsoft.com/en-us/cpp/intrinsics/rotr8-rotr16
    #include <intrin.h>
    #define rotl8(value, shift)  _rotl8(value, shift)
    #define rotl16(value, shift) _rotl16(value, shift)
    #define rotl32(value, shift) _rotl(value, shift)
    #define rotl64(value, shift) _rotl64(value, shift)
    #define rotr8(value, shift)  _rotr8(value, shift)
    #define rotr16(value, shift) _rotr16(value, shift)
    #define rotr32(value, shift) _rotr(value, shift)
    #define rotr64(value, shift) _rotr64(value, shift)
#else
    // Native implementation.
    #define rotl8(value, shift)  rotl_native(value, shift)
    #define rotl16(value, shift) rotl_native(value, shift)
    #define rotl32(value, shift) rotl_native(value, shift)
    #define rotl64(value, shift) rotl_native(value, shift)
    #define rotr8(value, shift)  rotr_native(value, shift)
    #define rotr16(value, shift) rotr_native(value, shift)
    #define rotr32(value, shift) rotr_native(value, shift)
    #define rotr64(value, shift) rotr_native(value, shift)
#endif

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

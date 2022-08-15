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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_ROTATE_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_ROTATE_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {

BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)

template <typename Unsigned,
    bc::if_unsigned_integral_integer<Unsigned> = true>
INLINE constexpr Unsigned rotl_native(Unsigned value, size_t shift) NOEXCEPT
{
    // unguarded.
    constexpr auto span = bits<Unsigned>;
    return static_cast<Unsigned>(
        (value << shift) | (value >> (span - shift)));
}

template <typename Unsigned,
    bc::if_unsigned_integral_integer<Unsigned> = true>
INLINE constexpr Unsigned rotr_native(Unsigned value, size_t shift) NOEXCEPT
{
    // unguarded.
    constexpr auto span = bits<Unsigned>;
    return static_cast<Unsigned>(
        (value >> shift) | (value << (span - shift)));
}

template <size_t Shift, typename Unsigned,
    bc::if_unsigned_integral_integer<Unsigned> = true>
INLINE constexpr Unsigned rotl_native(Unsigned value) NOEXCEPT
{
    constexpr auto span = bits<Unsigned>;
    constexpr auto cycle = Shift % span;
    if constexpr (is_zero(cycle))
        return value;
    else
        return static_cast<Unsigned>(
            (value << cycle) | (value >> (span - cycle)));
}

template <size_t Shift, typename Unsigned,
    bc::if_unsigned_integral_integer<Unsigned> = true>
INLINE constexpr Unsigned rotr_native(Unsigned value) NOEXCEPT
{
    constexpr auto span = bits<Unsigned>;
    constexpr auto cycle = Shift % span;
    if constexpr (is_zero(cycle))
        return value;
    else
        return static_cast<Unsigned>(
            (value >> cycle) | (value << (span - cycle)));
}

BC_POP_WARNING()

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

} // namespace libbitcoin

#endif

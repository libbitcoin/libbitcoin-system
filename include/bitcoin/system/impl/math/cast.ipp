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
#ifndef LIBBITCOIN_SYSTEM_MATH_CAST_IPP
#define LIBBITCOIN_SYSTEM_MATH_CAST_IPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

// Promotion of native operators.
// ----------------------------------------------------------------------------

template <typename From, if_integer<From>>
constexpr to_promoted_type<From> promote(From value) NOEXCEPT
{
    return value;
}

template <typename Restored, typename Common,
    if_common<Common>,
    if_integral_integer<Restored>,
    if_integral_integer<Common>>
constexpr Restored depromote(Common value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_IDENTITY_CAST)
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<Restored>(value);
    BC_POP_WARNING()
    BC_POP_WARNING()
}

template <typename Restored, typename Common,
    if_integer<Restored>,
    if_non_integral_integer<Common> >
constexpr Restored depromote(Common value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_IDENTITY_CAST)
    return static_cast<Restored>(value);
    BC_POP_WARNING()
}

// Explicit integral casts.
// ----------------------------------------------------------------------------

template <typename To, typename From,
    if_lesser_size<To, From>,
    if_same_signed_integral_integer<To, From>>
constexpr To narrow_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From,
    if_not_lesser_size<To, From>,
    if_not_same_signed_integral_integer<To, From>>
constexpr To sign_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From,
    if_lesser_size<To, From>,
    if_not_same_signed_integral_integer<To, From>>
constexpr To narrow_sign_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From,
    if_greater_size<To, From>,
    if_same_signed_integer<To, From>>
constexpr To wide_cast(From value) NOEXCEPT
{
    return value;
}

// Possible integer casts.
// ----------------------------------------------------------------------------

template <typename To, typename From, if_same_signed_integer<To, From>>
constexpr To possible_narrow_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From, if_not_lesser_size<To, From>>
constexpr To possible_sign_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From, if_not_same_signed_integer<To, From>>
constexpr To possible_narrow_sign_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From, if_lesser_size<To, From>>
constexpr To possible_sign_narrow_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From>
constexpr To possible_narrow_and_sign_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From,
    if_not_lesser_size<To, From>,
    if_same_signed_integer<To, From>>
constexpr To possible_wide_cast(From value) NOEXCEPT
{
    return value;
}

// Explicit pointer casts.
// ----------------------------------------------------------------------------

template <typename To, typename From>
inline To* pointer_cast(From* value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_REINTERPRET_CAST)
    return reinterpret_cast<To*>(value);
    BC_POP_WARNING()
}

template <typename To, typename From>
inline To* possible_pointer_cast(From* value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_IDENTITY_CAST)
    BC_PUSH_WARNING(NO_REINTERPRET_CAST)
    return reinterpret_cast<To*>(value);
    BC_POP_WARNING()
    BC_POP_WARNING()
}

// Sign casts.
// ----------------------------------------------------------------------------

template <typename Signed, if_signed_integer<Signed>>
constexpr Signed to_signed(Signed value) NOEXCEPT
{
    return value;
}

template <typename Unsigned, if_unsigned_integer<Unsigned>>
constexpr to_signed_type<Unsigned> to_signed(Unsigned value) NOEXCEPT
{
    return possible_sign_cast<to_signed_type<Unsigned>>(value);
}

template <typename Signed, if_signed_integer<Signed>>
constexpr to_unsigned_type<Signed> to_unsigned(Signed value) NOEXCEPT
{
    return possible_sign_cast<to_unsigned_type<Signed>>(value);
}

template <typename Unsigned, if_unsigned_integer<Unsigned>>
constexpr Unsigned to_unsigned(Unsigned value) NOEXCEPT
{
    return value;
}

} // namespace system
} // namespace libbitcoin

#endif

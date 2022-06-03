/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_MATH_SAFE_IPP
#define LIBBITCOIN_SYSTEM_MATH_SAFE_IPP

#include <limits>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/exceptions.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

/// Explicit integral casts.

template <typename To, typename From,
    if_lesser_width<To, From>,
    if_integral_integer<To>,
    if_integral_integer<From>,
    if_same_signed_integer<To, From>>
constexpr To narrow_cast(From value) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From,
    if_same_width<To, From>,
    if_integral_integer<To>,
    if_integral_integer<From>,
    if_not_same_signed_integer<To, From>>
constexpr To sign_cast(From value) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From,
    if_lesser_width<To, From>,
    if_integral_integer<To>,
    if_integral_integer<From>,
    if_not_same_signed_integer<To, From>>
constexpr To narrow_sign_cast(From value) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

/// Possible integer casts.

template <typename To, typename From,
    if_same_signed_integer<To, From>>
constexpr To possible_narrow_cast(From value) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From,
    if_not_lesser_width<To, From>>
constexpr To possible_sign_cast(From value) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From,
    if_not_same_signed_integer<To, From>>
constexpr To possible_narrow_sign_cast(From value) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From,
    if_lesser_width<To, From>>
constexpr To possible_sign_narrow_cast(From value) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From>
constexpr To possible_narrow_and_sign_cast(From value) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

/// Explicit pointer casts.

template <typename To, typename From>
constexpr To* pointer_cast(From* value) noexcept
{
    BC_PUSH_WARNING(NO_REINTERPRET_CAST)
    return reinterpret_cast<To*>(value);
    BC_POP_WARNING()
}

template <typename To, typename From>
constexpr To* possible_pointer_cast(From* value) noexcept
{
    BC_PUSH_WARNING(NO_IDENTITY_CAST)
    BC_PUSH_WARNING(NO_REINTERPRET_CAST)
    return reinterpret_cast<To*>(value);
    BC_POP_WARNING()
    BC_POP_WARNING()
}

template <typename To, typename From>
constexpr To* integer_pointer_cast(From value) noexcept
{
    BC_PUSH_WARNING(NO_REINTERPRET_CAST)
    return reinterpret_cast<To*>(value);
    BC_POP_WARNING()
}

// DEPRECATED: avoid structured exception handling.
// ----------------------------------------------------------------------------

template <typename Integer, if_unsigned_integer<Integer>>
Integer safe_multiply(Integer left, Integer right) noexcept(false)
{
    if (is_zero(left) || is_zero(right))
        return 0;

    if (left > (std::numeric_limits<Integer>::max() / right))
        throw overflow_exception("safe multiplication overflow");

    return left * right;
}

template <typename Integer, if_unsigned_integer<Integer>>
Integer safe_add(Integer left, Integer right) noexcept(false)
{
    if (left > (std::numeric_limits<Integer>::max() - right))
        throw overflow_exception("safe addition overflow");

    return left + right;
}

////template <typename Integer, if_unsigned_integer<Integer>>
////Integer safe_subtract(Integer left, Integer right) noexcept(false)
////{
////    if (left < (std::numeric_limits<Integer>::min() + right))
////        throw underflow_exception("safe subtraction underflow");
////
////    return left - right;
////}
////
////template <typename Integer, if_unsigned_integer<Integer>>
////void safe_increment(Integer& value) noexcept(false)
////{
////    value = safe_add(value, one);
////}
////
////template <typename Integer, if_unsigned_integer<Integer>>
////void safe_decrement(Integer& value)
////{
////    value = safe_subtract(value, one);
////}
////
////template <typename To, typename From, if_integer<To>, if_integer<From>>
////To safe_cast(From value) noexcept(false)
////{
////    if (is_lesser(value, std::numeric_limits<To>::min()))
////        throw range_exception("safe cast below minimum range");
////
////    if (is_greater(value, std::numeric_limits<To>::max()))
////        throw range_exception("safe cast above maximum range");
////
////    return static_cast<To>(value);
////}

} // namespace system
} // namespace libbitcoin

#endif

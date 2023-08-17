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
#ifndef LIBBITCOIN_SYSTEM_MATH_LIMITS_IPP
#define LIBBITCOIN_SYSTEM_MATH_LIMITS_IPP

#include <limits>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/cast.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

// Conditionals.
// ----------------------------------------------------------------------------

template <typename By, typename Integer,
    if_integer<By>, if_integer<Integer>>
constexpr bool is_limited(Integer value) NOEXCEPT
{
    return is_limited(value, maximum<By>);
}

template <typename By, typename Integer,
    if_integer<By>, if_integer<Integer>>
constexpr bool is_limited(Integer value, By maximum) NOEXCEPT
{
    return is_limited(value, minimum<By>, maximum);
}

template <typename By, typename Integer,
    if_integer<By>, if_integer<Integer>>
constexpr bool is_limited(Integer value, By minimum, By maximum) NOEXCEPT
{
    return is_lesser(value, minimum) || is_greater(value, maximum);
}

// limit

template <typename Result, typename Integer,
    if_integer<Result>, if_integer<Integer>>
constexpr Result limit(Integer value) NOEXCEPT
{
    return limit(value, maximum<Result>);
}

template <typename Result, typename Integer,
    if_integer<Result>, if_integer<Integer>>
constexpr Result limit(Integer value, Result maximum) NOEXCEPT
{
    return limit(value, minimum<Result>, maximum);
}

template <typename Result, typename Integer,
    if_integer<Result>, if_integer<Integer>>
constexpr Result limit(Integer value, Result minimum, Result maximum) NOEXCEPT
{
    return is_lesser(value, minimum) ? minimum :
        (is_greater(value, maximum) ? maximum :
            possible_narrow_and_sign_cast<Result>(value));
}

// Integral bounds (dispatch for absolute_min/unsigned_max).
// ----------------------------------------------------------------------------

template <typename Signed, if_signed_integral_integer<Signed>>
constexpr to_unsigned_type<Signed> absolute_minimum() NOEXCEPT
{
    // Avoids calling negate(minimum), which is undefined.
    return add1(to_unsigned(maximum<Signed>));
}

template <typename Unsigned, if_unsigned_integral_integer<Unsigned>>
constexpr Unsigned absolute_minimum() NOEXCEPT
{
    return minimum<Unsigned>;
}

template <typename Signed, if_signed_integral_integer<Signed>>
constexpr to_unsigned_type<Signed> unsigned_maximum() NOEXCEPT
{
    return to_unsigned(maximum<Signed>);
}

template <typename Unsigned, if_unsigned_integral_integer<Unsigned>>
constexpr Unsigned unsigned_maximum() NOEXCEPT
{
    return maximum<Unsigned>;
}

} // namespace system
} // namespace libbitcoin

#endif

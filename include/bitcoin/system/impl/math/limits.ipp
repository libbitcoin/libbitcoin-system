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
#ifndef LIBBITCOIN_SYSTEM_MATH_LIMITS_IPP
#define LIBBITCOIN_SYSTEM_MATH_LIMITS_IPP

#include <cstdint>
#include <limits>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/math/power.hpp>
#include <bitcoin/system/math/safe.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

// Conditionals.
// ----------------------------------------------------------------------------

template <typename By, typename Integer,
    if_integer<By>, if_integer<Integer>>
constexpr bool is_limited(Integer value) noexcept
{
    return is_limited(value, maximum<By>());
}

template <typename By, typename Integer,
    if_integer<By>, if_integer<Integer>>
constexpr bool is_limited(Integer value, By maximum) noexcept
{
    return is_limited(value, minimum<By>(), maximum);
}

template <typename By, typename Integer,
    if_integer<By>, if_integer<Integer>>
constexpr bool is_limited(Integer value, By minimum, By maximum) noexcept
{
    return is_lesser(value, minimum) || is_greater(value, maximum);
}

// limit

template <typename Result, typename Integer,
    if_integer<Result>, if_integer<Integer>>
constexpr Result limit(Integer value) noexcept
{
    return limit(value, maximum<Result>());
}

template <typename Result, typename Integer,
    if_integer<Result>, if_integer<Integer>>
constexpr Result limit(Integer value, Result maximum) noexcept
{
    return limit(value, minimum<Result>(), maximum);
}

template <typename Result, typename Integer,
    if_integer<Result>, if_integer<Integer>>
constexpr Result limit(Integer value, Result minimum, Result maximum) noexcept
{
    return is_lesser(value, minimum) ? minimum :
        (is_greater(value, maximum) ? maximum :
            possible_narrow_and_sign_cast<Result>(value));
}

// Integral bounds.
// ----------------------------------------------------------------------------

template <typename Integer,
    if_integral_integer<Integer>>
constexpr Integer minimum() noexcept
{
    return std::numeric_limits<Integer>::min();
}

template <typename Integer,
    if_integral_integer<Integer>>
constexpr Integer maximum() noexcept
{
    return std::numeric_limits<Integer>::max();
}

template <typename Integer, typename Absolute,
    if_signed_integral_integer<Integer>>
constexpr Absolute absolute_minimum() noexcept
{
    // Avoids calling negate(minimum), which is undefined.
    return add1(to_unsigned(maximum<Integer>()));
}

template <typename Integer,
    if_unsigned_integral_integer<Integer>>
constexpr Integer absolute_minimum() noexcept
{
    return minimum<Integer>();
}

template <typename Integer, typename Unsigned,
    if_signed_integral_integer<Integer>>
constexpr Unsigned unsigned_maximum() noexcept
{
    return to_unsigned(maximum<Integer>());
}

template <typename Integer,
    if_unsigned_integral_integer<Integer>>
constexpr Integer unsigned_maximum() noexcept
{
    return maximum<Integer>();
}

// byte-based minimum/maximum

template <size_t Bytes, typename Return>
constexpr Return minimum() noexcept
{
    return ones_complement(maximum<Bytes>());
}

template <size_t Bytes, typename Return>
constexpr Return maximum() noexcept
{
    using result = to_unsigned_type<Return>;
    constexpr auto bit = sub1(to_bits(Bytes));
    return to_signed(sub1(power2<result>(bit)));
}

// bitcoin minimum/maximum (stack_number)

template <size_t Bytes, typename Return>
constexpr Return bitcoin_min() noexcept
{
    return twos_complement(bitcoin_max<Bytes>());
}

template <size_t Bytes, typename Return>
constexpr Return bitcoin_max() noexcept
{
    return maximum<Bytes>();
}

} // namespace system
} // namespace libbitcoin

#endif

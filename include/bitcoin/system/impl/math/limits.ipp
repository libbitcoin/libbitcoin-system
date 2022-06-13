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

#include <limits>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/math/power.hpp>
#include <bitcoin/system/math/safe.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

// is_limited

template <typename By, typename Integer,
    if_integer<By>, if_integer<Integer>>
constexpr bool is_limited(Integer value) noexcept
{
    return is_limited(value, std::numeric_limits<By>::max());
}

template <typename By, typename Integer,
    if_integer<By>, if_integer<Integer>>
constexpr bool is_limited(Integer value, By maximum) noexcept
{
    return is_limited(value, std::numeric_limits<By>::min(), maximum);
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
    return limit(value, std::numeric_limits<Result>::max());
}

template <typename Result, typename Integer,
    if_integer<Result>, if_integer<Integer>>
constexpr Result limit(Integer value, Result maximum) noexcept
{
    return limit(value, std::numeric_limits<Result>::min(), maximum);
}

template <typename Result, typename Integer,
    if_integer<Result>, if_integer<Integer>>
constexpr Result limit(Integer value, Result minimum, Result maximum) noexcept
{
    return is_lesser(value, minimum) ? minimum :
        (is_greater(value, maximum) ? maximum :
            possible_narrow_and_sign_cast<Result>(value));
}

// minimum/maximum

template <size_t Bytes, typename Return>
constexpr Return minimum() noexcept
{
    // Use unsigned domain to preclude overflow of signed before negation.
    using positive = std::make_unsigned<Return>::type;
    return -to_signed(power2<positive>(sub1(to_bits(Bytes))));
}

template <size_t Bytes, typename Return>
constexpr Return maximum() noexcept
{
    // Use unsigned domain to preclude overflow of signed before negation.
    using positive = std::make_unsigned<Return>::type;
    return to_signed(sub1(power2<positive>(sub1(to_bits(Bytes)))));
}

// bitcoin_minimum/bitcoin_maximum

template <size_t Bytes, typename Return>
constexpr Return bitcoin_min() noexcept
{
    return add1(minimum<Bytes>());
}

template <size_t Bytes, typename Return>
constexpr Return bitcoin_max() noexcept
{
    return maximum<Bytes>();
}

} // namespace system
} // namespace libbitcoin

#endif

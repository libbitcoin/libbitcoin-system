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
#ifndef LIBBITCOIN_SYSTEM_MATH_LIMITS_IPP
#define LIBBITCOIN_SYSTEM_MATH_LIMITS_IPP

#include <limits>
#include <bitcoin/system/exceptions.hpp>
#include <bitcoin/system/math/sign.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

// TODO: move to addition.ipp.
// TODO: test with uintx.

template <typename Result, typename Left, typename Right,
    if_same_signed_integer<Left, Right>>
inline Result add(Left left, Right right) noexcept
{
    // Casts are not constexpr.
    return static_cast<Result>(left) + static_cast<Result>(right);
}

template <typename Result, typename Left, typename Right,
    if_same_signed_integer<Left, Right>>
inline Result subtract(Left left, Right right) noexcept
{
    // Casts are not constexpr.
    return static_cast<Result>(left) - static_cast<Result>(right);
}

template <typename Integer, if_unsigned_integer<Integer>>
constexpr Integer ceilinged_add(Integer left, Integer right) noexcept
{
    constexpr auto maximum = std::numeric_limits<Integer>::max();
    return (left > (maximum - right)) ? maximum : (left + right);
}

template <typename Integer, if_unsigned_integer<Integer>>
constexpr Integer floored_subtract(Integer left, Integer right) noexcept
{
    constexpr auto minimum = std::numeric_limits<Integer>::min();
    return (right >= left) ? minimum : (left - right);
}

// TODO: rename to limit.ipp.

template <typename Result, typename Integer,
    if_integer<Result>, if_integer<Integer>>
inline Result limit(Integer value) noexcept
{
    // Casts are not constexpr.
    return limit(static_cast<Result>(value),
        std::numeric_limits<Result>::min(),
        std::numeric_limits<Result>::max());
}

template <typename Result, typename Integer,
    if_integer<Result>, if_integer<Integer>>
inline Result limit(Integer value, Result minimum, Result maximum) noexcept
{
    // Casts are not constexpr.
    const auto cast = static_cast<Result>(value);
    return is_lesser(cast, minimum) ? minimum :
        (is_greater(cast, maximum) ? maximum : cast);
}

} // namespace system
} // namespace libbitcoin

#endif

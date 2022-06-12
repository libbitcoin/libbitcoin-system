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
#ifndef LIBBITCOIN_SYSTEM_MATH_ADDITION_IPP
#define LIBBITCOIN_SYSTEM_MATH_ADDITION_IPP

#include <limits>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/math/safe.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

// TODO: test with uintx.

template <typename Result, typename Left, typename Right,
    if_not_lesser_width<Result, Left>,
    if_not_lesser_width<Result, Right>,
    if_same_signed_integer<Left, Right>>
constexpr Result add(Left left, Right right) noexcept
{
    return possible_sign_cast<Result>(left) +
        possible_sign_cast<Result>(right);
}

template <typename Result, typename Left, typename Right,
    if_not_lesser_width<Result, Left>,
    if_not_lesser_width<Result, Right>,
    if_same_signed_integer<Left, Right>>
constexpr Result subtract(Left left, Right right) noexcept
{
    return possible_sign_cast<Result>(left) -
        possible_sign_cast<Result>(right);
}

template <typename Result, typename Integer,
    if_not_lesser_width<Result, Integer>>
constexpr Result increment(Integer value) noexcept
{
    return add<Result>(value, Integer{ 1 });
}

template <typename Result, typename Integer,
    if_not_lesser_width<Result, Integer>>
constexpr Result decrement(Integer value) noexcept
{
    return subtract<Result>(value, Integer{ 1 });
}

template <typename Integer, if_signed_integer<Integer>>
constexpr bool overflows(Integer left, Integer right) noexcept
{
    // C++14: local variables allowed in constexpr.
    return !is_zero(right) &&
        (!is_negative(right) ||
            (left < std::numeric_limits<Integer>::min() - right)) &&
        (is_negative(right) ||
            (left > std::numeric_limits<Integer>::max() - right));
}

template <typename Integer, if_unsigned_integer<Integer>>
constexpr bool overflows(Integer left, Integer right) noexcept
{
    return right > (std::numeric_limits<Integer>::max() - left);
}

template <typename Integer, if_signed_integer<Integer>>
constexpr bool underflows(Integer left, Integer right) noexcept
{
    // C++14: local variables allowed in constexpr.
    return !is_zero(right) &&
        (!is_negative(right) ||
            (left > std::numeric_limits<Integer>::max() + right)) &&
        (is_negative(right) ||
            (left < std::numeric_limits<Integer>::min() + right));
}

template <typename Integer, if_unsigned_integer<Integer>>
constexpr bool underflows(Integer left, Integer right) noexcept
{
    return right > left;
}

template <typename Integer, if_signed_integer<Integer>>
constexpr Integer ceilinged_add(Integer left, Integer right) noexcept
{
    return overflows(left, right) ? (is_negative(right) ?
        std::numeric_limits<Integer>::min() :
        std::numeric_limits<Integer>::max()) : (left + right);
}

template <typename Integer, if_unsigned_integer<Integer>>
constexpr Integer ceilinged_add(Integer left, Integer right) noexcept
{
    return overflows(left, right) ? std::numeric_limits<Integer>::max() :
        (left + right);
}

template <typename Integer, if_signed_integer<Integer>>
constexpr Integer floored_subtract(Integer left, Integer right) noexcept
{
    return underflows(left, right) ? (is_negative(right) ?
        std::numeric_limits<Integer>::max() :
        std::numeric_limits<Integer>::min()) : (left - right);
}

template <typename Integer, if_unsigned_integer<Integer>>
constexpr Integer floored_subtract(Integer left, Integer right) noexcept
{
    return underflows(left, right) ? std::numeric_limits<Integer>::min() :
        (left - right);
}

} // namespace system
} // namespace libbitcoin

#endif

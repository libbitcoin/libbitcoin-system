/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_MATH_LIMITS_HPP
#define LIBBITCOIN_SYSTEM_MATH_LIMITS_HPP

#include <limits>
#include <bitcoin/system/exceptions.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

// TODO: move to addition.hpp.

/// left + right after cast of each to Result type.
template <typename Result, typename Left, typename Right,
    if_same_signed_integer<Left, Right> = true>
inline Result add(Left left, Right right) noexcept;

/// left - right after cast of each to Result type.
template <typename Result, typename Left, typename Right,
    if_same_signed_integer<Left, Right> = true>
inline Result subtract(Left left, Right right) noexcept;

/// If would overflow return Integer maximum, otherwise the sum.
template <typename Integer, if_unsigned_integer<Integer> = true>
constexpr Integer ceilinged_add(Integer left, Integer right) noexcept;

/// If would underflow return Integer minimum (0), otherwise the difference.
template <typename Integer, if_unsigned_integer<Integer> = true>
constexpr Integer floored_subtract(Integer left, Integer right) noexcept;

// TODO: rename to limit.hpp.

/// Cast a value to Result, constrained to the limits of both types.
template <typename Result, typename Integer,
    if_integer<Result> = true, if_integer<Integer> = true>
inline Result limit(Integer value) noexcept;

/// Cast a value to Result, constrained to the specified limits.
/// Casting positive to/from negative will change the sign of the result unless
/// the specified limits constrain the result otherwise.
template <typename Result, typename Integer,
    if_integer<Result> = true, if_integer<Integer> = true>
inline Result limit(Integer value, Result minimum, Result maximum) noexcept;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/limits.ipp>

#endif

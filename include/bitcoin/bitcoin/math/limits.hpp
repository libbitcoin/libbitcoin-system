/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_LIMITS_HPP
#define LIBBITCOIN_LIMITS_HPP

#include <algorithm>
#include <stdexcept>
#include <limits>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {

#define UNSIGNED(T) std::enable_if<std::is_unsigned<T>::value>

template <typename Integer, typename = UNSIGNED(Integer)>
Integer ceiling_add(Integer left, Integer right)
{
    static const auto ceiling = std::numeric_limits<Integer>::max();
    return left > ceiling - right ? ceiling : left + right;
}

template <typename Integer, typename = UNSIGNED(Integer)>
Integer floor_subtract(Integer left, Integer right)
{
    static const auto floor = std::numeric_limits<Integer>::min();
    return right >= left ? floor : left - right;
}

template <typename Integer, typename = UNSIGNED(Integer)>
Integer safe_add(Integer left, Integer right)
{
    static const auto maximum = std::numeric_limits<Integer>::max();

    if (left > maximum - right)
        throw std::overflow_error("addition overflow");

    return left + right;
}

template <typename Integer, typename = UNSIGNED(Integer)>
Integer safe_subtract(Integer left, Integer right)
{
    static const auto minimum = std::numeric_limits<Integer>::min();

    if (left < minimum + right)
        throw std::underflow_error("subtraction underflow");

    return left - right;
}

template <typename Integer>
Integer& safe_increment(Integer& value)
{
    static BC_CONSTEXPR auto one = Integer{ 1 };
    value = safe_add(value, one);
    return value;
}

template <typename Integer>
Integer& safe_decrement(Integer& value)
{
    static BC_CONSTEXPR auto one = Integer{ 1 };
    value = safe_subtract(value, one);
    return value;
}

template <typename To, typename From>
To safe_assign(From value)
{
    static const auto maximum = std::numeric_limits<To>::max();
    static const auto minimum = std::numeric_limits<To>::min();

    if (value < minimum || value > maximum)
        throw std::range_error("assignment out of range");

    // This should be the only integer cast in the codebase.
    return static_cast<To>(value);
}

/// Constrain a numeric value within a given range.
template <typename Value, typename Bound>
Bound range_constrain(Value value, Bound minimum, Bound maximum)
{
    if (value < minimum)
        return minimum;

    if (value > maximum)
        return maximum;

    return safe_assign<Bound>(value);
}

#undef UNSIGNED

} // namespace libbitcoin

#endif

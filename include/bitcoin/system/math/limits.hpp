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
#ifndef LIBBITCOIN_SYSTEM_LIMITS_HPP
#define LIBBITCOIN_SYSTEM_LIMITS_HPP

#include <algorithm>
#include <stdexcept>
#include <limits>
#include <bitcoin/system/compat.hpp>
#include <bitcoin/system/utility/assert.hpp>

namespace libbitcoin {
namespace system {

#define IF(T) std::enable_if<T>
#define SIGN(T) std::is_signed<T>::value
#define UNSIGN(T) std::is_unsigned<T>::value

#define SIGNED(A) IF(SIGN(A))
#define UNSIGNED(A) IF(UNSIGN(A))
#define SIGNED_SIGNED(A, B) IF(SIGN(A) && SIGN(B))
#define SIGNED_UNSIGNED(A, B) IF(SIGN(A) && UNSIGN(B))
#define UNSIGNED_SIGNED(A, B) IF(UNSIGN(A) && SIGN(B))
#define UNSIGNED_UNSIGNED(A, B) IF(UNSIGN(A) && UNSIGN(B))

template <typename Space, typename Integer>
Space cast_add(Integer left, Integer right)
{
    return static_cast<Space>(left) + static_cast<Space>(right);
}

template <typename Space, typename Integer>
Space cast_subtract(Integer left, Integer right)
{
    return static_cast<Space>(left) - static_cast<Space>(right);
}

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
void safe_increment(Integer& value)
{
    static BC_CONSTEXPR auto one = Integer{1};
    value = safe_add(value, one);
}

template <typename Integer>
void safe_decrement(Integer& value)
{
    static BC_CONSTEXPR auto one = Integer{1};
    value = safe_subtract(value, one);
}

template <typename To, typename From, typename = SIGNED_SIGNED(To, From)>
To safe_signed(From signed_value)
{
    static const auto signed_minimum = std::numeric_limits<To>::min();
    static const auto signed_maximum = std::numeric_limits<To>::max();

    if (signed_value < signed_minimum || signed_value > signed_maximum)
        throw std::range_error("signed assignment out of range");

    return static_cast<To>(signed_value);
}

template <typename To, typename From, typename = UNSIGNED_UNSIGNED(To, From)>
To safe_unsigned(From unsigned_value)
{
    static const auto unsigned_minimum = std::numeric_limits<To>::min();
    static const auto unsigned_maximum = std::numeric_limits<To>::max();

    if (unsigned_value < unsigned_minimum || unsigned_value > unsigned_maximum)
        throw std::range_error("unsigned assignment out of range");

    return static_cast<To>(unsigned_value);
}

template <typename To, typename From, typename = SIGNED_UNSIGNED(To, From)>
To safe_to_signed(From unsigned_value)
{
    static_assert(sizeof(uint64_t) >= sizeof(To), "safe assign out of range");
    static const auto signed_maximum = std::numeric_limits<To>::max();

    if (unsigned_value > static_cast<uint64_t>(signed_maximum))
        throw std::range_error("to signed assignment out of range");

    return static_cast<To>(unsigned_value);
}

template <typename To, typename From, typename = UNSIGNED_SIGNED(To, From)>
To safe_to_unsigned(From signed_value)
{
    static_assert(sizeof(uint64_t) >= sizeof(To), "safe assign out of range");
    static const auto unsigned_maximum = std::numeric_limits<To>::max();

    if (signed_value < 0 ||
        static_cast<uint64_t>(signed_value) > unsigned_maximum)
        throw std::range_error("to unsigned assignment out of range");

    return static_cast<To>(signed_value);
}

/// Constrain a numeric value within a given type domain.
template <typename To, typename From>
To domain_constrain(From value)
{
    static const auto minimum = std::numeric_limits<To>::min();
    static const auto maximum = std::numeric_limits<To>::max();

    if (value < minimum)
        return minimum;

    if (value > maximum)
        return maximum;

    return static_cast<To>(value);
}

/// Constrain a numeric value within a given range.
template <typename To, typename From>
To range_constrain(From value, To minimum, To maximum)
{
    if (value < minimum)
        return minimum;

    if (value > maximum)
        return maximum;

    return static_cast<To>(value);
}

#undef IF
#undef SIGN
#undef UNSIGN
#undef SIGNED
#undef UNSIGNED
#undef SIGNED_SIGNED
#undef SIGNED_UNSIGNED
#undef UNSIGNED_SIGNED
#undef UNSIGNED_UNSIGNED

} // namespace system
} // namespace libbitcoin

#endif

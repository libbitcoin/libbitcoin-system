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
#ifndef LIBBITCOIN_SYSTEM_MATH_BITS_IPP
#define LIBBITCOIN_SYSTEM_MATH_BITS_IPP

#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/sign.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

template <typename Value, if_integral_integer<Value>>
constexpr size_t bit_width()
{
    return to_bits(sizeof(Value));
}

template <typename Value, if_integral_integer<Value>>
constexpr Value bit_lo()
{
    return to_int<Value>(true) << zero;
}

template <typename Value, if_integral_integer<Value>>
constexpr Value bit_hi()
{
    // sub1 for size-to-index translation.
    return to_int<Value>(true) << sub1(bit_width<Value>());
}

template <typename Value, if_integral_integer<Value>>
constexpr Value bit_right(size_t offset)
{
    return bit_lo<Value>() << offset;
}

template <typename Value, if_integral_integer<Value>>
constexpr Value bit_left(size_t offset)
{
    return bit_hi<Value>() >> offset;
}

template <typename Value, if_integral_integer<Value>>
constexpr void set_right(Value& target, size_t offset, bool state)
{
    state ?
        target |= bit_right<Value>(offset) :
        target &= ~bit_right<Value>(offset);
}

template <typename Value, if_integral_integer<Value>>
constexpr void set_left(Value& target, size_t offset, bool state)
{
    state ?
        target |= bit_left<Value>(offset) :
        target &= ~bit_left<Value>(offset);
}

template <typename Value, if_integral_integer<Value>>
constexpr bool get_right(Value value, size_t offset)
{
    return !is_zero(value & bit_right<Value>(offset));
}

template <typename Value, if_integral_integer<Value>>
constexpr bool get_left(Value value, size_t offset)
{
    return !is_zero(value & bit_left<Value>(offset));
}

} // namespace system
} // namespace libbitcoin

#endif

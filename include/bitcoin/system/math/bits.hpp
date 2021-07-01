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
#ifndef LIBBITCOIN_SYSTEM_MATH_BITS_HPP
#define LIBBITCOIN_SYSTEM_MATH_BITS_HPP

#include <cstddef>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

/// The number of bits representable in the type of Value.
template <typename Value, if_integral_integer<Value> = true>
constexpr size_t bit_width(Value optional=0);

/// An instance of value with only the low order bit set (0x...1).
template <typename Value, if_integral_integer<Value> = true>
constexpr Value bit_lo();

/// An instance of value with only the high order bit set (0x8...).
template <typename Value, if_integral_integer<Value> = true>
constexpr Value bit_hi();

/// A single bit bitmask, offset relative to lowest order bit.
template <typename Value, if_integral_integer<Value> = true>
constexpr Value bit_right(size_t offset);

/// A single bit bitmask, offset relative to highest order bit.
template <typename Value, if_integral_integer<Value> = true>
constexpr Value bit_left(size_t offset);

/// Set a bit in target, offset relative to lowest order bit.
template <typename Value, if_integral_integer<Value> = true>
constexpr void set_right(Value& target, size_t offset=zero, bool state=true);

/// Set a bit in target, offset relative to highest order bit.
template <typename Value, if_integral_integer<Value> = true>
constexpr void set_left(Value& target, size_t offset=zero, bool state=true);

/// Extract a bit from value, offset relative to lowest order bit.
template <typename Value, if_integral_integer<Value> = true>
constexpr bool get_right(Value value, size_t offset=zero);

/// Extract a bit from value, offset relative to highest order bit.
template <typename Value, if_integral_integer<Value> = true>
constexpr bool get_left(Value value, size_t offset=zero);

/// Rotate the bits of Value to the right by amount 'shift'.
template <typename Value, if_integral_integer<Value> = true>
constexpr Value rotate_right(Value value, size_t shift);

/// Rotate the bits of Value to the left by amount 'shift'.
template <typename Value, if_integral_integer<Value> = true>
constexpr Value rotate_left(Value value, size_t shift);

/// A bitmask with low order count of bits zero.
template <typename Value, if_unsigned_integer<Value> = true>
constexpr Value mask_right(size_t bits);

/// A bitmask with high order count of bits zero.
template <typename Value, if_unsigned_integer<Value> = true>
constexpr Value mask_left(size_t bits);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/bits.ipp>

#endif

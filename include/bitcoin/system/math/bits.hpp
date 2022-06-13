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
#ifndef LIBBITCOIN_SYSTEM_MATH_BITS_HPP
#define LIBBITCOIN_SYSTEM_MATH_BITS_HPP

#include <cstddef>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constraints.hpp>

namespace libbitcoin {
namespace system {

/// Unlike native operators, these functions do not promote integer types.
/// and have well-defined behavior for any type-enabled parameterization.

/// Bit field width determinations.
/// ---------------------------------------------------------------------------

/// See also std::bit_width (C++20).
/// The number of bytes required to store value, byte_width(0) returns zero.
template <typename Value, if_integer<Value> = true>
constexpr size_t byte_width(Value value) noexcept;

/// Bitwise logical operations.
/// ---------------------------------------------------------------------------

/// Two's complement (~value + 1).
/// Absolute value of an unsigned negative value.
/// Avoid use of unsigned negation as this relies on CPU architecture.
template <typename Value, if_integer<Value> = true>
constexpr Value twos_complement(Value value) noexcept;

/// Bitwise NOT (~value) operation.
/// One's complement, the bit inversion of any value.
template <typename Value, if_integer<Value> = true>
constexpr Value bit_not(Value value) noexcept;

/// Bitwise AND (left & right) operation.
template <typename Value, if_integer<Value> = true>
constexpr Value bit_and(Value left, Value right) noexcept;

/// Bitwise OR (left | right)  operation.
template <typename Value, if_integer<Value> = true>
constexpr Value bit_or(Value left, Value right) noexcept;

/// Bitwise XOR (left ^ right)  operation.
template <typename Value, if_integer<Value> = true>
constexpr Value bit_xor(Value left, Value right) noexcept;

// Value factories.
// ---------------------------------------------------------------------------

/// All bits set (maximum value for signed, minimum value for unsigned).
template <typename Value, if_integer<Value> = true>
constexpr Value bit_all() noexcept;

/// An instance of value with only the high order bit set (0x8...0).
template <typename Value, if_integer<Value> = true>
constexpr Value bit_hi() noexcept;

/// An instance of value with only the low order bit set (0x0...1).
template <typename Value, if_integer<Value> = true>
constexpr Value bit_lo() noexcept;

/// A single bit bitmask, offset relative to highest order bit.
template <typename Value, if_integer<Value> = true>
constexpr Value bit_left(size_t offset) noexcept;

/// A single bit bitmask, offset relative to lowest order bit.
template <typename Value, if_integer<Value> = true>
constexpr Value bit_right(size_t offset) noexcept;

/// Extract a bit from value, offset relative to highest order bit.
template <typename Value, if_integer<Value> = true>
constexpr bool get_left(Value value, size_t offset=zero) noexcept;

/// Extract a bit from value, offset relative to lowest order bit.
template <typename Value, if_integer<Value> = true>
constexpr bool get_right(Value value, size_t offset=zero) noexcept;

/// Set (a bit).
/// ---------------------------------------------------------------------------

/// Set a bit in target, offset relative to highest order bit.
template <typename Value, if_integer<Value> = true>
constexpr Value set_left(const Value& target, size_t offset=zero,
    bool state=true) noexcept;
template <typename Value, if_integer<Value> = true>
constexpr void set_left_into(Value& target, size_t offset=zero,
    bool state=true) noexcept;

/// Set a bit in target, offset relative to lowest order bit.
template <typename Value, if_integer<Value> = true>
constexpr Value set_right(const Value& target, size_t offset=zero,
    bool state=true) noexcept;
template <typename Value, if_integer<Value> = true>
constexpr void set_right_into(Value& target, size_t offset=zero,
    bool state=true) noexcept;

/// Mask (a field of bits from left/right).
/// ---------------------------------------------------------------------------

/// A set of bitmasks with high order count of bits unset.
template <typename Value, if_integer<Value> = true>
constexpr Value mask_left(size_t bits) noexcept;
template <typename Value, if_integer<Value> = true>
constexpr Value mask_left(const Value& target, size_t bits) noexcept;
template <typename Value, if_integer<Value> = true>
constexpr void mask_left_into(Value& target, size_t bits) noexcept;

/// A set of bitmasks with low order count of bits unset.
template <typename Value, if_integer<Value> = true>
constexpr Value mask_right(size_t bits) noexcept;
template <typename Value, if_integer<Value> = true>
constexpr Value mask_right(const Value& target, size_t bits) noexcept;
template <typename Value, if_integer<Value> = true>
constexpr void mask_right_into(Value& target, size_t bits) noexcept;

/// Unmask (~mask).
/// ---------------------------------------------------------------------------

/// A set of bitflags with high order count of bits set.
template <typename Value, if_integer<Value> = true>
constexpr Value unmask_left(size_t bits) noexcept;
template <typename Value, if_integer<Value> = true>
constexpr Value unmask_left(const Value& target, size_t bits) noexcept;
template <typename Value, if_integer<Value> = true>
constexpr Value unmask_left_into(Value& target, size_t bits) noexcept;

/// A set of bitflags with low order count of bits set.
template <typename Value, if_integer<Value> = true>
constexpr Value unmask_right(size_t bits) noexcept;
template <typename Value, if_integer<Value> = true>
constexpr Value unmask_right(const Value& target, size_t bits) noexcept;
template <typename Value, if_integer<Value> = true>
constexpr void unmask_right_into(Value& target, size_t bits) noexcept;

/// Shift (left/right).
/// ---------------------------------------------------------------------------

/// Bitwise LEFT SHIFT (value << shift) operation.
/// Set overflow to false to modulo shift by Value width.
/// Signed overload interpret value as unsigned for shift.
template <typename Value, if_signed_integer<Value> = true>
constexpr Value shift_left(const Value& value, size_t shift,
    bool overflow=true) noexcept;
template <typename Value, if_unsigned_integer<Value> = true>
constexpr Value shift_left(const Value& value, size_t shift,
    bool overflow = true) noexcept;
template <typename Value, if_signed_integer<Value> = true>
constexpr void shift_left_into(Value& value, size_t shift,
    bool overflow=true) noexcept;
template <typename Value, if_unsigned_integer<Value> = true>
constexpr void shift_left_into(Value& value, size_t shift,
    bool overflow = true) noexcept;

/// Bitwise RIGHT SHIFT (value >> shift) operation.
/// Set overflow to false to modulo shift by Value width.
/// Signed overload interpret value as unsigned for shift.
template <typename Value, if_signed_integer<Value> = true>
constexpr Value shift_right(const Value& value, size_t shift,
    bool overflow=true) noexcept;
template <typename Value, if_unsigned_integer<Value> = true>
constexpr Value shift_right(const Value& value, size_t shift,
    bool overflow = true) noexcept;
template <typename Value, if_signed_integer<Value> = true>
constexpr void shift_right_into(Value& value, size_t shift,
    bool overflow=true) noexcept;
template <typename Value, if_unsigned_integer<Value> = true>
constexpr void shift_right_into(Value& value, size_t shift,
    bool overflow = true) noexcept;

/// Rotate (left/right).
/// ---------------------------------------------------------------------------

/// Bitwise left rotation of value by shift.
/// Rotation by any shift behaves as a continuous rotation.
template <typename Value, if_unsigned_integer<Value> = true>
constexpr Value rotate_left(const Value& value, size_t shift) noexcept;
template <typename Value, if_unsigned_integer<Value> = true>
constexpr void rotate_left_into(Value& value, size_t shift) noexcept;

/// Bitwise right rotation of value by shift.
/// Rotation by any shift behaves as a continuous rotation.
template <typename Value, if_unsigned_integer<Value> = true>
constexpr Value rotate_right(const Value& value, size_t shift) noexcept;
template <typename Value, if_unsigned_integer<Value> = true>
constexpr void rotate_right_into(Value& value, size_t shift) noexcept;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/bits.ipp>

#endif

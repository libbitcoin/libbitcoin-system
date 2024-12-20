/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/cast.hpp>

namespace libbitcoin {
namespace system {

/// Unlike native operators, these functions do not promote integer types.
/// and have well-defined behavior for any type-enabled parameterization.

/// Widths.
/// ---------------------------------------------------------------------------
    
/// Same as std::bit_width (C++20) except this supports signed and unitx.
/// See constants.hpp for width<>(), which provides the integral bit domain.
/// A negative value always returns bits<Value>.
template <typename Value, if_unsigned_integer<Value> = true>
constexpr size_t bit_width(Value value) NOEXCEPT;
template <typename Value, if_signed_integer<Value> = true>
constexpr size_t bit_width(Value value) NOEXCEPT;

// Because std::countl_* (C++20) functions return int.
template <typename Value, if_unsigned_integer<Value> = true>
constexpr size_t left_zeros(Value value) NOEXCEPT;
template <typename Value, if_unsigned_integer<Value> = true>
constexpr size_t right_zeros(Value value) NOEXCEPT;

// Because std::countr_* (C++20) functions return int.
template <typename Value, if_unsigned_integer<Value> = true>
constexpr size_t left_ones(Value value) NOEXCEPT;
template <typename Value, if_unsigned_integer<Value> = true>
constexpr size_t right_ones(Value value) NOEXCEPT;

/// Bitwise logical operations.
/// ---------------------------------------------------------------------------

/// Bitwise NOT (~value) operation.
/// One's complement, the bit inversion of any value.
template <typename Value, if_integer<Value> = true>
constexpr Value bit_not(Value value) NOEXCEPT;

/// Bitwise AND (left & right) operation.
template <typename Value, if_integer<Value> = true>
constexpr Value bit_and(Value left, Value right) NOEXCEPT;

/// Bitwise OR (left | right)  operation.
template <typename Value, if_integer<Value> = true>
constexpr Value bit_or(Value left, Value right) NOEXCEPT;

/// Bitwise XOR (left ^ right)  operation.
template <typename Value, if_integer<Value> = true>
constexpr Value bit_xor(Value left, Value right) NOEXCEPT;

// Value factories.
// ---------------------------------------------------------------------------

template <typename Value, if_integer<Value> = true>
constexpr Value bit_all = bit_not<Value>(0);

template <typename Value, if_integer<Value> = true>
constexpr Value bit_lo = to_int<Value>(true);

template <typename Value, if_integral_integer<Value> = true>
constexpr Value bit_hi = depromote<Value>(bit_lo<Value> << sub1(bits<Value>));

/// A single bit bitmask, offset relative to highest order bit.
template <typename Value, if_integral_integer<Value> = true>
constexpr Value bit_left(size_t offset=zero) NOEXCEPT;

/// A single bit bitmask, offset relative to lowest order bit.
template <typename Value, if_integral_integer<Value> = true>
constexpr Value bit_right(size_t offset=zero) NOEXCEPT;

/// Extract a bit from value, offset relative to highest order bit.
template <typename Value, if_integral_integer<Value> = true>
constexpr bool get_left(Value value, size_t offset=zero) NOEXCEPT;

/// Extract a bit from value, offset relative to lowest order bit.
template <typename Value, if_integral_integer<Value> = true>
constexpr bool get_right(Value value, size_t offset=zero) NOEXCEPT;

/// Set (a bit).
/// ---------------------------------------------------------------------------

/// Set a bit in target, offset relative to highest order bit.
template <typename Value, if_integral_integer<Value> = true>
constexpr Value set_left(Value target, size_t offset=zero,
    bool state=true) NOEXCEPT;
template <typename Value, if_integral_integer<Value> = true>
constexpr void set_left_into(Value& target, size_t offset=zero,
    bool state=true) NOEXCEPT;

/// Set a bit in target, offset relative to lowest order bit.
template <typename Value, if_integral_integer<Value> = true>
constexpr Value set_right(Value target, size_t offset=zero,
    bool state=true) NOEXCEPT;
template <typename Value, if_integral_integer<Value> = true>
constexpr void set_right_into(Value& target, size_t offset=zero,
    bool state=true) NOEXCEPT;

/// Mask (a field of bits from left/right).
/// ---------------------------------------------------------------------------

/// A set of bitmasks with high order count of bits unset.
template <typename Value, if_integral_integer<Value> = true>
constexpr Value mask_left(size_t bits=one) NOEXCEPT;
template <typename Value, if_integral_integer<Value> = true>
constexpr Value mask_left(Value target, size_t bits=one) NOEXCEPT;
template <typename Value, if_integral_integer<Value> = true>
constexpr void mask_left_into(Value& target, size_t bits=one) NOEXCEPT;

/// A set of bitmasks with low order count of bits unset.
template <typename Value, if_integral_integer<Value> = true>
constexpr Value mask_right(size_t bits=one) NOEXCEPT;
template <typename Value, if_integral_integer<Value> = true>
constexpr Value mask_right(Value target, size_t bits=one) NOEXCEPT;
template <typename Value, if_integral_integer<Value> = true>
constexpr void mask_right_into(Value& target, size_t bits=one) NOEXCEPT;

/// Unmask (~mask).
/// ---------------------------------------------------------------------------

/// A set of bitflags with high order count of bits set.
template <typename Value, if_integral_integer<Value> = true>
constexpr Value unmask_left(size_t bits=one) NOEXCEPT;
template <typename Value, if_integral_integer<Value> = true>
constexpr Value unmask_left(Value target, size_t bits=one) NOEXCEPT;
template <typename Value, if_integral_integer<Value> = true>
constexpr Value unmask_left_into(Value& target, size_t bits=one) NOEXCEPT;

/// A set of bitflags with low order count of bits set.
template <typename Value, if_integral_integer<Value> = true>
constexpr Value unmask_right(size_t bits=one) NOEXCEPT;
template <typename Value, if_integral_integer<Value> = true>
constexpr Value unmask_right(Value target, size_t bits=one) NOEXCEPT;
template <typename Value, if_integral_integer<Value> = true>
constexpr void unmask_right_into(Value& target, size_t bits=one) NOEXCEPT;

/// Shift (left/right).
/// ---------------------------------------------------------------------------

/// Bitwise LEFT SHIFT (value << shift) operation.
/// Set overflow to false to modulo shift by Value width.
/// Signed overload interpret value as unsigned for shift.
template <typename Value, if_signed_integral_integer<Value> = true>
constexpr Value shift_left(Value value, size_t shift=one,
    bool overflow=true) NOEXCEPT;
template <typename Value, if_unsigned_integral_integer<Value> = true>
constexpr Value shift_left(Value value, size_t shift=one,
    bool overflow=true) NOEXCEPT;
template <typename Value, if_signed_integral_integer<Value> = true>
constexpr void shift_left_into(Value& value, size_t shift=one,
    bool overflow=true) NOEXCEPT;
template <typename Value, if_unsigned_integral_integer<Value> = true>
constexpr void shift_left_into(Value& value, size_t shift=one,
    bool overflow=true) NOEXCEPT;

/// Bitwise RIGHT SHIFT (value >> shift) operation.
/// Set overflow to false to modulo shift by Value width.
/// Signed overload interpret value as unsigned for shift.
template <typename Value, if_signed_integral_integer<Value> = true>
constexpr Value shift_right(Value value, size_t shift=one,
    bool overflow=true) NOEXCEPT;
template <typename Value, if_unsigned_integral_integer<Value> = true>
constexpr Value shift_right(Value value, size_t shift=one,
    bool overflow=true) NOEXCEPT;
template <typename Value, if_signed_integral_integer<Value> = true>
constexpr void shift_right_into(Value& value, size_t shift=one,
    bool overflow=true) NOEXCEPT;
template <typename Value, if_unsigned_integral_integer<Value> = true>
constexpr void shift_right_into(Value& value, size_t shift=one,
    bool overflow=true) NOEXCEPT;

/// Rotate (left/right).
/// ---------------------------------------------------------------------------
/// A negative rotation is a positive shift of the corresponding function.

/// Bitwise left rotation of value by shift.
/// Rotation by any shift behaves as a continuous rotation.
template <typename Value, if_integral_integer<Value> = true>
constexpr Value rotate_left(Value value, size_t shift=one) NOEXCEPT;
template <typename Value, if_integral_integer<Value> = true>
constexpr void rotate_left_into(Value& value, size_t shift=one) NOEXCEPT;

/// Bitwise right rotation of value by shift.
/// Rotation by any shift behaves as a continuous rotation.
template <typename Value, if_integral_integer<Value> = true>
constexpr Value rotate_right(Value value, size_t shift=one) NOEXCEPT;
template <typename Value, if_integral_integer<Value> = true>
constexpr void rotate_right_into(Value& value, size_t shift=one) NOEXCEPT;

/// High/Low word extraction.
/// ---------------------------------------------------------------------------

/// Extract the high sizeof(To) bits from From.
template <typename To, typename From,
    if_integral_integer<To> = true,
    if_uintx<From> = true>
constexpr To hi_word(From value) NOEXCEPT;
template <typename To, typename From,
    if_integral_integer<To> = true,
    if_not_uintx<From> = true>
constexpr To hi_word(From value) NOEXCEPT;

/// Extract the low sizeof(To) bits from From.
template <typename To, typename From,
    if_integral_integer<To> = true,
    if_uintx<From> = true>
constexpr To lo_word(From value) NOEXCEPT;
template <typename To, typename From,
    if_integral_integer<To> = true,
    if_not_uintx<From> = true>
constexpr To lo_word(From value) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/bits.ipp>

#endif

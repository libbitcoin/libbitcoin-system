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
#ifndef LIBBITCOIN_SYSTEM_MATH_BITS_IPP
#define LIBBITCOIN_SYSTEM_MATH_BITS_IPP

#include <cstddef>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/division.hpp>
#include <bitcoin/system/math/power.hpp>
#include <bitcoin/system/math/safe.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

// These functions avoid integral promotion, by recasting to the original type.
// The native operators promote sizeof(type) < 32 operands to int, causing
// a narrowing warning when it is coerced back to a positive Value type.
// Use of =&, =|,=<<, =>> is unguarded as these operators are not subject to
// promotion. All other native operators are guarded against promotion.

// Bit field width determinations.
// ----------------------------------------------------------------------------

template <typename Value, if_unsigned_integer<Value>>
constexpr size_t bit_width(Value value) noexcept
{
    return is_zero(value) ? zero : add1(floored_log2(value));
}

// Negtiave *values* always return width<Value>() as the high order bit is set.
template <typename Value, if_signed_integer<Value>>
constexpr size_t bit_width(Value value) noexcept
{
    return bit_width(to_unsigned(value));
}

template <typename Value, if_integer<Value>>
constexpr size_t byte_width(Value value) noexcept
{
    return ceilinged_divide(bit_width(value), byte_bits);
}

// Bitwise logical operations.
// ----------------------------------------------------------------------------

template <typename Value, if_integer<Value>>
constexpr Value twos_complement(Value value) noexcept
{
    return add1(bit_not(value));
}

// OPERATOR ~
template <typename Value, if_integer<Value>>
constexpr Value bit_not(Value value) noexcept
{
    // Revert domain promotion for < 32 bit Value.
    return possible_narrow_and_sign_cast<Value>(~value);
}

// OPERATOR &
template <typename Value, if_integer<Value>>
constexpr Value bit_and(Value left, Value right) noexcept
{
    // Revert sign and/or domain promotion for < 32 bit values.
    return possible_narrow_and_sign_cast<Value>(left & right);
}

// OPERATOR |
template <typename Value, if_integer<Value>>
constexpr Value bit_or(Value left, Value right) noexcept
{
    // Revert sign and/or domain promotion for < 32 bit Value.
    return possible_narrow_and_sign_cast<Value>(left | right);
}

// OPERATOR ^
template <typename Value, if_integer<Value>>
constexpr Value bit_xor(Value left, Value right) noexcept
{
    // Revert sign and/or domain promotion for < 32 bit Value.
    return possible_narrow_and_sign_cast<Value>(left ^ right);
}

// Value factories.
// ----------------------------------------------------------------------------

template <typename Value, if_integer<Value>>
constexpr Value bit_all() noexcept
{
    return bit_not<Value>(0);
}

template <typename Value, if_integral_integer<Value>>
constexpr Value bit_hi() noexcept
{
    // a << b is undefined for negative a, but shift into a negative is valid.
    return shift_left(to_int<Value>(true), sub1(width<Value>()));
}

template <typename Value, if_integer<Value>>
constexpr Value bit_lo() noexcept
{
    return to_int<Value>(true);
}

template <typename Value, if_integral_integer<Value>>
constexpr Value bit_left(size_t offset) noexcept
{
    // a >> b is implementation-dependent for negative a:
    // "or negative a, the value of a >> b is implementation-defined (in most
    // implementations, this performs arithmetic right shift, so that the
    // result remains negative)." - so shift to the left (see bit_right).
    // en.cppreference.com/w/cpp/language/operator_arithmetic

    // Subtraction is between size_t operands and guarded.
    // This converts the left offset to a right offset, overflow true.
    return offset >= width<Value>() ? 0 :
        bit_right<Value>(sub1(width<Value>() - offset));
}

template <typename Value, if_integral_integer<Value>>
constexpr Value bit_right(size_t offset) noexcept
{
    // a << b is undefined for negative a, but shift into a negative is valid:
    // "For signed and non-negative a, if a**b is representable in the unsigned
    // version of the return type, then that value, converted to signed, is the
    // value of a << b (this makes it legal to create INT_MIN as 1<<31)."
    // en.cppreference.com/w/cpp/language/operator_arithmetic
    return shift_left(bit_lo<Value>(), offset);
}

// Get (a bit).
// ----------------------------------------------------------------------------

template <typename Value, if_integral_integer<Value>>
constexpr bool get_left(Value value, size_t offset) noexcept
{
    return to_bool(bit_and(value, bit_left<Value>(offset)));
}

template <typename Value, if_integral_integer<Value>>
constexpr bool get_right(Value value, size_t offset) noexcept
{
    return to_bool(bit_and(value, bit_right<Value>(offset)));
}

// Set (a bit).
// ----------------------------------------------------------------------------

template <typename Value, if_integral_integer<Value>>
constexpr Value set_left(const Value& target, size_t offset,
    bool state) noexcept
{
    const auto bit = bit_left<Value>(offset);
    return state ? bit_or(target, bit) : bit_and(target, bit_not(bit));
}

template <typename Value, if_integral_integer<Value>>
constexpr void set_left_into(Value& target, size_t offset,
    bool state) noexcept
{
    const auto bit = bit_left<Value>(offset);
    state ? target |= bit : target &= bit_not(bit);
}

template <typename Value, if_integral_integer<Value>>
constexpr Value set_right(const Value& target, size_t offset,
    bool state) noexcept
{
    const auto bit = bit_right<Value>(offset);
    return state ? bit_or(target, bit) : bit_and(target, bit_not(bit));
}

template <typename Value, if_integral_integer<Value>>
constexpr void set_right_into(Value& target, size_t offset,
    bool state) noexcept
{
    const auto bit = bit_right<Value>(offset);
    state ? target |= bit : target &= bit_not(bit);
}

// Mask (a field of bits from left/right).
// ----------------------------------------------------------------------------

// C++ standard: "Right-shift on signed integral types is an arithmetic right
// shift, which performs sign-extension". In other words, repeatedly shifting
// -1 of any integer width will produce "1" bits, indefinitely.

// a << b is undefined for negative a.
// a >> b is implementation-dependent for negative a.

template <typename Value, if_integral_integer<Value>>
constexpr Value mask_left(size_t bits) noexcept
{
    return shift_right(bit_all<Value>(), bits, true);
}

template <typename Value, if_integral_integer<Value>>
constexpr Value mask_left(const Value& value, size_t bits) noexcept
{
    return bit_and(value, mask_left<Value>(bits));
}

template <typename Value, if_integral_integer<Value>>
constexpr void mask_left_into(Value& value, size_t bits) noexcept
{
    value &= mask_left<Value>(bits);
}

template <typename Value, if_integral_integer<Value>>
constexpr Value mask_right(size_t bits) noexcept
{
    return shift_left<Value>(bit_all<Value>(), bits, true);
}

template <typename Value, if_integral_integer<Value>>
constexpr Value mask_right(const Value& value, size_t bits) noexcept
{
    return bit_and(value, mask_right<Value>(bits));
}

template <typename Value, if_integral_integer<Value>>
constexpr void mask_right_into(Value& value, size_t bits) noexcept
{
    value &= mask_right<Value>(bits);
}

// Unmask (~mask).
// ----------------------------------------------------------------------------

template <typename Value, if_integral_integer<Value>>
constexpr Value unmask_left(size_t bits) noexcept
{
    return bit_not(mask_left<Value>(bits));
}

template <typename Value, if_integral_integer<Value>>
constexpr Value unmask_left(const Value& value, size_t bits) noexcept
{
    return bit_or(unmask_left<Value>(bits), value);
}

template <typename Value, if_integral_integer<Value>>
constexpr void unmask_left_into(Value& value, size_t bits) noexcept
{
    value |= unmask_left<Value>(bits);
}

template <typename Value, if_integral_integer<Value>>
constexpr Value unmask_right(size_t bits) noexcept
{
    return bit_not(mask_right<Value>(bits));
}

template <typename Value, if_integral_integer<Value>>
constexpr Value unmask_right(const Value& value, size_t bits) noexcept
{
    return bit_or(unmask_right<Value>(bits), value);
}

template <typename Value, if_integral_integer<Value>>
constexpr void unmask_right_into(Value& value, size_t bits) noexcept
{
    value |= unmask_right<Value>(bits);
}

// Shift (left/right).
// ----------------------------------------------------------------------------

// C++ standard: "The behavior is undefined if the right operand is negative,
// or greater than or equal to the width of the promoted left operand."
// These functions make explicit the intended behavior in that condition.
// overflow true is logically equivalent to shifting by one the specified
// shift number of times, which results in a zero if shift >= width(value).
// overflow true shifts shift % width(value), which cannot zeroize the value.
// These are both default behaviors of different compilers. Shift left/right of
// signed Value transits unsigned, as native -<</->> is undefined/unspecified.

// OPERATOR <<
template <typename Value, if_unsigned_integral_integer<Value>>
constexpr Value shift_left(const Value& value, size_t shift,
    bool overflow) noexcept
{
    constexpr auto span = width<Value>();
    return overflow && shift >= span ? 0 : value << shift % span;
}
template <typename Value, if_unsigned_integral_integer<Value>>
constexpr void shift_left_into(Value& value, size_t shift,
    bool overflow) noexcept
{
    constexpr auto span = width<Value>();
    overflow && shift >= span ? value = 0 : value <<= shift % span;
}

// signed overloads (shift left of negative is undefined behavior).
template <typename Value, if_signed_integral_integer<Value>>
constexpr Value shift_left(const Value& value, size_t shift,
    bool overflow) noexcept
{
    constexpr auto span = width<Value>();
    return overflow && shift >= span ? 0 :
        to_signed<Value>(to_unsigned(value) << shift % span);
}
template <typename Value, if_signed_integral_integer<Value>>
constexpr void shift_left_into(Value& value, size_t shift,
    bool overflow) noexcept
{
    // Defined behavior for negative value.
    value = to_signed<Value>(shift_left(to_unsigned(value), shift, overflow));
}

// OPERATOR >>
template <typename Value, if_unsigned_integral_integer<Value>>
constexpr Value shift_right(const Value& value, size_t shift,
    bool overflow) noexcept
{
    constexpr auto span = width<Value>();
    return overflow && shift >= span ? 0 : value >> shift % span;
}
template <typename Value, if_unsigned_integral_integer<Value>>
constexpr void shift_right_into(Value& value, size_t shift,
    bool overflow) noexcept
{
    constexpr auto span = width<Value>();
    overflow && shift >= span ? value = 0 : value >>= (shift % span);
}

// signed overloads (shift right of negative is unspecified behavior).
template <typename Value, if_signed_integral_integer<Value>>
constexpr Value shift_right(const Value& value, size_t shift,
    bool overflow) noexcept
{
    constexpr auto span = width<Value>();
    return overflow && shift >= span ? 0 :
        to_signed<Value>(to_unsigned(value) >> shift % span);
}
template <typename Value, if_signed_integral_integer<Value>>
constexpr void shift_right_into(Value& value, size_t shift,
    bool overflow) noexcept
{
    // Less efficient than =>>, but defined behavior for negative value.
    value = to_signed<Value>(shift_right(to_unsigned(value), shift, overflow));
}

// Rotate (left/right).
// ----------------------------------------------------------------------------

// C++20: std::rotl/rotr can replace rotate_left/rotate_right.
// C++20: std::rotl/rotr can replace rotate_left/rotate_right.
// Subtraction is between size_t operands and guarded by modulo.
// Shift left/right of negative is undefined/unspecified, so these are unsigned.

template <typename Value, if_unsigned_integral_integer<Value>>
constexpr Value rotate_left(const Value& value, size_t shift) noexcept
{
    constexpr auto span = width<Value>();
    return bit_or(shift_left(value, shift % span),
        shift_right(value, span - (shift % span)));
}

template <typename Value, if_unsigned_integral_integer<Value>>
constexpr void rotate_left_into(Value& value, size_t shift) noexcept
{
    constexpr auto span = width<Value>();
    value = bit_or(shift_left(value, shift % span),
        shift_right(value, span - (shift % span)));
}

template <typename Value, if_unsigned_integral_integer<Value>>
constexpr Value rotate_right(const Value& value, size_t shift) noexcept
{
    constexpr auto span = width<Value>();
    return bit_or(shift_right(value, shift % span),
        shift_left(value, span - (shift % span)));
}

template <typename Value, if_unsigned_integral_integer<Value>>
constexpr void rotate_right_into(Value& value, size_t shift) noexcept
{
    constexpr auto span = width<Value>();
    value = bit_or(shift_right(value, shift % span),
        shift_left(value, span - (shift % span)));
}

} // namespace system
} // namespace libbitcoin

#endif

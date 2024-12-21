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
#ifndef LIBBITCOIN_SYSTEM_MATH_BITS_IPP
#define LIBBITCOIN_SYSTEM_MATH_BITS_IPP

#include <bit>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/cast.hpp>
#include <bitcoin/system/math/logarithm.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

// Use depromote only for integral constrained functions.

// Widths.
// ----------------------------------------------------------------------------

// Called by bc::base2n (for sizing).
// Called by machine::number (for is_negated/to_unnegated).
template <typename Value, if_unsigned_integer<Value>>
constexpr size_t bit_width(Value value) NOEXCEPT
{
    // zero-based position of msb.
    return ceilinged_log2(value);
}

// Called by machine::number (for to_unnegated).
template <typename Value, if_signed_integer<Value>>
constexpr size_t bit_width(Value value) NOEXCEPT
{
    return is_negative(value) ? bits<Value> : bit_width(to_unsigned(value));
}

template <typename Value, if_unsigned_integer<Value>>
constexpr size_t left_zeros(Value value) NOEXCEPT
{
    return to_unsigned(std::countl_zero<Value>(value));
}

template <typename Value, if_unsigned_integer<Value>>
constexpr size_t right_zeros(Value value) NOEXCEPT
{
    return to_unsigned(std::countr_zero<Value>(value));
}

template <typename Value, if_unsigned_integer<Value>>
constexpr size_t left_ones(Value value) NOEXCEPT
{
    return to_unsigned(std::countl_one<Value>(value));
}

template <typename Value, if_unsigned_integer<Value>>
constexpr size_t right_ones(Value value) NOEXCEPT
{
    return to_unsigned(std::countr_one<Value>(value));
}

// Bitwise logical operations.
// ----------------------------------------------------------------------------

// OPERATOR ~
template <typename Value, if_integer<Value>>
constexpr Value bit_not(Value value) NOEXCEPT
{
    return possible_narrow_and_sign_cast<Value>(~value);
}

// OPERATOR &
template <typename Value, if_integer<Value>>
constexpr Value bit_and(Value left, Value right) NOEXCEPT
{
    // Promotion is safe as leading zeros are truncated.
    // Revert sign and/or domain promotion for < 32 bit values.
    return possible_narrow_and_sign_cast<Value>(left & right);
}

// OPERATOR |
template <typename Value, if_integer<Value>>
constexpr Value bit_or(Value left, Value right) NOEXCEPT
{
    // Promotion is safe as leading zeros are truncated.
    // Revert sign and/or domain promotion for < 32 bit Value.
    return possible_narrow_and_sign_cast<Value>(left | right);
}

// OPERATOR ^
template <typename Value, if_integer<Value>>
constexpr Value bit_xor(Value left, Value right) NOEXCEPT
{
    // Promotion is safe as leading zeros are truncated.
    // Revert sign and/or domain promotion for < 32 bit Value.
    return possible_narrow_and_sign_cast<Value>(left ^ right);
}

// Value factories.
// ----------------------------------------------------------------------------

template <typename Value, if_integral_integer<Value>>
constexpr Value bit_left(size_t offset) NOEXCEPT
{
    // a >> b is implementation-dependent for negative a:
    // "or negative a, the value of a >> b is implementation-defined (in most
    // implementations, this performs arithmetic right shift, so that the
    // result remains negative)." - so shift to the left (see bit_right).
    // en.cppreference.com/w/cpp/language/operator_arithmetic

    // Subtraction is between size_t operands and guarded.
    // This converts the left offset to a right offset, overflow true.
    return offset >= bits<Value> ? 0 :
        bit_right<Value>(sub1(bits<Value> - offset));
}

template <typename Value, if_integral_integer<Value>>
constexpr Value bit_right(size_t offset) NOEXCEPT
{
    // a << b is undefined for negative a, but shift into a negative is valid:
    // "For signed and non-negative a, if a**b is representable in the unsigned
    // version of the return type, then that value, converted to signed, is the
    // value of a << b (this makes it legal to create INT_MIN as 1<<31)."
    // en.cppreference.com/w/cpp/language/operator_arithmetic
    return shift_left(bit_lo<Value>, offset);
}

// Get (a bit).
// ----------------------------------------------------------------------------

template <typename Value, if_integral_integer<Value>>
constexpr bool get_left(Value value, size_t offset) NOEXCEPT
{
    return to_bool(bit_and(value, bit_left<Value>(offset)));
}

template <typename Value, if_integral_integer<Value>>
constexpr bool get_right(Value value, size_t offset) NOEXCEPT
{
    return to_bool(bit_and(value, bit_right<Value>(offset)));
}

// Set (a bit).
// ----------------------------------------------------------------------------

template <typename Value, if_integral_integer<Value>>
constexpr Value set_left(Value target, size_t offset, bool state) NOEXCEPT
{
    const auto bit = bit_left<Value>(offset);
    return state ? bit_or(target, bit) : bit_and(target, bit_not(bit));
}

template <typename Value, if_integral_integer<Value>>
constexpr void set_left_into(Value& target, size_t offset, bool state) NOEXCEPT
{
    const auto bit = bit_left<Value>(offset);
    state ? target |= bit : target &= bit_not(bit);
}

template <typename Value, if_integral_integer<Value>>
constexpr Value set_right(Value target, size_t offset, bool state) NOEXCEPT
{
    const auto bit = bit_right<Value>(offset);
    return state ? bit_or(target, bit) : bit_and(target, bit_not(bit));
}

template <typename Value, if_integral_integer<Value>>
constexpr void set_right_into(Value& target, size_t offset, bool state) NOEXCEPT
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
constexpr Value mask_left(size_t bits) NOEXCEPT
{
    return shift_right(bit_all<Value>, bits, true);
}

template <typename Value, if_integral_integer<Value>>
constexpr Value mask_left(Value value, size_t bits) NOEXCEPT
{
    return bit_and(value, mask_left<Value>(bits));
}

template <typename Value, if_integral_integer<Value>>
constexpr void mask_left_into(Value& value, size_t bits) NOEXCEPT
{
    value &= mask_left<Value>(bits);
}

template <typename Value, if_integral_integer<Value>>
constexpr Value mask_right(size_t bits) NOEXCEPT
{
    return shift_left(bit_all<Value>, bits, true);
}

template <typename Value, if_integral_integer<Value>>
constexpr Value mask_right(Value value, size_t bits) NOEXCEPT
{
    return bit_and(value, mask_right<Value>(bits));
}

template <typename Value, if_integral_integer<Value>>
constexpr void mask_right_into(Value& value, size_t bits) NOEXCEPT
{
    value &= mask_right<Value>(bits);
}

// Unmask (~mask).
// ----------------------------------------------------------------------------

template <typename Value, if_integral_integer<Value>>
constexpr Value unmask_left(size_t bits) NOEXCEPT
{
    return bit_not(mask_left<Value>(bits));
}

template <typename Value, if_integral_integer<Value>>
constexpr Value unmask_left(Value value, size_t bits) NOEXCEPT
{
    return bit_or(unmask_left<Value>(bits), value);
}

template <typename Value, if_integral_integer<Value>>
constexpr void unmask_left_into(Value& value, size_t bits) NOEXCEPT
{
    value |= unmask_left<Value>(bits);
}

template <typename Value, if_integral_integer<Value>>
constexpr Value unmask_right(size_t bits) NOEXCEPT
{
    return bit_not(mask_right<Value>(bits));
}

template <typename Value, if_integral_integer<Value>>
constexpr Value unmask_right(Value value, size_t bits) NOEXCEPT
{
    return bit_or(unmask_right<Value>(bits), value);
}

template <typename Value, if_integral_integer<Value>>
constexpr void unmask_right_into(Value& value, size_t bits) NOEXCEPT
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
constexpr Value shift_left(Value value, size_t shift, bool overflow) NOEXCEPT
{
    constexpr auto span = bits<Value>;
    return overflow && shift >= span ? 0 : depromote<Value>(
        value << (shift % span));
}
template <typename Value, if_unsigned_integral_integer<Value>>
constexpr void shift_left_into(Value& value, size_t shift, bool overflow) NOEXCEPT
{
    constexpr auto span = bits<Value>;
    overflow && shift >= span ? value = 0 : depromote<Value>(
        value <<= (shift % span));
}

// signed overloads (shift left of negative is undefined behavior).
template <typename Value, if_signed_integral_integer<Value>>
constexpr Value shift_left(Value value, size_t shift, bool overflow) NOEXCEPT
{
    constexpr auto span = bits<Value>;
    return overflow && shift >= span ? 0 : depromote<Value>(
        to_signed(to_unsigned(value) << (shift % span)));
}
template <typename Value, if_signed_integral_integer<Value>>
constexpr void shift_left_into(Value& value, size_t shift, bool overflow) NOEXCEPT
{
    // Defined behavior for negative value.
    value = to_signed<Value>(shift_left(to_unsigned(value), shift, overflow));
}

// OPERATOR >>
template <typename Value, if_unsigned_integral_integer<Value>>
constexpr Value shift_right(Value value, size_t shift, bool overflow) NOEXCEPT
{
    constexpr auto span = bits<Value>;
    return overflow && shift >= span ? 0 : depromote<Value>(
        value >> (shift % span));
}
template <typename Value, if_unsigned_integral_integer<Value>>
constexpr void shift_right_into(Value& value, size_t shift, bool overflow) NOEXCEPT
{
    constexpr auto span = bits<Value>;
    overflow && shift >= span ? value = 0 : depromote<Value>(
        value >>= (shift % span));
}

// signed overloads (shift right of negative is unspecified behavior).
template <typename Value, if_signed_integral_integer<Value>>
constexpr Value shift_right(Value value, size_t shift, bool overflow) NOEXCEPT
{
    constexpr auto span = bits<Value>;
    return overflow && shift >= span ? 0 : depromote<Value>(
        to_signed(to_unsigned(value) >> (shift % span)));
}
template <typename Value, if_signed_integral_integer<Value>>
constexpr void shift_right_into(Value& value, size_t shift,
    bool overflow) NOEXCEPT
{
    // Less efficient than =>>, but defined behavior for negative value.
    value = to_signed<Value>(shift_right(to_unsigned(value), shift, overflow));
}

// Rotate (left/right).
// ----------------------------------------------------------------------------
// C++20: std::rotl/rotr unsigned is not defined (at least for msvc).
// std lib should pass to intrinsic implementation (optimal).

template <typename Value, if_integral_integer<Value>>
constexpr Value rotate_left(Value value, size_t shift) NOEXCEPT
{
    if constexpr (!is_signed<Value>)
    {
        return std::rotl(value, possible_narrow_sign_cast<int>(shift));
    }
    else
    {
        constexpr auto span = bits<Value>;
        const auto cycle = shift % span;
        return bit_or(shift_left(value, cycle),
            shift_right(value, span - cycle));
    }
}

template <typename Value, if_integral_integer<Value>>
constexpr void rotate_left_into(Value& value, size_t shift) NOEXCEPT
{
    if constexpr (!is_signed<Value>)
    {
        value = std::rotl(value, possible_narrow_sign_cast<int>(shift));
        return;
    }
    else
    {
        constexpr auto span = bits<Value>;
        const auto cycle = shift % span;
        value = bit_or(shift_left(value, cycle),
            shift_right(value, span - cycle));
    }
}

template <typename Value, if_integral_integer<Value>>
constexpr Value rotate_right(Value value, size_t shift) NOEXCEPT
{
    if constexpr (!is_signed<Value>)
    {
        return std::rotr(value, possible_narrow_sign_cast<int>(shift));
    }
    else
    {
        constexpr auto span = bits<Value>;
        const auto cycle = shift % span;
        return bit_or(shift_right(value, cycle),
            shift_left(value, span - cycle));
    }
}

template <typename Value, if_integral_integer<Value>>
constexpr void rotate_right_into(Value& value, size_t shift) NOEXCEPT
{
    if constexpr (!is_signed<Value>)
    {
        value = std::rotr(value, possible_narrow_sign_cast<int>(shift));
        return;
    }
    else
    {
        constexpr auto span = bits<Value>;
        const auto cycle = shift % span;
        value = bit_or(shift_right(value, cycle),
            shift_left(value, span - cycle));
    }
}

// High/Low word extraction.
// ----------------------------------------------------------------------------

template <typename To, typename From,
    if_integral_integer<To>,
    if_uintx<From>>
constexpr To hi_word(From value) NOEXCEPT
{
    return (value >> bits<To>).template convert_to<To>();
}

template <typename To, typename From,
    if_integral_integer<To>,
    if_not_uintx<From>>
constexpr To hi_word(From value) NOEXCEPT
{
    return narrow_cast<To>(shift_right(value, bits<To>));
}

template <typename To, typename From,
    if_integral_integer<To>,
    if_uintx<From>>
constexpr To lo_word(From value) NOEXCEPT
{
    return value.template convert_to<To>();
}

template <typename To, typename From,
    if_integral_integer<To>,
    if_not_uintx<From>>
constexpr To lo_word(From value) NOEXCEPT
{
    return narrow_cast<To>(value);
}

} // namespace system
} // namespace libbitcoin

#endif

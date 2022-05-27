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

#include <cstddef>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/division.hpp>
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/math/power.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

// width (see constants.hpp for width<>()).

template <typename Integer, if_integer<Integer>>
inline size_t bit_width(Integer value) noexcept
{
    // C++20: std::bit_width.
    return add1(floored_log2(value));
}

template <typename Integer, if_integer<Integer>>
inline size_t byte_width(Integer value) noexcept
{
    return ceilinged_divide(bit_width(value), byte_bits);
}

// complements

template <typename Integer, if_integer<Integer>>
constexpr Integer ones_complement(Integer value) noexcept
{
    return ~value;
}

template <typename Integer, if_integer<Integer>>
constexpr Integer twos_complement(Integer value) noexcept
{
    return add1(ones_complement(value));
}

// bit

template <typename Value, if_integer<Value>>
constexpr Value bit_all() noexcept
{
    return ones_complement<Value>(0);
}

template <typename Value, if_integer<Value>>
constexpr Value bit_hi() noexcept
{
    // sub1 for size-to-index translation.
    // a << b is undefined for negative a, but shift into a negative is valid.
    // The << operator promotes the left operand to int, causing an overflow
    // warning when the value is coerced from a positive to a negative. So we
    // must explicitly cast the result back to the intended (Value) type.
    return static_cast<Value>(to_int<Value>(true) << sub1(width<Value>()));
}

template <typename Value, if_integer<Value>>
constexpr Value bit_lo() noexcept
{
    return to_int<Value>(true) << zero;
}

template <typename Value, if_integer<Value>>
constexpr Value bit_left(size_t offset) noexcept
{
    // a >> b is implementation-dependent for negative a:
    // "or negative a, the value of a >> b is implementation-defined (in most
    // implementations, this performs arithmetic right shift, so that the
    // result remains negative)." - so always shift from the right.
    // en.cppreference.com/w/cpp/language/operator_arithmetic
    return bit_right<Value>(sub1(width<Value>()) - offset);
}

// undefined if offset >= width<Value>()
template <typename Value, if_integer<Value>>
constexpr Value bit_right(size_t offset) noexcept
{
    // a << b is undefined for negative a, but shift into a negative is valid:
    // "For signed and non-negative a, if a**b is representable in the unsigned
    // version of the return type, then that value, converted to signed, is the
    // value of a << b (this makes it legal to create INT_MIN as 1<<31)."
    // en.cppreference.com/w/cpp/language/operator_arithmetic
    return bit_lo<Value>() << offset;
}

// get

template <typename Value, if_integer<Value>>
constexpr bool get_right(Value value, size_t offset) noexcept
{
    return to_bool(value & bit_right<Value>(offset));
}

template <typename Value, if_integer<Value>>
constexpr bool get_left(Value value, size_t offset) noexcept
{
    return to_bool(value & bit_left<Value>(offset));
}

// set

template <typename Value, if_integer<Value>>
constexpr Value set_left(const Value& target, size_t offset,
    bool state) noexcept
{
    // C++14: local variables allowed in constexpr.
    return state ? target | bit_left<Value>(offset) :
        target & ones_complement(bit_left<Value>(offset));
}

// constexpr reference assignment not allowed.
template <typename Value, if_integer<Value>>
inline void set_left_into(Value& target, size_t offset,
    bool state) noexcept
{
    // C++14: local variables allowed in constexpr.
    state ? target |= bit_left<Value>(offset) :
        target &= ones_complement(bit_left<Value>(offset));
}

template <typename Value, if_integer<Value>>
constexpr Value set_right(const Value& target, size_t offset,
    bool state) noexcept
{
    // C++14: local variables allowed in constexpr.
    return state ? target | bit_right<Value>(offset) :
        target & ones_complement(bit_right<Value>(offset));
}

// constexpr reference assignment not allowed.
template <typename Value, if_integer<Value>>
inline void set_right_into(Value& target, size_t offset,
    bool state) noexcept
{
    // C++14: local variables allowed in constexpr.
    state ? target |= bit_right<Value>(offset) :
        target &= ones_complement(bit_right<Value>(offset));
}

// All below support only unsigned Value because:
// a << b is undefined for negative a.
// a >> b is implementation-dependent for negative a.
// This is an implementation limitation and could otherwise be supported.

// mask

template <typename Value, if_unsigned_integer<Value>>
constexpr Value mask_left(size_t bits) noexcept
{
    return bit_all<Value>() >> bits;
}
template <typename Value, if_unsigned_integer<Value>>
constexpr Value mask_left(const Value& value, size_t bits) noexcept
{
    return value & mask_left<Value>(bits);
}

// constexpr reference assignment not allowed.
template <typename Value, if_unsigned_integer<Value>>
inline void mask_left_into(Value& value, size_t bits) noexcept
{
    value &= mask_left<Value>(bits);
}


template <typename Value, if_unsigned_integer<Value>>
constexpr Value mask_right(size_t bits) noexcept
{
    return bit_all<Value>() << bits;
}

template <typename Value, if_unsigned_integer<Value>>
constexpr Value mask_right(const Value& value, size_t bits) noexcept
{
    return value & mask_right<Value>(bits);
}

// constexpr reference assignment not allowed.
template <typename Value, if_unsigned_integer<Value>>
inline void mask_right_into(Value& value, size_t bits) noexcept
{
    value &= mask_right<Value>(bits);
}

// unmask

// Unmask supports only unsigned Value because:
// C++ standard: "Right-shift on signed integral types is an arithmetic right
// shift, which performs sign-extension". In other words, repeatedly shifting
// -1 of any integer width will produce "1" bits, indefinitely.
// This is an implementation limitation and could otherwise be supported.

template <typename Value, if_unsigned_integer<Value>>
constexpr Value unmask_left(size_t bits) noexcept
{
    // Suppress C4244: 'return' : conversion from 'Integer' to 'Value'.
    // The compiler does not recognize that Value and Integer must be same type.
    BC_PUSH_MSVC_WARNING(4244)

    return ones_complement(bit_all<Value>() >> bits);

    BC_POP_MSVC_WARNING()
}

template <typename Value, if_unsigned_integer<Value>>
constexpr Value unmask_left(const Value& value, size_t bits) noexcept
{
    return value | unmask_left<Value>(bits);
}

// constexpr reference assignment not allowed.
template <typename Value, if_unsigned_integer<Value>>
inline void unmask_left_into(Value& value, size_t bits) noexcept
{
    value |= unmask_left<Value>(bits);
}

template <typename Value, if_unsigned_integer<Value>>
constexpr Value unmask_right(size_t bits) noexcept
{
    // Suppress C4244: 'return' : conversion from 'Integer' to 'Value'.
    // The compiler does not recognize that Value and Integer must be same type.
    BC_PUSH_MSVC_WARNING(4244)

    return ones_complement(bit_all<Value>() << bits);

    BC_POP_MSVC_WARNING()
}

template <typename Value, if_unsigned_integer<Value>>
constexpr Value unmask_right(const Value& value, size_t bits) noexcept
{
    return value | unmask_right<Value>(bits);
}

// constexpr reference assignment not allowed.
template <typename Value, if_unsigned_integer<Value>>
inline void unmask_right_into(Value& value, size_t bits) noexcept
{
    value |= unmask_right<Value>(bits);
}

// rotate

// C++20: std::rotl/rotr can replace rotate_left/rotate_right.
// C++20: std::rotl/rotr can replace rotate_left/rotate_right.

template <typename Value, if_unsigned_integer<Value>>
constexpr Value rotate_left(const Value& value, size_t shift) noexcept
{
    // C++14: local variables allowed in constexpr.
    ////constexpr auto bits = width<Value>();
    return (value << (shift % width<Value>())) |
        (value >> (width<Value>() - (shift % width<Value>())));
}

// constexpr reference assignment not allowed.
template <typename Value, if_unsigned_integer<Value>>
inline void rotate_left_into(Value& value, size_t shift) noexcept
{
    // C++14: local variables allowed in constexpr.
    ////constexpr auto bits = width<Value>();
    (value = (value << (shift % width<Value>())) |
        (value >> (width<Value>() - (shift % width<Value>()))));
}

template <typename Value, if_unsigned_integer<Value>>
constexpr Value rotate_right(const Value& value, size_t shift) noexcept
{
    // C++14: local variables allowed in constexpr.
    ////constexpr auto bits = width<Value>();
    return (value << (width<Value>() - (shift % width<Value>()))) |
        (value >> (shift % width<Value>()));
}

// constexpr reference assignment not allowed.
template <typename Value, if_unsigned_integer<Value>>
inline void rotate_right_into(Value& value, size_t shift) noexcept
{
    // C++14: local variables allowed in constexpr.
    ////constexpr auto bits = width<Value>();
    (value = (value << (width<Value>() - (shift % width<Value>())) |
        (value >> (shift % width<Value>()))));
}

// shift

// C++ standard: "The behavior is undefined if the right operand is negative,
// or greater than or equal to the width of the promoted left operand."
// These functions make explicit the intended behavior in that condition.
// overflow true is logically equivalent to shifting by one the specified
// shift number of times, which results in a zero if shift >= width(value).
// overflow true shifts shift % width(value), which cannot zeroize the value.
// These are both default behaviors of different compilers.

template <typename Value, if_unsigned_integer<Value>>
constexpr Value shift_left(const Value& value, size_t shift,
    bool overflow) noexcept
{
    // C++14: local variables allowed in constexpr.
    ////constexpr auto bits = width<Value>();
    return (overflow && shift >= width<Value>()) ? 0 :
        value << (shift % width<Value>());
}

// constexpr reference assignment not allowed.
template <typename Value, if_unsigned_integer<Value>>
inline void shift_left_into(Value& value, size_t shift,
    bool overflow) noexcept
{
    // C++14: local variables allowed in constexpr.
    ////constexpr auto bits = width<Value>();
    overflow && shift >= width<Value>() ? value = 0 :
        value <<= (shift % width<Value>());
}

template <typename Value, if_unsigned_integer<Value>>
constexpr Value shift_right(const Value& value, size_t shift,
    bool overflow) noexcept
{
    // C++14: local variables allowed in constexpr.
    ////constexpr auto bits = width<Value>();
    return (overflow && shift >= width<Value>()) ? 0 :
        value >> (shift % width<Value>());
}

// constexpr reference assignment not allowed.
template <typename Value, if_unsigned_integer<Value>>
inline void shift_right_into(Value& value, size_t shift,
    bool overflow) noexcept
{
    // C++14: local variables allowed in constexpr.
    ////constexpr auto bits = width<Value>();
    overflow && shift >= width<Value>() ? value = 0 :
        value >>= (shift % width<Value>());
}

} // namespace system
} // namespace libbitcoin

#endif

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
#ifndef LIBBITCOIN_SYSTEM_MATH_OVERFLOW_IPP
#define LIBBITCOIN_SYSTEM_MATH_OVERFLOW_IPP

#include <limits>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/cast.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

// Avoid circularity with limits.
template <typename Integral, if_integer<Integral> = true>
constexpr Integral minimum_ = std::numeric_limits<Integral>::min();
template <typename Integral, if_integer<Integral> = true>
constexpr Integral maximum_ = std::numeric_limits<Integral>::max();

// shift
// ----------------------------------------------------------------------------

template <typename Value, if_unsigned_integral_integer<Value>>
constexpr bool is_left_shift_overflow(Value value, size_t shift) NOEXCEPT
{
    return to_bool(bit_and(value, unmask_left<Value>(shift)));
}

template <typename Value, if_unsigned_integral_integer<Value>>
constexpr bool is_right_shift_overflow(Value value, size_t shift) NOEXCEPT
{
    return to_bool(bit_and(value, unmask_right<Value>(shift)));
}

// add/subtract
// ----------------------------------------------------------------------------

template <typename Signed, if_signed_integer<Signed>>
constexpr bool is_negate_overflow(Signed value) NOEXCEPT
{
    return value == minimum_<Signed>;
}

template <typename Signed, if_signed_integer<Signed>>
constexpr bool is_add_overflow(Signed left, Signed right) NOEXCEPT
{
    const auto negative_right = is_negative(right);

    return is_nonzero(right) &&
        (!negative_right || (left < (minimum_<Signed> - right))) &&
        ( negative_right || (left > (maximum_<Signed> - right)));
}

template <typename Unsigned, if_unsigned_integer<Unsigned>>
constexpr bool is_add_overflow(Unsigned left, Unsigned right) NOEXCEPT
{
    return right > (maximum_<Unsigned> - left);
}

template <typename Signed, if_signed_integer<Signed>>
constexpr bool is_subtract_overflow(Signed left, Signed right) NOEXCEPT
{
    const auto negative_right = is_negative(right);

    return is_nonzero(right) &&
        (!negative_right || (left > (maximum_<Signed> + right))) &&
        ( negative_right || (left < (minimum_<Signed> + right)));
}

template <typename Unsigned, if_unsigned_integer<Unsigned>>
constexpr bool is_subtract_overflow(Unsigned left, Unsigned right) NOEXCEPT
{
    return right > left;
}
    
// multiply/divide
// ----------------------------------------------------------------------------

template <typename Unsigned, if_unsigned_integer<Unsigned>>
constexpr bool is_multiply_overflow(Unsigned left, Unsigned right) NOEXCEPT
{
    return is_nonzero(right) && (left > (maximum_<Unsigned> / right));
}

template <typename Dividend, typename Divisor,
    if_integer<Dividend>, if_integer<Divisor>>
constexpr bool is_divide_overflow(Dividend, Divisor divisor) NOEXCEPT
{
    return is_zero(divisor);
}

// power/log
// ----------------------------------------------------------------------------
// TODO: type-constrain Exponent by Base/Value to preclude value overflow.
// TODO: type-constrain Value by Base/Exponent to preclude value overflow.

// 0^0
template <size_t Base, typename Exponent,
    if_integer<Exponent>>
constexpr bool is_power_overflow(Exponent exponent) NOEXCEPT
{
    return is_zero(Base) && is_zero(exponent);
}

// 0^0
template <typename Base, typename Exponent,
    if_integer<Base>,
    if_integer<Exponent>>
constexpr bool is_power_overflow(Base base, Exponent exponent) NOEXCEPT
{
    return is_zero(base) && is_zero(exponent);
}

// base < 2 or value < 1
template <size_t Base, typename Value,
    if_integer<Value>>
constexpr bool is_log_overflow(Value value) NOEXCEPT
{
    return (Base < 2_size || value < 1);
}

// base < 2 or value < 1
template <typename Base, typename Value,
    if_integer<Base>,
    if_integer<Value>>
constexpr bool is_log_overflow(Base base, Value value) NOEXCEPT
{
    return (base < 2 || value < 1);
}

// safe functions (throw on overflow)
// ----------------------------------------------------------------------------
// TODO: power/log do not require safe calls as they are type-constrained
// TODO: against domain overflow and return zero when undefined.

// Signed only, since unsigned is always safe.
template <typename Signed, if_signed_integer<Signed>>
constexpr Signed safe_negate(Signed value) THROWS
{
    if (is_negate_overflow(value))
        throw overflow_exception("negation overflow");

    return depromote<Signed>(-promote(value));
}

// Unsigned only, since using native operator.
template <typename Integral, if_integer<Integral>>
constexpr Integral safe_add(Integral left, Integral right) THROWS
{
    // is_add_overflow handles common types (signed or unsigned).
    // is_add_overflow guards against overflow in the operands domain.
    if (is_add_overflow(left, right))
        throw overflow_exception("addition overflow");

    // TODO: generalize is_add_overflow so can use add<>() here.
    return depromote<Integral>(promote(left) + promote(right));
}

// Unsigned only, since using native operator.
template <typename Integral, if_integer<Integral>>
constexpr Integral safe_subtract(Integral left, Integral right) THROWS
{
    // is_subtract_overflow handles common types (signed or unsigned).
    // is_subtract_overflow guards against overflow in the operands domain.
    if (is_subtract_overflow(left, right))
        throw overflow_exception("subtraction overflow");

    // TODO: generalize is_subtract_overflow so can use add<>() here.
    return depromote<Integral>(promote(left) - promote(right));
}

// Unsigned only, since using native operator.
template <typename Unsigned, if_unsigned_integer<Unsigned>>
constexpr Unsigned safe_multiply(Unsigned left, Unsigned right) THROWS
{
    if (is_multiply_overflow(left, right))
        throw overflow_exception("multiplication overflow");
        
    // TODO: create type-generalized multiply<>().
    // TODO: generalize is_multiply_overflow so can use multiply<>() here.
    return depromote<Unsigned>(promote(left) * promote(right));
}

// Integers, since only checking for zero.
template <typename Dividend, typename Divisor,
    if_integer<Dividend>, if_integer<Divisor>>
constexpr Dividend safe_divide(Dividend dividend, Divisor divisor) THROWS
{
    if (is_divide_overflow(dividend, divisor))
        throw overflow_exception("division overflow");

    // TODO: create type-generalized divide<>().
    // TODO: generalize is_divide_overflow so can use divide<>() here.
    return depromote<Dividend>(promote(dividend) / promote(divisor));
}

// Integers, since only checking for zero.
template <typename Dividend, typename Divisor,
    if_integer<Dividend>, if_integer<Divisor>>
constexpr Dividend safe_modulo(Dividend dividend, Divisor divisor) THROWS
{
    if (is_divide_overflow(dividend, divisor))
        throw overflow_exception("modulo overflow");

    // TODO: create type-generalized modulo<>().
    // TODO: generalize is_divide_overflow so can use modulo<>() here.
    return depromote<Dividend>(promote(dividend) % promote(divisor));
}

} // namespace system
} // namespace libbitcoin

#endif

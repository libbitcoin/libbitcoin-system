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
#ifndef LIBBITCOIN_SYSTEM_MATH_POWER_IPP
#define LIBBITCOIN_SYSTEM_MATH_POWER_IPP

#include <bit>
#include <cstddef>
#include <cstdint>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/safe.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

// Ceilinged logarithms.
// ----------------------------------------------------------------------------

// Returns 0 for undefined (base < 2 or value < 1).
template <typename Exponent, typename Base, typename Value,
    if_integer<Exponent>, if_integer<Base>, if_integer<Value>>
constexpr Exponent ceilinged_log(Base base, Value value) noexcept
{
    if (base < 2 || value < 1)
        return 0;

    Exponent exponent = 0;
    while (value > 0) { ++exponent; value /= base; }
    return exponent;
}

// TODO: test (uintx).
// Returns 0 for undefined (value < 1).
template <typename Exponent, typename Value,
    if_integer<Exponent>, if_non_integral_integer<Value>>
constexpr Exponent ceilinged_log2(Value value) noexcept
{
    if (value < 1)
        return 0;

    // TODO: uintx ceilinged_log2 can be implemented by high bit (syntax?).
    Exponent exponent = 0;
    while (value > 0) { ++exponent; value >>= 1; }
    return exponent;
}

// TODO: test (uintx).
// Returns 0 for undefined (value < 1).
template <typename Exponent, typename Value,
    if_integer<Exponent>, if_non_integral_integer<Value>>
constexpr size_t ceilinged_log256(Value value) noexcept
{
    if (value < 1)
        return 0;

    // Shift is undefined if >= to the width of the promoted left operand.
    if constexpr (sizeof(Exponent) == one)
        return 0;

    Exponent exponent = 0;
    while (value > 0) { ++exponent; value >>= 8; }
    return exponent;
}

// optimizations

// Returns 0 for undefined (value < 1).
template <size_t Base, typename Exponent, typename Value>
constexpr Exponent ceilinged_log(Value value) noexcept
{
    if constexpr (Base == 2u)
        return ceilinged_log2<Exponent>(value);
    else if constexpr (Base == 256u)
        return ceilinged_log256<Exponent>(value);
    else
        return ceilinged_log<Exponent>(Base, value);
}
    
// Returns 0 for undefined (value < 1).
template <typename Exponent, typename Value,
    if_integer<Exponent>, if_integral_integer<Value>>
constexpr Exponent ceilinged_log2(Value value) noexcept
{
    if (value < 1)
        return 0;

    return possible_narrow_and_sign_cast<Exponent>(
        std::bit_width(to_unsigned(value)));
}

// Returns 0 for undefined (value < 1).
template <typename Exponent, typename Value,
    if_integer<Exponent>, if_integral_integer<Value>>
constexpr size_t ceilinged_log256(Value value) noexcept
{
    if (value < 1)
        return 0;

    return possible_narrow_and_sign_cast<size_t>(
        std::bit_width(to_unsigned(value)) / 8u);
}

// Floored logarithms.
// ----------------------------------------------------------------------------

// Returns 0 for undefined (base < 2 or value < 1).
template <typename Exponent, typename Base, typename Value,
    if_integer<Exponent>, if_integer<Base>, if_integer<Value>>
constexpr Exponent floored_log(Base base, Value value) noexcept
{
    if (base < 2 || value < 1)
        return 0;

    Exponent exponent = 0;
    while (((value /= base)) > 0) { ++exponent; }
    return exponent;
}

// TODO: test (uintx).
// Returns 0 for undefined (value < 1).
template <typename Exponent, typename Value,
    if_integer<Exponent>, if_non_integral_integer<Value>>
constexpr Exponent floored_log2(Value value) noexcept
{
    if (value < 1)
        return 0;

    Exponent exponent = 0;
    while (((value >>= 1)) > 0) { ++exponent; };
    return exponent;
}

// TODO: test (uintx).
// Returns 0 for undefined (value < 1).
template <typename Exponent, typename Value,
    if_integer<Exponent>, if_non_integral_integer<Value>>
constexpr Exponent floored_log256(Value value) noexcept
{
    if (value < 1)
        return 0;

    // Shift is undefined if >= to the width of the promoted left operand.
    if constexpr (sizeof(Exponent) == one)
        return 0;

    Exponent exponent = 0;
    while (((value >>= 8)) > 0) { ++exponent; };
    return exponent;
}

// optimizations

// Returns 0 for undefined (value < 1).
template <size_t Base, typename Exponent, typename Value>
constexpr Exponent floored_log(Value value) noexcept
{
    if constexpr (Base == 2u)
        return floored_log2<Exponent>(value);
    else if constexpr (Base == 256u)
        return floored_log256<Exponent>(value);
    else
        return floored_log<Exponent>(Base, value);
}

// Returns 0 for undefined (value < 1).
template <typename Exponent, typename Value,
    if_integer<Exponent>, if_integral_integer<Value>>
constexpr Exponent floored_log2(Value value) noexcept
{
    if (value < 1)
        return 0;

    return possible_narrow_and_sign_cast<Exponent>(
        sub1(std::bit_width(to_unsigned(value))));
}

// Returns 0 for undefined (value < 1).
template <typename Exponent, typename Value,
    if_integer<Exponent>, if_integral_integer<Value>>
constexpr Exponent floored_log256(Value value) noexcept
{
    if (value < 1)
        return 0;

    return possible_narrow_and_sign_cast<size_t>(
        sub1(std::bit_width(to_unsigned(value))) / 8u);
}

// Powers.
// ----------------------------------------------------------------------------

// Returns 0 for undefined (0^0).
template <typename Value, typename Base, typename Exponent,
    if_integer<Value>, if_integer<Base>, if_integer<Exponent>>
constexpr Value power(Base base, Exponent exponent) noexcept
{
    if (is_zero(base))
        return 0;

    if (is_zero(exponent))
        return 1;

    if (is_negative(exponent))
        return absolute(base) > 1 ? 0 :
            (is_odd(exponent) && is_negative(base) ? -1 : 1);

    // Allows Value narrower than Base.
    auto value = possible_narrow_and_sign_cast<Value>(base);

    // Overflow is allowed behavior as this mimics a mathematical operator.
    BC_PUSH_WARNING(NARROWING_CONVERSION)
    while (--exponent > 0) { value *= base; }
    BC_POP_WARNING();

    return value;
}

// optimizations

// Returns 0 for undefined (0^0).
template <size_t Base, typename Value, typename Exponent>
constexpr Value power(Exponent exponent) noexcept
{
    if constexpr (Base == 2u)
        return power2<Value>(exponent);
    else
        return power<Value>(Base, exponent);
}

// Returns 0 for undefined (0^0).
template <typename Value, typename Exponent,
    if_integral_integer<Value>, if_integer<Exponent>>
constexpr Value power2(Exponent exponent) noexcept
{
    if (is_negative(exponent))
        return 0;

    // Shift is undefined if >= to the width of the promoted left operand.
    if (!is_lesser(exponent, width<Value>()))
        return 0;

    return to_int<Value>(true) << to_unsigned(exponent);
}

// TODO: test (uintx).
// Returns 0 for undefined (0^0).
template <typename Value, typename Exponent,
    if_non_integral_integer<Value>, if_integer<Exponent>>
constexpr Value power2(Exponent exponent) noexcept
{
    if (is_negative(exponent))
        return 0;

    // TODO: uintx power2 can be implemented by bit_set(int, exponent).
    // TODO: uintx does not provide uintx << uintx overload (limit to integral).
    // Assumes that shifting greater than width is defined (cannot use width).
    return to_int<Value>(true) << to_unsigned(exponent);
}

} // namespace system
} // namespace libbitcoin

#endif

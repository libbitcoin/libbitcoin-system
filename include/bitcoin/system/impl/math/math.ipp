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
#ifndef LIBBITCOIN_SYSTEM_MATH_IPP
#define LIBBITCOIN_SYSTEM_MATH_IPP

#include <cstddef>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

// These eliminate (unsigned < 0) warnings (always false) and compile away the
// conditions entirely when unsigned (optimization).
// ----------------------------------------------------------------------------

template <typename Integer, IS_UNSIGNED_INTEGER(Integer)=true>
inline bool negative(Integer)
{
    return false;
}

template <typename Integer, IS_SIGNED_INTEGER(Integer)=true>
inline bool negative(Integer value)
{
    // The comparison compiles away for constexpr values.
    return value < 0;
}

template <typename Factor1, typename Factor2,
    IS_INTEGERS(Factor1, Factor2)=true>
inline bool negative(Factor1 factor1, Factor2 factor2)
{
    return negative(factor1) != negative(factor2);
}

// ----------------------------------------------------------------------------

template <typename Integer, IS_UNSIGNED_INTEGER(Integer)=true>
inline Integer absolute(Integer value)
{
    // This template supports the power template and compiles away.
    return value;
}

template <typename Integer, IS_SIGNED_INTEGER(Integer)>
inline Integer absolute(Integer value)
{
    // The negative call and ternary compile away for unsigned value.
    return negative(value) ? -value : value;
}

template <typename Integer, IS_INTEGER(Integer)>
inline Integer ceilinged_log2(Integer value)
{
    // The modulo compiles away for constexpr values.
    return floored_log2(value) + (value % 2);
}

template <typename Integer, IS_INTEGER(Integer)>
inline Integer floored_log2(Integer value)
{
    // The negative call compiles away for unsigned value.
    if (negative(value) || value == 0)
        return 0;

    Integer power = 0;
    while (((value >>= 1)) > 0) { ++power; };
    return power;
}

template <typename Integer, IS_INTEGER(Integer)>
inline Integer power2(Integer exponent)
{
    return power(Integer{ 2 }, exponent);
}

template <typename Integer, IS_INTEGER(Integer)>
inline Integer power(Integer base, Integer exponent)
{
    // This check is compiled away for constexpr exponent.
    if (exponent == 0)
        return 1;

    Integer value = base;

    // The absolute call compiles away for unsigned value.
    for (auto count = absolute(exponent); count > 1; --count)
        value *= base;

    // The negative call compiles away for unsigned value.
    // The ternary condition compiles awat for a constexpr value.
    // Do not shortcircuit negative exponent (to 1 or 0) because in the unusual
    // case when it is used this provides consistent overflow behavior.
    return negative(exponent) ? 1 / value : value;
}

template <typename Dividend, typename Divisor,
    IS_INTEGERS(Dividend, Divisor)=true>
inline bool remainder(Dividend dividend, Divisor divisor)
{
    // The modulo compiles away for constexpr values.
    return (dividend % divisor) != 0;
}

template <typename Dividend, typename Divisor,
    IS_INTEGERS(Dividend, Divisor)=true>
inline bool ceilinged(Dividend dividend, Divisor divisor)
{
    // These checks compile away for constexpr values.
    // The negative template type constraints eliminate one or two compares.
    // No change unless remainder, and c++ mod ceilinged for negative quotient.
    return !remainder(dividend, divisor) || negative(dividend, divisor);
}

template <typename Dividend, typename Divisor,
    IS_INTEGERS(Dividend, Divisor)=true>
inline bool floored(Dividend dividend, Divisor divisor)
{
    // These checks compile away for constexpr values.
    // The negative template type constraints eliminate one or two compares.
    // No change unless remainder, and c++ mod floored for positive quotient.
    return !remainder(dividend, divisor) || !negative(dividend, divisor);
}

// ----------------------------------------------------------------------------

template <typename Dividend, typename Divisor, typename Quotient,
    IS_INTEGERS(Dividend, Divisor)>
inline Quotient ceilinged_divide(Dividend dividend, Divisor divisor)
{
    return truncated_divide(dividend, divisor) + 
        (ceilinged(dividend, divisor) ? 0 : 1);
}

template <typename Dividend, typename Divisor, typename Remainder,
    IS_INTEGERS(Dividend, Divisor)>
inline Remainder ceilinged_modulo(Dividend dividend, Divisor divisor)
{
    return truncated_modulo(dividend, divisor) -
        (ceilinged(dividend, divisor) ? 0 : divisor);
}

template <typename Dividend, typename Divisor, typename Quotient,
    IS_INTEGERS(Dividend, Divisor)>
inline Quotient floored_divide(Dividend dividend, Divisor divisor)
{
    return truncated_divide(dividend, divisor) -
        (floored(dividend, divisor) ? 0 : 1);
}

template <typename Dividend, typename Divisor, typename Remainder,
    IS_INTEGERS(Dividend, Divisor)>
inline Remainder floored_modulo(Dividend dividend, Divisor divisor)
{
    return truncated_modulo(dividend, divisor) +
        (floored(dividend, divisor) ? 0 : divisor);
}

template <typename Dividend, typename Divisor, typename Quotient,
    IS_INTEGERS(Dividend, Divisor)>
inline Quotient truncated_divide(Dividend dividend, Divisor divisor)
{
    return dividend / divisor;
}

template <typename Dividend, typename Divisor, typename Remainder,
    IS_INTEGERS(Dividend, Divisor)>
inline Remainder truncated_modulo(Dividend dividend, Divisor divisor)
{
    return dividend % divisor;
}

} // namespace system
} // namespace libbitcoin

#endif

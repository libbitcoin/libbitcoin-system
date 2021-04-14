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

namespace libbitcoin {
namespace system {

template <typename Integer>
Integer absolute(Integer value)
{
    return value < 0 ? -1 * value : value;
}

template <typename Factor1, typename Factor2>
bool negative(Factor1 factor1, Factor2 factor2)
{
    return (factor1 < 0) != (factor2 < 0);
}

template <typename Dividend, typename Divisor>
bool remainder(Dividend dividend, Divisor divisor)
{
    return (dividend % divisor) != 0;
}

template <typename Integer>
Integer log2(Integer value)
{
    if (value <= 0)
        return 0;

    Integer power = 0;
    while (((value >>= 1)) > 0) { ++power; };
    return power;
}

template <typename Integer>
Integer pow2(Integer exponent)
{
    return power(Integer{ 2 }, exponent);
}

template <typename Integer>
Integer power(Integer base, Integer exponent)
{
    if (exponent == 0)
        return 1;

    Integer value = base;

    for (auto count = absolute(exponent); count > 1; --count)
        value *= base;

    return exponent < 0 ? 1 / value : value;
}

template <typename Dividend, typename Divisor>
Dividend ceilinged_modulo(Dividend dividend, Divisor divisor)
{
    // No change unless remainder, and c++ mod ceilinged for negative quotient.
    return !remainder(dividend, divisor) || negative(dividend, divisor) ?
        truncated_modulo(dividend, divisor) :
        divisor + truncated_modulo(dividend, divisor);
}

template <typename Dividend, typename Divisor>
Dividend floored_modulo(Dividend dividend, Divisor divisor)
{
    // No change unless remainder, and c++ mod floored for positive quotient.
    return !remainder(dividend, divisor) || !negative(dividend, divisor) ?
        truncated_modulo(dividend, divisor) :
        divisor - truncated_modulo(-dividend, divisor);
}

template <typename Dividend, typename Divisor>
Dividend truncated_modulo(Dividend dividend, Divisor divisor)
{
    // C++ applies "toward zero" integer division rounding (and remainder).
    // Floored for positive quotient, ceilinged for negative quotient.
    return dividend % divisor;
}

template <typename Dividend, typename Divisor>
Dividend ceilinged_divide(Dividend dividend, Divisor divisor)
{
    // No change unless remainder, and c++ div ceilinged for negative quotient.
    return !remainder(dividend, divisor) || negative(dividend, divisor) ?
        truncated_divide(dividend, divisor) :
        truncated_divide(dividend, divisor) + 1;
}

template <typename Dividend, typename Divisor>
Dividend floored_divide(Dividend dividend, Divisor divisor)
{
    // No change unless remainder, and c++ div floored for positive quotient.
    return !remainder(dividend, divisor) || !negative(dividend, divisor) ?
        truncated_divide(dividend, divisor) :
        truncated_divide(dividend, divisor) - 1;
}

template <typename Dividend, typename Divisor>
Dividend truncated_divide(Dividend dividend, Divisor divisor)
{
    // C++ applies "toward zero" integer division rounding (and remainder).
    // Floored for positive quotient, ceilinged for negative quotient.
    return dividend / divisor;
}

} // namespace system
} // namespace libbitcoin

#endif

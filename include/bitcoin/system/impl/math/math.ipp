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

// Specialize unsigned value (nop).
template <typename Integer, IS_UNSIGNED_INTEGER(Integer)>
inline Integer absolute(Integer value)
{
    return value;
}

// Specialize signed (all) types.
template <typename Integer, IS_SIGNED_INTEGER(Integer)>
inline Integer absolute(Integer value)
{
    return value < 0 ? -1 * value : value;
}

// Specialize unsigned factors (nop).
template <typename Factor1, typename Factor2,
    IS_UNSIGNED_INTEGERS(Factor1, Factor2)>
inline bool negative(Factor1 factor1, Factor2 factor2)
{
    return false;
}

// Specialize signed Factor1.
template <typename Factor1, typename Factor2,
    IS_SIGNED_INTEGER(Factor1), IS_UNSIGNED_INTEGER(Factor2)>
inline bool negative(Factor1 factor1, Factor2 factor2)
{
    return factor1 < 0;
}

// Specialize signed Factor2.
template <typename Factor1, typename Factor2,
    IS_UNSIGNED_INTEGER(Factor1), IS_SIGNED_INTEGER(Factor2)>
inline bool negative(Factor1 factor1, Factor2 factor2)
{
    return factor2 < 0;
}

// Specialize signed Factor1 and Factor2.
template <typename Factor1, typename Factor2,
    IS_SIGNED_INTEGERS(Factor1, Factor2)>
inline bool negative(Factor1 factor1, Factor2 factor2)
{
    return (factor1 < 0) != (factor2 < 0);
}

template <typename Dividend, typename Divisor,
    IS_INTEGERS(Dividend, Divisor)>
inline bool remainder(Dividend dividend, Divisor divisor)
{
    return (dividend % divisor) != 0;
}

// Signed types allowed but negatives are always zero.
template <typename Integer, IS_INTEGER(Integer)>
inline Integer floored_log2(Integer value)
{
    if (value <= 0)
        return 0;

    Integer power = 0;
    while (((value >>= 1)) > 0) { ++power; };
    return power;
}

template <typename Integer, IS_INTEGER(Integer)>
inline Integer ceilinged_log2(Integer value)
{
    return floored_log2(value) + (value % 2);
}

template <typename Integer, IS_INTEGER(Integer)>
inline Integer power2(Integer exponent)
{
    return power(Integer{ 2 }, exponent);
}

template <typename Integer, IS_INTEGER(Integer)>
inline Integer power(Integer base, Integer exponent)
{
    if (exponent == 0)
        return 1;

    Integer value = base;

    for (auto count = absolute(exponent); count > 1; --count)
        value *= base;

    return exponent < 0 ? 1 / value : value;
}

// Specialize unsigned operations.
template <typename Dividend, typename Divisor,
    IS_UNSIGNED_INTEGERS(Dividend, Divisor)>
inline Dividend ceilinged_modulo(Dividend dividend, Divisor divisor)
{
    // (x % y) + (!(x % y) ? y : 0)
    // (x % y) + ((x % y) != 0 ? y : 0)
    // No change unless remainder, and c++ mod ceilinged for negative quotient.
    return (remainder(dividend, divisor) ? divisor : 0) +
        truncated_modulo(dividend, divisor);
}

// Specialize signed (all) operations.
template <typename Dividend, typename Divisor,
    IS_EITHER_INTEGER_SIGNED(Dividend, Divisor)>
inline Dividend ceilinged_modulo(Dividend dividend, Divisor divisor)
{
    // The negative template type constraints eliminate two compares.
    // (x % y) + (!(x % y) && ((x < 0) == (y < 0)) ? y : 0)
    // (x % y) + (((x % y) != 0) && ((x < 0) == (y < 0)) ? y : 0)
    // No change unless remainder, and c++ mod ceilinged for negative quotient.
    return !remainder(dividend, divisor) || negative(dividend, divisor) ?
        truncated_modulo(dividend, divisor) :
        divisor + truncated_modulo(dividend, divisor);
}

// Specialize unsigned operations.
template <typename Dividend, typename Divisor,
    IS_UNSIGNED_INTEGERS(Dividend, Divisor)>
inline Dividend floored_modulo(Dividend dividend, Divisor divisor)
{
    // x % y
    return truncated_modulo(dividend, divisor);
}

// Specialize signed (all) operations.
template <typename Dividend, typename Divisor,
    IS_EITHER_INTEGER_SIGNED(Dividend, Divisor)>
inline Dividend floored_modulo(Dividend dividend, Divisor divisor)
{
    // The negative template type constraints eliminate two compares.
    // (x % y) - (!(x % y) && ((x < 0) != (y < 0)) ? y : 0)
    // (x % y) - (((x % y) != 0) && ((x < 0) != (y < 0)) ? y : 0)
    // No change unless remainder, and c++ mod floored for positive quotient.
    return !remainder(dividend, divisor) || !negative(dividend, divisor) ?
        truncated_modulo(dividend, divisor) :
        divisor - truncated_modulo(-dividend, divisor);
}

// Native operation.
template <typename Dividend, typename Divisor,
    IS_INTEGERS(Dividend, Divisor)>
inline Dividend truncated_modulo(Dividend dividend, Divisor divisor)
{
    // x % y
    // C++ applies "toward zero" integer division rounding (and remainder).
    // Floored for positive quotient, ceilinged for negative quotient.
    return dividend % divisor;
}

// Specialize unsigned operations.
template <typename Dividend, typename Divisor,
    IS_UNSIGNED_INTEGERS(Dividend, Divisor)>
inline Dividend ceilinged_divide(Dividend dividend, Divisor divisor)
{
    // (x / y) + !!(x % y)
    // (x / y) + ((x % y) != 0 ? 1 : 0)
    // No change unless remainder, and c++ div ceilinged for negative quotient.
    return truncated_divide(dividend, divisor) +
        (remainder(dividend, divisor) ? 1 : 0);
}

// Specialize signed (all) operations.
template <typename Dividend, typename Divisor,
    IS_EITHER_INTEGER_SIGNED(Dividend, Divisor)>
inline Dividend ceilinged_divide(Dividend dividend, Divisor divisor)
{
    // The negative template type constraints eliminate two compares.
    // (x / y) + !(((x < 0) != (y < 0)) || !(x % y));
    // (x / y) + (((x % y) != 0) && ((x < 0) == (y < 0)) ? 1 : 0)
    // No change unless remainder, and c++ div ceilinged for negative quotient.
    return !remainder(dividend, divisor) || negative(dividend, divisor) ?
        truncated_divide(dividend, divisor) :
        truncated_divide(dividend, divisor) + 1;
}

// Specialize unsigned operations.
template <typename Dividend, typename Divisor,
    IS_UNSIGNED_INTEGERS(Dividend, Divisor)>
inline Dividend floored_divide(Dividend dividend, Divisor divisor)
{
    // If same sign: x / y
    // No change unless remainder, and c++ div floored for positive quotient.
    return truncated_divide(dividend, divisor);
}

// Specialize signed (all) operations.
template <typename Dividend, typename Divisor,
    IS_EITHER_INTEGER_SIGNED(Dividend, Divisor)>
inline Dividend floored_divide(Dividend dividend, Divisor divisor)
{
    // The negative template type constraints eliminate two compares.
    // (x / y) - (((x < 0) != (y < 0) && !(x % y))
    // (x / y) - (((x % y) != 0) && ((x < 0) != (y < 0)) ? 1 : 0)
    // No change unless remainder, and c++ div floored for positive quotient.
    return !remainder(dividend, divisor) || !negative(dividend, divisor) ?
        truncated_divide(dividend, divisor) :
        truncated_divide(dividend, divisor) - 1;
}

// Native operation.
template <typename Dividend, typename Divisor,
    IS_INTEGERS(Dividend, Divisor)>
inline Dividend truncated_divide(Dividend dividend, Divisor divisor)
{
    // x / y
    // C++ applies "toward zero" integer division rounding (and remainder).
    // Floored for positive quotient, ceilinged for negative quotient.
    return dividend / divisor;
}

} // namespace system
} // namespace libbitcoin

#endif

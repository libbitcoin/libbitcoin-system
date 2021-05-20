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
#ifndef LIBBITCOIN_SYSTEM_MATH_MATH_HPP
#define LIBBITCOIN_SYSTEM_MATH_MATH_HPP

#include <cstddef>

namespace libbitcoin {
namespace system {

/**
 * Obtain the absolute value of the given integer value.
 * @param[in] value  The value.
 * @return           The absolute value.
 */
template <typename Integer>
Integer absolute(Integer value);

/**
 * Determine if the product (or quotient) of the two factors is negative. 
 * @param[in] factor1  A signed or unsigned factor.
 * @param[in] factor2  A signed or unsigned factor.
 * @return             True if the signs of the two factor differ.
 */
template <typename Factor1, typename Factor2>
bool negative(Factor1 factor1, Factor2 factor2);

/**
 * Determine if integer division produces a remainder
 * This is the equivalent to the c++ % operation non-zero result.
 * The result is independent of the type of division rounding.
 * @param[in] dividend  The signed or unsigned dividend.
 * @param[in] divisor   The signed or unsigned divisor.
 * @return              True if there is a remainder.
 */
template <typename Dividend, typename Divisor>
bool remainder(Dividend dividend, Divisor divisor);

/**
 * Obtain the ceilinged (rounded up) integer base 2 logarithm of given value.
 * @param[in] value   The value.
 * @return            The log2, or zero if value zero or negative (undefined).
 */
template <typename Integer>
Integer ceilinged_log2(Integer value);

/**
 * Obtain the floored (rounded down) integer base 2 logarithm of given value.
 * @param[in] value   The value.
 * @return            The log2, or zero if value zero or negative (undefined).
 */
template <typename Integer>
Integer floored_log2(Integer value);

/**
 * Obtain the integer base 2 power for given exponent (can overflow).
 * @param[in] exponent  The signed or unsigned exponent.
 * @return              The pow2, negative exponent results always zero.
 */
template <typename Integer>
Integer pow2(Integer exponent);

/**
 * Obtain the integer power of given base for given exponent (can overflow).
 * @param[in] base      The signed or unsigned base.
 * @param[in] exponent  The signed or unsigned exponent.
 * @return              The power, negative exponent results always zero.
 */
template <typename Integer>
Integer power(Integer base, Integer exponent);

/**
 * Obtain the ceilinged (rounded up) integer modulo quotient.
 * This is equivalent to the c++ % operator for negative quotients.
 * @param[in] dividend  The signed or unsigned dividend.
 * @param[in] divisor   The signed or unsigned divisor.
 * @return              The ceilinged divide remainder.
 */
template <typename Dividend, typename Divisor>
Dividend ceilinged_modulo(Dividend dividend, Divisor divisor);

/**
 * Obtain the floorded (rounded down) integer modulo quotient.
 * This is equivalent to the c++ % operator for positive quotients.
 * Python integer modulo (%) is floored.
 * @param[in] dividend  The signed or unsigned dividend.
 * @param[in] divisor   The signed or unsigned divisor.
 * @return              The floored divide remainder.
 */
template <typename Dividend, typename Divisor>
Dividend floored_modulo(Dividend dividend, Divisor divisor);

/**
 * Obtain the truncated (rounded toward zero) integer divide remainder.
 * This is the equivalent to the c++ % operation.
 * @param[in] dividend  The signed or unsigned dividend.
 * @param[in] divisor   The signed or unsigned divisor.
 * @return              The truncated divide remainder.
 */
template <typename Dividend, typename Divisor>
Dividend truncated_modulo(Dividend dividend, Divisor divisor);

/**
 * Obtain the ceilinged (rounded up) integer modulo quotient.
 * This is equivalent to the c++ % operator for negative quotients.
 * Python integer division is floored and so can create ceilinged integer
 * division by -(numerator//-denominator).
 * @param[in] dividend  The signed or unsigned dividend.
 * @param[in] divisor   The signed or unsigned dividend.
 * @return              The ceilinged quotient.
 */
template <typename Dividend, typename Divisor>
Dividend ceilinged_divide(Dividend dividend, Divisor divisor);

/**
 * Obtain the floored (rounded down) integer modulo quotient.
 * This is equivalent to the c++ % operator for positive quotients.
 * Python integer division is floored.
 * @param[in] dividend  The signed or unsigned dividend.
 * @param[in] divisor   The signed or unsigned divisor.
 * @return              The floored quotient.
 */
template <typename Dividend, typename Divisor>
Dividend floored_divide(Dividend dividend, Divisor divisor);

/**
 * Obtain the truncated (rounded toward zero) integer quotient.
 * This is the equivalent to the c++ / operation.
 * @param[in] dividend  The signed or unsigned dividend.
 * @param[in] divisor   The signed or unsigned divisor.
 * @return              The truncated quotient.
 */
template <typename Dividend, typename Divisor>
Dividend truncated_divide(Dividend dividend, Divisor divisor);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/math.ipp>

#endif


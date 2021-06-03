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
#ifndef LIBBITCOIN_SYSTEM_MATH_POWER_HPP
#define LIBBITCOIN_SYSTEM_MATH_POWER_HPP

#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

/// All operations below support signed and unsigned parameters.

/// Obtain the ceilinged (rounded up) integer logarithm of given value and base.
/// Returns 0 for undefined (base < 2 or value < 1).
template <typename Base, typename Integer, typename Log=Integer,
    IS_INTEGER(Base)=true, IS_INTEGER(Integer)=true>
Log ceilinged_log(Base base, Integer value);

/// Obtain the ceilinged (rounded up) integer base 2 logarithm of given value.
/// Returns 0 for undefined (value < 1).
template <typename Integer, IS_INTEGER(Integer)=true>
Integer ceilinged_log2(Integer value);

/// Obtain the floored (rounded down) integer logarithm of given value and base.
/// Returns 0 for undefined (base < 2 or value < 1).
template <typename Base, typename Integer, typename Log=Integer,
    IS_INTEGER(Base)=true, IS_INTEGER(Integer)=true>
Log floored_log(Base base, Integer value);

/// Obtain the floored (rounded down) integer base 2 logarithm of given value.
/// Returns 0 for undefined (value < 1).
template <typename Integer, IS_INTEGER(Integer)=true>
Integer floored_log2(Integer value);

/// Obtain the integer power of given base for given exponent.
/// Returns zero if both operands are zero (undefined).
template <typename Base, typename Integer, typename Power=Base,
    IS_INTEGER(Base)=true, IS_INTEGER(Integer)=true>
Power power(Base base, Integer exponent);

/// Obtain the integer base 2 power for given exponent.
template <typename Integer, IS_INTEGER(Integer)=true>
Integer power2(Integer exponent);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/power.ipp>

#endif


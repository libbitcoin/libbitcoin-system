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
#ifndef LIBBITCOIN_SYSTEM_MATH_DIVIDE_HPP
#define LIBBITCOIN_SYSTEM_MATH_DIVIDE_HPP

#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

/// All operations below support signed and unsigned parameters.
/// github.com/libbitcoin/libbitcoin-system/wiki/Integer-Division-Unraveled

/// Obtain the ceilinged (rounded up) integer modulo quotient.
/// This is equivalent to c++ % for negative quotients.
template <typename Dividend, typename Divisor, typename Quotient=Dividend,
    IS_INTEGER(Dividend)=true, IS_INTEGER(Divisor)=true>
inline Quotient ceilinged_divide(Dividend dividend, Divisor divisor);

/// Obtain the ceilinged (rounded up) integer modulo quotient.
/// This is equivalent to c++ % for negative quotients.
template <typename Dividend, typename Divisor, typename Remainder=Dividend,
    IS_INTEGER(Dividend)=true, IS_INTEGER(Divisor)=true>
inline Remainder ceilinged_modulo(Dividend dividend, Divisor divisor);

/// Obtain the floored (rounded down) integer modulo quotient.
/// This is equivalent to c++ % for positive quotients.
template <typename Dividend, typename Divisor, typename Quotient=Dividend,
    IS_INTEGER(Dividend)=true, IS_INTEGER(Divisor)=true>
inline Quotient floored_divide(Dividend dividend, Divisor divisor);

/// Obtain the floorded (rounded down) integer modulo quotient.
/// This is equivalent to c++ % for positive quotients.
template <typename Dividend, typename Divisor, typename Remainder=Dividend,
    IS_INTEGER(Dividend)=true, IS_INTEGER(Divisor)=true>
inline Remainder floored_modulo(Dividend dividend, Divisor divisor);

/// Obtain the truncated (rounded toward zero) integer quotient.
/// This is equivalent to c++ /.
template <typename Dividend, typename Divisor, typename Quotient=Dividend,
    IS_INTEGER(Dividend)=true, IS_INTEGER(Divisor)=true>
inline Quotient truncated_divide(Dividend dividend, Divisor divisor);

/// Obtain the truncated (rounded toward zero) integer divide remainder.
/// This is equivalent to c++ %.
template <typename Dividend, typename Divisor, typename Remainder=Dividend,
    IS_INTEGER(Dividend)=true, IS_INTEGER(Divisor)=true>
inline Remainder truncated_modulo(Dividend dividend, Divisor divisor);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/divide.ipp>

#endif


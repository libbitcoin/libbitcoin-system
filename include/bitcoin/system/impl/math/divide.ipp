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
#ifndef LIBBITCOIN_SYSTEM_MATH_IPP
#define LIBBITCOIN_SYSTEM_MATH_IPP

#include <bitcoin/system/math/sign.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

// local
template <typename Factor1, typename Factor2,
    IS_INTEGER(Factor1)=true, IS_INTEGER(Factor2)=true>
inline bool is_negative(Factor1 factor1, Factor2 factor2)
{
    return is_negative(factor1) != is_negative(factor2);
}

// local
template <typename Dividend, typename Divisor,
    IS_INTEGER(Dividend)=true, IS_INTEGER(Divisor)=true>
inline bool no_remainder(Dividend dividend, Divisor divisor)
{
    return (dividend % divisor) == 0;
}

// local
template <typename Dividend, typename Divisor,
    IS_INTEGER(Dividend)=true, IS_INTEGER(Divisor)=true>
inline bool is_ceilinged(Dividend dividend, Divisor divisor)
{
    return is_negative(dividend, divisor) || no_remainder(dividend, divisor);
}

// local
template <typename Dividend, typename Divisor,
    IS_INTEGER(Dividend)=true, IS_INTEGER(Divisor)=true>
inline bool is_floored(Dividend dividend, Divisor divisor)
{
    return !is_negative(dividend, divisor) || no_remainder(dividend, divisor);
}

// C++14: Quotient can be defaulted to decltype(Dividend / Divisor).
// C++14: Remainder can be defaulted to decltype(Dividend % Divisor).

template <typename Dividend, typename Divisor, typename Quotient,
    IS_INTEGER(Dividend), IS_INTEGER(Divisor)>
inline Quotient ceilinged_divide(Dividend dividend, Divisor divisor)
{
    return truncated_divide(dividend, divisor) + 
        (is_ceilinged(dividend, divisor) ? 0 : 1);
}

template <typename Dividend, typename Divisor, typename Remainder,
    IS_INTEGER(Dividend), IS_INTEGER(Divisor)>
inline Remainder ceilinged_modulo(Dividend dividend, Divisor divisor)
{
    return truncated_modulo(dividend, divisor) -
        (is_ceilinged(dividend, divisor) ? 0 : divisor);
}

template <typename Dividend, typename Divisor, typename Quotient,
    IS_INTEGER(Dividend), IS_INTEGER(Divisor)>
inline Quotient floored_divide(Dividend dividend, Divisor divisor)
{
    return truncated_divide(dividend, divisor) -
        (is_floored(dividend, divisor) ? 0 : 1);
}

template <typename Dividend, typename Divisor, typename Remainder,
    IS_INTEGER(Dividend), IS_INTEGER(Divisor)>
inline Remainder floored_modulo(Dividend dividend, Divisor divisor)
{
    return truncated_modulo(dividend, divisor) +
        (is_floored(dividend, divisor) ? 0 : divisor);
}

template <typename Dividend, typename Divisor, typename Quotient,
    IS_INTEGER(Dividend), IS_INTEGER(Divisor)>
inline Quotient truncated_divide(Dividend dividend, Divisor divisor)
{
    return dividend / divisor;
}

template <typename Dividend, typename Divisor, typename Remainder,
    IS_INTEGER(Dividend), IS_INTEGER(Divisor)>
inline Remainder truncated_modulo(Dividend dividend, Divisor divisor)
{
    return dividend % divisor;
}

} // namespace system
} // namespace libbitcoin

#endif

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
#ifndef LIBBITCOIN_SYSTEM_MATH_DIVISION_IPP
#define LIBBITCOIN_SYSTEM_MATH_DIVISION_IPP

#include <type_traits>
/// DELETEMENOW
/// DELETEMENOW
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

// helpers
// ----------------------------------------------------------------------------

template <typename Factor1, typename Factor2,
    if_integer<Factor1> = true, if_integer<Factor2> = true>
constexpr bool is_negative(Factor1 factor1, Factor2 factor2) NOEXCEPT
{
    return is_negative(factor1) != is_negative(factor2);
}

// local
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true, if_integer<Divisor> = true>
constexpr bool no_remainder(Dividend dividend, Divisor divisor) NOEXCEPT
{
    return is_zero(dividend % divisor);
}

// local
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true, if_integer<Divisor> = true>
constexpr bool is_ceilinged(Dividend dividend, Divisor divisor) NOEXCEPT
{
    return is_negative(dividend, divisor) || no_remainder(dividend, divisor);
}

// local
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true, if_integer<Divisor> = true>
constexpr bool is_floored(Dividend dividend, Divisor divisor) NOEXCEPT
{
    return !is_negative(dividend, divisor) || no_remainder(dividend, divisor);
}

// operations
// ----------------------------------------------------------------------------

template <typename Dividend, typename Divisor,
    if_integer<Dividend>, if_integer<Divisor>>
constexpr to_common_type<Dividend, Divisor>
ceilinged_divide(Dividend dividend, Divisor divisor) NOEXCEPT
{
    return truncated_divide(dividend, divisor) + 
        (is_ceilinged(dividend, divisor) ? 0 : 1);
}

// The native % operator is truncated and the common type is unsigned.
// But ceilinged modulo is logically negative, so type is changed to signed.
template <typename Dividend, typename Divisor,
    if_integer<Dividend>, if_integer<Divisor>>
constexpr to_signed_type<to_common_type<Dividend, Divisor>>
ceilinged_modulo(Dividend dividend, Divisor divisor) NOEXCEPT
{
    return truncated_modulo(dividend, divisor) -
        (is_ceilinged(dividend, divisor) ? 0 : divisor);
}

template <typename Dividend, typename Divisor,
    if_integer<Dividend>, if_integer<Divisor>>
constexpr to_common_type<Dividend, Divisor>
floored_divide(Dividend dividend, Divisor divisor) NOEXCEPT
{
    return truncated_divide(dividend, divisor) -
        (is_floored(dividend, divisor) ? 0 : 1);
}

// Always positive result.
template <typename Dividend, typename Divisor,
    if_integer<Dividend>, if_integer<Divisor>>
constexpr to_common_type<Dividend, Divisor>
floored_modulo(Dividend dividend, Divisor divisor) NOEXCEPT
{
    return truncated_modulo(dividend, divisor) +
        (is_floored(dividend, divisor) ? 0 : divisor);
}

template <typename Dividend, typename Divisor,
    if_integer<Dividend>, if_integer<Divisor>>
constexpr to_common_type<Dividend, Divisor>
truncated_divide(Dividend dividend, Divisor divisor) NOEXCEPT
{
    return dividend / divisor;
}

template <typename Dividend, typename Divisor,
    if_integer<Dividend>, if_integer<Divisor>>
constexpr to_common_type<Dividend, Divisor>
truncated_modulo(Dividend dividend, Divisor divisor) NOEXCEPT
{
    return dividend % divisor;
}

template <typename Value, if_integer<Value>>
constexpr bool
is_multiple(Value product, Value value) NOEXCEPT
{
    if (is_zero(value))
        return is_zero(product);

    return is_zero(product % value);
}

template <typename Value, if_integer<Value>>
constexpr bool
is_product(Value value, Value left, Value right) NOEXCEPT
{
    if (is_zero(left) || is_zero(right))
        return is_zero(value);

    return is_zero(value % left) && ((value / left) == right);
}

} // namespace system
} // namespace libbitcoin

#endif

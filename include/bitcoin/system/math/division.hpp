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
#ifndef LIBBITCOIN_SYSTEM_MATH_DIVISION_HPP
#define LIBBITCOIN_SYSTEM_MATH_DIVISION_HPP

#include <bitcoin/system/define.hpp>

/// These functions do not manage type promotion or division by zero.
/// With the exception of ceilinged_modulo these return the common type of the
/// native operators. The native modulo operator is truncated, resulting in a
/// positive common type. However ceilinged modulo is naturally negative, so
/// the modulo common type is converted to signed, as otherwise the result
/// would be the unsigned twos compliment of the intended value.

namespace libbitcoin {
namespace system {

/// Obtain the ceilinged (rounded up) integer modulo quotient.
/// This is equivalent to C++ % for negative quotients.
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true, if_integer<Divisor> = true>
constexpr to_common_type<Dividend, Divisor>
ceilinged_divide(Dividend dividend, Divisor divisor) NOEXCEPT;

/// Obtain the ceilinged (rounded up) integer modulo quotient.
/// This is equivalent to C++ % for negative quotients.
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true, if_integer<Divisor> = true>
constexpr to_signed_type<to_common_type<Dividend, Divisor>>
ceilinged_modulo(Dividend dividend, Divisor divisor) NOEXCEPT;

/// Obtain the floored (rounded down) integer modulo quotient.
/// This is equivalent to C++ % for positive quotients.
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true, if_integer<Divisor> = true>
constexpr to_common_type<Dividend, Divisor>
floored_divide(Dividend dividend, Divisor divisor) NOEXCEPT;

/// Obtain the floorded (rounded down) integer modulo quotient.
/// This is equivalent to C++ % for positive quotients.
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true, if_integer<Divisor> = true>
constexpr to_common_type<Dividend, Divisor>
floored_modulo(Dividend dividend, Divisor divisor) NOEXCEPT;

/// Obtain the truncated (rounded toward zero) integer quotient.
/// This is equivalent to C++ /.
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true, if_integer<Divisor> = true>
constexpr to_common_type<Dividend, Divisor>
truncated_divide(Dividend dividend, Divisor divisor) NOEXCEPT;

/// Obtain the truncated (rounded toward zero) integer divide remainder.
/// This is equivalent to C++ %.
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true, if_integer<Divisor> = true>
constexpr to_common_type<Dividend, Divisor>
truncated_modulo(Dividend dividend, Divisor divisor) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/division.ipp>

#endif


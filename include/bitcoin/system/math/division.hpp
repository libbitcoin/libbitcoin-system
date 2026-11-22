/**
 * Copyright (c) 2011-2026 libbitcoin developers
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

/// These functions do not manage type promotion or division by zero. Except for
/// ceilinged_modulo these return common type of native operators. Native modulo
/// operator is truncated, resulting in positive common type. Ceilinged modulo
/// is naturally negative, so modulo common type is converted to signed, as
/// otherwise result would be unsigned twos compliment of intended value.

namespace libbitcoin {
namespace system {

/// Obtain ceilinged (rounded up) integer modulo quotient.
/// Equivalent to C++ % for negative quotients.
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true, if_integer<Divisor> = true>
constexpr to_common_type<Dividend, Divisor>
ceilinged_divide(Dividend dividend, Divisor divisor) NOEXCEPT;

/// Obtain ceilinged (rounded up) integer modulo quotient.
/// Equivalent to C++ % for negative quotients.
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true, if_integer<Divisor> = true>
constexpr to_signed_type<to_common_type<Dividend, Divisor>>
ceilinged_modulo(Dividend dividend, Divisor divisor) NOEXCEPT;

/// Obtain floored (rounded down) integer modulo quotient.
/// Equivalent to C++ % for positive quotients.
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true, if_integer<Divisor> = true>
constexpr to_common_type<Dividend, Divisor>
floored_divide(Dividend dividend, Divisor divisor) NOEXCEPT;

/// Obtain floored (rounded down) integer modulo quotient.
/// Equivalent to C++ % for positive quotients.
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true, if_integer<Divisor> = true>
constexpr to_common_type<Dividend, Divisor>
floored_modulo(Dividend dividend, Divisor divisor) NOEXCEPT;

/// Obtain truncated (rounded toward zero) integer quotient.
/// Equivalent to C++ /.
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true, if_integer<Divisor> = true>
constexpr to_common_type<Dividend, Divisor>
truncated_divide(Dividend dividend, Divisor divisor) NOEXCEPT;

/// Obtain truncated (rounded toward zero) integer divide remainder.
/// Equivalent to C++ %.
template <typename Dividend, typename Divisor,
    if_integer<Dividend> = true, if_integer<Divisor> = true>
constexpr to_common_type<Dividend, Divisor>
truncated_modulo(Dividend dividend, Divisor divisor) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/division.ipp>

#endif


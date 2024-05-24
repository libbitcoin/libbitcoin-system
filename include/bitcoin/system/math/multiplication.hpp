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
#ifndef LIBBITCOIN_SYSTEM_MATH_MULTIPLICATION_HPP
#define LIBBITCOIN_SYSTEM_MATH_MULTIPLICATION_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Safely determine whether product is a whole mutiple of value.
template <typename Integer,
    if_integer<Integer> = true>
constexpr bool is_multiple(Integer product, Integer value) NOEXCEPT;

/// Safely determine whether product is left * right.
template <typename Integer,
    if_integer<Integer> = true>
constexpr bool is_product(Integer product, Integer left,
    Integer right) NOEXCEPT;

/// Integer maximum if would overflow, otherwise the product.
template <typename Unsigned,
    if_unsigned_integer<Unsigned> = true>
constexpr Unsigned ceilinged_multiply(Unsigned left, Unsigned right) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/multiplication.ipp>

#endif

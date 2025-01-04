/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_MATH_MULTIPLICATION_IPP
#define LIBBITCOIN_SYSTEM_MATH_MULTIPLICATION_IPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/cast.hpp>
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/math/overflow.hpp>

namespace libbitcoin {
namespace system {

template <typename Integer, if_integer<Integer>>
constexpr bool is_multiple(Integer product, Integer value) NOEXCEPT
{
    return is_nonzero(value) ? is_zero(product % value) : is_zero(product);
}

template <typename Integer, if_integer<Integer>>
constexpr bool is_product(Integer product, Integer left,
    Integer right) NOEXCEPT
{
    return (is_zero(left) || is_zero(right)) ? is_zero(product) :
        is_zero(product % left) && ((product / left) == right);
}

template <typename Unsigned, if_unsigned_integer<Unsigned>>
constexpr Unsigned ceilinged_multiply(Unsigned left, Unsigned right) NOEXCEPT
{
    return is_multiply_overflow(left, right) ? maximum<Unsigned> :
        depromote<Unsigned>(left * right);
}

} // namespace system
} // namespace libbitcoin

#endif

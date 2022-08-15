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
#ifndef LIBBITCOIN_SYSTEM_MATH_FUNCTIONAL_HPP
#define LIBBITCOIN_SYSTEM_MATH_FUNCTIONAL_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>

namespace libbitcoin {
namespace system {

template <unsigned int B, typename Word,
    if_integral_integer<Word> = true>
INLINE constexpr auto shr_(Word a) NOEXCEPT
{
    return a >> B;
}

// unused by sha
template <unsigned int B, typename Word,
    if_integral_integer<Word> = true>
INLINE constexpr auto shl_(Word a) NOEXCEPT
{
    return a << B;
}

template <unsigned int B, unsigned int = 0, typename Word,
    if_integral_integer<Word> = true>
INLINE constexpr auto ror_(Word a) NOEXCEPT
{
    // intrinsics
    return rotr<B>(a);
}

template <unsigned int B, unsigned int = 0, typename Word,
    if_integral_integer<Word> = true>
INLINE constexpr auto rol_(Word a) NOEXCEPT
{
    // intrinsics
    return rotl<B>(a);
}

template <auto K, typename Word,
    if_integral_integer<Word> = true>
INLINE constexpr auto add_(Word a) NOEXCEPT
{
    return a + K;
}

template <typename Word,
    if_integral_integer<Word> = true>
INLINE constexpr auto add_(Word a, Word b) NOEXCEPT
{
    return a + b;
}

template <typename Word,
    if_integral_integer<Word> = true>
INLINE constexpr auto and_(Word a, Word b) NOEXCEPT
{
    return a & b;
}

template <typename Word,
    if_integral_integer<Word> = true>
INLINE constexpr auto or_(Word a, Word b) NOEXCEPT
{
    return a | b;
}

template <typename Word,
    if_integral_integer<Word> = true>
INLINE constexpr auto xor_(Word a, Word b) NOEXCEPT
{
    return a ^ b;
}

} // namespace system
} // namespace libbitcoin

#endif

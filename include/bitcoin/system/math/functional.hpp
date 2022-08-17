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
#include <bitcoin/system/math/rotate.hpp>

namespace libbitcoin {
namespace system {
namespace f {

/// bitwise primitives
/// ---------------------------------------------------------------------------

template <typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto and_(Word a, Word b) NOEXCEPT
{
    return a & b;
}

template <typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto or_(Word a, Word b) NOEXCEPT
{
    return a | b;
}

template <typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto xor_(Word a, Word b) NOEXCEPT
{
    return a ^ b;
}

template <typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto not_(Word a) NOEXCEPT
{
    return ~a;
}

/// mathematical primitives
/// ---------------------------------------------------------------------------
// S arguments are set for common overload with extended functions.

template <auto B, auto S = 0, typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto shr(Word a) NOEXCEPT
{
    return a >> B;
}

// unused by sha
template <auto B, auto S = 0, typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto shl(Word a) NOEXCEPT
{
    return a << B;
}

// S unused by integral overload
template <auto B, auto S = 0, typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto ror(Word a) NOEXCEPT
{
    // math/intrinsics
    return rotr<B>(a);
}

// S unused by integral overload
template <auto B, auto S = 0, typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto rol(Word a) NOEXCEPT
{
    // math/intrinsics
    return rotl<B>(a);
}

template <auto S = 0, typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto add(Word a, Word b) NOEXCEPT
{
    return a + b;
}

template <auto K, auto S = 0, typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto add(Word a) NOEXCEPT
{
    return a + K;
}

} // namespace f

// Overload for vectorization coexistence.
// First parameter is used only as a polymorphic guide, as it cannot be
// cleanly provided as a template argument in the given context.
template <auto Lane, typename Word>
INLINE constexpr Word extract_(Word, Word a) NOEXCEPT
{
    static_assert(Lane == one);
    return a;
}

} // namespace system
} // namespace libbitcoin

#endif

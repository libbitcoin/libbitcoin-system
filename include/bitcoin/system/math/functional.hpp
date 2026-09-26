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
#ifndef LIBBITCOIN_SYSTEM_MATH_FUNCTIONAL_HPP
#define LIBBITCOIN_SYSTEM_MATH_FUNCTIONAL_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>
#include <bitcoin/system/math/cast.hpp>
#include <bitcoin/system/math/rotate.hpp>

namespace libbitcoin {
namespace system {
namespace f {

/// bitwise primitives
/// ---------------------------------------------------------------------------

template <typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto and_(Word a, Word b) NOEXCEPT
{
    return depromote<Word>(a & b);
}

template <typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto or_(Word a, Word b) NOEXCEPT
{
    return depromote<Word>(a | b);
}

template <typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto xor_(Word a, Word b) NOEXCEPT
{
    return depromote<Word>(a ^ b);
}

template <typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto not_(Word a) NOEXCEPT
{
    return depromote<Word>(~a);
}

/// mathematical primitives
/// ---------------------------------------------------------------------------
// S arguments are set for common overload with intrisic functionals.
// rotr/rotl are implemented as intrinsic where available, otherwise shift/or.

template <auto B, auto S = 0, typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto shr(Word a) NOEXCEPT
{
    return depromote<Word>(a >> B);
}

template <auto B, auto S = 0, typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto shl(Word a) NOEXCEPT
{
    return depromote<Word>(a << B);
}

template <auto B, auto S = 0, typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto ror(Word a) NOEXCEPT
{
    return rotr<B>(a);
}

template <auto B, auto S = 0, typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto rol(Word a) NOEXCEPT
{
    return rotl<B>(a);
}

template <auto S = 0, typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto add(Word a, Word b) NOEXCEPT
{
    return depromote<Word>(a + b);
}

template <auto S = 0, typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto sub(Word a, Word b) NOEXCEPT
{
    return depromote<Word>(a - b);
}

template <auto K, auto S = 0, typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto addc(Word a) NOEXCEPT
{
    return depromote<Word>(a + K);
}

template <auto S = 0, typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto mul(Word a, Word b) NOEXCEPT
{
    return depromote<Word>(a * b);
}

template <typename Word, if_integral_integer<Word> = true>
INLINE constexpr Word broadcast(Word a) NOEXCEPT
{
    return a;
}

/// wide multiplication
/// ---------------------------------------------------------------------------

template <typename Word, if_unsigned_integral_integer<Word> = true>
INLINE constexpr void mul_wide(Word& hi, Word& lo, Word a, Word b) NOEXCEPT
{
    system::mul_wide(hi, lo, a, b);
}

template <auto S = 0, typename Word, if_same<Word, uint64_t> = true>
INLINE constexpr Word madd52lo(Word c, Word a, Word b) NOEXCEPT
{
    return system::madd52lo(c, a, b);
}

template <auto S = 0, typename Word, if_same<Word, uint64_t> = true>
INLINE constexpr Word madd52hi(Word c, Word a, Word b) NOEXCEPT
{
    return system::madd52hi(c, a, b);
}

/// compare/select
/// ---------------------------------------------------------------------------
/// Masks are all bits set (true) or unset (false) per word.

template <typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto andnot(Word a, Word b) NOEXCEPT
{
    return depromote<Word>(~a & b);
}

template <auto S = 0, typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto eq(Word a, Word b) NOEXCEPT
{
    return depromote<Word>(a == b ? ~Word{} : Word{});
}

template <typename Word, if_integral_integer<Word> = true>
INLINE constexpr auto select(Word mask, Word a, Word b) NOEXCEPT
{
    return or_(and_(mask, a), andnot(mask, b));
}

template <typename Word, if_integral_integer<Word> = true>
INLINE constexpr bool any(Word a) NOEXCEPT
{
    return !is_zero(a);
}

/// gather
/// ---------------------------------------------------------------------------

template <typename Word, if_integral_integer<Word> = true>
INLINE constexpr Word gather(const Word* table, Word index) NOEXCEPT
{
    BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
    return table[index];
    BC_POP_WARNING()
}

} // namespace f
} // namespace system
} // namespace libbitcoin

#endif

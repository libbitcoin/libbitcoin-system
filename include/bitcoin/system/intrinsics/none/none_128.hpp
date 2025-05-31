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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_NONE_128_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_NONE_128_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/types.hpp>

#if !defined(HAVE_128)

namespace libbitcoin {
namespace system {
namespace f {

// These are implemented for integrals in /math/functional `f` namespace:
// and_, or_, xor_, not_, shr<>, shl<>, ror<>, rol<>, add<>, addc<>, add.
// Those names should not be applied explicitly to extended types unless
// conditionally compiled. The following are okay for explicit extension.

template <typename xWord, typename Word, if_integral_integer<Word> = true,
    if_same<xWord, xint128_t> = true>
INLINE xint128_t broadcast(Word) NOEXCEPT
{
    return {};
}

template <typename Word, auto Lane, if_integral_integer<Word> = true>
INLINE Word get(xint128_t) NOEXCEPT
{
    return {};
}

template <typename xWord, if_same<xWord, xint128_t> = true>
INLINE xint128_t set(uint64_t, uint64_t) NOEXCEPT
{
    return {};
}

template <typename xWord, if_same<xWord, xint128_t> = true>
INLINE xint128_t set(
    uint32_t, uint32_t, uint32_t, uint32_t) NOEXCEPT
{
    return {};
}

template <typename xWord, if_same<xWord, xint128_t> = true>
INLINE xint128_t set(
    uint16_t, uint16_t, uint16_t, uint16_t,
    uint16_t, uint16_t, uint16_t, uint16_t) NOEXCEPT
{
    return {};
}

template <typename xWord, if_same<xWord, xint128_t> = true>
INLINE xint128_t set(
    uint8_t, uint8_t, uint8_t, uint8_t,
    uint8_t, uint8_t, uint8_t, uint8_t,
    uint8_t, uint8_t, uint8_t, uint8_t,
    uint8_t, uint8_t, uint8_t, uint8_t) NOEXCEPT
{
    return {};
}

template <typename Word, if_same<Word, uint8_t> = true>
INLINE xint128_t byteswap(xint128_t) NOEXCEPT
{
    return {};
}

template <typename Word, if_same<Word, uint16_t> = true>
INLINE xint128_t byteswap(xint128_t) NOEXCEPT
{
    return {};
}

template <typename Word, if_same<Word, uint32_t> = true>
INLINE xint128_t byteswap(xint128_t) NOEXCEPT
{
    return {};
}

template <typename Word, if_same<Word, uint64_t> = true>
INLINE xint128_t byteswap(xint128_t) NOEXCEPT
{
    return {};
}

INLINE xint128_t load(const xint128_t&) NOEXCEPT
{
    return {};
}

INLINE void store(xint128_t&, xint128_t) NOEXCEPT
{
}

INLINE xint128_t load_aligned(const xint128_t&) NOEXCEPT
{
    return {};
}

INLINE void store_aligned(xint128_t&, xint128_t) NOEXCEPT
{
}

} // namespace f
} // namespace system
} // namespace libbitcoin

#endif // HAVE_128

#endif

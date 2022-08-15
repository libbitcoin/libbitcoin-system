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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_FUNCTIONAL256_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_FUNCTIONAL256_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/xcpu/interface.hpp>

namespace libbitcoin {

// AVX2 primitives (for 32 bit word_t).
// ----------------------------------------------------------------------------
// TODO: 64 bit word_t.

template <typename Word, if_same<Word, xint256_t> = true>
INLINE Word byteswap_mask() NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    static const auto mask = _mm256_set_epi32(
        bswap_mask[0], bswap_mask[1], bswap_mask[2], bswap_mask[3],
        bswap_mask[0], bswap_mask[1], bswap_mask[2], bswap_mask[3]);
    BC_POP_WARNING()
    return mask;
}

INLINE xint256_t byteswap(xint256_t value) NOEXCEPT
{
    return _mm256_shuffle_epi8(value, byteswap_mask<xint256_t>());
}

template <unsigned int B, typename Word,
    if_same<Word, xint256_t> = true>
INLINE Word shr_(Word a) NOEXCEPT
{
    return _mm256_srli_epi32(a, B);
}

template <unsigned int B, typename Word,
    if_same<Word, xint256_t> = true>
INLINE Word shl_(Word a) NOEXCEPT
{
    return _mm256_slli_epi32(a, B);
}

template <unsigned int B, unsigned int S, typename Word,
    if_same<Word, xint256_t> = true>
INLINE Word ror_(Word a) NOEXCEPT
{
    // TODO: S will become unnecessary as the function set must be 32/64.
    return or_(shr_<B>(a), shl_<S - B>(a));
}

template <unsigned int B, unsigned int S, typename Word,
    if_same<Word, xint256_t> = true>
INLINE Word rol_(Word a) NOEXCEPT
{
    // TODO: S will become unnecessary as the function set must be 32/64.
    return or_(shl_<B>(a), shr_<S - B>(a));
}

template <auto K, typename Word,
    if_same<Word, xint256_t> = true>
INLINE Word add_(Word a) NOEXCEPT
{
    // Broadcast 32-bit integer to all elements.
    return _mm256_add_epi32(a, _mm256_set1_epi32(K));
}

template <typename Word,
    if_same<Word, xint256_t> = true>
INLINE Word add_(Word a, Word b) NOEXCEPT
{
    return _mm256_add_epi32(a, b);
}

template <typename Word, if_same<Word, xint256_t> = true>
INLINE Word and_(Word a, Word b) NOEXCEPT
{
    return _mm256_and_si256(a, b);
}

template <typename Word, if_same<Word, xint256_t> = true>
INLINE Word or_(Word a, Word b) NOEXCEPT
{
    return _mm256_or_si256(a, b);
}

template <typename Word, if_same<Word, xint256_t> = true>
INLINE Word xor_(Word a, Word b) NOEXCEPT
{
    return _mm256_xor_si256(a, b);
}

} // namespace libbitcoin

#endif

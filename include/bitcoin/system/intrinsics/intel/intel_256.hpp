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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_INTEL_256_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_INTEL_256_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/types.hpp>
#include <bitcoin/system/intrinsics/intel/intel.hpp>

namespace libbitcoin {
namespace system {

#if defined(HAVE_AVX2)

namespace f {

/// bitwise primitives
/// ---------------------------------------------------------------------------

// AVX2
INLINE xint256_t and_(xint256_t a, xint256_t b) NOEXCEPT
{
    return _mm256_and_si256(a, b);
}

// AVX2
INLINE xint256_t or_(xint256_t a, xint256_t b) NOEXCEPT
{
    return _mm256_or_si256(a, b);
}

// AVX2
INLINE xint256_t xor_(xint256_t a, xint256_t b) NOEXCEPT
{
    return _mm256_xor_si256(a, b);
}

// AVX2
INLINE xint256_t not_(xint256_t a) NOEXCEPT
{
    return xor_(a, _mm256_set1_epi64x(-1));
}

/// vector primitives
/// ---------------------------------------------------------------------------

// AVX2
template <auto B, auto S>
INLINE xint256_t shr(xint256_t a) NOEXCEPT
{
    if constexpr (S == bits<uint8_t>)
        return mm256_srli_epi8<B>(a);
    if constexpr (S == bits<uint16_t>)
        return _mm256_srli_epi16(a, B);
    if constexpr (S == bits<uint32_t>)
        return _mm256_srli_epi32(a, B);
    if constexpr (S == bits<uint64_t>)
        return _mm256_srli_epi64(a, B);
}

// AVX2
template <auto B, auto S>
INLINE xint256_t shl(xint256_t a) NOEXCEPT
{
    if constexpr (S == bits<uint8_t>)
        return mm256_slli_epi8<B>(a);
    if constexpr (S == bits<uint16_t>)
        return _mm256_slli_epi16(a, B);
    if constexpr (S == bits<uint32_t>)
        return _mm256_slli_epi32(a, B);
    if constexpr (S == bits<uint64_t>)
        return _mm256_slli_epi64(a, B);
}

template <auto B, auto S>
INLINE xint256_t ror(xint256_t a) NOEXCEPT
{
    return or_(shr<B, S>(a), shl<S - B, S>(a));
}

template <auto B, auto S>
INLINE xint256_t rol(xint256_t a) NOEXCEPT
{
    return or_(shl<B, S>(a), shr<S - B, S>(a));
}

// AVX2
template <auto S>
INLINE xint256_t add(xint256_t a, xint256_t b) NOEXCEPT
{
    if constexpr (S == bits<uint8_t>)
        return _mm256_add_epi8(a, b);
    if constexpr (S == bits<uint16_t>)
        return _mm256_add_epi16(a, b);
    if constexpr (S == bits<uint32_t>)
        return _mm256_add_epi32(a, b);
    if constexpr (S == bits<uint64_t>)
        return _mm256_add_epi64(a, b);
}

// AVX
template <auto K, auto S>
INLINE xint256_t addc(xint256_t a) NOEXCEPT
{
    if constexpr (S == bits<uint8_t>)
        return add<S>(a, _mm256_set1_epi8(K));
    if constexpr (S == bits<uint16_t>)
        return add<S>(a, _mm256_set1_epi16(K));
    if constexpr (S == bits<uint32_t>)
        return add<S>(a, _mm256_set1_epi32(K));
    if constexpr (S == bits<uint64_t>)
        return add<S>(a, _mm256_set1_epi64x(K));
}

} // namespace f

/// add/broadcast/get/set
/// ---------------------------------------------------------------------------

// AVX
template <typename Word, if_integral_integer<Word> = true>
INLINE xint256_t add(xint256_t a, xint256_t b) NOEXCEPT
{
    if constexpr (is_same_type<Word, uint8_t>)
        return _mm256_add_epi8(a, b);
    if constexpr (is_same_type<Word, uint16_t>)
        return _mm256_add_epi16(a, b);
    if constexpr (is_same_type<Word, uint32_t>)
        return _mm256_add_epi32(a, b);
    if constexpr (is_same_type<Word, uint64_t>)
        return _mm256_add_epi64(a, b);
}

// AVX
template <typename xWord, typename Word, if_integral_integer<Word> = true,
    if_same<xWord, xint256_t> = true>
INLINE xint256_t broadcast(Word a) NOEXCEPT
{
    // set1 broadcasts integer to all elements.
    if constexpr (is_same_type<Word, uint8_t>)
        return _mm256_set1_epi8(a);
    if constexpr (is_same_type<Word, uint16_t>)
        return _mm256_set1_epi16(a);
    if constexpr (is_same_type<Word, uint32_t>)
        return _mm256_set1_epi32(a);
    if constexpr (is_same_type<Word, uint64_t>)
        return _mm256_set1_epi64x(a);
}

// AVX2
// Lane zero is lowest order word.
template <typename Word, auto Lane, if_integral_integer<Word> = true>
INLINE Word get(xint256_t a) NOEXCEPT
{
    if constexpr (is_same_type<Word, uint8_t>)
        return _mm256_extract_epi8(a, Lane);
    if constexpr (is_same_type<Word, uint16_t>)
        return _mm256_extract_epi16(a, Lane);
    if constexpr (is_same_type<Word, uint32_t>)
        return _mm256_extract_epi32(a, Lane);
    if constexpr (is_same_type<Word, uint64_t>)
        return mm256_extract_epi64<Lane>(a);
}

// AVX
// Low order word to the left.
template <typename xWord, if_same<xWord, xint256_t> = true>
INLINE xint256_t set(
    uint64_t x01, uint64_t x02, uint64_t x03, uint64_t x04) NOEXCEPT
{
    // Low order word to the right.
    return _mm256_set_epi64x(
        x04, x03, x02, x01);
}

// AVX
template <typename xWord, if_same<xWord, xint256_t> = true>
INLINE xint256_t set(
    uint32_t x01, uint32_t x02, uint32_t x03, uint32_t x04,
    uint32_t x05, uint32_t x06, uint32_t x07, uint32_t x08) NOEXCEPT
{
    return _mm256_set_epi32(
        x08, x07, x06, x05, x04, x03, x02, x01);
}

// AVX
template <typename xWord, if_same<xWord, xint256_t> = true>
INLINE xint256_t set(
    uint16_t x01, uint16_t x02, uint16_t x03, uint16_t x04,
    uint16_t x05, uint16_t x06, uint16_t x07, uint16_t x08,
    uint16_t x09, uint16_t x10, uint16_t x11, uint16_t x12,
    uint16_t x13, uint16_t x14, uint16_t x15, uint16_t x16) NOEXCEPT
{
    return _mm256_set_epi16(
        x16, x15, x14, x13, x12, x11, x10, x09,
        x08, x07, x06, x05, x04, x03, x02, x01);
}

// AVX
template <typename xWord, if_same<xWord, xint256_t> = true>
INLINE xint256_t set(
    uint8_t x01, uint8_t x02, uint8_t x03, uint8_t x04,
    uint8_t x05, uint8_t x06, uint8_t x07, uint8_t x08,
    uint8_t x09, uint8_t x10, uint8_t x11, uint8_t x12,
    uint8_t x13, uint8_t x14, uint8_t x15, uint8_t x16,
    uint8_t x17, uint8_t x18, uint8_t x19, uint8_t x20,
    uint8_t x21, uint8_t x22, uint8_t x23, uint8_t x24,
    uint8_t x25, uint8_t x26, uint8_t x27, uint8_t x28,
    uint8_t x29, uint8_t x30, uint8_t x31, uint8_t x32) NOEXCEPT
{
    return _mm256_set_epi8(
        x32, x31, x30, x29, x28, x27, x26, x25,
        x24, x23, x22, x21, x20, x19, x18, x17,
        x16, x15, x14, x13, x12, x11, x10, x09,
        x08, x07, x06, x05, x04, x03, x02, x01);
}

/// endianness
/// ---------------------------------------------------------------------------

template <typename Word, if_same<Word, uint8_t> = true>
INLINE xint256_t byteswap(xint256_t a) NOEXCEPT
{
    return a;
}

// AVX2
template <typename Word, if_same<Word, uint16_t> = true>
INLINE xint256_t byteswap(xint256_t a) NOEXCEPT
{
    return _mm256_shuffle_epi8(a, set<xint256_t>(
        1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14,
        17, 16, 19, 18, 21, 20, 23, 22, 25, 24, 27, 26, 29, 28, 31, 30));
}

// AVX2
template <typename Word, if_same<Word, uint32_t> = true>
INLINE xint256_t byteswap(xint256_t a) NOEXCEPT
{
    return _mm256_shuffle_epi8(a, set<xint256_t>(
        3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12,
        19, 18, 17, 16, 23, 22, 21, 20, 27, 26, 25, 24, 31, 30, 29, 28));
}

// AVX2
template <typename Word, if_same<Word, uint64_t> = true>
INLINE xint256_t byteswap(xint256_t a) NOEXCEPT
{
    return _mm256_shuffle_epi8(a, set<xint256_t>(
        7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8,
        23, 22, 21, 20, 19, 18, 17, 16, 31, 30, 29, 28, 27, 26, 25, 24));
}

/// load/store (from casted to loaded/stored)
/// ---------------------------------------------------------------------------

INLINE xint256_t load(const xint256_t& bytes) NOEXCEPT
{
    return _mm256_loadu_si256(&bytes);
}

INLINE void store(xint256_t& bytes, xint256_t a) NOEXCEPT
{
    _mm256_storeu_si256(&bytes, a);
}

INLINE xint256_t load_aligned(const xint256_t& bytes) NOEXCEPT
{
    return _mm256_load_si256(&bytes);
}

INLINE void store_aligned(xint256_t& bytes, xint256_t a) NOEXCEPT
{
    _mm256_store_si256(&bytes, a);
}

#endif // HAVE_AVX2

} // namespace system
} // namespace libbitcoin

#endif

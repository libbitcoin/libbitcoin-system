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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_FUNCTIONAL_512_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_FUNCTIONAL_512_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/types.hpp>
#include <bitcoin/system/intrinsics/platforms/platforms.hpp>

// shl_/shr_ are undefined for 8 bit.
// get() is AVX512_VBMI2 for 8/16 [disabled].
// shl_/shr_ AVX512BW for 16 bit  [EBX bit 30].
// add_ is AVX512BW for 8/16 bit  [EBX bit 30].
// byteswap() is AVX512BW for all [EBX bit 30].
// All others are at most AVX512F [EBX bit 16].
// AVX512F-only machines started phaseout in 8/2019.
// AVX512BW machines in production since 5/2017.

namespace libbitcoin {
namespace system {
    
// all 512 symbols must be defined whenever HAVE_512 is defined.
#if defined(HAVE_512)

namespace f {

/// bitwise primitives
/// ---------------------------------------------------------------------------

// AVX512F
INLINE xint512_t and_(xint512_t a, xint512_t b) NOEXCEPT
{
    return mm512_and_si512(a, b);
}

// AVX512F
INLINE xint512_t or_(xint512_t a, xint512_t b) NOEXCEPT
{
    return mm512_or_si512(a, b);
}

// AVX512F
INLINE xint512_t xor_(xint512_t a, xint512_t b) NOEXCEPT
{
    return mm512_xor_si512(a, b);
}

// AVX512F
INLINE xint512_t not_(xint512_t a) NOEXCEPT
{
    return xor_(a, mm512_set1_epi64(-1));
}

/// Vector primitives.
/// ---------------------------------------------------------------------------

template <auto B, auto S>
INLINE xint512_t shr(xint512_t a) NOEXCEPT
{
    // Undefined
    static_assert(S != bits<uint8_t>);
    ////if constexpr (S == bits<uint8_t>)
    ////    return mm512_srli_epi8(a, B);

    // AVX512BW
    if constexpr (S == bits<uint16_t>)
        return mm512_srli_epi16(a, B);

    // AVX512F
    if constexpr (S == bits<uint32_t>)
        return mm512_srli_epi32(a, B);
    if constexpr (S == bits<uint64_t>)
        return mm512_srli_epi64(a, B);
}

template <auto B, auto S>
INLINE xint512_t shl(xint512_t a) NOEXCEPT
{
    // Undefined
    static_assert(S != bits<uint8_t>);
    ////if constexpr (S == bits<uint8_t>)
    ////    return mm512_slli_epi8(a, B);

    // AVX512BW
    if constexpr (S == bits<uint16_t>)
        return mm512_slli_epi16(a, B);

    // AVX512F
    if constexpr (S == bits<uint32_t>)
        return mm512_slli_epi32(a, B);
    if constexpr (S == bits<uint64_t>)
        return mm512_slli_epi64(a, B);
}

template <auto B, auto S>
INLINE xint512_t ror(xint512_t a) NOEXCEPT
{
    return or_(shr<B, S>(a), shl<S - B, S>(a));
}

template <auto B, auto S>
INLINE xint512_t rol(xint512_t a) NOEXCEPT
{
    return or_(shl<B, S>(a), shr<S - B, S>(a));
}

template <auto S>
INLINE xint512_t add(xint512_t a, xint512_t b) NOEXCEPT
{
    // AVX512BW
    if constexpr (S == bits<uint8_t>)
        return mm512_add_epi8(a, b);
    if constexpr (S == bits<uint16_t>)
        return mm512_add_epi16(a, b);

    // AVX512F
    if constexpr (S == bits<uint32_t>)
        return mm512_add_epi32(a, b);
    if constexpr (S == bits<uint64_t>)
        return mm512_add_epi64(a, b);
}

// AVX512F
template <auto K, auto S>
INLINE xint512_t addc(xint512_t a) NOEXCEPT
{
    if constexpr (S == bits<uint8_t>)
        return add<S>(a, mm512_set1_epi8(K));
    if constexpr (S == bits<uint16_t>)
        return add<S>(a, mm512_set1_epi16(K));
    if constexpr (S == bits<uint32_t>)
        return add<S>(a, mm512_set1_epi32(K));
    if constexpr (S == bits<uint64_t>)
        return add<S>(a, mm512_set1_epi64(K));
}

} // namespace f

/// broadcast/get/set
/// ---------------------------------------------------------------------------

// AVX512F
template <typename Word, if_integral_integer<Word> = true>
INLINE xint512_t add(xint512_t a, xint512_t b) NOEXCEPT
{
    if constexpr (is_same_type<Word, uint8_t>)
        return mm512_add_epi8(a, b);
    if constexpr (is_same_type<Word, uint16_t>)
        return mm512_add_epi16(a, b);
    if constexpr (is_same_type<Word, uint32_t>)
        return mm512_add_epi32(a, b);
    if constexpr (is_same_type<Word, uint64_t>)
        return mm512_add_epi64(a, b);
}

// AVX512F
template <typename xWord, typename Word, if_integral_integer<Word> = true,
    if_same<xWord, xint512_t> = true>
INLINE xint512_t broadcast(Word a) NOEXCEPT
{
    // set1 broadcasts integer to all elements.
    if constexpr (is_same_type<Word, uint8_t>)
        return mm512_set1_epi8(a);
    if constexpr (is_same_type<Word, uint16_t>)
        return mm512_set1_epi16(a);
    if constexpr (is_same_type<Word, uint32_t>)
        return mm512_set1_epi32(a);
    if constexpr (is_same_type<Word, uint64_t>)
        return mm512_set1_epi64(a);
}

// Lane zero is lowest order word.
template <typename Word, auto Lane, if_integral_integer<Word> = true>
INLINE Word get(xint512_t a) NOEXCEPT
{
    // AVX512_VBMI2/AVX512F/SSE2
    static_assert(!is_same_type<Word, uint8_t>);
    static_assert(!is_same_type<Word, uint16_t>);
    ////if constexpr (is_same_type<Word, uint8_t>)
    ////    return mm512_extract_epi8(a, Lane);
    ////else if constexpr (is_same_type<Word, uint16_t>)
    ////    return mm512_extract_epi16(a, Lane);

    // AVX512F/SSE2
    if constexpr (is_same_type<Word, uint32_t>)
        return mm512_extract_epi32(a, Lane);
    else if constexpr (is_same_type<Word, uint64_t>)
        return mm512_extract_epi64(a, Lane);
}

// AVX512F
// Low order word to the left.
template <typename xWord, if_same<xWord, xint512_t> = true>
INLINE xint512_t set(
    uint64_t x01, uint64_t x02, uint64_t x03, uint64_t x04,
    uint64_t x05, uint64_t x06, uint64_t x07, uint64_t x08) NOEXCEPT
{
    // Low order word to the right.
    return mm512_set_epi64(
        x08, x07, x06, x05, x04, x03, x02, x01);
}

// AVX512F
template <typename xWord, if_same<xWord, xint512_t> = true>
INLINE xint512_t set(
    uint32_t x01, uint32_t x02, uint32_t x03, uint32_t x04,
    uint32_t x05, uint32_t x06, uint32_t x07, uint32_t x08,
    uint32_t x09, uint32_t x10, uint32_t x11, uint32_t x12,
    uint32_t x13, uint32_t x14, uint32_t x15, uint32_t x16) NOEXCEPT
{
    return mm512_set_epi32(
        x16, x15, x14, x13, x12, x11, x10, x09,
        x08, x07, x06, x05, x04, x03, x02, x01);
}

// AVX512F
template <typename xWord, if_same<xWord, xint512_t> = true>
INLINE xint512_t set(
    uint16_t x01, uint16_t x02, uint16_t x03, uint16_t x04,
    uint16_t x05, uint16_t x06, uint16_t x07, uint16_t x08,
    uint16_t x09, uint16_t x10, uint16_t x11, uint16_t x12,
    uint16_t x13, uint16_t x14, uint16_t x15, uint16_t x16,
    uint16_t x17, uint16_t x18, uint16_t x19, uint16_t x20,
    uint16_t x21, uint16_t x22, uint16_t x23, uint16_t x24,
    uint16_t x25, uint16_t x26, uint16_t x27, uint16_t x28,
    uint16_t x29, uint16_t x30, uint16_t x31, uint16_t x32) NOEXCEPT
{
    return mm512_set_epi16(
        x32, x31, x30, x29, x28, x27, x26, x25,
        x24, x23, x22, x21, x20, x19, x18, x17,
        x16, x15, x14, x13, x12, x11, x10, x09,
        x08, x07, x06, x05, x04, x03, x02, x01);
}

// AVX512F
template <typename xWord, if_same<xWord, xint512_t> = true>
INLINE xint512_t set(
    uint8_t x01, uint8_t x02, uint8_t x03, uint8_t x04,
    uint8_t x05, uint8_t x06, uint8_t x07, uint8_t x08,
    uint8_t x09, uint8_t x10, uint8_t x11, uint8_t x12,
    uint8_t x13, uint8_t x14, uint8_t x15, uint8_t x16,
    uint8_t x17, uint8_t x18, uint8_t x19, uint8_t x20,
    uint8_t x21, uint8_t x22, uint8_t x23, uint8_t x24,
    uint8_t x25, uint8_t x26, uint8_t x27, uint8_t x28,
    uint8_t x29, uint8_t x30, uint8_t x31, uint8_t x32,
    uint8_t x33, uint8_t x34, uint8_t x35, uint8_t x36,
    uint8_t x37, uint8_t x38, uint8_t x39, uint8_t x40,
    uint8_t x41, uint8_t x42, uint8_t x43, uint8_t x44,
    uint8_t x45, uint8_t x46, uint8_t x47, uint8_t x48,
    uint8_t x49, uint8_t x50, uint8_t x51, uint8_t x52,
    uint8_t x53, uint8_t x54, uint8_t x55, uint8_t x56,
    uint8_t x57, uint8_t x58, uint8_t x59, uint8_t x60,
    uint8_t x61, uint8_t x62, uint8_t x63, uint8_t x64) NOEXCEPT
{
    return mm512_set_epi8(
        x64, x63, x62, x61, x60, x59, x58, x57,
        x56, x55, x54, x53, x52, x51, x50, x49,
        x48, x47, x46, x45, x44, x43, x42, x41,
        x40, x39, x38, x37, x36, x35, x34, x33,
        x32, x31, x30, x29, x28, x27, x26, x25,
        x24, x23, x22, x21, x20, x19, x18, x17,
        x16, x15, x14, x13, x12, x11, x10, x09,
        x08, x07, x06, x05, x04, x03, x02, x01);
}

/// endianness
/// ---------------------------------------------------------------------------

// AVX512BW
template <typename Word, if_same<Word, uint8_t> = true>
INLINE xint512_t byteswap(xint512_t a) NOEXCEPT
{
    return a;
}

// AVX512BW
template <typename Word, if_same<Word, uint16_t> = true>
INLINE xint512_t byteswap(xint512_t a) NOEXCEPT
{
    return mm512_shuffle_epi8(a, set<xint512_t>(
        1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14,
        17, 16, 19, 18, 21, 20, 23, 22, 25, 24, 27, 26, 29, 28, 31, 30,
        33, 32, 35, 34, 37, 36, 39, 38, 41, 40, 43, 42, 45, 44, 47, 46,
        49, 48, 51, 50, 53, 52, 55, 54, 57, 56, 59, 58, 61, 60, 63, 62));
}

// AVX512BW
template <typename Word, if_same<Word, uint32_t> = true>
INLINE xint512_t byteswap(xint512_t a) NOEXCEPT
{
    return mm512_shuffle_epi8(a, set<xint512_t>(
        3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12,
        19, 18, 17, 16, 23, 22, 21, 20, 27, 26, 25, 24, 31, 30, 29, 28,
        35, 34, 33, 32, 39, 38, 37, 36, 43, 42, 41, 40, 47, 46, 45, 44,
        51, 50, 49, 48, 55, 54, 53, 52, 59, 58, 57, 56, 63, 62, 61, 60));
}

// AVX512BW
template <typename Word, if_same<Word, uint64_t> = true>
INLINE xint512_t byteswap(xint512_t a) NOEXCEPT
{
    return mm512_shuffle_epi8(a, set<xint512_t>(
        7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8,
        23, 22, 21, 20, 19, 18, 17, 16, 31, 30, 29, 28, 27, 26, 25, 24,
        39, 38, 37, 36, 35, 34, 33, 32, 47, 46, 45, 44, 43, 42, 41, 40,
        55, 54, 53, 52, 51, 50, 49, 48, 63, 62, 61, 60, 59, 58, 57, 56));
}

/// load/store (from casted to loaded/stored)
/// ---------------------------------------------------------------------------
/// These have defined overrides for !HAVE_512

INLINE xint512_t load(const xint512_t& bytes) NOEXCEPT
{
    return mm512_loadu_si512(&bytes);
}

INLINE void store(xint512_t& bytes, xint512_t a) NOEXCEPT
{
    mm512_storeu_si512(&bytes, a);
}

INLINE xint512_t load_aligned(const xint512_t& bytes) NOEXCEPT
{
    return mm512_load_si512(&bytes);
}

INLINE void store_aligned(xint512_t& bytes, xint512_t a) NOEXCEPT
{
    mm512_store_si512(&bytes, a);
}

#else // HAVE_512

INLINE xint512_t load(const xint512_t& bytes) NOEXCEPT
{
    return bytes;
}

INLINE void store(xint512_t&, xint512_t) NOEXCEPT
{
}

INLINE xint512_t load_aligned(const xint512_t& bytes) NOEXCEPT
{
    return bytes;
}

INLINE void store_aligned(xint512_t&, xint512_t) NOEXCEPT
{
}

#endif // HAVE_512

} // namespace system
} // namespace libbitcoin

#endif

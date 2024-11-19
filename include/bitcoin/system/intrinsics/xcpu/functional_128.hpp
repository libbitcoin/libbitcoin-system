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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_FUNCTIONAL_128_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_FUNCTIONAL_128_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/xcpu/defines.hpp>

// shl_/shr_ are undefined for 8 bit.
// All others are at most SSE4.1.
// Apart from extract and byteswap, all are at most SSE2.

namespace libbitcoin {
namespace system {

#if defined(HAVE_SSE41)

using xint128_t = __m128i;

namespace f {

/// bitwise primitives
/// ---------------------------------------------------------------------------

// SSE2
INLINE xint128_t and_(xint128_t a, xint128_t b) NOEXCEPT
{
    return mm_and_si128(a, b);
}

// SSE2
INLINE xint128_t or_(xint128_t a, xint128_t b) NOEXCEPT
{
    return mm_or_si128(a, b);
}

// SSE2
INLINE xint128_t xor_(xint128_t a, xint128_t b) NOEXCEPT
{
    return mm_xor_si128(a, b);
}

// SSE2
INLINE xint128_t not_(xint128_t a) NOEXCEPT
{
    return xor_(a, mm_set1_epi64x(-1));
}

/// vector primitives
/// ---------------------------------------------------------------------------

// SSE2
template <auto B, auto S>
INLINE xint128_t shr(xint128_t a) NOEXCEPT
{
    // Undefined.
    static_assert(S != bits<uint8_t>);
    ////if constexpr (S == bits<uint8_t>)
    ////    return mm_srli_epi8(a, B);

    if constexpr (S == bits<uint16_t>)
        return mm_srli_epi16(a, B);
    else if constexpr (S == bits<uint32_t>)
        return mm_srli_epi32(a, B);
    else if constexpr (S == bits<uint64_t>)
        return mm_srli_epi64(a, B);
}

// SSE2
template <auto B, auto S>
INLINE xint128_t shl(xint128_t a) NOEXCEPT
{
    // Undefined.
    static_assert(S != bits<uint8_t>);
    ////if constexpr (S == bits<uint8_t>)
    ////    return mm_slli_epi8(a, B);

    if constexpr (S == bits<uint16_t>)
        return mm_slli_epi16(a, B);
    else if constexpr (S == bits<uint32_t>)
        return mm_slli_epi32(a, B);
    else if constexpr (S == bits<uint64_t>)
        return mm_slli_epi64(a, B);
}

template <auto B, auto S>
INLINE xint128_t ror(xint128_t a) NOEXCEPT
{
    return or_(shr<B, S>(a), shl<S - B, S>(a));
}

template <auto B, auto S>
INLINE xint128_t rol(xint128_t a) NOEXCEPT
{
    return or_(shl<B, S>(a), shr<S - B, S>(a));
}

// SSE2
template <auto S>
INLINE xint128_t add(xint128_t a, xint128_t b) NOEXCEPT
{
    if constexpr (S == bits<uint8_t>)
        return mm_add_epi8(a, b);
    else if constexpr (S == bits<uint16_t>)
        return mm_add_epi16(a, b);
    else if constexpr (S == bits<uint32_t>)
        return mm_add_epi32(a, b);
    else if constexpr (S == bits<uint64_t>)
        return mm_add_epi64(a, b);
}

// SSE2
template <auto K, auto S>
INLINE xint128_t addc(xint128_t a) NOEXCEPT
{
    if constexpr (S == bits<uint8_t>)
        return add<S>(a, mm_set1_epi8(K));
    else if constexpr (S == bits<uint16_t>)
        return add<S>(a, mm_set1_epi16(K));
    else if constexpr (S == bits<uint32_t>)
        return add<S>(a, mm_set1_epi32(K));
    else if constexpr (S == bits<uint64_t>)
        return add<S>(a, mm_set1_epi64x(K));
}

} // namespace f

/// broadcast/get/set
/// ---------------------------------------------------------------------------

// SSE2
template <typename xWord, typename Word,
    if_same<xWord, xint128_t> = true, if_integral_integer<Word> = true>
INLINE xint128_t broadcast(Word a) NOEXCEPT
{
    // set1 broadcasts integer to all elements.
    if constexpr (is_same_type<Word, uint8_t>)
        return mm_set1_epi8(a);
    if constexpr (is_same_type<Word, uint16_t>)
        return mm_set1_epi16(a);
    if constexpr (is_same_type<Word, uint32_t>)
        return mm_set1_epi32(a);
    if constexpr (is_same_type<Word, uint64_t>)
        return mm_set1_epi64x(a);
}

// Lane zero is lowest order word.
template <typename Word, auto Lane, if_integral_integer<Word> = true>
INLINE Word get(xint128_t a) NOEXCEPT
{
    // mm_extract_epi64 defined as no-op on 32 bit builds (must exclude).
    ////static_assert(!build_x32 && is_same_type<Word, uint64_t>);

    // SSE4.1
    if constexpr (is_same_type<Word, uint8_t>)
        return mm_extract_epi8(a, Lane);

    // SSE2
    else if constexpr (is_same_type<Word, uint16_t>)
        return mm_extract_epi16(a, Lane);

    // SSE4.1
    else if constexpr (is_same_type<Word, uint32_t>)
        return mm_extract_epi32(a, Lane);
    else if constexpr (is_same_type<Word, uint64_t>)
        return mm_extract_epi64(a, Lane);
}

// SSE2
// Low order word to the left.
template <typename xWord, if_same<xWord, xint128_t> = true>
INLINE xint128_t set(uint64_t x01, uint64_t x02) NOEXCEPT
{
    // Low order word to the right.
    return mm_set_epi64x(x02, x01);
}

// SSE2
template <typename xWord, if_same<xWord, xint128_t> = true>
INLINE xint128_t set(
    uint32_t x01, uint32_t x02, uint32_t x03, uint32_t x04) NOEXCEPT
{
    return mm_set_epi32(x04, x03, x02, x01);
}

// SSE2
template <typename xWord, if_same<xWord, xint128_t> = true>
INLINE xint128_t set(
    uint16_t x01, uint16_t x02, uint16_t x03, uint16_t x04,
    uint16_t x05, uint16_t x06, uint16_t x07, uint16_t x08) NOEXCEPT
{
    return mm_set_epi16(
        x08, x07, x06, x05, x04, x03, x02, x01);
}

// SSE2
template <typename xWord, if_same<xWord, xint128_t> = true>
INLINE xint128_t set(
    uint8_t x01, uint8_t x02, uint8_t x03, uint8_t x04,
    uint8_t x05, uint8_t x06, uint8_t x07, uint8_t x08,
    uint8_t x09, uint8_t x10, uint8_t x11, uint8_t x12,
    uint8_t x13, uint8_t x14, uint8_t x15, uint8_t x16) NOEXCEPT
{
    return mm_set_epi8(
        x16, x15, x14, x13, x12, x11, x10, x09,
        x08, x07, x06, x05, x04, x03, x02, x01);
}

/// endianness
/// ---------------------------------------------------------------------------

template <typename Word, if_same<Word, uint8_t> = true>
INLINE xint128_t byteswap(xint128_t a) NOEXCEPT
{
    return a;
}

// SSE3
template <typename Word, if_same<Word, uint16_t> = true>
INLINE xint128_t byteswap(xint128_t a) NOEXCEPT
{
    static const auto mask = set<xint128_t>(
        1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);

    return mm_shuffle_epi8(a, mask);
}

// SSE3
template <typename Word, if_same<Word, uint32_t> = true>
INLINE xint128_t byteswap(xint128_t a) NOEXCEPT
{
    static const auto mask = set<xint128_t>(
        3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12);

    return mm_shuffle_epi8(a, mask);
}

// SSE3
template <typename Word, if_same<Word, uint64_t> = true>
INLINE xint128_t byteswap(xint128_t a) NOEXCEPT
{
    static const auto mask = set<xint128_t>(
        7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8);

    return mm_shuffle_epi8(a, mask);
}

/// pack/unpack
/// ---------------------------------------------------------------------------
////
////// TODO: auto pack<xWord>(const uint8_t*).
////INLINE auto unpack(xint128_t a) NOEXCEPT
////{
////    std_array<uint8_t, sizeof(xint128_t)> bytes{};
////    mm_storeu_si128(pointer_cast<xint128_t>(&bytes.front()), a);
////    return bytes;
////}

#else

// Symbol is defined but not usable as an integer.
using xint128_t = std_array<uint8_t, bytes<128>>;

#endif // HAVE_SSE41

} // namespace system
} // namespace libbitcoin

#endif

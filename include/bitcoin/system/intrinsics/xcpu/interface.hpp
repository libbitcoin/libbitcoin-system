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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_INTERFACE_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_INTERFACE_HPP

#include <limits>
#include <type_traits>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {

// TODO: factor and move to functionals.
// TODO: symbols defined with XCPU but functions cannot be compiled on non-MSC
// TODO: without compile switches, which requires compile-time tests. So
// TODO: functionals must be conditionally compiled. To reduce conditionality
// TODO: avoid incorporating explicit xint types into code.

/// Intel/AMD is always little-endian, and SHA is always big-endian (swap).
/// Primitives implement rotr/rotl because these are not available in SIMD.

/// Define XCPU extended integral aliases.
using xint128_t = __m128i; // SSE4
using xint256_t = __m256i; // AVX2
using xint512_t = __m512i; // AVX512

// TODO: change the element to false if type not compiled.
template <typename Type>
constexpr auto is_extended = is_same_type<Type, xint128_t> ||
    is_same_type<Type, xint256_t> || is_same_type<Type, xint512_t>;

template <typename Type>
using if_extended = bool_if<is_extended<Type>>;

// TODO: load as 2x64 for xint128, 4x64 for xint256, 8x64 for xint512.
constexpr std_array<uint32_t, 4> bswap_mask
{
    0x0c0d0e0ful, 0x08090a0bul, 0x04050607ul, 0x00010203ul // from simd
////0x00010203ul, 0x04050607ul, 0x08090a0bul, 0x0c0d0e0ful // from sha-ni
};

// TODO: define these macros (_mm512_extract_epi64 required for sha512v).
// TODO: these are multistep, as are the others, can be optimized.
// stackoverflow.com/questions/58303958/how-to-implement-16-and-32-bit-
// integer-insert-and-extract-operations-with-avx-51
#define _mm512_extract_epi8(a, Lane) 0
#define _mm512_extract_epi16(a, Lane) 0
#define _mm512_extract_epi32(a, Lane) 0
#define _mm512_extract_epi64(a, Lane) 0

// Overload for vectorization compatibility.
template <typename To, unsigned int Lane, typename From,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true>
INLINE constexpr To get(From a) NOEXCEPT
{
    static_assert(Lane == one);
    return a;
}

template <typename To, unsigned int Lane, typename From,
    if_extended<From> = true,
    if_integral_integer<To> = true>
INLINE To get(From a) NOEXCEPT
{
    if constexpr (is_same_type<To, uint16_t>)
    {
        if constexpr (is_same_type<From, xint128_t>)
            return _mm_extract_epi8(a, Lane);
        if constexpr (is_same_type<From, xint256_t>)
            return _mm256_extract_epi8(a, Lane);
        if constexpr (is_same_type<From, xint512_t>)
            return _mm512_extract_epi8(a, Lane);
    }
    else if constexpr (is_same_type<To, uint16_t>)
    {
        if constexpr (is_same_type<From, xint128_t>)
            return _mm_extract_epi16(a, Lane);
        if constexpr (is_same_type<From, xint256_t>)
            return _mm256_extract_epi16(a, Lane);
        if constexpr (is_same_type<From, xint512_t>)
            return _mm512_extract_epi16(a, Lane);
    }
    else if constexpr (is_same_type<To, uint32_t>)
    {
        if constexpr (is_same_type<From, xint128_t>)
            return _mm_extract_epi32(a, Lane);
        if constexpr (is_same_type<From, xint256_t>)
            return _mm256_extract_epi32(a, Lane);
        if constexpr (is_same_type<From, xint512_t>)
            return _mm512_extract_epi32(a, Lane);
    }
    if constexpr (is_same_type<To, uint64_t>)
    {
        if constexpr (is_same_type<From, xint128_t>)
            return _mm_extract_epi64(a, Lane);
        if constexpr (is_same_type<From, xint256_t>)
            return _mm256_extract_epi64(a, Lane);
        if constexpr (is_same_type<From, xint512_t>)
            return _mm512_extract_epi64(a, Lane);
    }
}

// Overload for vectorization compatibility (unnecessary).
////template <typename To, typename From,
////    if_integral_integer<From> = true>
////INLINE constexpr To set(
////    From = 0, From = 0, From = 0, From = 0,
////    From = 0, From = 0, From = 0, From = 0,
////    From = 0, From = 0, From = 0, From = 0,
////    From = 0, From = 0, From = 0, From = 0) NOEXCEPT
////{
////    return To{};
////}

// TODO: restrict uint64_t(2)
// TODO: restrict uint32_t(4)
// TODO: restrict uint16_t(8)
// TODO: can just expand to 4 templates.
template <typename To, typename From,
    if_same<To, xint128_t> = true,
    if_integral_integer<From> = true>
INLINE To set(
    From x01 = 0, From x02 = 0, From x03 = 0, From x04 = 0,
    From x05 = 0, From x06 = 0, From x07 = 0, From x08 = 0,
    From x09 = 0, From x10 = 0, From x11 = 0, From x12 = 0,
    From x13 = 0, From x14 = 0, From x15 = 0, From x16 = 0) NOEXCEPT
{
    if constexpr (is_same_type<From, uint8_t>)
    {
        return _mm_set_epi8(
            x01, x02, x03, x04, x05, x06, x07, x08,
            x09, x10, x11, x12, x13, x14, x15, x16);
    }
    else if constexpr (is_same_type<From, uint16_t>)
    {
        return _mm_set_epi16(
            x01, x02, x03, x04, x05, x06, x07, x08);
    }
    else if constexpr (is_same_type<From, uint32_t>)
    {
        return _mm_set_epi32(
            x01, x02, x03, x04);
    }
    else if constexpr (is_same_type<From, uint64_t>)
    {
        return _mm_set_epi64(
            x01, x02);
    }
}

// TODO: restrict uint64_t(4)
// TODO: restrict uint32_t(8)
// TODO: restrict uint16_t(16)
// TODO: can just expand to 4 templates.
template <typename To, typename From,
    if_same<To, xint256_t> = true,
    if_integral_integer<From> = true>
INLINE To set(
    From x01 = 0, From x02 = 0, From x03 = 0, From x04 = 0,
    From x05 = 0, From x06 = 0, From x07 = 0, From x08 = 0,
    From x09 = 0, From x10 = 0, From x11 = 0, From x12 = 0,
    From x13 = 0, From x14 = 0, From x15 = 0, From x16 = 0,
    From x17 = 0, From x18 = 0, From x19 = 0, From x20 = 0,
    From x21 = 0, From x22 = 0, From x23 = 0, From x24 = 0,
    From x25 = 0, From x26 = 0, From x27 = 0, From x28 = 0,
    From x29 = 0, From x30 = 0, From x31 = 0, From x32 = 0) NOEXCEPT
{
    if constexpr (is_same_type<From, uint8_t>)
    {
        return _mm256_set_epi8(
            x01, x02, x03, x04, x05, x06, x07, x08,
            x09, x10, x11, x12, x13, x14, x15, x16,
            x17, x18, x19, x20, x21, x22, x23, x24,
            x25, x26, x27, x28, x29, x30, x31, x32);
    }
    else if constexpr (is_same_type<From, uint16_t>)
    {
        return _mm256_set_epi16(
            x01, x02, x03, x04, x05, x06, x07, x08,
            x09, x10, x11, x12, x13, x14, x15, x16);
    }
    else if constexpr (is_same_type<From, uint32_t>)
    {
        return _mm256_set_epi32(
            x01, x02, x03, x04, x05, x06, x07, x08);
    }
    else if constexpr (is_same_type<From, uint64_t>)
    {
        return _mm256_set_epi64x(
            x01, x02, x03, x04);
    }
}

// TODO: restrict uint64_t(8)
// TODO: restrict uint32_t(16)
// TODO: restrict uint16_t(32)
// TODO: can just expand to 4 templates.
template <typename To, typename From,
    if_same<To, xint512_t> = true,
    if_integral_integer<From> = true>
INLINE To set(
    From x01 = 0, From x02 = 0, From x03 = 0, From x04 = 0,
    From x05 = 0, From x06 = 0, From x07 = 0, From x08 = 0,
    From x09 = 0, From x10 = 0, From x11 = 0, From x12 = 0,
    From x13 = 0, From x14 = 0, From x15 = 0, From x16 = 0,
    From x17 = 0, From x18 = 0, From x19 = 0, From x20 = 0,
    From x21 = 0, From x22 = 0, From x23 = 0, From x24 = 0,
    From x25 = 0, From x26 = 0, From x27 = 0, From x28 = 0,
    From x29 = 0, From x30 = 0, From x31 = 0, From x32 = 0,
    From x33 = 0, From x34 = 0, From x35 = 0, From x36 = 0,
    From x37 = 0, From x38 = 0, From x39 = 0, From x40 = 0,
    From x41 = 0, From x42 = 0, From x43 = 0, From x44 = 0,
    From x45 = 0, From x46 = 0, From x47 = 0, From x48 = 0,
    From x49 = 0, From x50 = 0, From x51 = 0, From x52 = 0,
    From x53 = 0, From x54 = 0, From x55 = 0, From x56 = 0,
    From x57 = 0, From x58 = 0, From x59 = 0, From x60 = 0,
    From x61 = 0, From x62 = 0, From x63 = 0, From x64 = 0) NOEXCEPT
{
    if constexpr (is_same_type<From, uint8_t>)
    {
        return _mm512_set_epi8(
            x01, x02, x03, x04, x05, x06, x07, x08,
            x09, x10, x11, x12, x13, x14, x15, x16,
            x17, x18, x19, x20, x21, x22, x23, x24,
            x25, x26, x27, x28, x29, x30, x31, x32,
            x33, x34, x35, x36, x37, x38, x39, x40,
            x41, x42, x43, x44, x45, x46, x47, x48,
            x49, x50, x51, x52, x53, x54, x55, x56,
            x57, x58, x59, x60, x61, x62, x63, x64);
    }
    else if constexpr (is_same_type<From, uint16_t>)
    {
        return _mm512_set_epi16(
            x01, x02, x03, x04, x05, x06, x07, x08,
            x09, x10, x11, x12, x13, x14, x15, x16,
            x17, x18, x19, x20, x21, x22, x23, x24,
            x25, x26, x27, x28, x29, x30, x31, x32);
    }
    else if constexpr (is_same_type<From, uint32_t>)
    {
        return _mm512_set_epi32(
            x01, x02, x03, x04, x05, x06, x07, x08,
            x09, x10, x11, x12, x13, x14, x15, x16);
    }
    else if constexpr (is_same_type<From, uint64_t>)
    {
        return _mm512_set_epi64(
            x01, x02, x03, x04, x05, x06, x07, x08);
    }
}

// ----------------------------------------------------------------------------

////template <typename Type, if_extended<Type> = true>
////using xchunk = std_array<uint8_t, <sizeof(Type)>>;
////using bytes128 = xchunk<xint128_t>;
////using bytes256 = xchunk<xint256_t>;
////using bytes512 = xchunk<xint512_t>;
////
////INLINE xint128_t align(const bytes128& word) NOEXCEPT
////{
////    return _mm_loadu_si128(pointer_cast<const xint128_t>(word.data()));
////}
////
////INLINE bytes128 unalign(xint128_t value) NOEXCEPT
////{
////    bytes128 word{};
////    _mm_storeu_si128(pointer_cast<xint128_t>(word.data()), value);
////    return word;
////    ////return *pointer_cast<bytes128>(&value);
////}
////
////// aligned to unaligned.
////INLINE xint128_t native_to_big_endian(xint128_t value) NOEXCEPT
////{
////    return *pointer_cast<xint128_t>(unalign(byteswap(value)).data());
////}
////
////// unaligned to aligned.
////INLINE xint128_t native_from_big_endian(xint128_t value) NOEXCEPT
////{
////    return byteswap(align(*pointer_cast<bytes128>(&value)));
////}
////
////INLINE xint256_t align(const bytes256& word) NOEXCEPT
////{
////    return _mm256_loadu_epi32(pointer_cast<const xint256_t>(word.data()));
////}
////
////INLINE bytes256 unalign(xint256_t value) NOEXCEPT
////{
////    bytes256 word{};
////    _mm256_storeu_epi32(pointer_cast<xint256_t>(word.data()), value);
////    return word;
////    ////return *pointer_cast<bytes256>(&value);
////}
////
////INLINE xint256_t native_to_big_endian(xint256_t value) NOEXCEPT
////{
////    return *pointer_cast<xint256_t>(unalign(byteswap(value)).data());
////}
////
////INLINE xint256_t native_from_big_endian(xint256_t value) NOEXCEPT
////{
////    return byteswap(align(*pointer_cast<bytes256>(&value)));
////}
////
////INLINE xint512_t align(const bytes512& word) NOEXCEPT
////{
////    return _mm512_loadu_epi32(pointer_cast<const xint512_t>(word.data()));
////}
////
////INLINE bytes512 unalign(xint512_t value) NOEXCEPT
////{
////    bytes512 word{};
////    _mm512_storeu_epi32(pointer_cast<xint512_t>(word.data()), value);
////    return word;
////    ////return *pointer_cast<bytes512>(&value);
////}
////
////INLINE xint512_t native_to_big_endian(xint512_t value) NOEXCEPT
////{
////    return *pointer_cast<xint512_t>(unalign(byteswap(value)).data());
////}
////
////INLINE xint512_t native_from_big_endian(xint512_t value) NOEXCEPT
////{
////    return byteswap(align(*pointer_cast<bytes512>(&value)));
////}

} // namespace libbitcoin

#endif

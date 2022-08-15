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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_FUNCTIONAL512_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_FUNCTIONAL512_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/xcpu/defines.hpp>

// TODO: truncated constant warning on add_<uint64_t K>() is indication that
// TODO: sha512 expect all 64 bit overloads. We are doing 32 bit operations on
// TODO: 64 bit logical elements. But should not be seeing in xint128 since we
// TODO: aren't yet doing sha512 (uint64_t) in two lanes (xint128_t). But maybe
// TODO: it's just the result of compiling for it, though that's not apparent.
// TODO: should static assert against word size.

namespace libbitcoin {
namespace system {

#if defined(HAVE_AVX512)

using xint512_t = __m512i;

// AVX512 primitives (for all element widths).
// ----------------------------------------------------------------------------
// Bitwise functions are idependent of element widths.

template <typename Word, if_same<Word, xint512_t> = true>
INLINE Word and_(Word a, Word b) NOEXCEPT
{
    return _mm512_and_si512(a, b);
}

template <typename Word, if_same<Word, xint512_t> = true>
INLINE Word or_(Word a, Word b) NOEXCEPT
{
    return _mm512_or_si512(a, b);
}
template <typename Word, if_same<Word, xint512_t> = true>
INLINE Word xor_(Word a, Word b) NOEXCEPT
{
    return _mm512_xor_si512(a, b);
}

// AVX512 primitives (for 32 bit elements).
// ----------------------------------------------------------------------------
// TODO: 64 bit elements.

template <auto B, typename Word, if_same<Word, xint512_t> = true>
INLINE Word shr_(Word a) NOEXCEPT
{
    return _mm512_srli_epi32(a, B);
}

template <auto B, typename Word, if_same<Word, xint512_t> = true>
INLINE Word shl_(Word a) NOEXCEPT
{
    return _mm512_slli_epi32(a, B);
}

template <auto B, auto S, typename Word, if_same<Word, xint512_t> = true>
INLINE Word ror_(Word a) NOEXCEPT
{
    // TODO: S will become unnecessary as the function set must be 32/64.
    return or_(shr_<B>(a), shl_<S - B>(a));
}

template <auto B, auto S, typename Word, if_same<Word, xint512_t> = true>
INLINE Word rol_(Word a) NOEXCEPT
{
    // TODO: S will become unnecessary as the function set must be 32/64.
    return or_(shl_<B>(a), shr_<S - B>(a));
}

template <typename Word, if_same<Word, xint512_t> = true>
INLINE Word add_(Word a, Word b) NOEXCEPT
{
    return _mm512_add_epi32(a, b);
}

template <auto K, typename Word, if_same<Word, xint512_t> = true>
INLINE Word add_(Word a) NOEXCEPT
{
    // Broadcast 32-bit integer to all elements.
    return add_(a, _mm512_set1_epi32(K));
}

// AVX512 set/get (for all element widths).
// ----------------------------------------------------------------------------

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
INLINE xint512_t byteswap(xint512_t value) NOEXCEPT
{
    constexpr std_array<uint32_t, 4> mask32
    {
        0x0c0d0e0f_u32, 0x08090a0b_u32, 0x04050607_u32, 0x00010203_u32
    };

    static const auto mask = _mm512_set_epi32(
        mask32[0], mask32[1], mask32[2], mask32[3],
        mask32[0], mask32[1], mask32[2], mask32[3],
        mask32[0], mask32[1], mask32[2], mask32[3],
        mask32[0], mask32[1], mask32[2], mask32[3]);

    return _mm512_shuffle_epi8(value, mask);
}
BC_POP_WARNING()

// TODO: define these macros (_mm512_extract_epi64 required for sha512v).
// TODO: these are multistep, as are the others, can be optimized.
// stackoverflow.com/questions/58303958/how-to-implement-16-and-32-bit-
// integer-insert-and-extract-operations-with-avx-51
#define _mm512_extract_epi8(a, Lane) 0
#define _mm512_extract_epi16(a, Lane) 0
#define _mm512_extract_epi32(a, Lane) 0
#define _mm512_extract_epi64(a, Lane) 0

// Lane zero is lowest order word.
template <typename To, auto Lane, if_integral_integer<To> = true>
INLINE To get(xint512_t) NOEXCEPT
{
    if constexpr      (is_same_type<To, uint8_t>)
    {
        return _mm512_extract_epi8(0, Lane);
    }
    else if constexpr (is_same_type<To, uint16_t>)
    {
        return _mm512_extract_epi16(0, Lane);
    }
    else if constexpr (is_same_type<To, uint32_t>)
    {
        return _mm512_extract_epi32(0, Lane);
    }
    else if constexpr (is_same_type<To, uint64_t>)
    {
        return _mm512_extract_epi64(0, Lane);
    }
}

// Low order word to the left.
template <typename To, if_same<To, xint512_t> = true>
INLINE To set(
    uint64_t x01 = 0, uint64_t x02 = 0,
    uint64_t x03 = 0, uint64_t x04 = 0,
    uint64_t x05 = 0, uint64_t x06 = 0,
    uint64_t x07 = 0, uint64_t x08 = 0) NOEXCEPT
{
    // Low order word to the right.
    return _mm512_set_epi64(
        x08, x07, x06, x05, x04, x03, x02, x01);
}

template <typename To, if_same<To, xint512_t> = true>
INLINE To set(
    uint32_t x01 = 0, uint32_t x02 = 0,
    uint32_t x03 = 0, uint32_t x04 = 0,
    uint32_t x05 = 0, uint32_t x06 = 0,
    uint32_t x07 = 0, uint32_t x08 = 0,
    uint32_t x09 = 0, uint32_t x10 = 0,
    uint32_t x11 = 0, uint32_t x12 = 0,
    uint32_t x13 = 0, uint32_t x14 = 0,
    uint32_t x15 = 0, uint32_t x16 = 0) NOEXCEPT
{
    return _mm512_set_epi32(
        x16, x15, x14, x13, x12, x11, x10, x09,
        x08, x07, x06, x05, x04, x03, x02, x01);
}

template <typename To, if_same<To, xint512_t> = true>
INLINE To set(
    uint16_t x01 = 0, uint16_t x02 = 0,
    uint16_t x03 = 0, uint16_t x04 = 0,
    uint16_t x05 = 0, uint16_t x06 = 0,
    uint16_t x07 = 0, uint16_t x08 = 0,
    uint16_t x09 = 0, uint16_t x10 = 0,
    uint16_t x11 = 0, uint16_t x12 = 0,
    uint16_t x13 = 0, uint16_t x14 = 0,
    uint16_t x15 = 0, uint16_t x16 = 0,
    uint16_t x17 = 0, uint16_t x18 = 0,
    uint16_t x19 = 0, uint16_t x20 = 0,
    uint16_t x21 = 0, uint16_t x22 = 0,
    uint16_t x23 = 0, uint16_t x24 = 0,
    uint16_t x25 = 0, uint16_t x26 = 0,
    uint16_t x27 = 0, uint16_t x28 = 0,
    uint16_t x29 = 0, uint16_t x30 = 0,
    uint16_t x31 = 0, uint16_t x32 = 0) NOEXCEPT
{
    return _mm512_set_epi16(
        x32, x31, x30, x29, x28, x27, x26, x25,
        x24, x23, x22, x21, x20, x19, x18, x17,
        x16, x15, x14, x13, x12, x11, x10, x09,
        x08, x07, x06, x05, x04, x03, x02, x01);
}

template <typename To, if_same<To, xint512_t> = true>
INLINE To set(
    uint8_t x01 = 0, uint8_t x02 = 0,
    uint8_t x03 = 0, uint8_t x04 = 0,
    uint8_t x05 = 0, uint8_t x06 = 0,
    uint8_t x07 = 0, uint8_t x08 = 0,
    uint8_t x09 = 0, uint8_t x10 = 0,
    uint8_t x11 = 0, uint8_t x12 = 0,
    uint8_t x13 = 0, uint8_t x14 = 0,
    uint8_t x15 = 0, uint8_t x16 = 0,
    uint8_t x17 = 0, uint8_t x18 = 0,
    uint8_t x19 = 0, uint8_t x20 = 0,
    uint8_t x21 = 0, uint8_t x22 = 0,
    uint8_t x23 = 0, uint8_t x24 = 0,
    uint8_t x25 = 0, uint8_t x26 = 0,
    uint8_t x27 = 0, uint8_t x28 = 0,
    uint8_t x29 = 0, uint8_t x30 = 0,
    uint8_t x31 = 0, uint8_t x32 = 0,
    uint8_t x33 = 0, uint8_t x34 = 0,
    uint8_t x35 = 0, uint8_t x36 = 0,
    uint8_t x37 = 0, uint8_t x38 = 0,
    uint8_t x39 = 0, uint8_t x40 = 0,
    uint8_t x41 = 0, uint8_t x42 = 0,
    uint8_t x43 = 0, uint8_t x44 = 0,
    uint8_t x45 = 0, uint8_t x46 = 0,
    uint8_t x47 = 0, uint8_t x48 = 0,
    uint8_t x49 = 0, uint8_t x50 = 0,
    uint8_t x51 = 0, uint8_t x52 = 0,
    uint8_t x53 = 0, uint8_t x54 = 0,
    uint8_t x55 = 0, uint8_t x56 = 0,
    uint8_t x57 = 0, uint8_t x58 = 0,
    uint8_t x59 = 0, uint8_t x60 = 0,
    uint8_t x61 = 0, uint8_t x62 = 0,
    uint8_t x63 = 0, uint8_t x64 = 0) NOEXCEPT
{
    return _mm512_set_epi8(
        x64, x63, x62, x61, x60, x59, x58, x57,
        x56, x55, x54, x53, x52, x51, x50, x49,
        x48, x47, x46, x45, x44, x43, x42, x41,
        x40, x39, x38, x37, x36, x35, x34, x33,
        x32, x31, x30, x29, x28, x27, x26, x25,
        x24, x23, x22, x21, x20, x19, x18, x17,
        x16, x15, x14, x13, x12, x11, x10, x09,
        x08, x07, x06, x05, x04, x03, x02, x01);
}

#else

// Symbol is defined but not usable.
struct xint512_t{};

#endif // HAVE_AVX512

} // namespace system
} // namespace libbitcoin

#endif

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

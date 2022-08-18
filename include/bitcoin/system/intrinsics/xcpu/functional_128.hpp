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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_FUNCTIONAL_128_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_FUNCTIONAL_128_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/xcpu/defines.hpp>

// shl_/shr_ are undefined for 8 bit.
// All others are at most SSE4.1.
// Apart from extract and byteswap, all are at most SSE2.

namespace libbitcoin {
namespace system {

#if defined(HAVE_SSE4)

using xint128_t = __m128i;

namespace f {

/// bitwise primitives
/// ---------------------------------------------------------------------------

// SSE2
template <typename Word, if_same<Word, xint128_t> = true>
INLINE Word and_(Word a, Word b) NOEXCEPT
{
    return mm_and_si128(a, b);
}

// SSE2
template <typename Word, if_same<Word, xint128_t> = true>
INLINE Word or_(Word a, Word b) NOEXCEPT
{
    return mm_or_si128(a, b);
}

// SSE2
template <typename Word, if_same<Word, xint128_t> = true>
INLINE Word xor_(Word a, Word b) NOEXCEPT
{
    return mm_xor_si128(a, b);
}

// SSE2
template <typename Word, if_same<Word, xint128_t> = true>
INLINE Word not_(Word a) NOEXCEPT
{
    return xor_(a, mm_set1_epi64x(-1));
}

/// vector primitives
/// ---------------------------------------------------------------------------

// SSE2
template <auto B, auto S, typename Word, if_same<Word, xint128_t> = true>
INLINE Word shr(Word a) NOEXCEPT
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
template <auto B, auto S, typename Word, if_same<Word, xint128_t> = true>
INLINE Word shl(Word a) NOEXCEPT
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

template <auto B, auto S, typename Word, if_same<Word, xint128_t> = true>
INLINE Word ror(Word a) NOEXCEPT
{
    return or_(shr<B, S>(a), shl<S - B, S>(a));
}

template <auto B, auto S, typename Word, if_same<Word, xint128_t> = true>
INLINE Word rol(Word a) NOEXCEPT
{
    return or_(shl<B, S>(a), shr<S - B, S>(a));
}

// SSE2
template <auto S, typename Word, if_same<Word, xint128_t> = true>
INLINE Word add(Word a, Word b) NOEXCEPT
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
template <auto K, auto S, typename Word, if_same<Word, xint128_t> = true>
INLINE Word addc(Word a) NOEXCEPT
{
    // set1 broadcast integer to all elements.
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

/// get/set
/// ---------------------------------------------------------------------------

// Lane zero is lowest order word.
template <typename Word, auto Lane>
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
template <typename Word, if_same<Word, xint128_t> = true>
INLINE xint128_t set(uint64_t x01 = 0, uint64_t x02 = 0) NOEXCEPT
{
    // Low order word to the right.
    return mm_set_epi64x(x02, x01);
}

// SSE2
template <typename Word, if_same<Word, xint128_t> = true>
INLINE xint128_t set(
    uint32_t x01 = 0, uint32_t x02 = 0,
    uint32_t x03 = 0, uint32_t x04 = 0) NOEXCEPT
{
    return mm_set_epi32(x04, x03, x02, x01);
}

// SSE2
template <typename Word, if_same<Word, xint128_t> = true>
INLINE xint128_t set(
    uint16_t x01 = 0, uint16_t x02 = 0,
    uint16_t x03 = 0, uint16_t x04 = 0,
    uint16_t x05 = 0, uint16_t x06 = 0,
    uint16_t x07 = 0, uint16_t x08 = 0) NOEXCEPT
{
    return mm_set_epi16(
        x08, x07, x06, x05, x04, x03, x02, x01);
}

// SSE2
template <typename Word, if_same<Word, xint128_t> = true>
INLINE xint128_t set(
    uint8_t x01 = 0, uint8_t x02 = 0,
    uint8_t x03 = 0, uint8_t x04 = 0,
    uint8_t x05 = 0, uint8_t x06 = 0,
    uint8_t x07 = 0, uint8_t x08 = 0,
    uint8_t x09 = 0, uint8_t x10 = 0,
    uint8_t x11 = 0, uint8_t x12 = 0,
    uint8_t x13 = 0, uint8_t x14 = 0,
    uint8_t x15 = 0, uint8_t x16 = 0) NOEXCEPT
{
    return mm_set_epi8(
        x16, x15, x14, x13, x12, x11, x10, x09,
        x08, x07, x06, x05, x04, x03, x02, x01);
}

/// pack/unpack
/// ---------------------------------------------------------------------------

// TODO: auto pack<Word>(const uint8_t*).
INLINE auto unpack(xint128_t a) NOEXCEPT
{
    std_array<uint8_t, sizeof(xint128_t)> bytes{};
    mm_storeu_si128(pointer_cast<xint128_t>(&bytes.front()), a);
    return bytes;
}

/// endianness
/// ---------------------------------------------------------------------------

// SSSE3
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
INLINE xint128_t byteswap(xint128_t a) NOEXCEPT
{
    static const auto mask = set<xint128_t>(
        0x08090a0b0c0d0e0f_u64, 0x0001020304050607_u64);

    return mm_shuffle_epi8(a, mask);
}
BC_POP_WARNING()

#else

// Symbol is defined but not usable.
struct xint128_t : xmock_t {};

#endif // HAVE_SSE4

} // namespace system
} // namespace libbitcoin

#endif

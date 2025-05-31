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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_NEON_128_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_NEON_128_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/types.hpp>
#include <bitcoin/system/intrinsics/neon/neon.hpp>

namespace libbitcoin {
namespace system {

#if defined(HAVE_NEON)

namespace f {

/// bitwise primitives
/// ---------------------------------------------------------------------------
/// 32 bit functions are applied because xint128_t is typed as uint32x4_t.

INLINE xint128_t and_(xint128_t a, xint128_t b) NOEXCEPT
{
    return vandq_u32(a, b);
}

INLINE xint128_t or_(xint128_t a, xint128_t b) NOEXCEPT
{
    return vorrq_u32(a, b);
}

INLINE xint128_t xor_(xint128_t a, xint128_t b) NOEXCEPT
{
    return veorq_u32(a, b);
}

INLINE xint128_t not_(xint128_t a) NOEXCEPT
{
    return xor_(a, vdupq_n_u32(-1));
}

/// vector primitives
/// ---------------------------------------------------------------------------

template <auto B, auto S>
INLINE xint128_t shr(xint128_t a) NOEXCEPT
{
    if constexpr (S == bits<uint8_t>)
        return (xint128_t)vshrq_n_u8((uint8x16_t)a, B);
    if constexpr (S == bits<uint16_t>)
        return (xint128_t)vshrq_n_u16((uint16x8_t)a, B);
    if constexpr (S == bits<uint32_t>)
        return (xint128_t)vshrq_n_u32((uint32x4_t)a, B);
    if constexpr (S == bits<uint64_t>)
        return (xint128_t)vshrq_n_u64((uint64x2_t)a, B);
}

template <auto B, auto S>
INLINE xint128_t shl(xint128_t a) NOEXCEPT
{
    if constexpr (S == bits<uint8_t>)
        return (xint128_t)vshlq_n_u8((uint8x16_t)a, B);
    if constexpr (S == bits<uint16_t>)
        return (xint128_t)vshlq_n_u16((uint16x8_t)a, B);
    if constexpr (S == bits<uint32_t>)
        return (xint128_t)vshlq_n_u32((uint32x4_t)a, B);
    if constexpr (S == bits<uint64_t>)
        return (xint128_t)vshlq_n_u64((uint64x2_t)a, B);
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

template <auto S>
INLINE xint128_t add(xint128_t a, xint128_t b) NOEXCEPT
{
    if constexpr (S == bits<uint8_t>)
        return (xint128_t)vaddq_u8((uint8x16_t)a, (uint8x16_t)b);
    if constexpr (S == bits<uint16_t>)
        return (xint128_t)vaddq_u16((uint16x8_t)a, (uint16x8_t)b);
    if constexpr (S == bits<uint32_t>)
        return (xint128_t)vaddq_u32((uint32x4_t)a, (uint32x4_t)b);
    if constexpr (S == bits<uint64_t>)
        return (xint128_t)vaddq_u64((uint64x2_t)a, (uint64x2_t)b);
}

template <auto K, auto S>
INLINE xint128_t addc(xint128_t a) NOEXCEPT
{
    if constexpr (S == bits<uint8_t>)
        return add<S>(a, (xint128_t)vdupq_n_u8((uint8_t)K));
    if constexpr (S == bits<uint16_t>)
        return add<S>(a, (xint128_t)vdupq_n_u16((uint16_t)K));
    if constexpr (S == bits<uint32_t>)
        return add<S>(a, (xint128_t)vdupq_n_u32((uint32_t)K));
    if constexpr (S == bits<uint64_t>)
        return add<S>(a, (xint128_t)vdupq_n_u64((uint64_t)K));
}

} // namespace f

/// add/broadcast/get/set
/// ---------------------------------------------------------------------------

template <typename Word, if_integral_integer<Word> = true>
INLINE xint128_t add(xint128_t a, xint128_t b) NOEXCEPT
{
    if constexpr (is_same_type<Word, uint8_t>)
        return (xint128_t)vaddq_u8((uint8x16_t)a, (uint8x16_t)b);
    if constexpr (is_same_type<Word, uint16_t>)
        return (xint128_t)vaddq_u16((uint16x8_t)a, (uint16x8_t)b);
    if constexpr (is_same_type<Word, uint32_t>)
        return (xint128_t)vaddq_u32((uint32x4_t)a, (uint32x4_t)b);
    if constexpr (is_same_type<Word, uint64_t>)
        return (xint128_t)vaddq_u64((uint64x2_t)a, (uint64x2_t)b);
}

template <typename xWord, typename Word, if_integral_integer<Word> = true,
    if_same<xWord, xint128_t> = true>
INLINE xint128_t broadcast(Word a) NOEXCEPT
{
    if constexpr (is_same_type<Word, uint8_t>)
        return (xint128_t)vdupq_n_u8((uint8_t)a);
    if constexpr (is_same_type<Word, uint16_t>)
        return (xint128_t)vdupq_n_u16((uint16_t)a);
    if constexpr (is_same_type<Word, uint32_t>)
        return (xint128_t)vdupq_n_u32((uint32_t)a);
    if constexpr (is_same_type<Word, uint64_t>)
        return (xint128_t)vdupq_n_u64((uint64_t)a);
}

// Lane zero is lowest order word.
template <typename Word, auto Lane, if_integral_integer<Word> = true>
INLINE Word get(xint128_t a) NOEXCEPT
{
    if constexpr (is_same_type<Word, uint8_t>)
        return (Word)vgetq_lane_u8((uint8x16_t)a, Lane);
    if constexpr (is_same_type<Word, uint16_t>)
        return (Word)vgetq_lane_u16((uint16x8_t)a, Lane);
    if constexpr (is_same_type<Word, uint32_t>)
        return (Word)vgetq_lane_u32((uint32x4_t)a, Lane);
    if constexpr (is_same_type<Word, uint64_t>)
        return (Word)vgetq_lane_u64((uint64x2_t)a, Lane);
}

// Low order word to the left.
template <typename xWord, if_same<xWord, xint128_t> = true>
INLINE xint128_t set(uint64_t x01, uint64_t x02) NOEXCEPT
{
    return (xint128_t)vcombine_u64(vcreate_u64(x01), vcreate_u64(x02));
}

template <typename xWord, if_same<xWord, xint128_t> = true>
INLINE xint128_t set(
    uint32_t x01, uint32_t x02, uint32_t x03, uint32_t x04) NOEXCEPT
{
    const uint32_t a[]{ x01, x02, x03, x04 };
    return (xint128_t)vld1q_u32(a);
}

template <typename xWord, if_same<xWord, xint128_t> = true>
INLINE xint128_t set(
    uint16_t x01, uint16_t x02, uint16_t x03, uint16_t x04,
    uint16_t x05, uint16_t x06, uint16_t x07, uint16_t x08) NOEXCEPT
{
    auto low  = vcreate_u16(
        (uint64_t)x02 << 16 | (uint64_t)x01 << 0);
    low  = vset_lane_u16((uint16_t)x03, low, 2);
    low  = vset_lane_u16((uint16_t)x04, low, 3);
    auto high = vcreate_u16(
        (uint64_t)x06 << 16 | (uint64_t)x05 << 0);
    high = vset_lane_u16((uint16_t)x07, high, 2);
    high = vset_lane_u16((uint16_t)x08, high, 3);
    return (xint128_t)vcombine_u16(low, high);
}


template <typename xWord, if_same<xWord, xint128_t> = true>
INLINE xint128_t set(
    uint8_t x01, uint8_t x02, uint8_t x03, uint8_t x04,
    uint8_t x05, uint8_t x06, uint8_t x07, uint8_t x08,
    uint8_t x09, uint8_t x10, uint8_t x11, uint8_t x12,
    uint8_t x13, uint8_t x14, uint8_t x15, uint8_t x16) NOEXCEPT
{
    auto low = vcreate_u8(
        (uint64_t)x04 << 24 | (uint64_t)x03 << 16 |
        (uint64_t)x02 <<  8 | (uint64_t)x01 << 0);
    low = vset_lane_u8((uint8_t)x05, low, 4);
    low = vset_lane_u8((uint8_t)x06, low, 5);
    low = vset_lane_u8((uint8_t)x07, low, 6);
    low = vset_lane_u8((uint8_t)x08, low, 7);
    auto high = vcreate_u8(
        (uint64_t)x12 << 24 | (uint64_t)x11 << 16 |
        (uint64_t)x10 <<  8 | (uint64_t)x09 << 0);
    high = vset_lane_u8((uint8_t)x13, high, 4);
    high = vset_lane_u8((uint8_t)x14, high, 5);
    high = vset_lane_u8((uint8_t)x15, high, 6);
    high = vset_lane_u8((uint8_t)x16, high, 7);
    return (xint128_t)vcombine_u8(low, high);
}

/// endianness
/// ---------------------------------------------------------------------------

template <typename Word, if_same<Word, uint8_t> = true>
INLINE xint128_t byteswap(xint128_t a) NOEXCEPT
{
    return (xint128_t)a;
}

template <typename Word, if_same<Word, uint16_t> = true>
INLINE xint128_t byteswap(xint128_t a) NOEXCEPT
{
    return (xint128_t)vrev16q_u8((uint8x16_t)a);
}

template <typename Word, if_same<Word, uint32_t> = true>
INLINE xint128_t byteswap(xint128_t a) NOEXCEPT
{
    return (xint128_t)vrev32q_u8((uint8x16_t)a);
}

template <typename Word, if_same<Word, uint64_t> = true>
INLINE xint128_t byteswap(xint128_t a) NOEXCEPT
{
    return (xint128_t)vrev64q_u8((uint8x16_t)a);
}

/// load/store
/// ---------------------------------------------------------------------------

INLINE xint128_t load(const xint128_t& bytes) NOEXCEPT
{
    return vld1q_u32((uint32_t*)&bytes);
}

INLINE void store(xint128_t& bytes, xint128_t a) NOEXCEPT
{
    vst1q_u32((uint32_t*)&bytes, a);
}

INLINE xint128_t load_aligned(const xint128_t& bytes) NOEXCEPT
{
    return vld1q_u32((uint32_t*)&bytes);
}

INLINE void store_aligned(xint128_t& bytes, xint128_t a) NOEXCEPT
{
    vst1q_u32((uint32_t*)&bytes, a);
}

#endif // HAVE_NEON

} // namespace system
} // namespace libbitcoin

#endif

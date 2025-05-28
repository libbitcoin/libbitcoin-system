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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_PLATFORMS_NEON_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_PLATFORMS_NEON_HPP

#include <bitcoin/system/define.hpp>

#if defined(HAVE_NEON)

#if defined(HAVE_ARM32)
#if defined(HAVE_128)
inline uint64_t vgetq_lane_s64(auto, auto) NOEXCEPT
{
    // This presently precludes sha512 vectorization on x32 builds.
    std::abort();
}
#endif
#endif // HAVE_X32

///////////////////////////////////////////////////////////////////////////////
// ARM is disabled until this is correct.
///////////////////////////////////////////////////////////////////////////////
#if defined(HAVE_128)
    #define mm_and_si128(a, b)              vandq_s32((int32x4_t)a, (int32x4_t)b)
    #define mm_or_si128(a, b)               vorrq_s32((int32x4_t)a, (int32x4_t)b)
    #define mm_xor_si128(a, b)              veorq_s32((int32x4_t)a, (int32x4_t)b)
    #define mm_srli_epi8(a, B)              vshrq_n_s8((int8x16_t)a, B) // defined
    #define mm_srli_epi16(a, B)             vshrq_n_s16((int16x8_t)a, B)
    #define mm_srli_epi32(a, B)             vshrq_n_s32((int32x4_t)a, B)
    #define mm_srli_epi64(a, B)             vshrq_n_s64((int64x2_t)a, B)
    #define mm_slli_epi8(a, B)              vshlq_n_s8((int8x16_t)a, B) // defined
    #define mm_slli_epi16(a, B)             vshlq_n_s16((int16x8_t)a, B)
    #define mm_slli_epi32(a, B)             vshlq_n_s32((int32x4_t)a, B)
    #define mm_slli_epi64(a, B)             vshlq_n_s64((int64x2_t)a, B)
    #define mm_add_epi8(a, b)               vaddq_s8((int8x16_t)a, (int8x16_t)b)
    #define mm_add_epi16(a, b)              vaddq_s16((int16x8_t)a, (int16x8_t)b)
    #define mm_add_epi32(a, b)              vaddq_s32((int32x4_t)a, (int32x4_t)b) // also sha
    #define mm_add_epi64(a, b)              vaddq_s64((int64x2_t)a, (int64x2_t)b)
    #define mm_extract_epi8(a, Lane)        vgetq_lane_s8((int8x16_t)a, Lane)
    #define mm_extract_epi16(a, Lane)       vgetq_lane_s16((int16x8_t)a, Lane)
    #define mm_extract_epi32(a, Lane)       vgetq_lane_s32((int32x4_t)a, Lane)
    #define mm_extract_epi64(a, Lane)       vgetq_lane_s64((int64x2_t)a, Lane) // undefined for X32
    #define mm_shuffle_epi8(a, mask)        vqtbl1q_s8((int8x16_t)a, (uint8x16_t)mask)
    #define mm_load_si128(a)                vld1q_s32((const int32_t*)a)
    #define mm_loadu_si128(a)               vld1q_s32((const int32_t*)a)
    #define mm_store_si128(memory, a)       vst1q_s32((int32_t*)memory, (int32x4_t)a)
    #define mm_storeu_si128(memory, a)      vst1q_s32((int32_t*)memory, (int32x4_t)a)
    #define mm_set1_epi8(K)                 vdupq_n_s8((int8_t)K)
    #define mm_set1_epi16(K)                vdupq_n_s16((int16_t)K)
    #define mm_set1_epi32(K)                vdupq_n_s32((int32_t)K)
    #define mm_set1_epi64x(K)               vdupq_n_s64((int64_t)K)
    #define mm_set_epi64x(x02, x01) \
            vcombine_s64(vcreate_s64(((int64_t)x01 << 0)), \
                         vcreate_s64(((int64_t)x02 << 0)))
    #define mm_set_epi32(x04, x03, x02, x01) \
            vcombine_s32(vcreate_s32(((int64_t)x02 << 32) | ((uint32_t)x01 << 0)), \
                         vcreate_s32(((int64_t)x04 << 32) | ((uint32_t)x03 << 0)))
    #define mm_set_epi16(x08, x07, x06, x05, x04, x03, x02, x01) \
            vcombine_s16(vcreate_s16(((int64_t)x04 << 48) | ((int64_t)x03 << 32) | ((int64_t)x02 << 16) | ((uint16_t)x01 << 0)), \
                         vcreate_s16(((int64_t)x08 << 48) | ((int64_t)x07 << 32) | ((int64_t)x06 << 16) | ((uint16_t)x05 << 0)))
    #define mm_set_epi8(x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01) \
            vcombine_s8( vcreate_s8( ((int64_t)x08 << 56) | ((int64_t)x07 << 48) | ((int64_t)x06 << 40) | ((int64_t)x05 << 32) | ((int64_t)x04 << 24) | ((int64_t)x03 << 16) | ((int64_t)x02 <<  8) | ((uint8_t)x01 << 0)),  \
                         vcreate_s8( ((int64_t)x16 << 56) | ((int64_t)x15 << 48) | ((int64_t)x14 << 40) | ((int64_t)x13 << 32) | ((int64_t)x12 << 24) | ((int64_t)x11 << 16) | ((int64_t)x10 <<  8) | ((uint8_t)x09 << 0)))
#endif // HAVE_128

///////////////////////////////////////////////////////////////////////////////
// ARM is disabled until this is correct.
///////////////////////////////////////////////////////////////////////////////
#if defined(HAVE_SHA)
    // sha1
    #define mm_sha1msg1_epu32(a, b)         veorq_u32((uint32x4_t)a, (uint32x4_t)b)
    #define mm_sha1msg2_epu32(a, b)         vsha1su1q_u32((uint32x4_t)a, (uint32x4_t)b)
    #define mm_sha1nexte_epu32(a, b)        vaddq_u32(vdupq_n_u32(vsha1h_u32(vgetq_lane_u32((uint32x4_t)a, 0))), \
                                                      vdupq_n_u32(vgetq_lane_u32((uint32x4_t)b, 0)))
    #define mm_sha1rnds4_epu32(a, b, f)     (f == 0 ? vsha1cq_u32((uint32x4_t)a, vgetq_lane_u32((uint32x4_t)b, 0), (uint32x4_t)a) : \
                                             f == 1 ? vsha1pq_u32((uint32x4_t)a, vgetq_lane_u32((uint32x4_t)b, 0), (uint32x4_t)a) : \
                                             f == 2 ? vsha1mq_u32((uint32x4_t)a, vgetq_lane_u32((uint32x4_t)b, 0), (uint32x4_t)a) : \
                                                      vsha1pq_u32((uint32x4_t)a, vgetq_lane_u32((uint32x4_t)b, 0), (uint32x4_t)a))


    // sha256
    #define mm_sha256msg1_epu32(a, b)       vsha256su0q_u32((uint32x4_t)a, (uint32x4_t)b)
    #define mm_sha256rnds2_epu32(a, b, k)   vsha256hq_u32((uint32x4_t)a, (uint32x4_t)b, (uint32x4_t)k)
    #define mm_sha256msg2_epu32(a, b)   ({ \
        uint32x4_t w = (uint32x4_t)a; \
        uint32x4_t w_m16 = (uint32x4_t)b; \
        uint32x4_t w_m15 = vextq_u32((uint32x4_t)b, (uint32x4_t)a, 3); \
        uint32x4_t w_m7 = vextq_u32((uint32x4_t)a, vdupq_n_u32(0), 3); \
        uint32x4_t w_m2 = w; \
        uint32x4_t s0 = veorq_u32(veorq_u32(vshrq_n_u32(w_m15, 7), vshrq_n_u32(w_m15, 18)), vshrq_n_u32(w_m15, 3)); \
        uint32x4_t s1 = veorq_u32(veorq_u32(vshrq_n_u32(w_m2, 17), vshrq_n_u32(w_m2, 19)), vshrq_n_u32(w_m2, 10)); \
        vaddq_u32(vaddq_u32(vaddq_u32(w_m16, s0), w_m7), s1); \
    })

    // supporting
    #define mm_alignr_epi8(a, b, c)         vextq_u8((uint8x16_t)b, (uint8x16_t)a, c)
    #define mm_shuffle_epi32(a, mask)       vqtbl1q_s8((int8x16_t)a, (uint8x16_t)vcombine_u8(vcreate_u8((uint64_t)mask), vcreate_u8((uint64_t)mask)))
    #define mm_blend_epi16(a, b, mask)      vbslq_u16(vcreate_u16( \
                                                ((mask & 0x01 ? 0xffff_u64 : 0_u64) <<  0) | \
                                                ((mask & 0x02 ? 0xffff_u64 : 0_u64) << 16) | \
                                                ((mask & 0x04 ? 0xffff_u64 : 0_u64) << 32) | \
                                                ((mask & 0x08 ? 0xffff_u64 : 0_u64) << 48)), \
                                                (uint16x8_t)b, (uint16x8_t)a)

    // unused argument suppression
    #define SHA_ONLY(a) a
#endif // HAVE_SHA

#endif // HAVE_NEON

#endif

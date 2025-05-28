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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_PLATFORMS_NONE_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_PLATFORMS_NONE_HPP

#include <bitcoin/system/define.hpp>

#if !defined(HAVE_128)
    #define mm_and_si128(a, b)  (a)
    #define mm_or_si128(a, b)   (a)
    #define mm_xor_si128(a, b)  (a)
    ////#define mm_srli_epi8(a, B) (a)
    #define mm_srli_epi16(a, B) (a)
    #define mm_srli_epi32(a, B) (a)
    #define mm_srli_epi64(a, B) (a)
    ////#define mm_slli_epi8(a, B) (a)
    #define mm_slli_epi16(a, B) (a)
    #define mm_slli_epi32(a, B) (a)
    #define mm_slli_epi64(a, B) (a)
    #define mm_add_epi8(a, b)   (a)
    #define mm_add_epi16(a, b)  (a)
    #define mm_add_epi32(a, b)  (a) // also sha
    #define mm_add_epi64(a, b)  (a)
    #define mm_extract_epi8(a, Lane)    {}
    #define mm_extract_epi16(a, Lane)   {}
    #define mm_extract_epi32(a, Lane)   {}
    #define mm_extract_epi64(a, Lane)   {}
    #define mm_shuffle_epi8(a, mask)    (a)
    #define mm_load_si128(a)            (a)
    #define mm_loadu_si128(a)           (a)
    #define mm_store_si128(memory, a)
    #define mm_storeu_si128(memory, a)
    #define mm_set1_epi8(K)
    #define mm_set1_epi16(K)
    #define mm_set1_epi32(K)
    #define mm_set1_epi64x(K)
    #define mm_set_epi64x(x02, x01)
    #define mm_set_epi32(x04, x03, x02, x01)
    #define mm_set_epi16(x08, x07, x06, x05, x04, x03, x02, x01)
    #define mm_set_epi8(x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
#endif // !HAVE_128

#if !defined(HAVE_256)
    #define mm256_and_si256(a, b)   (a)
    #define mm256_or_si256(a, b)    (a)
    #define mm256_xor_si256(a, b)   (a)
    ////#define mm256_srli_epi8(a, B)   (a)
    #define mm256_srli_epi16(a, B)  (a)
    #define mm256_srli_epi32(a, B)  (a)
    #define mm256_srli_epi64(a, B)  (a)
    ////#define mm256_slli_epi8(a, B)   (a)
    #define mm256_slli_epi16(a, B)  (a)
    #define mm256_slli_epi32(a, B)  (a)
    #define mm256_slli_epi64(a, B)  (a)
    #define mm256_add_epi8(a, b)    (a)
    #define mm256_add_epi16(a, b)   (a)
    #define mm256_add_epi32(a, b)   (a)
    #define mm256_add_epi64(a, b)   (a)
    #define mm256_extract_epi8(a, Lane)     {}
    #define mm256_extract_epi16(a, Lane)    {}
    #define mm256_extract_epi32(a, Lane)    {}
    #define mm256_extract_epi64(a, Lane)    {}
    #define mm256_shuffle_epi8(a, mask)     (a)
    #define mm256_load_si256(a)             (a)
    #define mm256_loadu_si256(a)            (a)
    #define mm256_store_si256(memory, a)    {}
    #define mm256_storeu_si256(memory, a)   {}
    #define mm256_set1_epi8(K)
    #define mm256_set1_epi16(K)
    #define mm256_set1_epi32(K)
    #define mm256_set1_epi64x(K)
    #define mm256_set_epi64x(x04, x03, x02, x01)
    #define mm256_set_epi32(x08, x07, x06, x05, x04, x03, x02, x01)
    #define mm256_set_epi16(x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
    #define mm256_set_epi8(x32, x31, x30, x29, x28, x27, x26, x25, x24, x23, x22, x21, x20, x19, x18, x17, x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
#endif // !HAVE_256

#if !defined(HAVE_512)
    #define mm512_and_si512(a, b)   (a)
    #define mm512_or_si512(a, b)    (a)
    #define mm512_xor_si512(a, b)   (a)
    ////#define mm512_srli_epi8(a, B)  (a)
    #define mm512_srli_epi16(a, B)  (a)
    #define mm512_srli_epi32(a, B)  (a)
    #define mm512_srli_epi64(a, B)  (a)
    ////#define mm512_slli_epi8(a, B)  (a)
    #define mm512_slli_epi16(a, B)  (a)
    #define mm512_slli_epi32(a, B)  (a)
    #define mm512_slli_epi64(a, B)  (a)
    #define mm512_add_epi8(a, b)    (a)
    #define mm512_add_epi16(a, b)   (a)
    #define mm512_add_epi32(a, b)   (a)
    #define mm512_add_epi64(a, b)   (a)
    ////#define mm512_extract_epi8(a, Lane)     {}
    ////#define mm512_extract_epi16(a, Lane)    {}
    #define mm512_extract_epi32(a, Lane)    {}
    #define mm512_extract_epi64(a, Lane)    {}
    #define mm512_shuffle_epi8(a, mask)     (a)
    #define mm512_load_si512(a)             (a)
    #define mm512_loadu_si512(a)            (a)
    #define mm512_store_si512(memory, a)
    #define mm512_storeu_si512(memory, a)
    #define mm512_set1_epi8(K)
    #define mm512_set1_epi16(K)
    #define mm512_set1_epi32(K)
    #define mm512_set1_epi64(K)
    #define mm512_set_epi64(x08, x07, x06, x05, x04, x03, x02, x01)
    #define mm512_set_epi32(x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
    #define mm512_set_epi16(x32, x31, x30, x29, x28, x27, x26, x25, x24, x23, x22, x21, x20, x19, x18, x17, x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
    #define mm512_set_epi8(x64, x63, x62, x61, x60, x59, x58, x57, x56, x55, x54, x53, x52, x51, x50, x49, x48, x47, x46, x45, x44, x43, x42, x41, x40, x39, x38, x37, x36, x35, x34, x33, x32, x31, x30, x29, x28, x27, x26, x25, x24, x23, x22, x21, x20, x19, x18, x17, x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
#endif // !HAVE_512

#if !defined(HAVE_SHA)
    #define mm_sha1msg1_epu32(a, b)         {}
    #define mm_sha1msg2_epu32(a, b)         {}
    #define mm_sha1rnds4_epu32(a, b, f)     {}
    #define mm_sha1nexte_epu32(a, b)        {}
    #define mm_sha256msg1_epu32(a, b)       (b)
    #define mm_sha256msg2_epu32(a, b)       (b)
    #define mm_sha256rnds2_epu32(a, b, k)   (k)
    #define mm_alignr_epi8(a, b, c)         (a)
    #define mm_shuffle_epi32(a, mask)       (a)
    #define mm_blend_epi16(a, b, mask)      (a)
    #define SHA_ONLY(a)
#endif // !HAVE_SHA

#endif

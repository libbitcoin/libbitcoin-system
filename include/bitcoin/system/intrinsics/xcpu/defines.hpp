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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_DEFINES_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_DEFINES_HPP

#include <bitcoin/system/define.hpp>

/// Base type for mock extended integer types, used for differentiation.
struct xmock_t {};

#if defined(HAVE_XCPU)
    #include <immintrin.h>
    #if defined(HAVE_X64)
        #include <nmmintrin.h>
    #endif
    #if defined(HAVE_MSC)
        #include <intrin.h>
    #endif
#endif

// TODO: ARM is unverified.
#if defined(HAVE_ARM)
    #include <arm_acle.h>
    #if defined(HAVE_NEON)
        #include <arm_neon.h>
    #endif
#endif

// ****************************************************************************
// The xint extended integer symbols are always defined for XCPU builds, but
// these intrinsics are dependent upon the underlying platform and will cause
// a build failure on CLANG/GCC if not configured into the build (after being
// detected on the build platform). On all platforms these also require runtime
// detection. This creates three layers of conditionality, exploding complexity.
// These macros suppress the build failure, allowing code to operation over the
// necessary runtime detection, with runtime detection layered over build
// configuration symbols. Consequently there is no need for preprocessor
// statements within application code, just a runtime test for availability.
// These symbols will be defined where availability is possible (compiled).
// with<xint>() is defined for constexpr conditionality and have<xint>() is
// defined for runtime conditionality. The latter is false if is the former.
// ****************************************************************************

// These are not defined for 32 bit bit builds.
// This presently precludes sha512 vectorization on x32 builds.
#if defined(HAVE_X32)
#if defined(HAVE_SSE4)
inline auto _mm_extract_epi64(auto a, auto) NOEXCEPT
{
    return a;
}
#endif
#if defined(HAVE_AVX2)
inline auto _mm256_extract_epi64(auto a, auto) NOEXCEPT
{
    return a;
}
#endif
#if defined(HAVE_AVX512)
// required for _mm512_extract_epi64
inline auto _mm_cvtsi128_si64(auto a) NOEXCEPT
{
    return a;
}
#endif
#endif

// These are not defined.
// 8/16 bit are not implemented here due to AVX512_VBMI2 requirement.
// github.com/vectorclass/version2/blob/master/vectori512.h
#if defined(HAVE_AVX512)
////inline auto _mm512_extract_epi8(auto a, auto Lane) NOEXCEPT
////{
////    // AVX512_VBMI2/AVX512F/SSE2
////    const auto t = _mm512_maskz_compress_epi8(__mmask16(1u << Lane), a);
////    return _mm_cvtsi128_si8(_mm512_castsi512_si128(t));
////}
////inline auto _mm512_extract_epi16(auto a, auto Lane) NOEXCEPT
////{
////    // AVX512_VBMI2/AVX512F/SSE2
////    const auto t = _mm512_maskz_compress_epi16(__mmask8(1u << Lane), a);
////    return _mm_cvtsi128_si16(_mm512_castsi512_si128(t));
////}
inline auto _mm512_extract_epi32(auto a, auto Lane) NOEXCEPT
{
    // AVX512F/SSE2/AVX512F
    const auto t = _mm512_maskz_compress_epi32(__mmask16(1u << Lane), a);
    return _mm_cvtsi128_si32(_mm512_castsi512_si128(t));
}
inline auto _mm512_extract_epi64(auto a, auto Lane) NOEXCEPT
{
    // AVX512F/SSE2/AVX512F
    // cvt undefined for 32 bit (see above).
    const auto t = _mm512_maskz_compress_epi64(__mmask8(1u << Lane), a);
    return _mm_cvtsi128_si64(_mm512_castsi512_si128(t));
}
#endif

#if !defined(HAVE_SSE4)
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
    #define mm_add_epi32(a, b)  (a)
    #define mm_add_epi64(a, b)  (a)
    #define mm_extract_epi8(a, Lane)    {}
    #define mm_extract_epi16(a, Lane)   {}
    #define mm_extract_epi32(a, Lane)   {}
    #define mm_extract_epi64(a, Lane)   {}
    #define mm_shuffle_epi8(a, mask)    (a)
    #define mm_storeu_si128(a, b)
    #define mm_set1_epi8(K)
    #define mm_set1_epi16(K)
    #define mm_set1_epi32(K)
    #define mm_set1_epi64x(K)
    #define mm_set_epi64x(x02, x01)
    #define mm_set_epi32(x04, x03, x02, x01)
    #define mm_set_epi16(x08, x07, x06, x05, x04, x03, x02, x01)
    #define mm_set_epi8(x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
#else
    #define mm_and_si128(a, b)          _mm_and_si128(a, b)
    #define mm_or_si128(a, b)           _mm_or_si128(a, b)
    #define mm_xor_si128(a, b)          _mm_xor_si128(a, b)
    ////#define mm_srli_epi8(a, B)      _mm_srli_epi8(a, B) // undefined
    #define mm_srli_epi16(a, B)         _mm_srli_epi16(a, B)
    #define mm_srli_epi32(a, B)         _mm_srli_epi32(a, B)
    #define mm_srli_epi64(a, B)         _mm_srli_epi64(a, B)
    ////#define mm_slli_epi8(a, B)      _mm_slli_epi8(a, B) // undefined
    #define mm_slli_epi16(a, B)         _mm_slli_epi16(a, B)
    #define mm_slli_epi32(a, B)         _mm_slli_epi32(a, B)
    #define mm_slli_epi64(a, B)         _mm_slli_epi64(a, B)
    #define mm_add_epi8(a, b)           _mm_add_epi8(a, b)
    #define mm_add_epi16(a, b)          _mm_add_epi16(a, b)
    #define mm_add_epi32(a, b)          _mm_add_epi32(a, b)
    #define mm_add_epi64(a, b)          _mm_add_epi64(a, b)
    #define mm_extract_epi8(a, Lane)    _mm_extract_epi8(a, Lane)
    #define mm_extract_epi16(a, Lane)   _mm_extract_epi16(a, Lane)
    #define mm_extract_epi32(a, Lane)   _mm_extract_epi32(a, Lane)
    #define mm_extract_epi64(a, Lane)   _mm_extract_epi64(a, Lane) // undefined for X32
    #define mm_shuffle_epi8(a, mask)    _mm_shuffle_epi8(a, mask)
    #define mm_storeu_si128(a, b)       _mm_storeu_si128(a, b)
    #define mm_set1_epi8(K)             _mm_set1_epi8(K)
    #define mm_set1_epi16(K)            _mm_set1_epi16(K)
    #define mm_set1_epi32(K)            _mm_set1_epi32(K)
    #define mm_set1_epi64x(K)           _mm_set1_epi64x(K)
    #define mm_set_epi64x(x02, x01)     _mm_set_epi64x(x02, x01)
    #define mm_set_epi32(x04, x03, x02, x01) \
           _mm_set_epi32(x04, x03, x02, x01)
    #define mm_set_epi16(x08, x07, x06, x05, x04, x03, x02, x01) \
           _mm_set_epi16(x08, x07, x06, x05, x04, x03, x02, x01)
    #define  mm_set_epi8(x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01) \
            _mm_set_epi8(x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
#endif

#if !defined(HAVE_AVX2)
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
    #define mm256_storeu_si256(a, b)
    #define mm256_set1_epi8(K)
    #define mm256_set1_epi16(K)
    #define mm256_set1_epi32(K)
    #define mm256_set1_epi64x(K)
    #define mm256_set_epi64x(x04, x03, x02, x01)
    #define mm256_set_epi32(x08, x07, x06, x05, x04, x03, x02, x01)
    #define mm256_set_epi16(x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
    #define  mm256_set_epi8(x32, x31, x30, x29, x28, x27, x26, x25, x24, x23, x22, x21, x20, x19, x18, x17, x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
#else
    #define mm256_and_si256(a, b)           _mm256_and_si256(a, b)
    #define mm256_or_si256(a, b)            _mm256_or_si256(a, b)
    #define mm256_xor_si256(a, b)           _mm256_xor_si256(a, b)
    ////#define mm256_srli_epi8(a, B)       _mm256_srli_epi8(a, B) // undefined
    #define mm256_srli_epi16(a, B)          _mm256_srli_epi16(a, B)
    #define mm256_srli_epi32(a, B)          _mm256_srli_epi32(a, B)
    #define mm256_srli_epi64(a, B)          _mm256_srli_epi64(a, B)
    ////#define mm256_slli_epi8(a, B)       _mm256_slli_epi8(a, B) // undefined
    #define mm256_slli_epi16(a, B)          _mm256_slli_epi16(a, B)
    #define mm256_slli_epi32(a, B)          _mm256_slli_epi32(a, B)
    #define mm256_slli_epi64(a, B)          _mm256_slli_epi64(a, B)
    #define mm256_add_epi8(a, b)            _mm256_add_epi8(a, b)
    #define mm256_add_epi16(a, b)           _mm256_add_epi16(a, b)
    #define mm256_add_epi32(a, b)           _mm256_add_epi32(a, b)
    #define mm256_add_epi64(a, b)           _mm256_add_epi64(a, b)
    #define mm256_extract_epi8(a, Lane)     _mm256_extract_epi8(a, Lane)
    #define mm256_extract_epi16(a, Lane)    _mm256_extract_epi16(a, Lane)
    #define mm256_extract_epi32(a, Lane)    _mm256_extract_epi32(a, Lane)
    #define mm256_extract_epi64(a, Lane)    _mm256_extract_epi64(a, Lane) // undefined for X32
    #define mm256_shuffle_epi8(a, mask)     _mm256_shuffle_epi8(a, mask)
    #define mm256_storeu_si256(a, b)        _mm256_storeu_si256(a, b)
    #define mm256_set1_epi8(K)              _mm256_set1_epi8(K)
    #define mm256_set1_epi16(K)             _mm256_set1_epi16(K)
    #define mm256_set1_epi32(K)             _mm256_set1_epi32(K)
    #define mm256_set1_epi64x(K)            _mm256_set1_epi64x(K)
    #define mm256_set_epi64x(x04, x03, x02, x01) \
           _mm256_set_epi64x(x04, x03, x02, x01)
    #define mm256_set_epi32(x08, x07, x06, x05, x04, x03, x02, x01) \
           _mm256_set_epi32(x08, x07, x06, x05, x04, x03, x02, x01)
    #define mm256_set_epi16(x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01) \
           _mm256_set_epi16(x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
    #define  mm256_set_epi8(x32, x31, x30, x29, x28, x27, x26, x25, x24, x23, x22, x21, x20, x19, x18, x17, x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01) \
            _mm256_set_epi8(x32, x31, x30, x29, x28, x27, x26, x25, x24, x23, x22, x21, x20, x19, x18, x17, x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
#endif

#if !defined(HAVE_AVX512)
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
    #define mm512_storeu_si512(a, b)
    #define mm512_set1_epi8(K)
    #define mm512_set1_epi16(K)
    #define mm512_set1_epi32(K)
    #define mm512_set1_epi64(K)
    #define mm512_set_epi64(x08, x07, x06, x05, x04, x03, x02, x01)
    #define mm512_set_epi32(x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
    #define mm512_set_epi16(x32, x31, x30, x29, x28, x27, x26, x25, x24, x23, x22, x21, x20, x19, x18, x17, x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
    #define  mm512_set_epi8(x64, x63, x62, x61, x60, x59, x58, x57, x56, x55, x54, x53, x52, x51, x50, x49, x48, x47, x46, x45, x44, x43, x42, x41, x40, x39, x38, x37, x36, x35, x34, x33, x32, x31, x30, x29, x28, x27, x26, x25, x24, x23, x22, x21, x20, x19, x18, x17, x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
#else
    #define mm512_and_si512(a, b)           _mm512_and_si512(a, b)
    #define mm512_or_si512(a, b)            _mm512_or_si512(a, b)
    #define mm512_xor_si512(a, b)           _mm512_xor_si512(a, b)
    ////#define mm512_srli_epi8(a, B)       _mm512_srli_epi8(a, B)  // undefined
    #define mm512_srli_epi16(a, B)          _mm512_srli_epi16(a, B) // AVX512BW
    #define mm512_srli_epi32(a, B)          _mm512_srli_epi32(a, B)
    #define mm512_srli_epi64(a, B)          _mm512_srli_epi64(a, B)
    ////#define mm512_slli_epi8(a, B)       _mm512_slli_epi8(a, B)  // undefined
    #define mm512_slli_epi16(a, B)          _mm512_slli_epi16(a, B) // AVX512BW
    #define mm512_slli_epi32(a, B)          _mm512_slli_epi32(a, B)
    #define mm512_slli_epi64(a, B)          _mm512_slli_epi64(a, B)
    #define mm512_add_epi8(a, b)            _mm512_add_epi8(a, b)   // AVX512BW
    #define mm512_add_epi16(a, b)           _mm512_add_epi16(a, b)  // AVX512BW
    #define mm512_add_epi32(a, b)           _mm512_add_epi32(a, b)
    #define mm512_add_epi64(a, b)           _mm512_add_epi64(a, b)
    ////#define mm512_extract_epi8(a, Lane) _mm512_extract_epi8(a, Lane)  // AVX512_VBMI2
    ////#define mm512_extract_epi16(a, Lane)_mm512_extract_epi16(a, Lane) // AVX512_VBMI2
    #define mm512_extract_epi32(a, Lane)    _mm512_extract_epi32(a, Lane) // undefined
    #define mm512_extract_epi64(a, Lane)    _mm512_extract_epi64(a, Lane) // undefined
    #define mm512_shuffle_epi8(a, mask)     _mm512_shuffle_epi8(a, mask)
    #define mm512_storeu_si512(a, b)        _mm512_storeu_si512(a, b)
    #define mm512_set1_epi8(K)              _mm512_set1_epi8(K)
    #define mm512_set1_epi16(K)             _mm512_set1_epi16(K)
    #define mm512_set1_epi32(K)             _mm512_set1_epi32(K)
    #define mm512_set1_epi64(K)             _mm512_set1_epi64(K)
    #define mm512_set_epi64(x08, x07, x06, x05, x04, x03, x02, x01) \
           _mm512_set_epi64(x08, x07, x06, x05, x04, x03, x02, x01)
    #define mm512_set_epi32(x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01) \
           _mm512_set_epi32(x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
    #define mm512_set_epi16(x32, x31, x30, x29, x28, x27, x26, x25, x24, x23, x22, x21, x20, x19, x18, x17, x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01) \
           _mm512_set_epi16(x32, x31, x30, x29, x28, x27, x26, x25, x24, x23, x22, x21, x20, x19, x18, x17, x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
    #define  mm512_set_epi8(x64, x63, x62, x61, x60, x59, x58, x57, x56, x55, x54, x53, x52, x51, x50, x49, x48, x47, x46, x45, x44, x43, x42, x41, x40, x39, x38, x37, x36, x35, x34, x33, x32, x31, x30, x29, x28, x27, x26, x25, x24, x23, x22, x21, x20, x19, x18, x17, x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01) \
            _mm512_set_epi8(x64, x63, x62, x61, x60, x59, x58, x57, x56, x55, x54, x53, x52, x51, x50, x49, x48, x47, x46, x45, x44, x43, x42, x41, x40, x39, x38, x37, x36, x35, x34, x33, x32, x31, x30, x29, x28, x27, x26, x25, x24, x23, x22, x21, x20, x19, x18, x17, x16, x15, x14, x13, x12, x11, x10, x09, x08, x07, x06, x05, x04, x03, x02, x01)
#endif

#endif

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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_HPP

#include <cstdint>
#include <bitcoin/system/cpuid.hpp>

#if defined(HAVE_XCPU)
    #include <immintrin.h>
    #if defined(HAVE_X64)
        #include <nmmintrin.h>
    #endif
    #if defined(HAVE_MSC)
        #include <intrin.h>
    #endif
#endif

#if defined(HAVE_ARM)
    #include <arm_acle.h>
    #if defined(HAVE_NEON)
        #include <arm_neon.h>
    #endif
#endif

namespace libbitcoin {

/// Runtime checks for Intel SIMD and ARM Neon availability.
/// ---------------------------------------------------------------------------

// 1 or 2 blocks, sha256 (optimum).
inline bool try_shani() noexcept
{
// TODO: shani is unverified.
#if defined(HAVE_X64)
    uint32_t eax, ebx, ecx, edx;
    return cpuid_count(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse4_ecx_bit>(ecx)  // SSE4.1
        //// && (eax >= cpu7_0::leaf)
        && cpuid_count(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::shani_ebx_bit>(ebx); // SHA-NI
#else
    return false;
#endif
}

// 8 blocks, 8 lanes (next optimum for 8 blocks).
inline bool try_avx2() noexcept
{
#if defined(HAVE_X64)
    ////uint64_t extended;
    uint32_t eax, ebx, ecx, edx;
    return cpuid_count(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse4_ecx_bit>(ecx)  // SSE4.1
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx) // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)   // AVX
        ////&& xgetbv(extended, xcr0::feature)
        ////&& get_bit<xcr0::sse_bit>(extended)
        ////&& get_bit<xcr0::avx_bit>(extended)
        && cpuid_count(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx);  // AVX2
#else
    return false;
#endif
}

// 4 blocks, 4 lanes (next optimum for 4 blocks).
inline bool try_sse41() noexcept
{
// bc::sha256 sse41 faulting on x64 32bit build.
#if defined(HAVE_X64)
    uint32_t eax, ebx, ecx, edx;
    return cpuid_count(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse4_ecx_bit>(ecx); // SSE4.1
#else
    return false;
#endif
}

// 1 block, 4 lanes (nex optimum for 1 block, requires inline assembly - port).
inline bool try_sse4() noexcept
{
// bc::sha256 sse4 requires 64 bit build, implemented as inline assembly.
#if defined(HAVE_X64) && defined(HAVE_XASSEMBLY)
    return try_sse41();
#else
    return false;
#endif
}

// 1 block, 4 lanes  (optimum for neon).
constexpr bool try_neon() noexcept
{
// TODO: neon is unverified.
#if defined(HAVE_NEON)
    return true;
#else
    return false;
#endif
}

/// Published tests for Intel SIMD, and ARM SIMD (Neon) availability.
/// ---------------------------------------------------------------------------

inline bool have_shani() noexcept
{
    static auto enable = try_shani();
    return enable;
}

inline bool have_avx2() noexcept
{
    static auto enable = try_avx2();
    return enable;
}

inline bool have_sse41() noexcept
{
    static auto enable = try_sse41();
    return enable;
}

inline bool have_sse4() noexcept
{
    static auto enable = try_sse4();
    return enable;
}

inline bool have_neon() noexcept
{
    static auto enable = try_neon();
    return enable;
}

/// Intel SIMD (single instruction multiple data) types.
/// ---------------------------------------------------------------------------
/// intel.com/content/www/us/en/docs/intrinsics-guide/index.html

#if defined (HAVE_XCPU)

namespace i128 {

using mint128_t = __m128i;

template <uint32_t Offset>
uint32_t inline get(mint128_t a) noexcept
{
    return _mm_extract_epi32(a, Offset);
}

mint128_t inline set(uint32_t a) noexcept
{
    return _mm_set1_epi32(a);
}

mint128_t inline set(uint64_t a, uint64_t b) noexcept
{
    return _mm_set_epi64x(a, b);
}

mint128_t inline set(uint32_t a, uint32_t b, uint32_t c, uint32_t d) noexcept
{
    return _mm_set_epi32(a, b, c, d);
}

mint128_t inline sum(mint128_t a, mint128_t b) noexcept
{
    return _mm_add_epi32(a, b);
}

mint128_t inline sum(mint128_t a, mint128_t b, mint128_t c) noexcept
{
    
    return sum(sum(a, b), c);
}

mint128_t inline sum(mint128_t a, mint128_t b, mint128_t c,
    mint128_t d) noexcept
{
    return sum(sum(a, b), sum(c, d));
}

mint128_t inline sum(mint128_t a, mint128_t b, mint128_t c, mint128_t d,
    mint128_t e) noexcept
{
    return sum(sum(a, b, c), sum(d, e));
}

mint128_t inline inc(mint128_t& outa, mint128_t b) noexcept
{
    return ((outa = sum(outa, b)));
}

mint128_t inline inc(mint128_t& outa, mint128_t b, mint128_t c) noexcept
{
    return ((outa = sum(outa, b, c)));
}

mint128_t inline inc(mint128_t& outa, mint128_t b, mint128_t c,
    mint128_t d) noexcept
{
    return ((outa = sum(outa, b, c, d)));
}

mint128_t inline exc(mint128_t a, mint128_t b) noexcept
{
    return _mm_xor_si128(a, b);
}

mint128_t inline exc(mint128_t a, mint128_t b, mint128_t c) noexcept
{
    return exc(exc(a, b), c);
}

mint128_t inline dis(mint128_t a, mint128_t b) noexcept
{
    return _mm_or_si128(a, b);
}

mint128_t inline con(mint128_t a, mint128_t b) noexcept
{
    return _mm_and_si128(a, b);
}

mint128_t inline shr(mint128_t a, uint32_t bits) noexcept
{
    return _mm_srli_epi32(a, bits);
}

mint128_t inline shl(mint128_t a, uint32_t bits) noexcept
{
    return _mm_slli_epi32(a, bits);
}

/// Concatenate two 16-byte blocks into a 32-byte temporary result, shift the 
/// result right by Shift bytes, and return the low 16 bytes.
template <uint32_t Shift>
mint128_t inline align_right(mint128_t a, mint128_t b) noexcept
{
    return _mm_alignr_epi8(a, b, Shift);
}

/// Blend two packed 16-bit integers using Mask.
template <uint32_t Mask>
mint128_t inline blend(mint128_t a, mint128_t b) noexcept
{
    return _mm_blend_epi16(a, b, Mask);
}

/// Shuffle 32-bit integers using Control.
template <uint32_t Control>
mint128_t inline shuffle(mint128_t a) noexcept
{
    return _mm_shuffle_epi32(a, Control);
}

/// Shuffle packed 8-bit integers in a according to shuffle control mask in the
/// corresponding 8-bit element of b.
mint128_t inline shuffle(mint128_t a, mint128_t b) noexcept
{
    return _mm_shuffle_epi8(a, b);
}

} // namespace i128

namespace i256 {
    
using mint256_t = __m256i;

template <uint32_t Offset>
uint32_t inline get(mint256_t a) noexcept
{
    return _mm256_extract_epi32(a, Offset);
}

// GCC:
// warning: AVX vector return without AVX enabled changes the ABI [-Wpsabi]
// This should be harmless as long as runtime support is executed.
mint256_t inline set(uint32_t a) noexcept 
{
    return _mm256_set1_epi32(a);
}

mint256_t inline set(uint32_t a, uint32_t b, uint32_t c, uint32_t d,
    uint32_t e, uint32_t f, uint32_t g, uint32_t h) noexcept
{
    return _mm256_set_epi32(a, b, c, d, e, f, g, h);
}

mint256_t inline shuffle(mint256_t a, mint256_t b) noexcept
{
    return _mm256_shuffle_epi8(a, b);
}

mint256_t inline sum(mint256_t a, mint256_t b) noexcept
{
    return _mm256_add_epi32(a, b);
}

mint256_t inline sum(mint256_t a, mint256_t b, mint256_t c) noexcept
{
    return sum(sum(a, b), c);
}

mint256_t inline sum(mint256_t a, mint256_t b, mint256_t c,
    mint256_t d) noexcept
{
    return sum(sum(a, b), sum(c, d));
}

mint256_t inline sum(mint256_t a, mint256_t b, mint256_t c, mint256_t d,
    mint256_t e) noexcept
{
    return sum(sum(a, b, c), sum(d, e));
}

mint256_t inline inc(mint256_t& outa, mint256_t b) noexcept
{
    return ((outa = sum(outa, b)));
}

mint256_t inline inc(mint256_t& outa, mint256_t b, mint256_t c) noexcept
{
    return ((outa = sum(outa, b, c)));
}

mint256_t inline inc(mint256_t& outa, mint256_t b, mint256_t c,
    mint256_t d) noexcept
{
    return ((outa = sum(outa, b, c, d)));
}

mint256_t inline exc(mint256_t a, mint256_t b) noexcept
{
    return _mm256_xor_si256(a, b);
}

mint256_t inline exc(mint256_t a, mint256_t b, mint256_t c) noexcept
{
    return exc(exc(a, b), c);
}

mint256_t inline dis(mint256_t a, mint256_t b) noexcept
{
    return _mm256_or_si256(a, b);
}

mint256_t inline con(mint256_t a, mint256_t b) noexcept
{
    return _mm256_and_si256(a, b);
}

mint256_t inline shr(mint256_t a, uint32_t bits) noexcept
{
    return _mm256_srli_epi32(a, bits);
}

mint256_t inline shl(mint256_t a, uint32_t bits) noexcept
{
    return _mm256_slli_epi32(a, bits);
}

} // namespace i256

#endif // HAVE_XCPU

/// ARM SIMD (single instruction multiple data) types.
/// ---------------------------------------------------------------------------

#if defined(HAVE_ARM)
#endif // HAVE_ARM

} // namespace libbitcoin

#endif

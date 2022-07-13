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
#include <bitcoin/system/warnings.hpp>

#if defined(HAVE_ARM)
    #include <arm_neon.h>
#endif

#if defined(HAVE_INTEL)
    #include <immintrin.h>

    // Nonstandard header.
    #if defined(HAVE_MSC)
        #include <intrin.h>
    #endif
#endif

/// General CPU instructions (xgetbv, cpuid_count) used to locate CPU features.
/// ---------------------------------------------------------------------------
/// Given ISO define, intrinsics always compile, however on other platforms this
/// support is inconsistent, so revert to lowest common interface (assembly).
/// MSVC has ISO definitions but not x64 inline assembly, so this correlates.

namespace libbitcoin {

/// Runtime checks for Intel SIMD and ARM Neon availability.
/// ---------------------------------------------------------------------------
/// Verifying feature support at runtime ensures build portability.
    
#if defined (HAVE_INTEL)

namespace cpu1_0
{
    constexpr auto leaf = 1;
    constexpr auto subleaf = 0;
    constexpr auto sse4_ecx_bit = 19;
    constexpr auto xsave_ecx_bit = 27;
    constexpr auto avx_ecx_bit = 28;
}

namespace cpu7_0
{
    constexpr auto leaf = 7;
    constexpr auto subleaf = 0;
    constexpr auto avx2_ebx_bit = 5;
    constexpr auto shani_ebx_bit = 29;
}

namespace xcr0
{
    constexpr auto feature = 0;
    constexpr auto sse_bit = 1;
    constexpr auto avx_bit = 2;
}

template <size_t Bit, typename Value>
constexpr bool get_bit(Value value) noexcept
{
    constexpr auto mask = (Value{ 1 } << Bit);
    return (value & mask) != 0;
}

static bool xgetbv(uint64_t& value, uint32_t index) noexcept
{
#if defined(HAVE_INTEL_INTRINSICS)
    value = _xgetbv(index);
    return true;
#elif defined(HAVE_INTEL_INLINE_ASSEMBLY)
    // Compile error: built-in _xgetbv requires target feature xsave.
    // But xsave can only be determined at run time, so must use assembly.
    ////value = _xgetbv(index);
    uint32_t a{}, d{};
    __asm__("xgetbv" : "=a"(a), "=d"(d) : "c"(index));
    value = (static_cast<uint64_t>(d) << 32) | a;
    return true;
#else
    return false;
#endif
}

static bool cpuid_count(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d,
    uint32_t leaf, uint32_t subleaf) noexcept
{
#if defined(HAVE_INTEL_INTRINSICS)
    int out[4]{};
    __cpuidex(&out[0], leaf, subleaf);
    a = out[0];
    b = out[1];
    c = out[2];
    d = out[3];
    return true;
#elif defined(HAVE_INTEL_INLINE_ASSEMBLY)
    // __cpuid_count too commonly undefined, so just always use assembly.
    ////__cpuid_count(leaf, subleaf, a, b, c, d);
    __asm__("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "0"(leaf), "2"(subleaf));
    return true;
#else
    return false;
#endif
}

inline bool try_avx2() noexcept
{
    uint64_t extended;
    uint32_t eax, ebx, ecx, edx;
    return cpuid_count(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse4_ecx_bit>(ecx)
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)
        && xgetbv(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && cpuid_count(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx);
}

inline bool try_sse41() noexcept
{
    uint32_t eax, ebx, ecx, edx;
    return cpuid_count(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse4_ecx_bit>(ecx);
}

inline bool try_sse4() noexcept
{
    uint32_t eax, ebx, ecx, edx;
    return cpuid_count(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse4_ecx_bit>(ecx);
}

inline bool try_shani() noexcept
{
    uint32_t eax, ebx, ecx, edx;
    return cpuid_count(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && (eax >= cpu7_0::leaf)
        && get_bit<cpu1_0::sse4_ecx_bit>(ecx)
        && cpuid_count(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::shani_ebx_bit>(ebx);
}

#endif // HAVE_INTEL

#if defined (HAVE_ARM)

// TODO: not implemented.
constexpr bool try_neon() noexcept
{
#if defined(HAVE_NEON_INTRINSICS)
    return false;
#else
    return false;
#endif
}

#endif // HAVE_ARM

/// Published tests for Intel SIMD, and ARM SIMD (Neon) availability.
/// ---------------------------------------------------------------------------

#if defined (HAVE_INTEL)

inline bool have_avx2() noexcept
{
    static auto enable = try_avx2();
    return enable;
}

// TODO: sse41 throws on x86 builds, so disabled here.
inline bool have_sse41() noexcept
{
#if defined(HAVE_X64)
    static auto enable = try_sse41();
    return enable;
#else
    return false;
#endif
}

// TODO: sse4 is not yet implemented for msvc (currently requires __asm__), so
// TODO: while try_sse4 may succeed, a call to sha256::sse4 will fail on msvc.
// TODO: so this must return false if HAVE_INTEL_INLINE_ASSEMBLY undefined. 
inline bool have_sse4() noexcept
{
#if defined(HAVE_INTEL_INLINE_ASSEMBLY)
    static auto enable = try_sse4();
    return enable;
#else
    return false;
#endif
}

inline bool have_shani() noexcept
{
    static auto enable = try_shani();
    return enable;
}

#else

constexpr bool have_avx2() noexcept
{
    return false;
}

constexpr bool have_sse41() noexcept
{
    return false;
}

constexpr bool have_sse4() noexcept
{
    return false;
}

constexpr bool have_shani() noexcept
{
    return false;
}

#endif // HAVE_INTEL

#if defined(HAVE_ARM)

inline bool have_neon() noexcept
{
    static auto enable = try_neon();
    return enable;
}

#else

constexpr bool have_neon() noexcept
{
    return false;
}

#endif // HAVE_ARM

/// SIMD (single instruction multiple data) types.
/// ---------------------------------------------------------------------------
/// intel.com/content/www/us/en/docs/intrinsics-guide/index.html

#if defined (HAVE_INTEL)

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

#endif

} // namespace libbitcoin

#endif

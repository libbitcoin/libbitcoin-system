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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_HAVE_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_HAVE_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/xcpu/cpuid.hpp>

// -msse4 -mavx2 -msha (gcc/clang)
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
// -march=armv8-a+crc+crypto (gcc/clang)
// -arch arm64 -isysroot... (xcode)
#if defined(HAVE_ARM)
    #include <arm_acle.h>
    #if defined(HAVE_NEON)
        #include <arm_neon.h>
    #endif
#endif

namespace libbitcoin {

/// Runtime checks for Intel SIMD and ARM Neon availability.
/// ---------------------------------------------------------------------------

inline bool try_shani() noexcept
{
// TODO: shani is unverified.
#if defined(HAVE_X64)
    uint32_t eax, ebx, ecx, edx;
    return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse4_ecx_bit>(ecx)  // SSE4.1
        && (eax >= cpu7_0::leaf)
        && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::shani_ebx_bit>(ebx); // SHA-NI
#else
    return false;
#endif
}

inline bool try_avx512() noexcept
{
    // TODO: implement.
    return false;
}

inline bool try_avx2() noexcept
{
#if defined(HAVE_X64)
    uint64_t extended;
    uint32_t eax, ebx, ecx, edx;
    return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse4_ecx_bit>(ecx)  // SSE4.1
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx) // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)   // AVX
        && get_xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx);  // AVX2
#else
    return false;
#endif
}

inline bool try_sse41() noexcept
{
// TODO: sse41 faulting on HAVE_X32.
#if defined(HAVE_X64)
    uint32_t eax, ebx, ecx, edx;
    return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse4_ecx_bit>(ecx);  // SSE4.1
#else
    return false;
#endif
}

inline bool try_sse41a() noexcept
{
// sse4 is sse41 but requires X64 build (inline assembly).
#if defined(HAVE_X64) && defined(HAVE_XASSEMBLY)
    return try_sse41();
#else
    return false;
#endif
}

constexpr bool try_neon() noexcept
{
// TODO: ARM/Neon is unverified.
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

inline bool have_avx512() noexcept
{
    static auto enable = try_avx512();
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

inline bool have_sse41a() noexcept
{
    static auto enable = try_sse41a();
    return enable;
}

inline bool have_neon() noexcept
{
    static auto enable = try_neon();
    return enable;
}

} // namespace libbitcoin

#endif

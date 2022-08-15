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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_HAVES_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_HAVES_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/xcpu/cpuid.hpp>

namespace libbitcoin {
namespace system {

namespace cpu1_0
{
    constexpr auto leaf = 1;
    constexpr auto subleaf = 0;
    constexpr auto sse4_ecx_bit = 19;
    constexpr auto xsave_ecx_bit = 27;
    constexpr auto avx_ecx_bit = 28;
}

// wikichip.org/wiki/x86/avx-512
namespace cpu7_0
{
    constexpr auto leaf = 7;
    constexpr auto subleaf = 0;
    constexpr auto avx2_ebx_bit = 5;
    constexpr auto avx512_ebx_bit = 16;
    constexpr auto shani_ebx_bit = 29;
}

namespace xcr0
{
    constexpr auto feature = 0;
    constexpr auto sse_bit = 1;
    constexpr auto avx_bit = 2;
}

// Local util because no dependency on /math.
template <size_t Bit, typename Value>
constexpr bool get_bit(Value value) noexcept
{
    constexpr auto mask = (Value{ 1 } << Bit);
    return !is_zero(value & mask);
}

/// Runtime checks for Intel SIMD and ARM Neon availability.
/// ---------------------------------------------------------------------------

inline bool try_shani() noexcept
{
// TODO: shani is unverified.
#if defined(HAVE_X64)
    uint32_t eax, ebx, ecx, edx;
    return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse4_ecx_bit>(ecx)   // SSE4.1
        && (eax >= cpu7_0::leaf)
        && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::shani_ebx_bit>(ebx);  // SHA-NI
#else
    return false;
#endif
}

inline bool try_avx512() noexcept
{
// TODO: avx512 is unverified.
#if defined(HAVE_X64)
    uint64_t extended;
    uint32_t eax, ebx, ecx, edx;
    return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse4_ecx_bit>(ecx)   // SSE4.1
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)  // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)    // AVX
        && get_xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx512_ebx_bit>(ebx); // AVX512
#else
    return false;
#endif
}

inline bool try_avx2() noexcept
{
#if defined(HAVE_X64)
    uint64_t extended;
    uint32_t eax, ebx, ecx, edx;
    return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse4_ecx_bit>(ecx)   // SSE4.1
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)  // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)    // AVX
        && get_xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx);   // AVX2
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
        && get_bit<cpu1_0::sse4_ecx_bit>(ecx);   // SSE4.1
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
/// TODO: statics introduce a thread guard, not ideal for excessive iteration.

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

} // namespace system
} // namespace libbitcoin

#endif

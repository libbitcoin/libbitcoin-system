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
#include <immintrin.h>
#include <bitcoin/system/warnings.hpp>

// Nonstandard intrinsics header.
#if defined(HAVE_MSC)
    #include <intrin.h>
#endif

// CPU instructions (xgetbv, cpuid_count) required to test for intrinsics.
// ----------------------------------------------------------------------------
// Given ISO define, intrinsics always compile, however on other platforms this
// support is inconsistent, so revert to the lowest common interface (assembly).
// MSVC provides ISO definitions and doesn't define __asm__, so this correlates.

namespace libbitcoin {

static bool xgetbv(uint64_t& value, uint32_t index) noexcept
{
#if defined(HAVE_GNU_INTEL)
    // Compile error: built-in _xgetbv requires target feature xsave.
    // But xsave can only be determined at run time, so must use assembly.
    ////value = _xgetbv(index);
    uint32_t a{}, d{};
    __asm__("xgetbv" : "=a"(a), "=d"(d) : "c"(index));
    value = (static_cast<uint64_t>(d) << 32) | a;
    return true;
#elif defined(HAVE_ISO_INTEL)
    value = _xgetbv(index);
    return true;
#else
    return false;
#endif
}

static bool cpuid_count(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d,
    uint32_t leaf, uint32_t subleaf) noexcept
{
#if defined(HAVE_GNU_INTEL)
    // __cpuid_count too commonly undefined, so just always use assembly.
    ////__cpuid_count(leaf, subleaf, a, b, c, d);
    __asm__("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "0"(leaf), "2"(subleaf));
    return true;
#elif defined(HAVE_ISO_INTEL)
    int out[4]{};
    __cpuidex(&out[0], leaf, subleaf);
    a = out[0];
    b = out[1];
    c = out[2];
    d = out[3];
    return true;
#else
    return false;
#endif
}

// Runtime checks for intrinsics.
// ----------------------------------------------------------------------------

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

// Try helper, tested.
template <size_t Bit, typename Value>
constexpr bool get_bit(Value value) noexcept
{
    constexpr auto mask = (Value{1} << Bit);
    return (value & mask) != 0;
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

// TODO: test for ARM Neon.
inline bool try_neon() noexcept
{
#if defined (HAVE_NEON)
    return true;
#else
    return false;
#endif
}

// Call to test for intrinsic availability.
// ----------------------------------------------------------------------------

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

// TODO: sse4 is not yet implemented for msvc (currently requires __asm__), so
// TODO: while try_sse4 may succeed, a call to sha256::sse4 will fail on msvc.
inline bool have_sse4() noexcept
{
#if defined (HAVE_MSC)
    return false;
#else
    static auto enable = try_sse4();
    return enable;
#endif
}

inline bool have_shani() noexcept
{
    static auto enable = try_shani();
    return enable;
}

inline bool have_neon() noexcept
{
    static auto enable = try_neon();
    return enable;
}

} // namespace libbitcoin

#endif

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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_DETECTION_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_DETECTION_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/cpuid.hpp>

namespace libbitcoin {
namespace system {

/// Runtime checks for Intel SIMD availability.
/// ---------------------------------------------------------------------------

namespace cpu1_0
{
    constexpr auto leaf = 1;
    constexpr auto subleaf = 0;
    constexpr auto sse41_ecx_bit = 19;
    constexpr auto xsave_ecx_bit = 27;
    constexpr auto avx_ecx_bit = 28;
}

namespace cpu7_0
{
    constexpr auto leaf = 7;
    constexpr auto subleaf = 0;
    constexpr auto avx2_ebx_bit = 5;
    ////constexpr auto avx512f_ebx_bit = 16;
    constexpr auto avx512bw_ebx_bit = 30;
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
constexpr bool get_bit(Value value) NOEXCEPT
{
    constexpr auto mask = (Value{ 1 } << Bit);
    return !is_zero(value & mask);
}

inline bool try_shani() NOEXCEPT
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
        && (eax >= cpu7_0::leaf)
        && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::shani_ebx_bit>(ebx);     // SHA
}

inline bool try_avx512() NOEXCEPT
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)      // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)        // AVX
        && get_xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx)       // AVX2 (implied?)
        && get_bit<cpu7_0::avx512bw_ebx_bit>(ebx);  // AVX512BW
}

inline bool try_avx2() NOEXCEPT
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)      // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)        // AVX
        && get_xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx);      // AVX2
}

inline bool try_sse41() NOEXCEPT
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx);     // SSE4.1
}

} // namespace system
} // namespace libbitcoin

#endif

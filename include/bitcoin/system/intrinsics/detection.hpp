/**
 * Copyright (c) 2011-2026 libbitcoin developers
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

namespace cpu0_0
{
    constexpr auto leaf = 0;
    constexpr auto subleaf = 0;
    constexpr uint32_t intel_ebx = 0x756e6547;
    constexpr uint32_t intel_edx = 0x49656e69;
    constexpr uint32_t intel_ecx = 0x6c65746e;
}

namespace cpu1_0
{
    constexpr auto leaf = 1;
    constexpr auto subleaf = 0;
    constexpr auto pclmulqdq_ecx_bit = 1;
    constexpr auto sse41_ecx_bit = 19;
    constexpr auto aes_ecx_bit = 25;
    constexpr auto xsave_ecx_bit = 27;
    constexpr auto avx_ecx_bit = 28;
    constexpr uint32_t signature_mask = 0x0fff0ff0;
    constexpr uint32_t skylake_server = 0x00050650;
    constexpr uint32_t knights_landing = 0x00050670;
    constexpr uint32_t knights_mill = 0x00080650;
}

namespace cpu7_0
{
    constexpr auto leaf = 7;
    constexpr auto subleaf = 0;
    constexpr auto avx2_ebx_bit = 5;
    constexpr auto avx512f_ebx_bit = 16;
    constexpr auto avx512bw_ebx_bit = 30;
    constexpr auto shani_ebx_bit = 29;
    constexpr auto avx512ifma_ebx_bit = 21;
    constexpr auto avx512vl_ebx_bit = 31;
    constexpr auto vaes_ecx_bit = 9;
    constexpr auto vpclmulqdq_ecx_bit = 10;
}

namespace cpu7_1
{
    constexpr auto leaf = 7;
    constexpr auto subleaf = 1;
    constexpr auto sha512_eax_bit = 0;
    constexpr auto avxifma_eax_bit = 23;
}

namespace xcr0
{
    constexpr auto feature = 0;
    constexpr auto sse_bit = 1;
    constexpr auto avx_bit = 2;
    constexpr auto opmask_bit = 5;
    constexpr auto zmm_upper_bit = 6;
    constexpr auto zmm_high_bit = 7;
}

// Local utils because no dependency on /math.
template <size_t Bit, typename Value>
constexpr bool get_bit(Value value) NOEXCEPT
{
    constexpr auto mask = (Value{ 1 } << Bit);
    return !is_zero(value & mask);
}

// Cores that lower the clock for any 512 bit instruction.
constexpr bool is_throttled(uint32_t signature) NOEXCEPT
{
    signature &= cpu1_0::signature_mask;
    return signature == cpu1_0::skylake_server
        || signature == cpu1_0::knights_landing
        || signature == cpu1_0::knights_mill;
}

template <auto Cpu = get_cpu>
inline bool is_avx512_low() NOEXCEPT
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu0_0::leaf, cpu0_0::subleaf)
        && ebx == cpu0_0::intel_ebx                 // Genu
        && edx == cpu0_0::intel_edx                 // ineI
        && ecx == cpu0_0::intel_ecx                 // ntel
        && Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && is_throttled(eax);                       // Skylake-SP, Xeon Phi
}

template <auto Cpu = get_cpu>
inline bool try_shani() NOEXCEPT
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
        && Cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::shani_ebx_bit>(ebx);     // SHA
}

template <auto Cpu = get_cpu, auto Xcr = get_xcr>
inline bool try_sha512() NOEXCEPT
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)      // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)        // AVX
        && Xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && Cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx)       // AVX2
        && eax >= cpu7_1::subleaf                   // Subleaf 1
        && Cpu(eax, ebx, ecx, edx, cpu7_1::leaf, cpu7_1::subleaf)
        && get_bit<cpu7_1::sha512_eax_bit>(eax);    // SHA512
}

template <auto Cpu = get_cpu, auto Xcr = get_xcr>
inline bool try_avx512() NOEXCEPT
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)      // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)        // AVX
        && Xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && get_bit<xcr0::opmask_bit>(extended)
        && get_bit<xcr0::zmm_upper_bit>(extended)
        && get_bit<xcr0::zmm_high_bit>(extended)
        && Cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx)       // AVX2
        && get_bit<cpu7_0::avx512f_ebx_bit>(ebx)    // AVX512F
        && get_bit<cpu7_0::avx512bw_ebx_bit>(ebx)   // AVX512BW
        && get_bit<cpu7_0::avx512vl_ebx_bit>(ebx)   // AVX512VL
        && !is_avx512_low<Cpu>();                   // Not throttled
}

template <auto Cpu = get_cpu, auto Xcr = get_xcr>
inline bool try_avx512ifma() NOEXCEPT
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)      // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)        // AVX
        && Xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && get_bit<xcr0::opmask_bit>(extended)
        && get_bit<xcr0::zmm_upper_bit>(extended)
        && get_bit<xcr0::zmm_high_bit>(extended)
        && Cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx)       // AVX2
        && get_bit<cpu7_0::avx512f_ebx_bit>(ebx)    // AVX512F
        && get_bit<cpu7_0::avx512vl_ebx_bit>(ebx)   // AVX512VL
        && get_bit<cpu7_0::avx512ifma_ebx_bit>(ebx);// AVX512IFMA
}

template <auto Cpu = get_cpu, auto Xcr = get_xcr>
inline bool try_avxifma() NOEXCEPT
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)      // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)        // AVX
        && Xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && Cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx)       // AVX2
        && eax >= cpu7_1::subleaf                   // Subleaf 1
        && Cpu(eax, ebx, ecx, edx, cpu7_1::leaf, cpu7_1::subleaf)
        && get_bit<cpu7_1::avxifma_eax_bit>(eax);   // AVXIFMA
}

template <auto Cpu = get_cpu, auto Xcr = get_xcr>
inline bool try_avx2() NOEXCEPT
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)      // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)        // AVX
        && Xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && Cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx);      // AVX2
}

template <auto Cpu = get_cpu>
inline bool try_sse41() NOEXCEPT
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx);     // SSE4.1
}

template <auto Cpu = get_cpu>
inline bool try_aesni() NOEXCEPT
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
        && get_bit<cpu1_0::pclmulqdq_ecx_bit>(ecx)  // PCLMULQDQ
        && get_bit<cpu1_0::aes_ecx_bit>(ecx);       // AES
}

template <auto Cpu = get_cpu, auto Xcr = get_xcr>
inline bool try_vaes() NOEXCEPT
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
        && get_bit<cpu1_0::pclmulqdq_ecx_bit>(ecx)  // PCLMULQDQ
        && get_bit<cpu1_0::aes_ecx_bit>(ecx)        // AES
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)      // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)        // AVX
        && Xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && Cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx)       // AVX2
        && get_bit<cpu7_0::vaes_ecx_bit>(ecx)       // VAES
        && get_bit<cpu7_0::vpclmulqdq_ecx_bit>(ecx);// VPCLMULQDQ
}

/// Runtime checks for ARM NEON, SHA, AES and SHA3 availability.
/// ---------------------------------------------------------------------------

template <auto Arm = get_arm>
inline bool try_neon() NOEXCEPT
{
    return Arm(arm_feature::neon);
}

template <auto Arm = get_arm>
inline bool try_neon_sha() NOEXCEPT
{
    return Arm(arm_feature::sha1)
        && Arm(arm_feature::sha256);
}

template <auto Arm = get_arm>
inline bool try_neon_aes() NOEXCEPT
{
    return Arm(arm_feature::aes)
        && Arm(arm_feature::pmull);
}

template <auto Arm = get_arm>
inline bool try_neon_sha3() NOEXCEPT
{
    return Arm(arm_feature::sha3)
        && Arm(arm_feature::sha512);
}

} // namespace system
} // namespace libbitcoin

#endif

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

#include <cstdio>
#include <cstdlib>
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
    constexpr auto intel_ebx = 0x756e6547_u32;
    constexpr auto intel_edx = 0x49656e69_u32;
    constexpr auto intel_ecx = 0x6c65746e_u32;
}

namespace cpu1_0
{
    constexpr auto leaf = 1;
    constexpr auto subleaf = 0;
    constexpr auto sse3_ecx_bit = 0;
    constexpr auto pclmulqdq_ecx_bit = 1;
    constexpr auto ssse3_ecx_bit = 9;
    constexpr auto fma_ecx_bit = 12;
    constexpr auto cx16_ecx_bit = 13;
    constexpr auto sse41_ecx_bit = 19;
    constexpr auto sse42_ecx_bit = 20;
    constexpr auto movbe_ecx_bit = 22;
    constexpr auto popcnt_ecx_bit = 23;
    constexpr auto aes_ecx_bit = 25;
    constexpr auto xsave_ecx_bit = 27;
    constexpr auto avx_ecx_bit = 28;
    constexpr auto f16c_ecx_bit = 29;
    constexpr auto signature_mask = 0x0fff0ff0_u32;
    constexpr auto skylake_server = 0x00050650_u32;
    constexpr auto knights_landing = 0x00050670_u32;
    constexpr auto knights_mill = 0x00080650_u32;
}

namespace cpu7_0
{
    constexpr auto leaf = 7;
    constexpr auto subleaf = 0;
    constexpr auto bmi1_ebx_bit = 3;
    constexpr auto avx2_ebx_bit = 5;
    constexpr auto bmi2_ebx_bit = 8;
    constexpr auto avx512f_ebx_bit = 16;
    constexpr auto avx512dq_ebx_bit = 17;
    constexpr auto avx512cd_ebx_bit = 28;
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

namespace cpu80000001_0
{
    constexpr auto leaf = 0x80000001;
    constexpr auto subleaf = 0;
    constexpr auto lahf_ecx_bit = 0;
    constexpr auto lzcnt_ecx_bit = 5;
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
BASELINE constexpr bool get_bit(Value value) NOEXCEPT
{
    constexpr auto mask = (Value{ 1 } << Bit);
    return (value & mask) != 0;
}

// Cores that lower the clock for any 512 bit instruction.
BASELINE constexpr bool is_throttled(uint32_t signature) NOEXCEPT
{
    signature &= cpu1_0::signature_mask;
    return signature == cpu1_0::skylake_server
        || signature == cpu1_0::knights_landing
        || signature == cpu1_0::knights_mill;
}

template <auto Cpu = get_cpu>
BASELINE inline bool is_avx512_low() NOEXCEPT
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu0_0::leaf, cpu0_0::subleaf)
        && ebx == cpu0_0::intel_ebx                     // Genu
        && edx == cpu0_0::intel_edx                     // ineI
        && ecx == cpu0_0::intel_ecx                     // ntel
        && Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && is_throttled(eax);                           // Skylake-SP, Xeon Phi
}

template <auto Cpu = get_cpu>
BASELINE inline bool try_shani() NOEXCEPT
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)          // SSE4.1
        && Cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::shani_ebx_bit>(ebx);         // SHA
}

template <auto Cpu = get_cpu, auto Xcr = get_xcr>
BASELINE inline bool try_sha512() NOEXCEPT
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)          // SSE4.1
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)          // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)            // AVX
        && Xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && Cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx)           // AVX2
        && eax >= cpu7_1::subleaf                       // Subleaf 1
        && Cpu(eax, ebx, ecx, edx, cpu7_1::leaf, cpu7_1::subleaf)
        && get_bit<cpu7_1::sha512_eax_bit>(eax);        // SHA512
}

template <auto Cpu = get_cpu, auto Xcr = get_xcr>
BASELINE inline bool is_avx512() NOEXCEPT
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)          // SSE4.1
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)          // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)            // AVX
        && Xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && get_bit<xcr0::opmask_bit>(extended)
        && get_bit<xcr0::zmm_upper_bit>(extended)
        && get_bit<xcr0::zmm_high_bit>(extended)
        && Cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx)           // AVX2
        && get_bit<cpu7_0::avx512f_ebx_bit>(ebx)        // AVX512F
        && get_bit<cpu7_0::avx512bw_ebx_bit>(ebx)       // AVX512BW
        && get_bit<cpu7_0::avx512vl_ebx_bit>(ebx);      // AVX512VL
}

template <auto Cpu = get_cpu, auto Xcr = get_xcr>
BASELINE inline bool try_avx512() NOEXCEPT
{
    return is_avx512<Cpu, Xcr>()
        && !is_avx512_low<Cpu>();                       // Not throttled
}

template <auto Cpu = get_cpu, auto Xcr = get_xcr>
BASELINE inline bool try_avx512ifma() NOEXCEPT
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)          // SSE4.1
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)          // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)            // AVX
        && Xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && get_bit<xcr0::opmask_bit>(extended)
        && get_bit<xcr0::zmm_upper_bit>(extended)
        && get_bit<xcr0::zmm_high_bit>(extended)
        && Cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx)           // AVX2
        && get_bit<cpu7_0::avx512f_ebx_bit>(ebx)        // AVX512F
        && get_bit<cpu7_0::avx512vl_ebx_bit>(ebx)       // AVX512VL
        && get_bit<cpu7_0::avx512ifma_ebx_bit>(ebx);    // AVX512IFMA
}

template <auto Cpu = get_cpu, auto Xcr = get_xcr>
BASELINE inline bool try_avxifma() NOEXCEPT
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)          // SSE4.1
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)          // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)            // AVX
        && Xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && Cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx)           // AVX2
        && eax >= cpu7_1::subleaf                       // Subleaf 1
        && Cpu(eax, ebx, ecx, edx, cpu7_1::leaf, cpu7_1::subleaf)
        && get_bit<cpu7_1::avxifma_eax_bit>(eax);       // AVXIFMA
}

template <auto Cpu = get_cpu, auto Xcr = get_xcr>
BASELINE inline bool try_avx2() NOEXCEPT
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)          // SSE4.1
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)          // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)            // AVX
        && Xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && Cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx);          // AVX2
}

template <auto Cpu = get_cpu>
BASELINE inline bool try_sse41() NOEXCEPT
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx);         // SSE4.1
}

template <auto Cpu = get_cpu>
BASELINE inline bool try_aesni() NOEXCEPT
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)          // SSE4.1
        && get_bit<cpu1_0::pclmulqdq_ecx_bit>(ecx)      // PCLMULQDQ
        && get_bit<cpu1_0::aes_ecx_bit>(ecx);           // AES
}

template <auto Cpu = get_cpu, auto Xcr = get_xcr>
BASELINE inline bool try_vaes() NOEXCEPT
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)          // SSE4.1
        && get_bit<cpu1_0::pclmulqdq_ecx_bit>(ecx)      // PCLMULQDQ
        && get_bit<cpu1_0::aes_ecx_bit>(ecx)            // AES
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)          // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)            // AVX
        && Xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && Cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx)           // AVX2
        && get_bit<cpu7_0::vaes_ecx_bit>(ecx)           // VAES
        && get_bit<cpu7_0::vpclmulqdq_ecx_bit>(ecx);    // VPCLMULQDQ
}

template <auto Cpu = get_cpu>
BASELINE inline bool try_x86_64_v2() NOEXCEPT
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse3_ecx_bit>(ecx)           // SSE3
        && get_bit<cpu1_0::ssse3_ecx_bit>(ecx)          // SSSE3
        && get_bit<cpu1_0::cx16_ecx_bit>(ecx)           // CMPXCHG16B
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)          // SSE4.1
        && get_bit<cpu1_0::sse42_ecx_bit>(ecx)          // SSE4.2
        && get_bit<cpu1_0::popcnt_ecx_bit>(ecx)         // POPCNT
        && Cpu(eax, ebx, ecx, edx, cpu80000001_0::leaf, cpu80000001_0::subleaf)
        && get_bit<cpu80000001_0::lahf_ecx_bit>(ecx);   // LAHF/SAHF
}

template <auto Cpu = get_cpu, auto Xcr = get_xcr>
BASELINE inline bool try_x86_64_v3() NOEXCEPT
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return try_x86_64_v2<Cpu>()
        && try_avx2<Cpu, Xcr>()                         // AVX, AVX2, XSAVE
        && Cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::fma_ecx_bit>(ecx)            // FMA
        && get_bit<cpu1_0::movbe_ecx_bit>(ecx)          // MOVBE
        && get_bit<cpu1_0::f16c_ecx_bit>(ecx)           // F16C
        && Cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::bmi1_ebx_bit>(ebx)           // BMI1
        && get_bit<cpu7_0::bmi2_ebx_bit>(ebx)           // BMI2
        && Cpu(eax, ebx, ecx, edx, cpu80000001_0::leaf, cpu80000001_0::subleaf)
        && get_bit<cpu80000001_0::lzcnt_ecx_bit>(ecx);  // LZCNT
}

template <auto Cpu = get_cpu, auto Xcr = get_xcr>
BASELINE inline bool try_x86_64_v4() NOEXCEPT
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return try_x86_64_v3<Cpu, Xcr>()
        && is_avx512<Cpu, Xcr>()                        // AVX512F/BW/VL, ZMM
        && Cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx512cd_ebx_bit>(ebx)       // AVX512CD
        && get_bit<cpu7_0::avx512dq_ebx_bit>(ebx);      // AVX512DQ
}

/// Runtime checks for ARM NEON, SHA, AES, SHA3 and LSE availability.
/// ---------------------------------------------------------------------------

template <auto Arm = get_arm>
BASELINE inline bool try_neon() NOEXCEPT
{
    return Arm(arm_feature::neon);
}

template <auto Arm = get_arm>
BASELINE inline bool try_neon_sha2() NOEXCEPT
{
    return Arm(arm_feature::sha1)
        && Arm(arm_feature::sha256);
}

template <auto Arm = get_arm>
BASELINE inline bool try_neon_aes() NOEXCEPT
{
    return Arm(arm_feature::aes)
        && Arm(arm_feature::pmull);
}

template <auto Arm = get_arm>
BASELINE inline bool try_neon_sha3() NOEXCEPT
{
    return Arm(arm_feature::sha3)
        && Arm(arm_feature::sha512);
}

template <auto Arm = get_arm>
BASELINE inline bool try_lse() NOEXCEPT
{
    return Arm(arm_feature::atomics);
}

/// Runtime check for every instruction set extension compiled in.
/// ---------------------------------------------------------------------------

template <auto Cpu = get_cpu, auto Xcr = get_xcr, auto Arm = get_arm>
BASELINE inline bool try_compiled() NOEXCEPT
{
    // Operating systems that report arm64 features.
#if defined(HAVE_ARM64) && \
    (defined(HAVE_LINUX) || defined(HAVE_APPLE) || defined(HAVE_WINDOWS))
    constexpr auto arm = true;
#else
    constexpr auto arm = false;
#endif

    return (!have_x86_64_v2 || try_x86_64_v2<Cpu>())
        && (!have_x86_64_v3 || try_x86_64_v3<Cpu, Xcr>())
        && (!have_x86_64_v4 || try_x86_64_v4<Cpu, Xcr>())
        && (!have_sse41 || try_sse41<Cpu>())
        && (!have_avx2 || try_avx2<Cpu, Xcr>())
        && (!have_avx512 || is_avx512<Cpu, Xcr>())
        && (!have_avx512ifma || try_avx512ifma<Cpu, Xcr>())
        && (!have_avxifma || try_avxifma<Cpu, Xcr>())
        && (!have_shani || try_shani<Cpu>())
        && (!(have_xcpu && have_sha512) || try_sha512<Cpu, Xcr>())
        && (!have_aesni || try_aesni<Cpu>())
        && (!have_vaes || try_vaes<Cpu, Xcr>())
        && (!(arm && have_neon) || try_neon<Arm>())
        && (!(arm && have_neon_aes) || try_neon_aes<Arm>())
        && (!(arm && have_neon_sha) || try_neon_sha2<Arm>())
        && (!(arm && have_neon_sha3) || try_neon_sha3<Arm>())
        && (!(arm && have_lse) || try_lse<Arm>());
}

/// Exit with the message if the processor lacks a compiled extension.
BASELINE inline void hardware_check(const char* message) NOEXCEPT
{
    if (try_compiled())
        return;

    std::fputs(message, stderr);
    std::fputs("\n", stderr);
    std::_Exit(EXIT_FAILURE);
}

} // namespace system
} // namespace libbitcoin

/// Exit with the message before static initialization if the processor lacks a
/// compiled extension. Use once, at file scope, in the first object linked into
/// the executable (macOS runs initializers in link order).
#if defined(_MSC_VER)
    #if defined(HAVE_X32)
        #define BC_HARDWARE_CHECK_SYMBOL "_bc_hardware_check_entry"
    #else
        #define BC_HARDWARE_CHECK_SYMBOL "bc_hardware_check_entry"
    #endif
    #define BC_USE_HARDWARE_CHECK(message) \
        BASELINE static int bc_hardware_check() NOEXCEPT \
        { \
            libbitcoin::system::hardware_check(message); \
            return 0; \
        } \
        __pragma(section(".CRT$XIU", long, read)) \
        extern "C" __declspec(allocate(".CRT$XIU")) \
            int(*bc_hardware_check_entry)() = &bc_hardware_check; \
        __pragma(comment(linker, "/include:" BC_HARDWARE_CHECK_SYMBOL))
#else
    #define BC_USE_HARDWARE_CHECK(message) \
        __attribute__((constructor(101))) BASELINE \
        static void bc_hardware_check() NOEXCEPT \
        { \
            libbitcoin::system::hardware_check(message); \
        }
#endif

#endif

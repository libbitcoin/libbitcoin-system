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

#if defined(HAVE_ARM)
    #if defined(HAVE_LINUX)
        #include <sys/auxv.h>
        #include <asm/hwcap.h>
    #endif
    #if defined(HAVE_APPLE)
        #include <sys/sysctl.h>
    #endif
#endif

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

// Local util because no dependency on /math.
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

inline bool try_shani() NOEXCEPT
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
        && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::shani_ebx_bit>(ebx);     // SHA
}

inline bool try_sha512() NOEXCEPT
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
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx)       // AVX2
        && eax >= cpu7_1::subleaf                   // Subleaf 1
        && get_cpu(eax, ebx, ecx, edx, cpu7_1::leaf, cpu7_1::subleaf)
        && get_bit<cpu7_1::sha512_eax_bit>(eax);    // SHA512
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
        && get_bit<xcr0::opmask_bit>(extended)
        && get_bit<xcr0::zmm_upper_bit>(extended)
        && get_bit<xcr0::zmm_high_bit>(extended)
        && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx)       // AVX2
        && get_bit<cpu7_0::avx512f_ebx_bit>(ebx)    // AVX512F
        && get_bit<cpu7_0::avx512bw_ebx_bit>(ebx);  // AVX512BW
}

inline bool try_avx512ifma() NOEXCEPT
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
        && get_bit<xcr0::opmask_bit>(extended)
        && get_bit<xcr0::zmm_upper_bit>(extended)
        && get_bit<xcr0::zmm_high_bit>(extended)
        && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx)       // AVX2
        && get_bit<cpu7_0::avx512f_ebx_bit>(ebx)    // AVX512F
        && get_bit<cpu7_0::avx512vl_ebx_bit>(ebx)   // AVX512VL
        && get_bit<cpu7_0::avx512ifma_ebx_bit>(ebx);// AVX512IFMA
}

inline bool try_avx512_throttled() NOEXCEPT
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return try_avx512()
        && get_cpu(eax, ebx, ecx, edx, cpu0_0::leaf, cpu0_0::subleaf)
        && ebx == cpu0_0::intel_ebx                 // Genu
        && edx == cpu0_0::intel_edx                 // ineI
        && ecx == cpu0_0::intel_ecx                 // ntel
        && get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && is_throttled(eax);                       // Skylake-SP, Xeon Phi
}

inline bool try_avxifma() NOEXCEPT
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
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx)       // AVX2
        && eax >= cpu7_1::subleaf                   // Subleaf 1
        && get_cpu(eax, ebx, ecx, edx, cpu7_1::leaf, cpu7_1::subleaf)
        && get_bit<cpu7_1::avxifma_eax_bit>(eax);   // AVXIFMA
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

inline bool try_aesni() NOEXCEPT
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
        && get_bit<cpu1_0::pclmulqdq_ecx_bit>(ecx)  // PCLMULQDQ
        && get_bit<cpu1_0::aes_ecx_bit>(ecx);       // AES
}

inline bool try_vaes() NOEXCEPT
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
        && get_bit<cpu1_0::pclmulqdq_ecx_bit>(ecx)  // PCLMULQDQ
        && get_bit<cpu1_0::aes_ecx_bit>(ecx)        // AES
        && get_bit<cpu1_0::xsave_ecx_bit>(ecx)      // XSAVE
        && get_bit<cpu1_0::avx_ecx_bit>(ecx)        // AVX
        && get_xcr(extended, xcr0::feature)
        && get_bit<xcr0::sse_bit>(extended)
        && get_bit<xcr0::avx_bit>(extended)
        && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx)       // AVX2
        && get_bit<cpu7_0::vaes_ecx_bit>(ecx)       // VAES
        && get_bit<cpu7_0::vpclmulqdq_ecx_bit>(ecx);// VPCLMULQDQ
}

/// Runtime checks for ARM NEON and CRYPTO availability.
/// ---------------------------------------------------------------------------

inline bool try_neon() NOEXCEPT
{
#if defined(HAVE_ARM)
    #if defined(HAVE_LINUX)
        #if defined(HAVE_ARM64)
            constexpr auto hwcap = HWCAP_ASIMD;
        #else
            constexpr auto hwcap = HWCAP_NEON;
        #endif
        const auto caps = getauxval(AT_HWCAP);
        return to_bool(bit_and<uint64_t>(caps, hwcap));
    #elif defined(HAVE_APPLE)
        int value{};
        auto size = sizeof(int);
        sysctlbyname("hw.optional.neon", &value, &size, nullptr, zero);
        return to_bool(value);
    #elif defined(HAVE_MSVC)
        constexpr auto neon_flag = PF_ARM_NEON_INSTRUCTIONS_AVAILABLE;
        return to_bool(::IsProcessorFeaturePresent(neon_flag));
    #else
        return false;
    #endif
#else
    return false;
#endif
}

inline bool try_crypto() NOEXCEPT
{
#if defined(HAVE_ARM)
    #if defined(HAVE_LINUX)
        const auto caps = getauxval(AT_HWCAP);
        return
            to_bool(bit_and<uint64_t>(caps, HWCAP_AES)) &&
            to_bool(bit_and<uint64_t>(caps, HWCAP_SHA1)) &&
            to_bool(bit_and<uint64_t>(caps, HWCAP_SHA2));
    #elif defined(HAVE_APPLE)
        int aes{}, sha1{}, sha256{};
        auto size = sizeof(int);
        sysctlbyname("hw.optional.armv8_aes", &aes, &size, nullptr, zero);
        sysctlbyname("hw.optional.armv8_sha1", &sha1, &size, nullptr, zero);
        sysctlbyname("hw.optional.armv8_sha256",&sha256, &size, nullptr, zero);
        return to_bool(aes) && to_bool(sha1) && to_bool(sha256);
    #elif defined(HAVE_MSVC)
        constexpr auto crypto_flag = PF_ARM_V8_CRYPTO_INSTRUCTIONS_AVAILABLE;
        return to_bool(::IsProcessorFeaturePresent(crypto_flag));
    #else
        return false;
    #endif
#else
    return false;
#endif
}

} // namespace system
} // namespace libbitcoin

#endif

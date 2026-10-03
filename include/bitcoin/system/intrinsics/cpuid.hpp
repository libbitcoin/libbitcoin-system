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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_CPUID_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_CPUID_HPP

#include <bitcoin/system/define.hpp>

#if defined(HAVE_APPLE)
    #include <sys/sysctl.h>
#endif
#if defined(HAVE_ARM) && defined(HAVE_LINUX)
    #include <sys/auxv.h>
    #include <asm/hwcap.h>
#endif

/// Common CPU instructions used to locate CPU features.

namespace libbitcoin {
namespace system {

#if defined(HAVE_XCPU)

inline bool read_xcr(uint64_t& value, uint32_t index) noexcept
{
#if defined(HAVE_XGETBV)
    value = _xgetbv(index);
    return true;
#elif defined(HAVE_XCPU_ASSEMBLY)
    uint32_t a{}, d{};
    __asm__("xgetbv" : "=a"(a), "=d"(d) : "c"(index));
    value = (static_cast<uint64_t>(d) << 32) | a;
    return true;
#else
    return false;
#endif
}

inline bool read_cpu(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d,
    uint32_t leaf, uint32_t subleaf) noexcept
{
#if defined(HAVE_XCPUIDEX)
    int out[4]{};
    __cpuidex(&out[0], leaf, subleaf);
    a = out[0];
    b = out[1];
    c = out[2];
    d = out[3];
    return true;
#elif defined(HAVE_XCPUID_COUNT)
    __cpuid_count(a, b, c, d, leaf, subleaf);
    return true;
#elif defined(HAVE_XCPU_ASSEMBLY)
    __asm__("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "0"(leaf), "2"(subleaf));
    return true;
#else
    return false;
#endif
}

// macOS enables avx512 state (xcr0 bits 5-7) on first use.
inline bool get_xcr(uint64_t& value, uint32_t index) noexcept
{
#if defined(HAVE_APPLE)
    constexpr uint64_t avx512_state = 0xe0;
    int avx512{};
    auto size = sizeof(int);
    if (!read_xcr(value, index))
        return false;

    if (index == 0 && sysctlbyname("hw.optional.avx512f", &avx512, &size,
        nullptr, 0) == 0 && avx512 != 0)
        value |= avx512_state;

    return true;
#else
    return read_xcr(value, index);
#endif
}

// A leaf above the highest of its range (standard or extended) is invalid.
inline bool get_cpu(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d,
    uint32_t leaf, uint32_t subleaf) noexcept
{
    constexpr uint32_t extended = 0x80000000;
    return read_cpu(a, b, c, d, leaf & extended, 0) && leaf <= a
        && read_cpu(a, b, c, d, leaf, subleaf);
}

#else // HAVE_XCPU

inline bool get_xcr(uint64_t&, uint32_t) noexcept
{
    return false;
}

inline bool get_cpu(uint32_t&, uint32_t&, uint32_t&, uint32_t&, uint32_t,
    uint32_t) noexcept
{
    return false;
}

#endif // HAVE_XCPU

/// ARM features located by the operating system.
enum class arm_feature
{
    neon,
    aes,
    pmull,
    sha1,
    sha256,
    sha3,
    sha512
};

#if defined(HAVE_ARM)

#if defined(HAVE_APPLE)

// The legacy name is read only when the name is not defined.
inline bool read_sysctl(const char* name, const char* legacy = nullptr) noexcept
{
    int value{};
    auto size = sizeof(int);
    if (sysctlbyname(name, &value, &size, nullptr, 0) == 0)
        return value != 0;

    if (legacy == nullptr)
        return false;

    size = sizeof(int);
    return sysctlbyname(legacy, &value, &size, nullptr, 0) == 0 && value != 0;
}

#endif // HAVE_APPLE

inline bool get_arm(arm_feature feature) noexcept
{
#if defined(HAVE_LINUX) && defined(HAVE_ARM64)
    const auto caps = getauxval(AT_HWCAP);
    switch (feature)
    {
        case arm_feature::neon:
            return (caps & HWCAP_ASIMD) != 0;
        case arm_feature::aes:
            return (caps & HWCAP_AES) != 0;
        case arm_feature::pmull:
            return (caps & HWCAP_PMULL) != 0;
        case arm_feature::sha1:
            return (caps & HWCAP_SHA1) != 0;
        case arm_feature::sha256:
            return (caps & HWCAP_SHA2) != 0;
        case arm_feature::sha3:
            return (caps & HWCAP_SHA3) != 0;
        case arm_feature::sha512:
            return (caps & HWCAP_SHA512) != 0;
        default:
            return false;
    }
#elif defined(HAVE_LINUX)
    return feature == arm_feature::neon &&
        (getauxval(AT_HWCAP) & HWCAP_NEON) != 0;
#elif defined(HAVE_APPLE)
    switch (feature)
    {
        case arm_feature::neon:
            return read_sysctl("hw.optional.arm.AdvSIMD", "hw.optional.neon");
        case arm_feature::aes:
            return read_sysctl("hw.optional.arm.FEAT_AES");
        case arm_feature::pmull:
            return read_sysctl("hw.optional.arm.FEAT_PMULL");
        case arm_feature::sha1:
            return read_sysctl("hw.optional.arm.FEAT_SHA1");
        case arm_feature::sha256:
            return read_sysctl("hw.optional.arm.FEAT_SHA256");
        case arm_feature::sha3:
            return read_sysctl("hw.optional.arm.FEAT_SHA3",
                "hw.optional.armv8_2_sha3");
        case arm_feature::sha512:
            return read_sysctl("hw.optional.arm.FEAT_SHA512",
                "hw.optional.armv8_2_sha512");
        default:
            return false;
    }
#elif defined(HAVE_MSC)
    switch (feature)
    {
        case arm_feature::neon:
            return ::IsProcessorFeaturePresent(
                PF_ARM_NEON_INSTRUCTIONS_AVAILABLE) != FALSE;
        case arm_feature::aes:
        case arm_feature::pmull:
        case arm_feature::sha1:
        case arm_feature::sha256:
            return ::IsProcessorFeaturePresent(
                PF_ARM_V8_CRYPTO_INSTRUCTIONS_AVAILABLE) != FALSE;
        case arm_feature::sha3:
            return ::IsProcessorFeaturePresent(
                PF_ARM_SHA3_INSTRUCTIONS_AVAILABLE) != FALSE;
        case arm_feature::sha512:
            return ::IsProcessorFeaturePresent(
                PF_ARM_SHA512_INSTRUCTIONS_AVAILABLE) != FALSE;
        default:
            return false;
    }
#else
    return false;
#endif
}

#else // HAVE_ARM

inline bool get_arm(arm_feature) noexcept
{
    return false;
}

#endif // HAVE_ARM

} // namespace system
} // namespace libbitcoin

#endif

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
#include <bitcoin/system/crypto/intrinsics/intrinsics.hpp>

#include <array>
#include <immintrin.h>

#ifdef HAVE_MSC
    // __cpuidex
    #include <intrin.h>
#endif

#include <iterator>
#include <bitcoin/system/crypto/external/external.hpp>
#include <bitcoin/system/crypto/hash.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>

namespace libbitcoin {
namespace system {
namespace intrinsics {

namespace cpu1_0
{
#if defined(WITH_AVX2) || defined(WITH_SSE41) || defined(WITH_SSE4) || defined(WITH_SHANI)
    constexpr auto leaf = 1_u32;
    constexpr auto subleaf = 0_u32;
    constexpr auto sse4_ecx_bit = 19_size;
#endif
#if defined(WITH_AVX2)
    constexpr auto xsave_ecx_bit = 27_size;
    constexpr auto avx_ecx_bit = 28_size;
#endif
}

namespace cpu7_0
{
#if defined(WITH_AVX2) || defined(WITH_SHANI)
    constexpr auto leaf = 7_u32;
    constexpr auto subleaf = 0_u32;
#endif
#if defined(WITH_AVX2)
    constexpr auto avx2_ebx_bit = 5_size;
#endif
#if defined(WITH_SHANI)
    constexpr auto shani_ebx_bit = 29_size;
#endif
}

namespace xcr0
{
#if defined(WITH_AVX2)
    constexpr auto feature = 0_u32;
    constexpr auto sse_bit = 1_size;
    constexpr auto avx_bit = 2_size;
#endif
}

// In msvc intrinsics always compile, however on other platforms this support
// is unreliable, so we revert to the lowest common interface (assembly).

#if defined(WITH_AVX2)
static bool xgetbv(uint64_t& value, uint32_t index) NOEXCEPT
{
#if defined(__x86_64__) || defined(__amd64__) || defined(__i386__)
    // Compile error: built-in _xgetbv requires target feature xsave.
    // But xsave can only be determined at run time, so must use assembly.
    ////value = _xgetbv(index);
    uint32_t a, d;
    __asm__("xgetbv" : "=a"(a), "=d"(d) : "c"(index));
    value = shift_left<uint64_t>(d, 32_size) | a;
    return true;
#elif defined(_M_X64) || defined(_M_AMD64)  || defined(_M_IX86)
    value = _xgetbv(index);
    return true;
#else
    value = {};
    return false;
#endif
}
#endif

#if defined(WITH_AVX2) || defined(WITH_SSE41) || defined(WITH_SSE4) || defined(WITH_SHANI)
static bool cpuid_ex(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d,
    uint32_t leaf, uint32_t subleaf) NOEXCEPT
{
#if defined(__x86_64__) || defined(__amd64__) || defined(__i386__)
    // __cpuid_count too commonly undefined, so just always use assembly.
    ////__cpuid_count(leaf, subleaf, a, b, c, d);
    __asm__("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "0"(leaf), "2"(subleaf));
    return true;
#elif defined(_M_X64) || defined(_M_AMD64) || defined(_M_IX86)
    int out[4];
    __cpuidex(out, leaf, subleaf);
    a = sign_cast<uint32_t>(out[0]);
    b = sign_cast<uint32_t>(out[1]);
    c = sign_cast<uint32_t>(out[2]);
    d = sign_cast<uint32_t>(out[3]);
    return true;
#else
    return false;
#endif
}
#endif

// runtime checks for intrinsic availability
// ----------------------------------------------------------------------------

#ifdef WITH_AVX2
inline bool try_avx2() NOEXCEPT
{
    uint64_t extended;
    uint32_t eax, ebx, ecx, edx;
    return cpuid_ex(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_right(ecx, cpu1_0::sse4_ecx_bit)
        && get_right(ecx, cpu1_0::xsave_ecx_bit)
        && get_right(ecx, cpu1_0::avx_ecx_bit)
        && xgetbv(extended, xcr0::feature)
        && get_right(extended, xcr0::sse_bit)
        && get_right(extended, xcr0::avx_bit)
        && cpuid_ex(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_right(ebx, cpu7_0::avx2_ebx_bit);
}
bool have_avx2() NOEXCEPT
{
    static auto enable = try_avx2();
    return enable;
}
#endif
#ifdef WITH_NEON
inline bool try_neon() NOEXCEPT
{
    // TODO: test for ARM Neon.
    return false;
}
bool have_neon() NOEXCEPT
{
    static auto enable = try_neon();
    return enable;
}
#endif
#ifdef WITH_SSE41
inline bool try_sse41() NOEXCEPT
{
    uint32_t eax, ebx, ecx, edx;
    return cpuid_ex(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_right(ecx, cpu1_0::sse4_ecx_bit);
}
bool have_sse41() NOEXCEPT
{
    static auto enable = try_sse41();
    return enable;
}
#endif
#ifdef WITH_SSE4
inline bool try_sse4() NOEXCEPT
{
#if defined(_M_X64) || defined(_M_AMD64)  || defined(_M_IX86)
    // sha256_x1_sse4() is not yet defined in this context.
    return false;
#else
    uint32_t eax, ebx, ecx, edx;
    return cpuid_ex(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_right(ecx, cpu1_0::sse4_ecx_bit);
#endif
}
bool have_sse4() NOEXCEPT
{
    static auto enable = try_sse4();
    return enable;
}
#endif
#ifdef WITH_SHANI
inline bool try_shani() NOEXCEPT
{
    uint32_t eax, ebx, ecx, edx;
    return cpuid_ex(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && (eax >= cpu7_0::leaf)
        && get_right(ecx, cpu1_0::sse4_ecx_bit)
        && cpuid_ex(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_right(ebx, cpu7_0::shani_ebx_bit);
}
bool have_shani() NOEXCEPT
{
    static auto enable = try_shani();
    return enable;
}
#endif

#ifdef WITH_AVX2
void sha256_x1_avx2(uint32_t state[8], const uint8_t block[64]) NOEXCEPT;
void double_sha256_x8_avx2(uint8_t* out, const uint8_t in[8 * 64]) NOEXCEPT;
#endif
#ifdef WITH_NEON
void sha256_x1_neon(uint32_t state[8], const uint8_t block[64]) NOEXCEPT;
////void double_sha256_x4_neon(uint8_t* out, const uint8_t in[4 * 64]) NOEXCEPT;
void double_sha256_x1_neon(uint8_t* out, const uint8_t in[1 * 64]) NOEXCEPT;
#endif
#ifdef WITH_SSE41
void sha256_x1_sse41(uint32_t state[8], const uint8_t block[64]) NOEXCEPT;
void double_sha256_x4_sse41(uint8_t* out, const uint8_t in[4 * 64]) NOEXCEPT;
#endif
#ifdef WITH_SSE4
void sha256_x1_sse4(uint32_t state[8], const uint8_t block[64]) NOEXCEPT;
////void double_sha256_x4_sse4(uint8_t* out, const uint8_t in[4 * 64]) NOEXCEPT;
void double_sha256_x1_sse4(uint8_t* out, const uint8_t in[1 * 64]) NOEXCEPT;
#endif
#ifdef WITH_SHANI
void sha256_x1_shani(uint32_t state[8], const uint8_t block[64]) NOEXCEPT;
void double_sha256_x2_shani(uint8_t* out, const uint8_t in[2 * 64]) NOEXCEPT;
void double_sha256_x1_shani(uint8_t* out, const uint8_t in[1 * 64]) NOEXCEPT;
#endif

// single sha256
// ----------------------------------------------------------------------------

// Iterate over contibuous blocks, with hash accumulation in 'state'. This can
// be used to replace SHA256Transform.
void sha256_single(uint32_t state[8], const uint8_t block[64]) NOEXCEPT
{
#ifdef WITH_SHANI
    if (have_shani())
    {
        sha256_x1_shani(state, block);
        return;
    }
#endif
////#ifdef WITH_AVX2
////    if (have_avx2())
////    {
////        sha256_x1_avx2(state, block);
////        return;
////    }
////#endif
////#ifdef WITH_SSE41
////    if (have_sse41())
////    {
////        sha256_x1_sse41(state, block);
////        return;
////    }
////#endif
#ifdef WITH_NEON
    if (have_neon())
    {
        sha256_x1_neon(state, block);
        return;
    }
#endif
#ifdef WITH_SSE4
    if (have_sse4())
    {
        sha256_x1_sse4(state, block);
        return;
    }
#endif

    sha256_x1_portable(state, block);
}

// paired double sha256
// ----------------------------------------------------------------------------

// Multiple blocks are hashed independently into an array of hash values stored
// into 'out'. This is used to reduce hash sets during merkle tree computation.
void sha256_paired_double(uint8_t out[], const uint8_t in[],
    size_t blocks) NOEXCEPT
{
    constexpr auto block_size = 64_size;

#ifdef WITH_SHANI
    if (have_shani())
    {
        while (blocks >= 2_size)
        {
            double_sha256_x2_shani(out, in);
            std::advance(out, hash_size * 2_size);
            std::advance(in, block_size * 2_size);
            blocks -= 2_size;
        }

        while (blocks >= 1_size)
        {
            double_sha256_x1_shani(out, in);
            std::advance(out, hash_size * 1_size);
            std::advance(in, block_size * 1_size);
            blocks -= 1_size;
        }
    }
#endif
#ifdef WITH_AVX2
    if (have_avx2())
    {
        while (blocks >= 8_size)
        {
            double_sha256_x8_avx2(out, in);
            std::advance(out, hash_size * 8_size);
            std::advance(in, block_size * 8_size);
            blocks -= 8_size;
        }
    }
#endif
#ifdef WITH_SSE41
    if (have_sse41())
    {
        while (blocks >= 4_size)
        {
            // BUGBUG: throws on 32 bit builds.
            double_sha256_x4_sse41(out, in);
            std::advance(out, hash_size * 4_size);
            std::advance(in, block_size * 4_size);
            blocks -= 4_size;
        }
    }
#endif
#ifdef WITH_NEON
    if (have_neon())
    {
        while (blocks >= 1_size)
        {
            double_sha256_x1_neon(out, in);
            std::advance(out, hash_size * 1_size);
            std::advance(in, block_size * 1_size);
            blocks -= 1_size;
        }
    }
#endif
#ifdef WITH_SSE4
    if (have_sse4())
    {
        while (blocks >= 1_size)
        {
            double_sha256_x1_sse4(out, in);
            std::advance(out, hash_size * 1_size);
            std::advance(in, block_size * 1_size);
            blocks -= 1_size;
        }
    }
#endif
    while (blocks >= 1_size)
    {
        double_sha256_x1_portable(out, in);
        std::advance(out, hash_size * 1_size);
        std::advance(in, block_size * 1_size);
        blocks -= 1_size;
    }
}

void double_sha256_x1_portable(uint8_t out[], const uint8_t in[1 * 64]) NOEXCEPT
{
    constexpr auto count = 32_size / sizeof(uint32_t);

    auto state = sha256_initial;
    auto buffer = sha256x2_buffer;
    sha256_x1_portable(state.data(), in);
    sha256_x1_portable(state.data(), sha256x2_padding.data());
    to_big_endian_set(
        narrowing_array_cast<uint32_t, count>(buffer),
        array_cast<uint32_t, count>(state));

    state = sha256_initial;
    sha256_x1_portable(state.data(), buffer.data());
    to_big_endian_set(
        unsafe_array_cast<uint32_t, count>(&out[0]),
        array_cast<uint32_t, count>(state));
}

} // namespace intrinsics
} // namespace system
} // namespace libbitcoin

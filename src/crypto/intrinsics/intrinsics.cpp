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

#include <cstddef>
#include <cstdint>
#include <immintrin.h>

#ifdef _MSC_VER
    #include <intrin.h> // __cpuidex
#endif // _MSC_VER

#include <iterator>
#include <bitcoin/system/crypto/external/sha256.h>
#include <bitcoin/system/crypto/hash.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/serial/serial.hpp>

namespace libbitcoin {
namespace system {
namespace intrinsics {

// In msvc intrinsics always compile, however on other platforms this support
// is unreliable, so we revert to the lowest common interface (assembler).

bool xgetbv(uint64_t& value, uint32_t index) noexcept
{
#if defined(__x86_64__) || defined(__amd64__) || defined(__i386__)
    // Compile error: built-in _xgetbv requires target feature xsave.
    // But xsave can only be determined at run time, so must use assembly.
    ////value = _xgetbv(index);
    uint32_t a, d;
    __asm__("xgetbv" : "=a"(a), "=d"(d) : "c"(index));
    value = shift_left<uint64_t>(d, 32) | a;
    return true;
#elif defined(_M_X64) || defined(_M_AMD64)  || defined(_M_IX86)
    value = _xgetbv(index);
    return true;
#else
    return false;
#endif
}

bool cpuid_ex(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d,
    uint32_t leaf, uint32_t subleaf) noexcept
{
#if defined(__x86_64__) || defined(__amd64__) || defined(__i386__)
    // __cpuid_count too commonly undefined, so just always use assembly.
    ////__cpuid_count(leaf, subleaf, a, b, c, d);
    __asm__("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "0"(leaf), "2"(subleaf));
    return true;
#elif defined(_M_X64) || defined(_M_AMD64) || defined(_M_IX86)
    int out[4];
    __cpuidex(out, leaf, subleaf);
    a = static_cast<uint32_t>(out[0]);
    b = static_cast<uint32_t>(out[1]);
    c = static_cast<uint32_t>(out[2]);
    d = static_cast<uint32_t>(out[3]);
    return true;
#else
    return false;
#endif
}

// runtime checks for intrinsic availability
// ----------------------------------------------------------------------------

#ifdef WITH_AVX2
inline bool try_avx2() noexcept
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
bool have_avx2() noexcept
{
    static auto enable = try_avx2();
    return enable;
}
#endif

#ifdef WITH_SSE41
inline bool try_sse41() noexcept
{
    uint32_t eax, ebx, ecx, edx;
    return cpuid_ex(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_right(ecx, cpu1_0::sse4_ecx_bit);
}
bool have_sse41() noexcept
{
    static auto enable = try_sse41();
    return enable;
}
#endif

#ifdef WITH_NEON
inline bool try_neon() noexcept
{
    // TODO: test for ARM Neon.
    return false;
}
bool have_neon() noexcept
{
    static auto enable = try_neon();
    return enable;
}
#endif

#ifdef WITH_SHANI
inline bool try_shani() noexcept
{
    uint32_t eax, ebx, ecx, edx;
    return cpuid_ex(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_right(ecx, cpu1_0::sse4_ecx_bit)
        && cpuid_ex(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_right(ebx, cpu7_0::shani_ebx_bit);
}
bool have_shani() noexcept
{
    static auto enable = try_shani();
    return enable;
}
#endif

#ifndef WITH_SSE4
inline bool try_sse4() noexcept
{
    uint32_t eax, ebx, ecx, edx;
    return cpuid_ex(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_right(ecx, cpu1_0::sse4_ecx_bit);
}
bool have_sse4() noexcept
{
    static auto enable = try_sse4();
    return enable;
}
#endif

#ifdef WITH_AVX2
////void single_sha256_64x8_avx2(uint8_t* output, const uint8_t* input) noexcept;
void double_sha256_64x8_avx2(uint8_t* output, const uint8_t* input) noexcept;
#endif
#ifdef WITH_SSE41
////void single_sha256_64x4_sse41(uint8_t* output, const uint8_t* input) noexcept;
void double_sha256_64x4_sse41(uint8_t* output, const uint8_t* input) noexcept;
#endif
#ifdef WITH_NEON
////void single_sha256_64x4_neon(uint8_t* output, const uint8_t* input) noexcept;
void double_sha256_64x4_neon(uint8_t* output, const uint8_t* input) noexcept;
#endif
#ifdef WITH_SHANI
////void single_sha256_64x2_shani(uint8_t* output, const uint8_t* input) noexcept;
void double_sha256_64x1_shani(uint8_t* output, const uint8_t* input) noexcept;
void double_sha256_64x2_shani(uint8_t* output, const uint8_t* input) noexcept;
#endif
#ifdef WITH_SSE4
////void single_sha256_64x1_sse4(uint8_t* output, const uint8_t* input) noexcept;
void double_sha256_64x1_sse4(uint8_t* output, const uint8_t* input) noexcept;
#else
void single_sha256_64x1_port(uint8_t* output, const uint8_t* input) noexcept;
void double_sha256_64x1_port(uint8_t* output, const uint8_t* input) noexcept;
#endif

// single sha256 64
// ----------------------------------------------------------------------------

constexpr size_t hash_size = SHA256_DIGEST_LENGTH;
constexpr size_t block_size = SHA256_BLOCK_LENGTH;

void single_sha256_64(uint8_t*, const uint8_t*, size_t) noexcept
{
    // TODO: similar to double_sha256_64 implementation.
}

void single_sha256_64x1_port(uint8_t*, const uint8_t*) noexcept
{
    // TODO: build on existing implementation.
    // void SHA256(const uint8_t* in, size_t length, uint8_t digest[]);
}

// double sha256 64
// ----------------------------------------------------------------------------

void double_sha256_64(uint8_t* out, const uint8_t* in, size_t blocks) noexcept
{
#ifdef WITH_AVX2
    if (have_avx2() && !have_shani())
    {
        while (blocks >= 8)
        {
            double_sha256_64x8_avx2(out, in);
            std::advance(out, hash_size * 8);
            std::advance(in, block_size * 8);
            blocks -= 8;
        }
    }
#endif
#ifdef WITH_SSE41
    if (have_sse41() && !have_shani())
    {
        while (blocks >= 4)
        {
            double_sha256_64x4_sse41(out, in);
            std::advance(out, hash_size * 4);
            std::advance(in, block_size * 4);
            blocks -= 4;
        }
    }
#endif
#ifdef WITH_NEON
    if (have_neon())
    {
        while (blocks >= 4)
        {
            double_sha256_64x4_neon(out, in);
            std::advance(out, hash_size * 4);
            std::advance(in, block_size * 4);
            blocks -= 4;
        }
    }
#endif
#ifdef WITH_SHANI
    if (have_shani())
    {
        while (blocks >= 2)
        {
            double_sha256_64x2_shani(out, in);
            std::advance(out, hash_size * 2);
            std::advance(in, block_size * 2);
            blocks -= 2;
        }

        while (blocks >= 1)
        {
            double_sha256_64x1_shani(out, in);
            std::advance(out, hash_size * 1);
            std::advance(in, block_size * 1);
            blocks -= 1;
        }
    }
#endif
#ifdef WITH_SSE4
    if (have_sse4())
    {
        while (blocks >= 1)
        {
            double_sha256_64x1_sse4(out, in);
            std::advance(out, hash_size * 1);
            std::advance(in, block_size * 1);
            blocks -= 1;
        }
    }
#else // portable
    while (blocks >= 1)
    {
        double_sha256_64x1_port(out, in);
        std::advance(out, hash_size * 1);
        std::advance(in, block_size * 1);
        blocks -= 1;
    }
#endif
}

void inline initialize(uint32_t state[8])
{
    state[0] = 0x6a09e667ul;
    state[1] = 0xbb67ae85ul;
    state[2] = 0x3c6ef372ul;
    state[3] = 0xa54ff53aul;
    state[4] = 0x510e527ful;
    state[5] = 0x9b05688cul;
    state[6] = 0x1f83d9abul;
    state[7] = 0x5be0cd19ul;
}

static void to_big_endian_vector(uint8_t* to, const uint32_t* from,
    size_t size)
{
    size_t i;
    for (i = 0; i < size / sizeof(uint32_t); i++)
    {
        to_big_endian(to + i * sizeof(uint32_t), from[i]);
    }
}

void double_sha256_64x1_port(uint8_t* out, const uint8_t* in) noexcept
{
    uint32_t state[8];

    static const uint8_t padding1[64] =
    {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0
    };

    uint8_t buffer2[64] =
    {
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0
    };

    initialize(state);
    SHA256Transform(state, in);
    SHA256Transform(state, padding1);
    to_big_endian_vector(buffer2, state, SHA256_DIGEST_LENGTH);

    initialize(state);
    SHA256Transform(state, buffer2);
    to_big_endian_vector(out, state, SHA256_DIGEST_LENGTH);
}

} // namespace intrinsics
} // namespace system
} // namespace libbitcoin

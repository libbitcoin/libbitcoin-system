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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_PLATFORMS_INTEL_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_PLATFORMS_INTEL_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

#if defined(HAVE_XCPU)
#include <immintrin.h>

namespace libbitcoin {
namespace system {

/// Provide 8 bit shifts to complete matrix.
/// ---------------------------------------------------------------------------
/// These are no epi8 versions of these.

#if defined(HAVE_128)

// SSE2
template <auto B>
inline auto mm_srli_epi8(auto a) NOEXCEPT
{
    static_assert(B < 8);
    constexpr auto mask = shift_right(0xff_u8, B);
    return _mm_and_si128(_mm_srli_epi16(a, B), _mm_set1_epi8(mask));
}

// SSE2
template <auto B>
inline auto mm_slli_epi8(auto a) NOEXCEPT
{
    static_assert(B < 8);
    constexpr auto mask = shift_left(0xff_u8, B);
    return _mm_and_si128(_mm_slli_epi16(a, B), _mm_set1_epi8(mask));
}

#endif // HAVE_128

#if defined(HAVE_256)

// AVX2
template <auto B>
inline auto mm256_srli_epi8(auto a) NOEXCEPT
{
    static_assert(B < 8);
    constexpr auto mask = shift_right(0xff_u8, B);
    return _mm256_and_si256(_mm256_srli_epi16(a, B), _mm256_set1_epi8(mask));
}

// AVX2
template <auto B>
inline auto mm256_slli_epi8(auto a) NOEXCEPT
{
    static_assert(B < 8);
    constexpr auto mask = shift_left(0xff_u8, B);
    return _mm256_and_si256(_mm256_slli_epi16(a, B), _mm256_set1_epi8(mask));
}

#endif // HAVE_256

#if defined(HAVE_512)

// AVX512BW
template <auto B>
inline auto mm512_srli_epi8(auto a) NOEXCEPT
{
    static_assert(B < 8);
    constexpr auto mask = shift_right(0xff_u8, B);
    return _mm512_and_si512(_mm512_srli_epi16(a, B), _mm512_set1_epi8(mask));
}

// AVX512BW
template <auto B>
inline auto mm512_slli_epi8(auto a) NOEXCEPT
{
    static_assert(B < 8);
    constexpr auto mask = shift_left(0xff_u8, B);
    return _mm512_and_si512(_mm512_slli_epi16(a, B), _mm512_set1_epi8(mask));
}

#endif // HAVE_512

/// Provide 64 bit extractors in 32 bit builds to complete matrix.
/// ---------------------------------------------------------------------------
/// There are no 32 bit build versions of these.

#if defined(HAVE_128)

// SSE2
template <auto Lane>
inline uint64_t mm_extract_epi64(auto a) NOEXCEPT
{
    // _mm_extract_epi64 is not available in 32 bit builds.
    static_assert(Lane < 2);
    alignas(16) uint64_t buffer[2];
    _mm_storeu_si128(pointer_cast<__m128i>(buffer), a);
    return buffer[Lane];
}

#endif // HAVE_128

#if defined(HAVE_256)

// AVX
template <auto Lane>
inline uint64_t mm256_extract_epi64(auto a) NOEXCEPT
{
    // _mm256_extract_epi64 is not available in 32 bit builds.
    static_assert(Lane < 4);
    alignas(32) uint64_t buffer[4];
    _mm256_storeu_si256(pointer_cast<__m256i>(buffer), a);
    return buffer[Lane];
}

#endif // HAVE_256

/// Provide mm512 extractors to complete matrix.
/// ---------------------------------------------------------------------------
/// There are no intrisic versions of these.

#if defined(HAVE_512)

// AVX512F
template <auto Lane>
inline uint8_t mm512_extract_epi8(auto a) NOEXCEPT
{
    static_assert(Lane < 64);
    alignas(64) uint8_t buffer[64];
    _mm512_storeu_si512(pointer_cast<__m512i>(buffer), a);
    return buffer[Lane];
}

// AVX512F
template <auto Lane>
inline uint16_t mm512_extract_epi16(auto a) NOEXCEPT
{
    static_assert(Lane < 32);
    alignas(64) uint16_t buffer[32];
    _mm512_storeu_si512(pointer_cast<__m512i>(buffer), a);
    return buffer[Lane];
}

// AVX512F
template <auto Lane>
inline uint32_t mm512_extract_epi32(auto a) NOEXCEPT
{
    static_assert(Lane < 16);
    alignas(64) uint32_t buffer[16];
    _mm512_storeu_si512(pointer_cast<__m512i>(buffer), a);
    return buffer[Lane];
}

// AVX512F
template <auto Lane>
inline uint64_t mm512_extract_epi64(auto a) NOEXCEPT
{
    // _mm_cvtsi128_si64 is not available in 32 bit builds.
    static_assert(Lane < 8);
    alignas(64) uint64_t buffer[8];
    _mm512_storeu_si512(pointer_cast<__m512i>(buffer), a);
    return buffer[Lane];
}

#endif // HAVE_512

} // namespace system
} // namespace libbitcoin

#endif // HAVE_XCPU

#endif

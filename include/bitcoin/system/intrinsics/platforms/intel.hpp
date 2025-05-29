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
/// These are only defined for 16/32/64 bits.

#if defined(HAVE_128)

// SSE2
template <auto B>
inline auto mm_srli_epi8(auto a) NOEXCEPT
{
    constexpr auto mask = shift_right(0xff_u8, B);
    return _mm_and_si128(_mm_srli_epi16(a, B), _mm_set1_epi8(mask));
}

// SSE2
template <auto B>
inline auto mm_slli_epi8(auto a) NOEXCEPT
{
    constexpr auto mask = shift_left(0xff_u8, B);
    return _mm_and_si128(_mm_slli_epi16(a, B), _mm_set1_epi8(mask));
}

#endif // HAVE_128

#if defined(HAVE_256)

// AVX2
template <auto B>
inline auto mm256_srli_epi8(auto a) NOEXCEPT
{
    constexpr auto mask = shift_right(0xff_u8, B);
    return _mm256_and_si256(_mm256_srli_epi16(a, B), _mm256_set1_epi8(mask));
}

// AVX2
template <auto B>
inline auto mm256_slli_epi8(auto a) NOEXCEPT
{
    constexpr auto mask = shift_left(0xff_u8, B);
    return _mm256_and_si256(_mm256_slli_epi16(a, B), _mm256_set1_epi8(mask));
}

#endif // HAVE_256

#if defined(HAVE_512)

// AVX512BW
template <auto B>
inline auto mm512_srli_epi8(auto a) NOEXCEPT
{
    constexpr auto mask = shift_right(0xff_u8, B);
    return _mm512_and_si512(_mm512_srli_epi16(a, B), _mm512_set1_epi8(mask));
}

// AVX512BW
template <auto B>
inline auto mm512_slli_epi8(auto a) NOEXCEPT
{
    constexpr auto mask = shift_left(0xff_u8, B);
    return _mm512_and_si512(_mm512_slli_epi16(a, B), _mm512_set1_epi8(mask));
}

#endif // HAVE_512

/// Provide mm512 extractors to complete matrix.
/// ---------------------------------------------------------------------------
/// There are no native versions of these.

#if defined(HAVE_512)

// AVX512BW
template <auto Lane>
inline uint8_t mm512_extract_epi8(auto a) NOEXCEPT
{
    constexpr __mmask64 mask = shift_left(1_u64, Lane);
    return narrow_sign_cast<uint8_t>(_mm_cvtsi128_si32(
        _mm512_castsi512_si128(_mm512_maskz_compress_epi8(mask, a))));
}

// AVX512BW
template <auto Lane>
inline uint16_t mm512_extract_epi16(auto a) NOEXCEPT
{
    constexpr __mmask32 mask = shift_left(1_u32, Lane);
    return narrow_sign_cast<uint16_t>(_mm_cvtsi128_si32(
        _mm512_castsi512_si128(_mm512_maskz_compress_epi16(mask, a))));
}

// AVX512F
template <auto Lane>
inline uint32_t mm512_extract_epi32(auto a) NOEXCEPT
{
    constexpr __mmask16 mask = shift_left(1_u16, Lane);
    return sign_cast<uint32_t>(_mm_cvtsi128_si32(
        _mm512_castsi512_si128(_mm512_maskz_compress_epi32(mask, a))));
}

// AVX512F
template <auto Lane>
inline uint64_t mm512_extract_epi64(auto a) NOEXCEPT
{
    constexpr __mmask8 mask = shift_left(1_u8, Lane);
    const auto value = _mm512_castsi512_si128(
        _mm512_maskz_compress_epi64(mask, a));

    if constexpr (have_64b)
    {
        // This is not available in 32 bit builds.
        return sign_cast<uint64_t>(_mm_cvtsi128_si64(value));
    }
    else
    {
        constexpr auto shift = bits<uint32_t>;
        return bit_or
        (
            shift_left<uint64_t>(_mm_extract_epi32(value, 1), shift),
            wide_sign_cast<uint64_t>(_mm_cvtsi128_si32(value))
        );
    }
}

#endif // HAVE_512

/// Provide 64 bit extractors in 32 bit builds to complete matrix.
/// ---------------------------------------------------------------------------
/// There are no 32 bit versions of these.

#if defined(HAVE_128)

template <auto Lane>
inline uint64_t mm_extract_epi64(auto a) NOEXCEPT
{
    if constexpr (have_64b)
    {
        // This is not available in 32 bit builds.
        return sign_cast<uint64_t>(_mm_extract_epi64(a, Lane));
    }
    else
    {
        constexpr auto shift = bits<uint32_t>;
        constexpr auto lane = shift_left(Lane);
        return bit_or
        (
            shift_left<uint64_t>(_mm_extract_epi32(a, add1(lane)), shift),
            wide_sign_cast<uint64_t>(_mm_extract_epi32(a, lane))
        );
    }
}

#endif // HAVE_128

#if defined(HAVE_256)

template <auto Lane>
inline uint64_t mm256_extract_epi64(auto a) NOEXCEPT
{
    if constexpr (have_64b)
    {
        // This is not available in 32 bit builds.
        return sign_cast<uint64_t>(_mm256_extract_epi64(a, Lane));
    }
    else
    {
        constexpr auto shift = bits<uint32_t>;
        constexpr auto lane = shift_left(Lane);
        return bit_or
        (
            shift_left<uint64_t>(_mm256_extract_epi32(a, add1(lane)), shift),
            wide_sign_cast<uint64_t>(_mm256_extract_epi32(a, lane))
        );
    }
}

#endif // HAVE_256

} // namespace system
} // namespace libbitcoin

#endif // HAVE_XCPU

#endif

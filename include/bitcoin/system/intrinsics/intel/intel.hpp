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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_INTEL_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_INTEL_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

#if defined(HAVE_XCPU)
#include <immintrin.h>

namespace libbitcoin {
namespace system {

/// Byte shuffle selector for rotation (see f::ror/f::rol).
/// ---------------------------------------------------------------------------
/// The selector for byte at index for rotate right by B bits of S-bit words,
/// where B is byte-aligned (B % 8 == 0). A single byte shuffle then performs
/// the rotation of all words in the vector.

template <auto B, auto S>
constexpr uint8_t mm_ror_selector(uint8_t index) NOEXCEPT
{
    constexpr auto size = S / byte_bits;
    constexpr auto skip = (B / byte_bits) % size;
    return possible_narrow_cast<uint8_t>(
        ((index / size) * size) +
        ((index + skip) % size));
}

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
template <typename ...Args>
inline auto mm512_setr_epi8(Args ...args) NOEXCEPT
{
    static_assert(sizeof...(Args) == 64);
    const std_array<uint8_t, sizeof...(Args)> bytes
    {
        args...
    };

    return _mm512_loadu_si512(bytes.data());
}

// AVX512F
template <typename ...Args>
inline auto mm512_setr_epi16(Args ...args) NOEXCEPT
{
    static_assert(sizeof...(Args) == 32);
    const std_array<uint16_t, sizeof...(Args)> words
    {
        args...
    };

    return _mm512_loadu_si512(words.data());
}

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

/// Provide 52 bit fused multiply-add to complete matrix.
/// ---------------------------------------------------------------------------
/// Emulated from 26 bit halves where there is no ifma for the width.

#if defined(HAVE_IFMA_128)

// AVX512IFMA+AVX512VL
inline auto mm_madd52lo_epu64(auto c, auto a, auto b) NOEXCEPT
{
    return _mm_madd52lo_epu64(c, a, b);
}

// AVX512IFMA+AVX512VL
inline auto mm_madd52hi_epu64(auto c, auto a, auto b) NOEXCEPT
{
    return _mm_madd52hi_epu64(c, a, b);
}

#elif defined(HAVE_128)

// SSE2
inline void mm_mul52_epu64(auto& lo, auto& hi, auto a, auto b) NOEXCEPT
{
    const auto mask52 = _mm_set1_epi64x(0x000fffffffffffff);
    const auto mask26 = _mm_set1_epi64x(0x0000000003ffffff);
    const auto x = _mm_and_si128(a, mask52);
    const auto y = _mm_and_si128(b, mask52);
    const auto x0 = _mm_and_si128(x, mask26);
    const auto y0 = _mm_and_si128(y, mask26);
    const auto x1 = _mm_srli_epi64(x, 26);
    const auto y1 = _mm_srli_epi64(y, 26);
    const auto mid = _mm_add_epi64(_mm_mul_epu32(x0, y1), _mm_mul_epu32(x1, y0));
    const auto low = _mm_add_epi64(_mm_mul_epu32(x0, y0),
        _mm_slli_epi64(_mm_and_si128(mid, mask26), 26));
    lo = _mm_and_si128(low, mask52);
    hi = _mm_add_epi64(_mm_add_epi64(_mm_mul_epu32(x1, y1),
        _mm_srli_epi64(mid, 26)), _mm_srli_epi64(low, 52));
}

// SSE2
inline auto mm_madd52lo_epu64(auto c, auto a, auto b) NOEXCEPT
{
    decltype(c) lo{}, hi{};
    mm_mul52_epu64(lo, hi, a, b);
    return _mm_add_epi64(c, lo);
}

// SSE2
inline auto mm_madd52hi_epu64(auto c, auto a, auto b) NOEXCEPT
{
    decltype(c) lo{}, hi{};
    mm_mul52_epu64(lo, hi, a, b);
    return _mm_add_epi64(c, hi);
}

#endif // HAVE_IFMA_128

#if defined(HAVE_IFMA_256)

// AVX512IFMA+AVX512VL
inline auto mm256_madd52lo_epu64(auto c, auto a, auto b) NOEXCEPT
{
    return _mm256_madd52lo_epu64(c, a, b);
}

// AVX512IFMA+AVX512VL
inline auto mm256_madd52hi_epu64(auto c, auto a, auto b) NOEXCEPT
{
    return _mm256_madd52hi_epu64(c, a, b);
}

#elif defined(HAVE_256)

// AVX2
inline void mm256_mul52_epu64(auto& lo, auto& hi, auto a, auto b) NOEXCEPT
{
    const auto mask52 = _mm256_set1_epi64x(0x000fffffffffffff);
    const auto mask26 = _mm256_set1_epi64x(0x0000000003ffffff);
    const auto x = _mm256_and_si256(a, mask52);
    const auto y = _mm256_and_si256(b, mask52);
    const auto x0 = _mm256_and_si256(x, mask26);
    const auto y0 = _mm256_and_si256(y, mask26);
    const auto x1 = _mm256_srli_epi64(x, 26);
    const auto y1 = _mm256_srli_epi64(y, 26);
    const auto mid = _mm256_add_epi64(_mm256_mul_epu32(x0, y1),
        _mm256_mul_epu32(x1, y0));
    const auto low = _mm256_add_epi64(_mm256_mul_epu32(x0, y0),
        _mm256_slli_epi64(_mm256_and_si256(mid, mask26), 26));
    lo = _mm256_and_si256(low, mask52);
    hi = _mm256_add_epi64(_mm256_add_epi64(_mm256_mul_epu32(x1, y1),
        _mm256_srli_epi64(mid, 26)), _mm256_srli_epi64(low, 52));
}

// AVX2
inline auto mm256_madd52lo_epu64(auto c, auto a, auto b) NOEXCEPT
{
    decltype(c) lo{}, hi{};
    mm256_mul52_epu64(lo, hi, a, b);
    return _mm256_add_epi64(c, lo);
}

// AVX2
inline auto mm256_madd52hi_epu64(auto c, auto a, auto b) NOEXCEPT
{
    decltype(c) lo{}, hi{};
    mm256_mul52_epu64(lo, hi, a, b);
    return _mm256_add_epi64(c, hi);
}

#endif // HAVE_IFMA_256

#if defined(HAVE_IFMA_512)

// AVX512IFMA
inline auto mm512_madd52lo_epu64(auto c, auto a, auto b) NOEXCEPT
{
    return _mm512_madd52lo_epu64(c, a, b);
}

// AVX512IFMA
inline auto mm512_madd52hi_epu64(auto c, auto a, auto b) NOEXCEPT
{
    return _mm512_madd52hi_epu64(c, a, b);
}

#elif defined(HAVE_512)

// AVX512F
inline void mm512_mul52_epu64(auto& lo, auto& hi, auto a, auto b) NOEXCEPT
{
    const auto mask52 = _mm512_set1_epi64(0x000fffffffffffff);
    const auto mask26 = _mm512_set1_epi64(0x0000000003ffffff);
    const auto x = _mm512_and_si512(a, mask52);
    const auto y = _mm512_and_si512(b, mask52);
    const auto x0 = _mm512_and_si512(x, mask26);
    const auto y0 = _mm512_and_si512(y, mask26);
    const auto x1 = _mm512_srli_epi64(x, 26);
    const auto y1 = _mm512_srli_epi64(y, 26);
    const auto mid = _mm512_add_epi64(_mm512_mul_epu32(x0, y1),
        _mm512_mul_epu32(x1, y0));
    const auto low = _mm512_add_epi64(_mm512_mul_epu32(x0, y0),
        _mm512_slli_epi64(_mm512_and_si512(mid, mask26), 26));
    lo = _mm512_and_si512(low, mask52);
    hi = _mm512_add_epi64(_mm512_add_epi64(_mm512_mul_epu32(x1, y1),
        _mm512_srli_epi64(mid, 26)), _mm512_srli_epi64(low, 52));
}

// AVX512F
inline auto mm512_madd52lo_epu64(auto c, auto a, auto b) NOEXCEPT
{
    decltype(c) lo{}, hi{};
    mm512_mul52_epu64(lo, hi, a, b);
    return _mm512_add_epi64(c, lo);
}

// AVX512F
inline auto mm512_madd52hi_epu64(auto c, auto a, auto b) NOEXCEPT
{
    decltype(c) lo{}, hi{};
    mm512_mul52_epu64(lo, hi, a, b);
    return _mm512_add_epi64(c, hi);
}

#endif // HAVE_IFMA_512

} // namespace system
} // namespace libbitcoin

#endif // HAVE_XCPU

#endif

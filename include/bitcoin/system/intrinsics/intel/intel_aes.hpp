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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_INTEL_AES_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_INTEL_AES_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/types.hpp>
#include <bitcoin/system/intrinsics/intel/intel.hpp>
#include <bitcoin/system/intrinsics/intel/intel_128.hpp>
#include <bitcoin/system/intrinsics/intel/intel_256.hpp>
#include <bitcoin/system/intrinsics/intel/intel_512.hpp>

#if defined(HAVE_AESNI)

namespace libbitcoin {
namespace system {
namespace aes {

/// Round primitives (one 128 bit block per 128 bit lane).
/// ---------------------------------------------------------------------------

INLINE xint128_t cipher(xint128_t a, xint128_t key) NOEXCEPT
{
    return _mm_aesenc_si128(a, key);
}

INLINE xint128_t cipher_last(xint128_t a, xint128_t key) NOEXCEPT
{
    return _mm_aesenclast_si128(a, key);
}

INLINE xint128_t inverse_cipher(xint128_t a, xint128_t key) NOEXCEPT
{
    return _mm_aesdec_si128(a, key);
}

INLINE xint128_t inverse_cipher_last(xint128_t a, xint128_t key) NOEXCEPT
{
    return _mm_aesdeclast_si128(a, key);
}

#if defined(HAVE_VAES) && defined(HAVE_AVX2)

INLINE xint256_t cipher(xint256_t a, xint256_t key) NOEXCEPT
{
    return _mm256_aesenc_epi128(a, key);
}

INLINE xint256_t cipher_last(xint256_t a, xint256_t key) NOEXCEPT
{
    return _mm256_aesenclast_epi128(a, key);
}

#endif // HAVE_VAES && HAVE_AVX2

#if defined(HAVE_VAES) && defined(HAVE_AVX512)

INLINE xint512_t cipher(xint512_t a, xint512_t key) NOEXCEPT
{
    return _mm512_aesenc_epi128(a, key);
}

INLINE xint512_t cipher_last(xint512_t a, xint512_t key) NOEXCEPT
{
    return _mm512_aesenclast_epi128(a, key);
}

#endif // HAVE_VAES && HAVE_AVX512

/// Round keys.
/// ---------------------------------------------------------------------------

/// The inverse mix columns of a round key (equivalent inverse cipher).
INLINE xint128_t inverse(xint128_t key) NOEXCEPT
{
    return _mm_aesimc_si128(key);
}

/// The round key replicated to each 128 bit lane.
template <typename xWord>
INLINE xWord replicate(xint128_t key) NOEXCEPT
{
    if constexpr (is_same_type<xWord, xint128_t>)
        return key;
#if defined(HAVE_VAES) && defined(HAVE_AVX2)
    else if constexpr (is_same_type<xWord, xint256_t>)
        return _mm256_broadcastsi128_si256(key);
#endif
#if defined(HAVE_VAES) && defined(HAVE_AVX512)
    else if constexpr (is_same_type<xWord, xint512_t>)
        return _mm512_broadcast_i32x4(key);
#endif
}

/// Ciphers.
/// ---------------------------------------------------------------------------

/// Encrypt Count words of blocks (interleaved to cover round latency).
template <size_t Rounds, typename xWord, size_t Count>
INLINE void encrypt(std_array<xWord, Count>& state,
    const std_array<xWord, add1(Rounds)>& keys) NOEXCEPT
{
    for (auto& word: state)
        word = f::xor_(word, keys[0]);

    for (auto round = one; round < Rounds; ++round)
        for (auto& word: state)
            word = cipher(word, keys[round]);

    for (auto& word: state)
        word = cipher_last(word, keys[Rounds]);
}

/// Decrypt Count blocks with equivalent inverse cipher keys.
template <size_t Rounds, size_t Count>
INLINE void decrypt(std_array<xint128_t, Count>& state,
    const std_array<xint128_t, add1(Rounds)>& keys) NOEXCEPT
{
    for (auto& word: state)
        word = f::xor_(word, keys[0]);

    for (auto round = one; round < Rounds; ++round)
        for (auto& word: state)
            word = inverse_cipher(word, keys[round]);

    for (auto& word: state)
        word = inverse_cipher_last(word, keys[Rounds]);
}

/// GF(2^128) (ghash).
/// ---------------------------------------------------------------------------
/// Operands are byte reflected blocks, as loaded by reflect.
/// [Gueron, Kounavis] Intel Carry-Less Multiplication Instruction and its
/// Usage for Computing the GCM Mode (algorithms 2 and 4).

/// Sum (xor) of a and b.
INLINE xint128_t sum(xint128_t a, xint128_t b) NOEXCEPT
{
    return _mm_xor_si128(a, b);
}

INLINE xint128_t reflect(xint128_t a) NOEXCEPT
{
    const auto reverse = _mm_set_epi64x(0x0001020304050607, 0x08090a0b0c0d0e0f);
    return _mm_shuffle_epi8(a, reverse);
}

/// Unreduced carryless product (lo, hi) of a and b.
INLINE void multiply(xint128_t& lo, xint128_t& hi, xint128_t a,
    xint128_t b) NOEXCEPT
{
    const auto p00 = _mm_clmulepi64_si128(a, b, 0x00);
    const auto p01 = _mm_clmulepi64_si128(a, b, 0x01);
    const auto p10 = _mm_clmulepi64_si128(a, b, 0x10);
    const auto p11 = _mm_clmulepi64_si128(a, b, 0x11);
    const auto middle = _mm_xor_si128(p10, p01);
    lo = _mm_xor_si128(p00, _mm_slli_si128(middle, 8));
    hi = _mm_xor_si128(p11, _mm_srli_si128(middle, 8));
}

/// Reduction of the reflected product (lo, hi) modulo the ghash polynomial.
INLINE xint128_t reduce(xint128_t lo, xint128_t hi) NOEXCEPT
{
    // Shift the 256 bit product left one bit (reflection).
    const auto carry_lo = _mm_srli_epi32(lo, 31);
    const auto carry_hi = _mm_srli_epi32(hi, 31);
    const auto carry = _mm_srli_si128(carry_lo, 12);
    const auto shifted_lo = _mm_slli_epi32(lo, 1);
    const auto shifted_hi = _mm_slli_epi32(hi, 1);
    const auto into_lo = _mm_slli_si128(carry_lo, 4);
    const auto into_hi = _mm_or_si128(_mm_slli_si128(carry_hi, 4), carry);
    auto low = _mm_or_si128(shifted_lo, into_lo);
    const auto high = _mm_or_si128(shifted_hi, into_hi);

    // First phase of the reduction.
    const auto a31 = _mm_slli_epi32(low, 31);
    const auto a30 = _mm_slli_epi32(low, 30);
    const auto a25 = _mm_slli_epi32(low, 25);
    const auto first = _mm_xor_si128(_mm_xor_si128(a31, a30), a25);
    const auto remainder = _mm_srli_si128(first, 4);
    low = _mm_xor_si128(low, _mm_slli_si128(first, 12));

    // Second phase of the reduction.
    const auto b1 = _mm_srli_epi32(low, 1);
    const auto b2 = _mm_srli_epi32(low, 2);
    const auto b7 = _mm_srli_epi32(low, 7);
    const auto second = _mm_xor_si128(_mm_xor_si128(b1, b2), b7);
    const auto fold = _mm_xor_si128(second, remainder);
    return _mm_xor_si128(high, _mm_xor_si128(low, fold));
}

} // namespace aes
} // namespace system
} // namespace libbitcoin

#endif // HAVE_AESNI

#endif

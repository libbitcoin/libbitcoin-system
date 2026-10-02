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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_NEON_AES_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_NEON_AES_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/types.hpp>
#include <bitcoin/system/intrinsics/neon/neon.hpp>

#if defined(HAVE_NEON_AES)

namespace libbitcoin {
namespace system {
namespace aes {

/// Byte views of the 32 bit lane type.
/// ---------------------------------------------------------------------------

INLINE uint8x16_t bytes_of(xint128_t a) NOEXCEPT
{
    return vreinterpretq_u8_u32(a);
}

INLINE xint128_t words_of(uint8x16_t a) NOEXCEPT
{
    return vreinterpretq_u32_u8(a);
}

/// Round keys.
/// ---------------------------------------------------------------------------

/// The inverse mix columns of a round key (equivalent inverse cipher).
INLINE xint128_t inverse(xint128_t key) NOEXCEPT
{
    return words_of(vaesimcq_u8(bytes_of(key)));
}

/// The round key replicated to each 128 bit lane.
template <typename xWord, if_same<xWord, xint128_t> = true>
INLINE xWord replicate(xint128_t key) NOEXCEPT
{
    return key;
}

/// Ciphers.
/// ---------------------------------------------------------------------------
/// AESE/AESD add the round key before substitution and shift, so each round
/// key is applied one round earlier than in the fips197 formulation.

/// Encrypt Count blocks (interleaved to cover round latency).
template <size_t Rounds, typename xWord, size_t Count,
    if_same<xWord, xint128_t> = true>
INLINE void encrypt(std_array<xWord, Count>& state,
    const std_array<xWord, add1(Rounds)>& keys) NOEXCEPT
{
    std_array<uint8x16_t, Count> bytes{};
    for (size_t block{}; block < Count; ++block)
        bytes[block] = bytes_of(state[block]);

    for (size_t round{}; round < sub1(Rounds); ++round)
        for (auto& block: bytes)
            block = vaesmcq_u8(vaeseq_u8(block, bytes_of(keys[round])));

    const auto penultimate = bytes_of(keys[sub1(Rounds)]);
    const auto last = bytes_of(keys[Rounds]);
    for (size_t block{}; block < Count; ++block)
    {
        const auto round = vaeseq_u8(bytes[block], penultimate);
        state[block] = words_of(veorq_u8(round, last));
    }
}

/// Decrypt Count blocks with equivalent inverse cipher keys.
template <size_t Rounds, size_t Count>
INLINE void decrypt(std_array<xint128_t, Count>& state,
    const std_array<xint128_t, add1(Rounds)>& keys) NOEXCEPT
{
    std_array<uint8x16_t, Count> bytes{};
    for (size_t block{}; block < Count; ++block)
        bytes[block] = bytes_of(state[block]);

    for (size_t round{}; round < sub1(Rounds); ++round)
        for (auto& block: bytes)
            block = vaesimcq_u8(vaesdq_u8(block, bytes_of(keys[round])));

    const auto penultimate = bytes_of(keys[sub1(Rounds)]);
    const auto last = bytes_of(keys[Rounds]);
    for (size_t block{}; block < Count; ++block)
    {
        const auto round = vaesdq_u8(bytes[block], penultimate);
        state[block] = words_of(veorq_u8(round, last));
    }
}

/// GF(2^128) (ghash).
/// ---------------------------------------------------------------------------
/// Operands are byte reflected blocks, as loaded by reflect. The algorithm
/// is that of intel_aes, expressed in neon.

template <size_t Bytes>
INLINE uint8x16_t shift_bytes_left(uint8x16_t a) NOEXCEPT
{
    return vextq_u8(vdupq_n_u8(0), a, 16 - Bytes);
}

template <size_t Bytes>
INLINE uint8x16_t shift_bytes_right(uint8x16_t a) NOEXCEPT
{
    return vextq_u8(a, vdupq_n_u8(0), Bytes);
}

// The given 64 bit lane positioned high (vmull_high_p64 multiplies high lanes).
template <size_t Lane>
INLINE poly64x2_t high_lane(xint128_t a) NOEXCEPT
{
    const auto bytes = bytes_of(a);
    if constexpr (is_zero(Lane))
        return vreinterpretq_p64_u8(vextq_u8(bytes, bytes, 8));
    else
        return vreinterpretq_p64_u8(bytes);
}

template <size_t A, size_t B>
INLINE uint8x16_t carryless(xint128_t a, xint128_t b) NOEXCEPT
{
    const auto x = high_lane<A>(a);
    const auto y = high_lane<B>(b);
    return vreinterpretq_u8_p128(vmull_high_p64(x, y));
}

/// Sum (xor) of a and b.
INLINE xint128_t sum(xint128_t a, xint128_t b) NOEXCEPT
{
    return veorq_u32(a, b);
}

INLINE xint128_t reflect(xint128_t a) NOEXCEPT
{
    const auto bytes = vrev64q_u8(bytes_of(a));
    return words_of(vextq_u8(bytes, bytes, 8));
}

/// Unreduced carryless product (lo, hi) of a and b.
INLINE void multiply(xint128_t& lo, xint128_t& hi, xint128_t a,
    xint128_t b) NOEXCEPT
{
    // Template argument lists are kept out of intrinsic macro arguments.
    const auto p00 = carryless<0, 0>(a, b);
    const auto p01 = carryless<0, 1>(a, b);
    const auto p10 = carryless<1, 0>(a, b);
    const auto p11 = carryless<1, 1>(a, b);
    const auto middle = veorq_u8(p01, p10);
    const auto up = shift_bytes_left<8>(middle);
    const auto down = shift_bytes_right<8>(middle);
    lo = words_of(veorq_u8(p00, up));
    hi = words_of(veorq_u8(p11, down));
}

/// Reduction of the reflected product (lo, hi) modulo the ghash polynomial.
INLINE xint128_t reduce(xint128_t lo, xint128_t hi) NOEXCEPT
{
    // Shift the 256 bit product left one bit (reflection).
    const auto carry_lo = bytes_of(vshrq_n_u32(lo, 31));
    const auto carry_hi = bytes_of(vshrq_n_u32(hi, 31));
    const auto carry = words_of(shift_bytes_right<12>(carry_lo));
    const auto shifted_lo = vshlq_n_u32(lo, 1);
    const auto shifted_hi = vshlq_n_u32(hi, 1);
    const auto into_lo = words_of(shift_bytes_left<4>(carry_lo));
    const auto moved_hi = words_of(shift_bytes_left<4>(carry_hi));
    const auto into_hi = vorrq_u32(moved_hi, carry);
    auto low = vorrq_u32(shifted_lo, into_lo);
    const auto high = vorrq_u32(shifted_hi, into_hi);

    // First phase of the reduction.
    const auto a31 = vshlq_n_u32(low, 31);
    const auto a30 = vshlq_n_u32(low, 30);
    const auto a25 = vshlq_n_u32(low, 25);
    const auto first = bytes_of(veorq_u32(veorq_u32(a31, a30), a25));
    const auto remainder = words_of(shift_bytes_right<4>(first));
    low = veorq_u32(low, words_of(shift_bytes_left<12>(first)));

    // Second phase of the reduction.
    const auto b1 = vshrq_n_u32(low, 1);
    const auto b2 = vshrq_n_u32(low, 2);
    const auto b7 = vshrq_n_u32(low, 7);
    const auto second = veorq_u32(veorq_u32(b1, b2), b7);
    const auto fold = veorq_u32(second, remainder);
    return veorq_u32(high, veorq_u32(low, fold));
}

} // namespace aes
} // namespace system
} // namespace libbitcoin

#endif // HAVE_NEON_AES

#endif

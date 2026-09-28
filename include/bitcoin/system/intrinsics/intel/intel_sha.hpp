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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_INTEL_SHA_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_INTEL_SHA_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/types.hpp>
#include <bitcoin/system/intrinsics/intel/intel.hpp>
#include <bitcoin/system/intrinsics/intel/intel_256.hpp>

#if defined(HAVE_SHANI)

namespace libbitcoin {
namespace system {
namespace sha {

INLINE void schedule(xint128_t& message0, xint128_t message1) NOEXCEPT
{
    message0 = _mm_sha256msg1_epu32(message0, message1);
}

INLINE void schedule(xint128_t& message0, xint128_t message1,
    xint128_t message2) NOEXCEPT
{
    message0 = _mm_sha256msg2_epu32(_mm_add_epi32(message0,
        _mm_alignr_epi8(message2, message1, 4)), message2);
}

INLINE void compress(xint128_t& state0, xint128_t& state1,
    xint128_t wk) NOEXCEPT
{
    state1 = _mm_sha256rnds2_epu32(state1, state0, wk);
    state0 = _mm_sha256rnds2_epu32(state0, state1, _mm_shuffle_epi32(wk, 0x0e));
}

INLINE void shuffle(xint128_t& state0, xint128_t& state1) NOEXCEPT
{
    // shuffle organizes state as expected by sha256rnds2.
    const auto shuffle0 = _mm_shuffle_epi32(state0, 0xb1);
    const auto shuffle1 = _mm_shuffle_epi32(state1, 0x1b);
    state0 = _mm_alignr_epi8(shuffle0, shuffle1, 0x08);
    state1 = _mm_blend_epi16(shuffle1, shuffle0, 0xf0);
}

INLINE void unshuffle(xint128_t& state0, xint128_t& state1) NOEXCEPT
{
    // unshuffle restores state to normal form.
    const auto shuffle0 = _mm_shuffle_epi32(state0, 0x1b);
    const auto shuffle1 = _mm_shuffle_epi32(state1, 0xb1);
    state0 = _mm_blend_epi16(shuffle0, shuffle1, 0xf0);
    state1 = _mm_alignr_epi8(shuffle1, shuffle0, 0x08);
}

INLINE void schedule_160(xint128_t& message0, xint128_t message1,
    xint128_t message2) NOEXCEPT
{
    message0 = _mm_xor_si128(_mm_sha1msg1_epu32(message0, message1), message2);
}

INLINE void schedule_160(xint128_t& message0, xint128_t message3) NOEXCEPT
{
    message0 = _mm_sha1msg2_epu32(message0, message3);
}

template <size_t Round>
INLINE void compress_160(xint128_t& abcd, xint128_t& carry,
    xint128_t message) NOEXCEPT
{
    const auto we = _mm_sha1nexte_epu32(carry, message);
    carry = abcd;
    abcd = _mm_sha1rnds4_epu32(abcd, we, Round);
}

INLINE xint128_t next_160(xint128_t carry, xint128_t e) NOEXCEPT
{
    return _mm_sha1nexte_epu32(carry, e);
}

INLINE xint128_t order_160(xint128_t message) NOEXCEPT
{
    return _mm_shuffle_epi32(message, 0x1b);
}

INLINE void shuffle_160(xint128_t& abcd) NOEXCEPT
{
    abcd = _mm_shuffle_epi32(abcd, 0x1b);
}

INLINE void unshuffle_160(xint128_t& abcd) NOEXCEPT
{
    abcd = _mm_shuffle_epi32(abcd, 0x1b);
}

INLINE xint128_t set_160(uint32_t e) NOEXCEPT
{
    return _mm_set_epi32(e, 0, 0, 0);
}

INLINE uint32_t get_160(xint128_t e) NOEXCEPT
{
    return _mm_extract_epi32(e, 3);
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif // HAVE_SHANI

#if defined(HAVE_SHANI512)

namespace libbitcoin {
namespace system {
namespace sha {

/// Four 64 bit words.
using xquad_t = xint256_t;

INLINE xquad_t set_512(uint64_t a, uint64_t b, uint64_t c, uint64_t d) NOEXCEPT
{
    return _mm256_set_epi64x(d, c, b, a);
}

INLINE xquad_t load_512(const xquad_t& words) NOEXCEPT
{
    return _mm256_loadu_si256(&words);
}

INLINE void store_512(xquad_t& words, xquad_t a) NOEXCEPT
{
    _mm256_storeu_si256(&words, a);
}

INLINE xquad_t add_512(xquad_t a, xquad_t b) NOEXCEPT
{
    return _mm256_add_epi64(a, b);
}

INLINE xquad_t swap_512(xquad_t a) NOEXCEPT
{
    return f::byteswap<uint64_t>(a);
}

INLINE void schedule_512(xquad_t& message0, xquad_t message1) NOEXCEPT
{
    message0 = _mm256_sha512msg1_epi64(message0,
        _mm256_castsi256_si128(message1));
}

INLINE void schedule_512(xquad_t& message0, xquad_t message1,
    xquad_t message2) NOEXCEPT
{
    // alignr is 128 bit laned, so span the lanes with permute2x128.
    const auto span = _mm256_permute2x128_si256(message1, message2, 0x21);
    message0 = _mm256_sha512msg2_epi64(_mm256_add_epi64(message0,
        _mm256_alignr_epi8(span, message1, 8)), message2);
}

INLINE void compress_512(xquad_t& state0, xquad_t& state1,
    xquad_t wk) NOEXCEPT
{
    state1 = _mm256_sha512rnds2_epi64(state1, state0,
        _mm256_castsi256_si128(wk));
    state0 = _mm256_sha512rnds2_epi64(state0, state1,
        _mm256_extracti128_si256(wk, 1));
}

INLINE void shuffle_512(xquad_t& state0, xquad_t& state1) NOEXCEPT
{
    // shuffle organizes state as expected by sha512rnds2.
    const auto shuffle0 = _mm256_permute4x64_epi64(state0, 0x1b);
    const auto shuffle1 = _mm256_permute4x64_epi64(state1, 0x1b);
    state0 = _mm256_permute2x128_si256(shuffle0, shuffle1, 0x13);
    state1 = _mm256_permute2x128_si256(shuffle0, shuffle1, 0x02);
}

INLINE void unshuffle_512(xquad_t& state0, xquad_t& state1) NOEXCEPT
{
    // unshuffle restores state to normal form.
    const auto shuffle0 = _mm256_permute2x128_si256(state0, state1, 0x13);
    const auto shuffle1 = _mm256_permute2x128_si256(state0, state1, 0x02);
    state0 = _mm256_permute4x64_epi64(shuffle0, 0x1b);
    state1 = _mm256_permute4x64_epi64(shuffle1, 0x1b);
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif // HAVE_SHANI512

#endif

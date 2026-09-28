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

#endif

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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_INTEL_SHA_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_INTEL_SHA_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/types.hpp>
#include <bitcoin/system/intrinsics/platforms/intel.hpp>

namespace libbitcoin {
namespace system {
namespace sha {

#if defined(HAVE_SHANI)

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

#endif // HAVE_SHANI

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

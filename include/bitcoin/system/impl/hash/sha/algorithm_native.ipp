/**
 * Copyright (c) 2011-2024 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_NATIVE_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_NATIVE_IPP

// Native (SHA-NI or NEON)
// ============================================================================
// The iterative method is used for sha native as it is an order of magnitude
// more efficient and cannot benefit from vectorization.

namespace libbitcoin {
namespace system {
namespace sha {

// TODO: intel sha160, arm sha160, arm sha256

// intel sha256
// ----------------------------------------------------------------------------
// protected

TEMPLATE
INLINE void CLASS::
shuffle(xint128_t& state0, xint128_t& state1) NOEXCEPT
{
    const auto shuffle0 = mm_shuffle_epi32(state0, 0xb1);
    const auto shuffle1 = mm_shuffle_epi32(state1, 0x1b);
    state0 = mm_alignr_epi8(shuffle0, shuffle1, 0x08);
    state1 = mm_blend_epi16(shuffle1, shuffle0, 0xf0);
}

TEMPLATE
INLINE void CLASS::
unshuffle(xint128_t& state0, xint128_t& state1) NOEXCEPT
{
    const auto shuffle0 = mm_shuffle_epi32(state0, 0x1b);
    const auto shuffle1 = mm_shuffle_epi32(state1, 0xb1);
    state0 = mm_blend_epi16(shuffle0, shuffle1, 0xf0);
    state1 = mm_alignr_epi8(shuffle1, shuffle0, 0x08);
}

TEMPLATE
INLINE void CLASS::
prepare(xint128_t& message0, xint128_t message1) NOEXCEPT
{
    message0 = mm_sha256msg1_epu32(message0, message1);
}

TEMPLATE
INLINE void CLASS::
prepare(xint128_t& SHANI_ONLY(message0), xint128_t message1,
    xint128_t& message2) NOEXCEPT
{
    message2 = mm_sha256msg2_epu32(mm_add_epi32(message2,
        mm_alignr_epi8(message1, message0, 4)), message1);
}

TEMPLATE
template <size_t Round>
INLINE void CLASS::
round_4(xint128_t& state0, xint128_t& state1, xint128_t message) NOEXCEPT
{
    constexpr auto r = Round * 4;
    const auto wk = add<word_t>(message, set<xint128_t>(
        K::get[r + 0], K::get[r + 1], K::get[r + 2], K::get[r + 3]));

    state1 = mm_sha256rnds2_epu32(state1, state0, wk);
    state0 = mm_sha256rnds2_epu32(state0, state1, mm_shuffle_epi32(wk, 0x0e));
}

TEMPLATE
void CLASS::
native_rounds(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    // Individual state vars are used vs. array to ensure register persistence.
    auto& wstate = array_cast<xint128_t>(state);
    auto lo = load(wstate[0]);
    auto hi = load(wstate[1]);

    // shuffle organizes state as expected by sha256rnds2.
    shuffle(lo, hi);

    while (!blocks.empty())
    {
        const auto start_lo = lo;
        const auto start_hi = hi;
        const auto& wblock = array_cast<xint128_t>(blocks.to_array());

        auto message0 = byteswap<uint32_t>(load(wblock[0]));
        round_4<0>(lo, hi, message0);

        auto message1 = byteswap<uint32_t>(load(wblock[1]));
        round_4<1>(lo, hi, message1);

        prepare(message0, message1);
        auto message2 = byteswap<uint32_t>(load(wblock[2]));
        round_4<2>(lo, hi, message2);

        prepare(message1, message2);
        auto message3 = byteswap<uint32_t>(load(wblock[3]));
        round_4<3>(lo, hi, message3);

        prepare(message2, message3, message0);
        prepare(message2, message3);
        round_4<4>(lo, hi, message0);

        prepare(message3, message0, message1);
        prepare(message3, message0);
        round_4<5>(lo, hi, message1);

        prepare(message0, message1, message2);
        prepare(message0, message1);
        round_4<6>(lo, hi, message2);

        prepare(message1, message2, message3);
        prepare(message1, message2);
        round_4<7>(lo, hi, message3);

        prepare(message2, message3, message0);
        prepare(message2, message3);
        round_4<8>(lo, hi, message0);

        prepare(message3, message0, message1);
        prepare(message3, message0);
        round_4<9>(lo, hi, message1);

        prepare(message0, message1, message2);
        prepare(message0, message1);
        round_4<10>(lo, hi, message2);

        prepare(message1, message2, message3);
        prepare(message1, message2);
        round_4<11>(lo, hi, message3);

        prepare(message2, message3, message0);
        prepare(message2, message3);
        round_4<12>(lo, hi, message0);

        prepare(message3, message0, message1);
        prepare(message3, message0);
        round_4<13>(lo, hi, message1);

        prepare(message0, message1, message2);
        round_4<14>(lo, hi, message2);

        prepare(message1, message2, message3);
        round_4<15>(lo, hi, message3);

        lo = add<word_t>(lo, start_lo);
        hi = add<word_t>(hi, start_hi);
        blocks.advance();
    }

    // unshuffle restores state to normal form.
    unshuffle(lo, hi);

    store(wstate[0], lo);
    store(wstate[1], hi);
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

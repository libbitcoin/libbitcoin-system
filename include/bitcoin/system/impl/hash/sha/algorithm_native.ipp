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
// Native does not change the buffer size (not expanded), just its "shape".
// Four words are buffered into one xint128_t, resulting in 1/4 the buffer
// array size and number of rounds. Four state words are  packed into each of
// two state variables. This applies to sha160 and sha256, but sha512 native
// is not supported.

// The base buffer is already populated with proper endianness.
// Input could be optimized using intrinsics (see comments in parse).
// The unextended state vector is already output with proper endianness.
// Output could also be optimized using intrinsics (see comments in parse).

namespace libbitcoin {
namespace system {
namespace sha {

#if defined(DISABLED)

// schedule
// ----------------------------------------------------------------------------
// protected

TEMPLATE
template<size_t Round>
INLINE void CLASS::
prepare_native(wbuffer_t<xint128_t>& wbuffer) NOEXCEPT
{
    if constexpr (SHA::strength == 160)
    {
        if constexpr (use_neon)
        {
        }
        else if constexpr (use_shani)
        {
        }
    }
    else if constexpr (SHA::strength == 256)
    {
        if constexpr (use_neon)
        {
        }
        else if constexpr (use_shani)
        {
            wbuffer[Round] = mm_sha256msg2_epu32
            (
                mm_add_epi32
                (
                    mm_alignr_epi8
                    (
                        wbuffer[Round - 1], wbuffer[Round - 2], SHA::word_bytes
                    ),
                    mm_sha256msg1_epu32
                    (
                        wbuffer[Round - 4], wbuffer[Round - 3]
                    )
                ),
                wbuffer[Round - 1]
            );
        }
    }
}

TEMPLATE
void CLASS::
schedule_native(wbuffer_t<xint128_t>& wbuffer) NOEXCEPT
{
    prepare_native<4>(wbuffer);
    prepare_native<5>(wbuffer);
    prepare_native<6>(wbuffer);
    prepare_native<7>(wbuffer);
    prepare_native<8>(wbuffer);
    prepare_native<9>(wbuffer);
    prepare_native<10>(wbuffer);
    prepare_native<11>(wbuffer);
    prepare_native<12>(wbuffer);
    prepare_native<13>(wbuffer);
    prepare_native<14>(wbuffer);
    prepare_native<15>(wbuffer);

    if constexpr (SHA::rounds == 80)
    {
        prepare_native<16>(wbuffer);
        prepare_native<17>(wbuffer);
        prepare_native<18>(wbuffer);
        prepare_native<19>(wbuffer);
    }

    konstant(array_cast<word_t>(wbuffer));
}

TEMPLATE
INLINE void CLASS::
schedule_native(buffer_t& buffer) NOEXCEPT
{
    // neon and sha160 not yet implemented, sha512 is not native.
    if constexpr (SHA::strength == 256 && !use_neon)
    {
        schedule_native(array_cast<xint128_t>(buffer));
    }
    else
    {
        schedule_(buffer);
    }
}

TEMPLATE
template <typename xWord>
INLINE void CLASS::
schedule_native(xbuffer_t<xWord>& xbuffer) NOEXCEPT
{
    // Merkle extended buffer is not native dispatched.
    schedule_(xbuffer);
}

// compression
// ----------------------------------------------------------------------------
// protected

TEMPLATE
template<size_t Round, size_t Lane>
INLINE void CLASS::
round_native(wstate_t<xint128_t>& state,
    const wbuffer_t<xint128_t>& wk) NOEXCEPT
{
    if constexpr (SHA::strength == 160)
    {
        if constexpr (use_neon)
        {
        }
        else if constexpr (use_shani)
        {
        }
    }
    else if constexpr (SHA::strength == 256)
    {
        if constexpr (use_neon)
        {
        }
        else if constexpr (use_shani)
        {
            // Process wk[Round][0..1], [HGDC][FEBA] (initial state)
            state[1] = mm_sha256rnds2_epu32(state[1], state[0], wk[Round]);

            // Process wk[Round][2..3] (shifted down)
            state[0] = mm_sha256rnds2_epu32(state[0], state[1],
                mm_shuffle_epi32(wk[Round], 0x0e));
        }
    }
}

TEMPLATE
INLINE void CLASS::
summarize_native(wstate_t<xint128_t>& out,
    const wstate_t<xint128_t>& in) NOEXCEPT
{
    if constexpr (SHA::strength == 160)
    {
        if constexpr (use_neon)
        {
        }
        else if constexpr (use_shani)
        {
        }
    }
    else if constexpr (SHA::strength == 256)
    {
        if constexpr (use_neon)
        {
        }
        else if constexpr (use_shani)
        {
            out[0] = mm_add_epi32(out[0], in[0]);
            out[1] = mm_add_epi32(out[1], in[1]);
        }
    }
}

TEMPLATE
INLINE void CLASS::
shuffle(wstate_t<xint128_t>& wstate) NOEXCEPT
{
    // Change wstate to mm_sha256rnds2_epu32 expected form:
    // [ABCD][EFGH] -> [FEBA][HGDC] (ordered low to high).
    const auto t1 = mm_shuffle_epi32(wstate[0], 0xb1);
    const auto t2 = mm_shuffle_epi32(wstate[1], 0x1b);
    wstate[0] = mm_alignr_epi8(t1, t2, 0x08);
    wstate[1] = mm_blend_epi16(t2, t1, 0xf0);
}

TEMPLATE
INLINE void CLASS::
unshuffle(wstate_t<xint128_t>& wstate) NOEXCEPT
{
    // Restore wstate to normal form:
    // [FEBA][HGDC] -> [ABCD][EFGH] (ordered low to high).
    const auto t1 = mm_shuffle_epi32(wstate[0], 0x1b);
    const auto t2 = mm_shuffle_epi32(wstate[1], 0xb1);
    wstate[0] = mm_blend_epi16(t1, t2, 0xf0);
    wstate[1] = mm_alignr_epi8(t2, t1, 0x08);
}

TEMPLATE
template <size_t Lane>
void CLASS::
compress_native(wstate_t<xint128_t>& wstate,
    const wbuffer_t<xint128_t>& wbuffer) NOEXCEPT
{ 
    // Shuffle and unshuffle can be done outside of all blocks, but this would
    // leave state in a non-normal form, so presently absorbing that cost.
    shuffle(wstate);

    // This is a copy.
    const auto start = wstate;

    round_native< 0, Lane>(wstate, wbuffer);
    round_native< 1, Lane>(wstate, wbuffer);
    round_native< 2, Lane>(wstate, wbuffer);
    round_native< 3, Lane>(wstate, wbuffer);
    round_native< 4, Lane>(wstate, wbuffer);
    round_native< 5, Lane>(wstate, wbuffer);
    round_native< 6, Lane>(wstate, wbuffer);
    round_native< 7, Lane>(wstate, wbuffer);
    round_native< 8, Lane>(wstate, wbuffer);
    round_native< 9, Lane>(wstate, wbuffer);
    round_native<10, Lane>(wstate, wbuffer);
    round_native<11, Lane>(wstate, wbuffer);
    round_native<12, Lane>(wstate, wbuffer);
    round_native<13, Lane>(wstate, wbuffer);
    round_native<14, Lane>(wstate, wbuffer);
    round_native<15, Lane>(wstate, wbuffer);

    if constexpr (SHA::rounds == 80)
    {
        round_native<16, Lane>(wstate, wbuffer);
        round_native<17, Lane>(wstate, wbuffer);
        round_native<18, Lane>(wstate, wbuffer);
        round_native<19, Lane>(wstate, wbuffer);
    }

    // This is just a vectorized version of summarize().
    summarize_native(wstate, start);

    // See above comments on shuffle().
    unshuffle(wstate);
}

TEMPLATE
template <typename xWord, size_t Lane>
INLINE void CLASS::
compress_native(xstate_t<xWord>& xstate,
    const xbuffer_t<xWord>& xbuffer) NOEXCEPT
{
    // Merkle extended state/buffer is not native dispatched.
    compress_<Lane>(xstate, xbuffer);
}

TEMPLATE
template <typename xWord, size_t Lane>
INLINE void CLASS::
compress_native(state_t& state, const xbuffer_t<xWord>& xbuffer) NOEXCEPT
{
    // Iterate extended buffer is not native dispatched.
    compress_<Lane>(state, xbuffer);
}

TEMPLATE
template <size_t Lane>
INLINE void CLASS::
compress_native(state_t& state, const buffer_t& buffer) NOEXCEPT
{
    // TODO: debug.
    // TODO: sha160 state is too small to array cast into two xwords.
    // neon and sha160 not yet implemented, sha512 is not native.
    if constexpr (SHA::strength == 256 && !use_neon)
    {
        compress_native<Lane>(array_cast<xint128_t>(state),
            array_cast<xint128_t>(buffer));
    }
    else
    {
        compress_<Lane>(state, buffer);
    }
}

#else // DISABLED

// rotating variables (no buffer)
// ----------------------------------------------------------------------------
// protected

// The iterative method is used for sha native as it is an order of magnitude
// more efficient and cannot benefit from vectorization.

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
prepare(xint128_t& message0, xint128_t message1, xint128_t& message2) NOEXCEPT
{
    message2 = mm_sha256msg2_epu32(f::add<SHA::word_bits>(message2,
        mm_alignr_epi8(message1, message0, 4)), message1);
}

TEMPLATE
template <size_t Round>
INLINE void CLASS::
round_4(xint128_t& state0, xint128_t& state1, xint128_t message) NOEXCEPT
{
    // TODO: evaluate static setter (local to Round).
    ////static const auto k = set<xint128_t>(
    ////    K::get[r + 0], K::get[r + 1], K::get[r + 2], K::get[r + 3]);

    constexpr auto r = Round * 4;
    const auto wk = f::add<SHA::word_bits>(message, set<xint128_t>(
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

    // shufle organizes state as expected by sha256rnds2.
    shuffle(lo, hi);

    while (!blocks.empty())
    {
        const auto start_lo = lo;
        const auto start_hi = hi;
        auto& block = array_cast<xint128_t>(blocks.to_array());

        auto message0 = byteswap<uint32_t>(block[0]);
        round_4<0>(lo, hi, message0);

        auto message1 = byteswap<uint32_t>(block[1]);
        round_4<1>(lo, hi, message1);

        prepare(message0, message1);
        auto message2 = byteswap<uint32_t>(block[2]);
        round_4<2>(lo, hi, message2);

        prepare(message1, message2);
        auto message3 = byteswap<uint32_t>(block[3]);
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

        lo = f::add<SHA::word_bits>(lo, start_lo);
        hi = f::add<SHA::word_bits>(hi, start_hi);
        blocks.advance();
    }

    // unshuffle restores state to normal form.
    unshuffle(lo, hi);

    store(wstate[0], lo);
    store(wstate[1], hi);
}

#endif // DISABLED

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

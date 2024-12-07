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
template <bool Swap>
INLINE xint128_t CLASS::
bytes(xint128_t message) NOEXCEPT
{
    if constexpr (Swap)
        return byteswap<uint32_t>(message);
    else
        return message;
}

TEMPLATE
INLINE void CLASS::
shuffle(xint128_t& state0, xint128_t& state1) NOEXCEPT
{
    // shuffle organizes state as expected by sha256rnds2.
    const auto shuffle0 = mm_shuffle_epi32(state0, 0xb1);
    const auto shuffle1 = mm_shuffle_epi32(state1, 0x1b);
    state0 = mm_alignr_epi8(shuffle0, shuffle1, 0x08);
    state1 = mm_blend_epi16(shuffle1, shuffle0, 0xf0);
}

TEMPLATE
INLINE void CLASS::
unshuffle(xint128_t& state0, xint128_t& state1) NOEXCEPT
{
    // unshuffle restores state to normal form.
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

// Platform agnostic.
// ----------------------------------------------------------------------------
// Individual state vars are used vs. array to ensure register persistence.
// This creates bifurcations in this template because of the lack of a buffer
// and the differing optimal locations for applying endianness conversions.

TEMPLATE
template <bool Swap>
void CLASS::
native_rounds(xint128_t& lo, xint128_t& hi, const block_t& block) NOEXCEPT
{
    const auto& wblock = array_cast<xint128_t>(block);

    const auto start_lo = lo;
    const auto start_hi = hi;

    auto message0 = bytes<Swap>(load(wblock[0]));
    round_4<0>(lo, hi, message0);

    auto message1 = bytes<Swap>(load(wblock[1]));
    round_4<1>(lo, hi, message1);

    prepare(message0, message1);
    auto message2 = bytes<Swap>(load(wblock[2]));
    round_4<2>(lo, hi, message2);

    prepare(message1, message2);
    auto message3 = bytes<Swap>(load(wblock[3]));
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
}

// Transforms perform scheduling and compression with optional endianness
// conversion of the block input. State is normalized, which requires some
// additional shuffle/unshuffle calls between transformations of same state.
// State output is not finalized, which is endianness conversion to digest_t.
// ----------------------------------------------------------------------------

TEMPLATE
void CLASS::
native_transform(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    auto& wstate = array_cast<xint128_t>(state);
    auto lo = load(wstate[0]);
    auto hi = load(wstate[1]);
    shuffle(lo, hi);

    for (auto& block: blocks)
        native_rounds<true>(lo, hi, block);

    unshuffle(lo, hi);
    store(wstate[0], lo);
    store(wstate[1], hi);
}

TEMPLATE
template <bool Swap>
void CLASS::
native_transform(state_t& state, const auto& block) NOEXCEPT
{
    auto& wstate = array_cast<xint128_t>(state);
    auto lo = load(wstate[0]);
    auto hi = load(wstate[1]);
    shuffle(lo, hi);
    native_rounds<Swap>(lo, hi, array_cast<byte_t>(block));
    unshuffle(lo, hi);
    store(wstate[0], lo);
    store(wstate[1], hi);
}

// Finalization creates and/or applies a given padding block to the state
// accumulation and performs big-endian conversion from state_t to digest_t.
// As padding blocks are generated and therefore do not require endianness
// conversion, those calls are not applied when transforming the pad block.
// ----------------------------------------------------------------------------

TEMPLATE
typename CLASS::digest_t CLASS::
native_finalize(state_t& state, const words_t& pad) NOEXCEPT
{
    auto& wstate = array_cast<xint128_t>(state);
    auto lo = load(wstate[0]);
    auto hi = load(wstate[1]);
    shuffle(lo, hi);
    native_rounds<false>(lo, hi, array_cast<byte_t>(pad));
    unshuffle(lo, hi);

    // digest is copied so that state remains valid (LE).
    digest_t digest{};
    auto& wdigest = array_cast<xint128_t>(digest);
    store(wdigest[0], byteswap<uint32_t>(lo));
    store(wdigest[1], byteswap<uint32_t>(hi));
    return digest;
}

TEMPLATE
template <size_t Blocks>
typename CLASS::digest_t CLASS::
native_finalize(state_t& state) NOEXCEPT
{
    // We could use Blocks to cache padding but given the padding blocks are
    // unscheduled when performing native transformations there's no benefit.
    return native_finalize(state, Blocks);
}

TEMPLATE
typename CLASS::digest_t CLASS::
native_finalize(state_t& state, size_t blocks) NOEXCEPT
{
    return native_finalize(state, pad_blocks(blocks));
}

TEMPLATE
typename CLASS::digest_t CLASS::
native_finalize_second(const state_t& state) NOEXCEPT
{
    // No hash(state_t) optimizations for sha160 (requires chunk_t/half_t).
    static_assert(is_same_type<state_t, chunk_t>);

    // Hash a state value and finalize it.
    auto state2 = H::get;
    words_t block{};
    reinput_left(block, state);             // swapped
    pad_half(block);                        // swapped
    return native_finalize(state2, block);  // no block swap (swaps state)
}

TEMPLATE
typename CLASS::digest_t CLASS::
native_finalize_double(state_t& state, size_t blocks) NOEXCEPT
{
    // Complete first hash by transforming padding, but don't convert state.
    auto block = pad_blocks(blocks);
    native_transform<false>(state, block);  // no swap

    // This is native_finalize_second() but reuses the initial block.
    auto state2 = H::get;
    reinput_left(block, state);             // swapped
    pad_half(block);                        // swapped
    return native_finalize(state2, block);  // no block swap (swaps state)
}

// Hash functions start with BE data and end with BE digest_t.
// ----------------------------------------------------------------------------

TEMPLATE
typename CLASS::digest_t CLASS::
native_hash(const half_t& half) NOEXCEPT
{
    // No hash(state_t) optimizations for sha160 (requires chunk_t/half_t).
    static_assert(is_same_type<state_t, chunk_t>);

    auto state = H::get;
    words_t block{};
    input_left(block, half);                // swaps
    pad_half(block);                        // swapped
    return native_finalize(state, block);   // no block swap (swaps state)
}

TEMPLATE
typename CLASS::digest_t CLASS::
native_hash(const half_t& left, const half_t& right) NOEXCEPT
{
    auto state = H::get;
    words_t block{};
    reinput_left(block, array_cast<word_t>(left));      // unswapped
    reinput_right(block, array_cast<word_t>(right));    // unswapped
    native_transform<true>(state, block);   // swap
    return native_finalize<one>(state);     // no block swap (swaps state)
}

// Double hash functions start with BE data and end with BE digest_t.
// ----------------------------------------------------------------------------

TEMPLATE
typename CLASS::digest_t CLASS::
native_double_hash(const block_t& block) NOEXCEPT
{
    auto state = H::get;
    native_transform<true>(state, block);           // swap
    native_transform<false>(state, pad_block());    // swapped

    // Second hash
    words_t block2{};
    reinput_left(block2, state);            // swapped
    pad_half(block2);                       // swapped
    state = H::get;                         // [reuse state var]
    return native_finalize(state, block2);  // no block swap (swaps state)
}

TEMPLATE
typename CLASS::digest_t CLASS::
native_double_hash(const half_t& half) NOEXCEPT
{
    auto state = H::get;
    words_t block{};
    input_left(block, half);                // swaps
    pad_half(block);                        // swapped
    native_transform<false>(state, block);  // no block swap

    // Second hash
    reinput_left(block, state);             // swapped
    pad_half(block);                        // swapped
    state = H::get;                         // [reuse state var]
    return native_finalize(state, block);   // no block swap (swaps state)
}

TEMPLATE
typename CLASS::digest_t CLASS::
native_double_hash(const half_t& left, const half_t& right) NOEXCEPT
{
    auto state = H::get;
    words_t block{};
    reinput_left(block, array_cast<word_t>(left));      // unswapped
    reinput_right(block, array_cast<word_t>(right));    // unswapped
    native_transform<true>(state, block);               // swap
    native_transform<false>(state, pad_block());        // swapped

    // Second hash
    reinput_left(block, state);             // swapped
    pad_half(block);                        // swapped
    state = H::get;                         // [reuse state var]
    return native_finalize(state, block);   // no block swap (swaps state)
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

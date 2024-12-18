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
// The rotating variables method is used for sha native. Tha native
// instructions rely on register locality to achieve performance benefits.
// Implementation of native sha using buffer expansion is horribly slow.
// This split creates bifurcations (additional complexities) in this template.

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

TEMPLATE
template <bool Swap>
INLINE void CLASS::
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
    // Individual state vars are used vs. array to ensure register persistence.
    auto& wstate = array_cast<xint128_t>(state);
    auto lo = load(wstate[0]);
    auto hi = load(wstate[1]);
    shuffle(lo, hi);

    // native_rounds must be inlined here (register boundary).
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

    // native_rounds must be inlined here (register boundary).
    native_rounds<Swap>(lo, hi, array_cast<byte_t>(block));

    unshuffle(lo, hi);
    store(wstate[0], lo);
    store(wstate[1], hi);
}

// Finalization creates and/or applies a given padding block to the state
// accumulation and performs big-endian conversion from state_t to digest_t.
// As padding blocks are generated and therefore do not require endianness
// conversion, those calls are not applied when transforming the pad block.
// This lack of conversion also applies to double hashing. In both cases
// the "inject" functions are using in place of the "input" functions.
// There is no benefit to caching pading because it is not prescheduled.
// ----------------------------------------------------------------------------

// TODO: These transitions require state to be unloaded/loaded and
// shuffled/unshuffled, whereas this is not logically necessary. This is a
// fixed cost imposed once for any accumulation (which is inconsequential for
// larger iterations), but reduces efficiency for lower block counts and hash
// doubling. Large iterations are 15-16% wheras small iterations are 20-26%.
// native_transform -> native_transform -> native_finalize
// native_transform -> native_finalize
// This can be resolved in the non-iterator scenarios (below) through
// implementation of a finalizing and a doubling native_transform. This means
// that padding must be incorporated, however since it is not prescheduled or
// cached this is not an issue.

TEMPLATE
template <size_t Blocks>
typename CLASS::digest_t CLASS::
native_finalize(state_t& state) NOEXCEPT
{
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
native_finalize(state_t& state, const words_t& pad) NOEXCEPT
{
    auto& wstate = array_cast<xint128_t>(state);
    auto lo = load(wstate[0]);
    auto hi = load(wstate[1]);
    shuffle(lo, hi);

    // native_rounds must be inlined here (register boundary).
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
typename CLASS::digest_t CLASS::
native_finalize_second(const state_t& state) NOEXCEPT
{
    // No hash(state_t) optimizations for sha160 (requires chunk_t/half_t).
    static_assert(is_same_type<state_t, chunk_t>);

    // Hash a state value and finalize it.
    auto state2 = H::get;
    words_t block{};
    inject_left(block, state);
    pad_half(block);
    return native_finalize(state2, block);
}

TEMPLATE
typename CLASS::digest_t CLASS::
native_finalize_double(state_t& state, size_t blocks) NOEXCEPT
{
    // Complete first hash by transforming padding, but don't convert state.
    auto block = pad_blocks(blocks);
    native_transform<false>(state, block);

    // This is native_finalize_second() but reuses the initial block.
    auto state2 = H::get;
    inject_left(block, state);
    pad_half(block);
    return native_finalize(state2, block);
}

// Hash functions start with BE data and end with BE digest_t.
// ----------------------------------------------------------------------------

TEMPLATE
typename CLASS::digest_t CLASS::
native_hash(const block_t& block) NOEXCEPT
{
    auto state = H::get;
    native_transform<true>(state, block);
    return native_finalize(state, pad_block());
}

TEMPLATE
typename CLASS::digest_t CLASS::
native_hash(const half_t& half) NOEXCEPT
{
    // No hash(state_t) optimizations for sha160 (requires chunk_t/half_t).
    static_assert(is_same_type<state_t, chunk_t>);

    // input_left is a non-native endianness conversion.
    auto state = H::get;
    words_t block{};
    input_left(block, half);
    pad_half(block);
    return native_finalize(state, block);
}

TEMPLATE
typename CLASS::digest_t CLASS::
native_hash(const half_t& left, const half_t& right) NOEXCEPT
{
    auto state = H::get;
    words_t block{};
    inject_left(block, array_cast<word_t>(left));
    inject_right(block, array_cast<word_t>(right));
    native_transform<true>(state, block);
    return native_finalize<one>(state);
}

TEMPLATE
typename CLASS::digest_t CLASS::
native_hash(uint8_t byte) NOEXCEPT
{
    constexpr auto pad = bit_hi<uint8_t>;

    block_t block{};
    block.at(0) = byte;
    block.at(1) = pad;
    block.at(63) = byte_bits;
    return native_hash(block);
}

// Double hash functions start with BE data and end with BE digest_t.
// ----------------------------------------------------------------------------

TEMPLATE
typename CLASS::digest_t CLASS::
native_double_hash(const block_t& block) NOEXCEPT
{
    auto state = H::get;
    native_transform<true>(state, block);
    native_transform<false>(state, pad_block());

    // Second hash
    words_t block2{};
    inject_left(block2, state);
    pad_half(block2);
    state = H::get;
    return native_finalize(state, block2);
}

TEMPLATE
typename CLASS::digest_t CLASS::
native_double_hash(const half_t& half) NOEXCEPT
{
    // input_left is a non-native endianness conversion.
    auto state = H::get;
    words_t block{};
    input_left(block, half);
    pad_half(block);
    native_transform<false>(state, block);

    // Second hash
    inject_left(block, state);
    pad_half(block);
    state = H::get;
    return native_finalize(state, block);
}

TEMPLATE
typename CLASS::digest_t CLASS::
native_double_hash(const half_t& left, const half_t& right) NOEXCEPT
{
    auto state = H::get;
    words_t block{};
    inject_left(block, array_cast<word_t>(left));
    inject_right(block, array_cast<word_t>(right));
    native_transform<true>(state, block);
    native_transform<false>(state, pad_block());

    // Second hash
    inject_left(block, state);
    pad_half(block);
    state = H::get;
    return native_finalize(state, block);
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_NATIVE_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_NATIVE_IPP

// Native (SHA-NI or NEON)
// ============================================================================
// The rotating variables method is used for sha native. Tha native
// instructions rely on register locality to achieve performance benefits.
// Implementation of native sha using buffer expansion is horribly slow.
// This split creates bifurcations (additional complexities) in this template.

// protected
// ----------------------------------------------------------------------------

namespace libbitcoin {
namespace system {
namespace sha {

TEMPLATE
template <bool Swap>
INLINE xint128_t CLASS::
endian(xint128_t message) NOEXCEPT
{
    if constexpr (Swap)
        return native_from_big_end<uint32_t>(message);
    else
        return message;
}

TEMPLATE
INLINE void CLASS::
shuffle(xint128_t& state0, xint128_t& state1) NOEXCEPT
{
    sha::shuffle(state0, state1);
}

TEMPLATE
INLINE void CLASS::
unshuffle(xint128_t& state0, xint128_t& state1) NOEXCEPT
{
    sha::unshuffle(state0, state1);
}

TEMPLATE
INLINE void CLASS::
prepare(xint128_t& message0, xint128_t message1) NOEXCEPT
{
    sha::schedule(message0, message1);
}

TEMPLATE
INLINE void CLASS::
prepare(xint128_t& message0, xint128_t message1, xint128_t message2) NOEXCEPT
{
    sha::schedule(message0, message1, message2);
}

TEMPLATE
template <size_t Round>
INLINE void CLASS::
round_4(xint128_t& state0, xint128_t& state1, xint128_t message) NOEXCEPT
{
    constexpr auto r = Round * 4;
    sha::compress(state0, state1, f::add<word_t>(message, f::set<xint128_t>(
        K::get[r + 0], K::get[r + 1], K::get[r + 2], K::get[r + 3])));
}

TEMPLATE
template <size_t Strength, bool_if<Strength == 256>>
INLINE void CLASS::
native_rounds(xint128_t& lo, xint128_t& hi, xint128_t message0,
    xint128_t message1, xint128_t message2, xint128_t message3) NOEXCEPT
{
    const auto start_lo = lo;
    const auto start_hi = hi;

    round_4<0>(lo, hi, message0);
    round_4<1>(lo, hi, message1);
    round_4<2>(lo, hi, message2);
    round_4<3>(lo, hi, message3);

    prepare(message0, message1);
    prepare(message0, message2, message3);
    round_4<4>(lo, hi, message0);

    prepare(message1, message2);
    prepare(message1, message3, message0);
    round_4<5>(lo, hi, message1);

    prepare(message2, message3);
    prepare(message2, message0, message1);
    round_4<6>(lo, hi, message2);

    prepare(message3, message0);
    prepare(message3, message1, message2);
    round_4<7>(lo, hi, message3);

    prepare(message0, message1);
    prepare(message0, message2, message3);
    round_4<8>(lo, hi, message0);

    prepare(message1, message2);
    prepare(message1, message3, message0);
    round_4<9>(lo, hi, message1);

    prepare(message2, message3);
    prepare(message2, message0, message1);
    round_4<10>(lo, hi, message2);

    prepare(message3, message0);
    prepare(message3, message1, message2);
    round_4<11>(lo, hi, message3);

    prepare(message0, message1);
    prepare(message0, message2, message3);
    round_4<12>(lo, hi, message0);

    prepare(message1, message2);
    prepare(message1, message3, message0);
    round_4<13>(lo, hi, message1);

    prepare(message2, message3);
    prepare(message2, message0, message1);
    round_4<14>(lo, hi, message2);

    prepare(message3, message0);
    prepare(message3, message1, message2);
    round_4<15>(lo, hi, message3);

    lo = f::add<word_t>(lo, start_lo);
    hi = f::add<word_t>(hi, start_hi);
}

TEMPLATE
template <size_t Strength, bool_if<Strength == 160>>
INLINE void CLASS::
native_rounds(xint128_t& lo, xint128_t& hi, xint128_t message0,
    xint128_t message1, xint128_t message2, xint128_t message3) NOEXCEPT
{
    const auto start_lo = lo;
    const auto start_hi = hi;

    // The carry is abcd before the last four rounds, seeded to produce e.
    auto carry = f::rol<2, SHA::word_bits>(hi);
    message0 = sha::order_160(message0);
    message1 = sha::order_160(message1);
    message2 = sha::order_160(message2);
    message3 = sha::order_160(message3);

    sha::compress_160<0>(lo, carry, message0);
    sha::compress_160<0>(lo, carry, message1);
    sha::compress_160<0>(lo, carry, message2);
    sha::compress_160<0>(lo, carry, message3);

    sha::schedule_160(message0, message1, message2);
    sha::schedule_160(message0, message3);
    sha::compress_160<0>(lo, carry, message0);

    sha::schedule_160(message1, message2, message3);
    sha::schedule_160(message1, message0);
    sha::compress_160<1>(lo, carry, message1);

    sha::schedule_160(message2, message3, message0);
    sha::schedule_160(message2, message1);
    sha::compress_160<1>(lo, carry, message2);

    sha::schedule_160(message3, message0, message1);
    sha::schedule_160(message3, message2);
    sha::compress_160<1>(lo, carry, message3);

    sha::schedule_160(message0, message1, message2);
    sha::schedule_160(message0, message3);
    sha::compress_160<1>(lo, carry, message0);

    sha::schedule_160(message1, message2, message3);
    sha::schedule_160(message1, message0);
    sha::compress_160<1>(lo, carry, message1);

    sha::schedule_160(message2, message3, message0);
    sha::schedule_160(message2, message1);
    sha::compress_160<2>(lo, carry, message2);

    sha::schedule_160(message3, message0, message1);
    sha::schedule_160(message3, message2);
    sha::compress_160<2>(lo, carry, message3);

    sha::schedule_160(message0, message1, message2);
    sha::schedule_160(message0, message3);
    sha::compress_160<2>(lo, carry, message0);

    sha::schedule_160(message1, message2, message3);
    sha::schedule_160(message1, message0);
    sha::compress_160<2>(lo, carry, message1);

    sha::schedule_160(message2, message3, message0);
    sha::schedule_160(message2, message1);
    sha::compress_160<2>(lo, carry, message2);

    sha::schedule_160(message3, message0, message1);
    sha::schedule_160(message3, message2);
    sha::compress_160<3>(lo, carry, message3);

    sha::schedule_160(message0, message1, message2);
    sha::schedule_160(message0, message3);
    sha::compress_160<3>(lo, carry, message0);

    sha::schedule_160(message1, message2, message3);
    sha::schedule_160(message1, message0);
    sha::compress_160<3>(lo, carry, message1);

    sha::schedule_160(message2, message3, message0);
    sha::schedule_160(message2, message1);
    sha::compress_160<3>(lo, carry, message2);

    sha::schedule_160(message3, message0, message1);
    sha::schedule_160(message3, message2);
    sha::compress_160<3>(lo, carry, message3);

    lo = f::add<word_t>(lo, start_lo);
    hi = sha::next_160(carry, start_hi);
}

TEMPLATE
INLINE void CLASS::
native_rounds(xint128_t& lo, xint128_t& hi, const buffer_t& buffer) NOEXCEPT
{
    const auto& wbuffer = array_cast<xint128_t>(buffer);

    const auto start_lo = lo;
    const auto start_hi = hi;

    sha::compress(lo, hi, f::load(wbuffer[0]));
    sha::compress(lo, hi, f::load(wbuffer[1]));
    sha::compress(lo, hi, f::load(wbuffer[2]));
    sha::compress(lo, hi, f::load(wbuffer[3]));
    sha::compress(lo, hi, f::load(wbuffer[4]));
    sha::compress(lo, hi, f::load(wbuffer[5]));
    sha::compress(lo, hi, f::load(wbuffer[6]));
    sha::compress(lo, hi, f::load(wbuffer[7]));
    sha::compress(lo, hi, f::load(wbuffer[8]));
    sha::compress(lo, hi, f::load(wbuffer[9]));
    sha::compress(lo, hi, f::load(wbuffer[10]));
    sha::compress(lo, hi, f::load(wbuffer[11]));
    sha::compress(lo, hi, f::load(wbuffer[12]));
    sha::compress(lo, hi, f::load(wbuffer[13]));
    sha::compress(lo, hi, f::load(wbuffer[14]));
    sha::compress(lo, hi, f::load(wbuffer[15]));

    lo = f::add<word_t>(lo, start_lo);
    hi = f::add<word_t>(hi, start_hi);
}

TEMPLATE
template <bool Swap>
INLINE void CLASS::
native_rounds(xint128_t& lo, xint128_t& hi, const block_t& block) NOEXCEPT
{
    const auto& wblock = array_cast<xint128_t>(block);
    const auto message0 = endian<Swap>(f::load(wblock[0]));
    const auto message1 = endian<Swap>(f::load(wblock[1]));
    const auto message2 = endian<Swap>(f::load(wblock[2]));
    const auto message3 = endian<Swap>(f::load(wblock[3]));
    native_rounds(lo, hi, message0, message1, message2, message3);
}

// Transforms perform scheduling and compression with optional endianness
// conversion of the block input. State is normalized, which requires some
// additional shuffle/unshuffle calls between transformations of same state.
// State output is not finalized, which is endianness conversion to digest_t.
// ----------------------------------------------------------------------------

TEMPLATE
template <size_t Strength, bool_if<Strength == 256>>
void CLASS::
native_transform(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    // Individual state vars are used vs. array to ensure register
    // persistence.
    auto& wstate = array_cast<xint128_t>(state);
    auto lo = f::load(wstate[0]);
    auto hi = f::load(wstate[1]);
    shuffle(lo, hi);

    // native_rounds must be inlined here (register boundary).
    for (auto& block: blocks)
        native_rounds<true>(lo, hi, block);

    unshuffle(lo, hi);
    f::store(wstate[0], lo);
    f::store(wstate[1], hi);
}

TEMPLATE
template <size_t Strength, bool_if<Strength == 160>>
void CLASS::
native_transform(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    auto& wstate = array_cast<xint128_t, one>(state);
    auto lo = f::load(wstate[0]);
    auto hi = sha::set_160(state[4]);
    sha::shuffle_160(lo);

    // native_rounds must be inlined here (register boundary).
    for (auto& block: blocks)
        native_rounds<true>(lo, hi, block);

    sha::unshuffle_160(lo);
    f::store(wstate[0], lo);
    state[4] = sha::get_160(hi);
}

TEMPLATE
template <bool Swap, size_t Strength, bool_if<Strength == 256>>
void CLASS::
native_transform(state_t& state, const auto& block) NOEXCEPT
{
    auto& wstate = array_cast<xint128_t>(state);
    auto lo = f::load(wstate[0]);
    auto hi = f::load(wstate[1]);
    shuffle(lo, hi);

    // native_rounds must be inlined here (register boundary).
    native_rounds<Swap>(lo, hi, array_cast<byte_t>(block));

    unshuffle(lo, hi);
    f::store(wstate[0], lo);
    f::store(wstate[1], hi);
}

TEMPLATE
template <bool Swap, size_t Strength, bool_if<Strength == 160>>
void CLASS::
native_transform(state_t& state, const auto& block) NOEXCEPT
{
    auto& wstate = array_cast<xint128_t, one>(state);
    auto lo = f::load(wstate[0]);
    auto hi = sha::set_160(state[4]);
    sha::shuffle_160(lo);

    // native_rounds must be inlined here (register boundary).
    native_rounds<Swap>(lo, hi, array_cast<byte_t>(block));

    sha::unshuffle_160(lo);
    f::store(wstate[0], lo);
    state[4] = sha::get_160(hi);
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
// larger iterations), but reduces efficiency for lower block counts. Large
// iterations are 15-16% wheras small iterations are 20-26%.
// native_transform -> native_transform -> native_finalize
// native_transform -> native_finalize
// This can be resolved in the non-iterator scenarios (below) through
// implementation of a finalizing native_transform. This means that padding
// must be incorporated, however since it is not prescheduled or cached this is
// not an issue.

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
template <size_t Strength, bool_if<Strength == 256>>
typename CLASS::digest_t CLASS::
native_finalize(state_t& state, const words_t& pad) NOEXCEPT
{
    auto& wstate = array_cast<xint128_t>(state);
    auto lo = f::load(wstate[0]);
    auto hi = f::load(wstate[1]);
    shuffle(lo, hi);

    // native_rounds must be inlined here (register boundary).
    native_rounds<false>(lo, hi, array_cast<byte_t>(pad));
    unshuffle(lo, hi);

    // digest is copied so that state remains valid (LE).
    std::array<xint128_t, 2> wdigest{};
    f::store(wdigest[0], native_to_big_end<uint32_t>(lo));
    f::store(wdigest[1], native_to_big_end<uint32_t>(hi));
    return array_cast<byte_t, array_count<digest_t>>(wdigest);
}

TEMPLATE
template <size_t Strength, bool_if<Strength == 160>>
typename CLASS::digest_t CLASS::
native_finalize(state_t& state, const words_t& pad) NOEXCEPT
{
    auto& wstate = array_cast<xint128_t, one>(state);
    auto lo = f::load(wstate[0]);
    auto hi = sha::set_160(state[4]);
    sha::shuffle_160(lo);

    // native_rounds must be inlined here (register boundary).
    native_rounds<false>(lo, hi, array_cast<byte_t>(pad));
    sha::unshuffle_160(lo);

    // digest is copied so that state remains valid (LE).
    state_t out{};
    f::store(array_cast<xint128_t, one>(out)[0], lo);
    out[4] = sha::get_160(hi);
    return output(out);
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
    inject_left_half(block, state);
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
    inject_left_half(block, state);
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
    inject_left_half(block, array_cast<word_t>(left));
    inject_right_half(block, array_cast<word_t>(right));
    native_transform<true>(state, block);
    return native_finalize<one>(state);
}

TEMPLATE
typename CLASS::digest_t CLASS::
native_hash(const quart_t& left, const quart_t& right) NOEXCEPT
{
    auto state = H::get;
    words_t block{};
    inject_left_quarter(block, array_cast<word_t>(left));
    inject_right_quarter(block, array_cast<word_t>(right));
    pad_half(block);
    return native_finalize(state, block);
}

TEMPLATE
typename CLASS::digest_t CLASS::
native_hash(uint8_t byte) NOEXCEPT
{
    constexpr auto pad = bit_hi<uint8_t>;

    auto state = H::get;
    words_t block{};
    block.front() = bit_or(shift_left<word_t>(byte, 24),
        shift_left<word_t>(pad, 16));
    block.back() = byte_bits;
    return native_finalize(state, block);
}

// Double hash functions start with BE data and end with BE digest_t.
// ----------------------------------------------------------------------------
// State is retained in registers across all blocks of a double hash.

TEMPLATE
INLINE void CLASS::
native_initialize(xint128_t& lo, xint128_t& hi) NOEXCEPT
{
    const auto& wstate = array_cast<xint128_t>(H::get);
    lo = f::load(wstate[0]);
    hi = f::load(wstate[1]);
    shuffle(lo, hi);
}

TEMPLATE
INLINE typename CLASS::digest_t CLASS::
native_finalize_second(xint128_t lo, xint128_t hi) NOEXCEPT
{
    static constexpr auto pad = chunk_pad();
    const auto& wpad = array_cast<xint128_t>(pad);

    // The first digest (normal form) is the first half of the second block.
    unshuffle(lo, hi);
    const auto message0 = lo;
    const auto message1 = hi;
    const auto message2 = f::load(wpad[0]);
    const auto message3 = f::load(wpad[1]);

    native_initialize(lo, hi);
    native_rounds(lo, hi, message0, message1, message2, message3);
    unshuffle(lo, hi);

    std::array<xint128_t, 2> wdigest{};
    f::store(wdigest[0], native_to_big_end<uint32_t>(lo));
    f::store(wdigest[1], native_to_big_end<uint32_t>(hi));
    return array_cast<byte_t, array_count<digest_t>>(wdigest);
}

TEMPLATE
typename CLASS::digest_t CLASS::
native_double_hash(const block_t& block) NOEXCEPT
{
    static constexpr auto pad = scheduled_pad<one>();

    xint128_t lo{};
    xint128_t hi{};
    native_initialize(lo, hi);
    native_rounds<true>(lo, hi, block);
    native_rounds(lo, hi, pad);
    return native_finalize_second(lo, hi);
}

TEMPLATE
typename CLASS::digest_t CLASS::
native_double_hash(const half_t& half) NOEXCEPT
{
    static constexpr auto pad = chunk_pad();
    const auto& wpad = array_cast<xint128_t>(pad);
    const auto& whalf = array_cast<xint128_t>(half);
    const auto message0 = endian<true>(f::load(whalf[0]));
    const auto message1 = endian<true>(f::load(whalf[1]));
    const auto message2 = f::load(wpad[0]);
    const auto message3 = f::load(wpad[1]);

    xint128_t lo{};
    xint128_t hi{};
    native_initialize(lo, hi);
    native_rounds(lo, hi, message0, message1, message2, message3);
    return native_finalize_second(lo, hi);
}

TEMPLATE
typename CLASS::digest_t CLASS::
native_double_hash(const half_t& left, const half_t& right) NOEXCEPT
{
    static constexpr auto pad = scheduled_pad<one>();
    const auto& wleft = array_cast<xint128_t>(left);
    const auto& wright = array_cast<xint128_t>(right);
    const auto message0 = endian<true>(f::load(wleft[0]));
    const auto message1 = endian<true>(f::load(wleft[1]));
    const auto message2 = endian<true>(f::load(wright[0]));
    const auto message3 = endian<true>(f::load(wright[1]));

    xint128_t lo{};
    xint128_t hi{};
    native_initialize(lo, hi);
    native_rounds(lo, hi, message0, message1, message2, message3);
    native_rounds(lo, hi, pad);
    return native_finalize_second(lo, hi);
}

// Two block transforms interleave the independent rounds of each block, so
// that the latency of each native round is hidden by the other block.
// ----------------------------------------------------------------------------

TEMPLATE
INLINE void CLASS::
native_rounds(xint128_t& lo0, xint128_t& hi0, xint128_t& lo1, xint128_t& hi1,
    xint128_t a0, xint128_t a1, xint128_t a2, xint128_t a3, xint128_t b0,
    xint128_t b1, xint128_t b2, xint128_t b3) NOEXCEPT
{
    const auto start_lo0 = lo0;
    const auto start_hi0 = hi0;
    const auto start_lo1 = lo1;
    const auto start_hi1 = hi1;

    round_4<0>(lo0, hi0, a0);
    round_4<0>(lo1, hi1, b0);
    round_4<1>(lo0, hi0, a1);
    round_4<1>(lo1, hi1, b1);
    round_4<2>(lo0, hi0, a2);
    round_4<2>(lo1, hi1, b2);
    round_4<3>(lo0, hi0, a3);
    round_4<3>(lo1, hi1, b3);

    prepare(a0, a1);
    prepare(b0, b1);
    prepare(a0, a2, a3);
    prepare(b0, b2, b3);
    round_4<4>(lo0, hi0, a0);
    round_4<4>(lo1, hi1, b0);

    prepare(a1, a2);
    prepare(b1, b2);
    prepare(a1, a3, a0);
    prepare(b1, b3, b0);
    round_4<5>(lo0, hi0, a1);
    round_4<5>(lo1, hi1, b1);

    prepare(a2, a3);
    prepare(b2, b3);
    prepare(a2, a0, a1);
    prepare(b2, b0, b1);
    round_4<6>(lo0, hi0, a2);
    round_4<6>(lo1, hi1, b2);

    prepare(a3, a0);
    prepare(b3, b0);
    prepare(a3, a1, a2);
    prepare(b3, b1, b2);
    round_4<7>(lo0, hi0, a3);
    round_4<7>(lo1, hi1, b3);

    prepare(a0, a1);
    prepare(b0, b1);
    prepare(a0, a2, a3);
    prepare(b0, b2, b3);
    round_4<8>(lo0, hi0, a0);
    round_4<8>(lo1, hi1, b0);

    prepare(a1, a2);
    prepare(b1, b2);
    prepare(a1, a3, a0);
    prepare(b1, b3, b0);
    round_4<9>(lo0, hi0, a1);
    round_4<9>(lo1, hi1, b1);

    prepare(a2, a3);
    prepare(b2, b3);
    prepare(a2, a0, a1);
    prepare(b2, b0, b1);
    round_4<10>(lo0, hi0, a2);
    round_4<10>(lo1, hi1, b2);

    prepare(a3, a0);
    prepare(b3, b0);
    prepare(a3, a1, a2);
    prepare(b3, b1, b2);
    round_4<11>(lo0, hi0, a3);
    round_4<11>(lo1, hi1, b3);

    prepare(a0, a1);
    prepare(b0, b1);
    prepare(a0, a2, a3);
    prepare(b0, b2, b3);
    round_4<12>(lo0, hi0, a0);
    round_4<12>(lo1, hi1, b0);

    prepare(a1, a2);
    prepare(b1, b2);
    prepare(a1, a3, a0);
    prepare(b1, b3, b0);
    round_4<13>(lo0, hi0, a1);
    round_4<13>(lo1, hi1, b1);

    prepare(a2, a3);
    prepare(b2, b3);
    prepare(a2, a0, a1);
    prepare(b2, b0, b1);
    round_4<14>(lo0, hi0, a2);
    round_4<14>(lo1, hi1, b2);

    prepare(a3, a0);
    prepare(b3, b0);
    prepare(a3, a1, a2);
    prepare(b3, b1, b2);
    round_4<15>(lo0, hi0, a3);
    round_4<15>(lo1, hi1, b3);

    lo0 = f::add<word_t>(lo0, start_lo0);
    hi0 = f::add<word_t>(hi0, start_hi0);
    lo1 = f::add<word_t>(lo1, start_lo1);
    hi1 = f::add<word_t>(hi1, start_hi1);
}

TEMPLATE
INLINE void CLASS::
native_rounds(xint128_t& lo0, xint128_t& hi0, xint128_t& lo1, xint128_t& hi1,
    const buffer_t& buffer) NOEXCEPT
{
    const auto& wbuffer = array_cast<xint128_t>(buffer);

    const auto start_lo0 = lo0;
    const auto start_hi0 = hi0;
    const auto start_lo1 = lo1;
    const auto start_hi1 = hi1;

    sha::compress(lo0, hi0, f::load(wbuffer[0]));
    sha::compress(lo1, hi1, f::load(wbuffer[0]));
    sha::compress(lo0, hi0, f::load(wbuffer[1]));
    sha::compress(lo1, hi1, f::load(wbuffer[1]));
    sha::compress(lo0, hi0, f::load(wbuffer[2]));
    sha::compress(lo1, hi1, f::load(wbuffer[2]));
    sha::compress(lo0, hi0, f::load(wbuffer[3]));
    sha::compress(lo1, hi1, f::load(wbuffer[3]));
    sha::compress(lo0, hi0, f::load(wbuffer[4]));
    sha::compress(lo1, hi1, f::load(wbuffer[4]));
    sha::compress(lo0, hi0, f::load(wbuffer[5]));
    sha::compress(lo1, hi1, f::load(wbuffer[5]));
    sha::compress(lo0, hi0, f::load(wbuffer[6]));
    sha::compress(lo1, hi1, f::load(wbuffer[6]));
    sha::compress(lo0, hi0, f::load(wbuffer[7]));
    sha::compress(lo1, hi1, f::load(wbuffer[7]));
    sha::compress(lo0, hi0, f::load(wbuffer[8]));
    sha::compress(lo1, hi1, f::load(wbuffer[8]));
    sha::compress(lo0, hi0, f::load(wbuffer[9]));
    sha::compress(lo1, hi1, f::load(wbuffer[9]));
    sha::compress(lo0, hi0, f::load(wbuffer[10]));
    sha::compress(lo1, hi1, f::load(wbuffer[10]));
    sha::compress(lo0, hi0, f::load(wbuffer[11]));
    sha::compress(lo1, hi1, f::load(wbuffer[11]));
    sha::compress(lo0, hi0, f::load(wbuffer[12]));
    sha::compress(lo1, hi1, f::load(wbuffer[12]));
    sha::compress(lo0, hi0, f::load(wbuffer[13]));
    sha::compress(lo1, hi1, f::load(wbuffer[13]));
    sha::compress(lo0, hi0, f::load(wbuffer[14]));
    sha::compress(lo1, hi1, f::load(wbuffer[14]));
    sha::compress(lo0, hi0, f::load(wbuffer[15]));
    sha::compress(lo1, hi1, f::load(wbuffer[15]));

    lo0 = f::add<word_t>(lo0, start_lo0);
    hi0 = f::add<word_t>(hi0, start_hi0);
    lo1 = f::add<word_t>(lo1, start_lo1);
    hi1 = f::add<word_t>(hi1, start_hi1);
}

TEMPLATE
template <bool Swap>
INLINE void CLASS::
native_rounds(xint128_t& lo0, xint128_t& hi0, xint128_t& lo1, xint128_t& hi1,
    const block_t& block0, const block_t& block1) NOEXCEPT
{
    const auto& wblock0 = array_cast<xint128_t>(block0);
    const auto& wblock1 = array_cast<xint128_t>(block1);
    const auto a0 = endian<Swap>(f::load(wblock0[0]));
    const auto a1 = endian<Swap>(f::load(wblock0[1]));
    const auto a2 = endian<Swap>(f::load(wblock0[2]));
    const auto a3 = endian<Swap>(f::load(wblock0[3]));
    const auto b0 = endian<Swap>(f::load(wblock1[0]));
    const auto b1 = endian<Swap>(f::load(wblock1[1]));
    const auto b2 = endian<Swap>(f::load(wblock1[2]));
    const auto b3 = endian<Swap>(f::load(wblock1[3]));
    native_rounds(lo0, hi0, lo1, hi1, a0, a1, a2, a3, b0, b1, b2, b3);
}

TEMPLATE
INLINE void CLASS::
native_finalize_second(digest_t& digest0, digest_t& digest1, xint128_t lo0,
    xint128_t hi0, xint128_t lo1, xint128_t hi1) NOEXCEPT
{
    static constexpr auto pad = chunk_pad();
    const auto& wpad = array_cast<xint128_t>(pad);
    const auto pad0 = f::load(wpad[0]);
    const auto pad1 = f::load(wpad[1]);

    // The first digest (normal form) is the first half of the second block.
    unshuffle(lo0, hi0);
    unshuffle(lo1, hi1);
    const auto a0 = lo0;
    const auto a1 = hi0;
    const auto b0 = lo1;
    const auto b1 = hi1;

    native_initialize(lo0, hi0);
    native_initialize(lo1, hi1);
    native_rounds(lo0, hi0, lo1, hi1, a0, a1, pad0, pad1, b0, b1, pad0, pad1);
    unshuffle(lo0, hi0);
    unshuffle(lo1, hi1);

    auto& wdigest0 = array_cast<xint128_t>(digest0);
    auto& wdigest1 = array_cast<xint128_t>(digest1);
    f::store(wdigest0[0], native_to_big_end<uint32_t>(lo0));
    f::store(wdigest0[1], native_to_big_end<uint32_t>(hi0));
    f::store(wdigest1[0], native_to_big_end<uint32_t>(lo1));
    f::store(wdigest1[1], native_to_big_end<uint32_t>(hi1));
}

TEMPLATE
void CLASS::
native_double_hash(digest_t& digest0, digest_t& digest1,
    const block_t& block0, const block_t& block1) NOEXCEPT
{
    static constexpr auto pad = scheduled_pad<one>();

    xint128_t lo0{};
    xint128_t hi0{};
    xint128_t lo1{};
    xint128_t hi1{};
    native_initialize(lo0, hi0);
    native_initialize(lo1, hi1);
    native_rounds<true>(lo0, hi0, lo1, hi1, block0, block1);
    native_rounds(lo0, hi0, lo1, hi1, pad);
    native_finalize_second(digest0, digest1, lo0, hi0, lo1, hi1);
}

// Native SHA512 (single lane)
// ============================================================================
// Four 64 bit word registers, as sha256 native but with 20 rounds of 4.

TEMPLATE
template <bool Swap>
INLINE sha::xquad_t CLASS::
endian(xquad_t message) NOEXCEPT
{
    if constexpr (Swap)
        return native_from_big_end<uint64_t>(message);
    else
        return message;
}

TEMPLATE
INLINE void CLASS::
shuffle(xquad_t& state0, xquad_t& state1) NOEXCEPT
{
    sha::shuffle_512(state0, state1);
}

TEMPLATE
INLINE void CLASS::
unshuffle(xquad_t& state0, xquad_t& state1) NOEXCEPT
{
    sha::unshuffle_512(state0, state1);
}

TEMPLATE
INLINE void CLASS::
prepare(xquad_t& message0, xquad_t message1) NOEXCEPT
{
    sha::schedule_512(message0, message1);
}

TEMPLATE
INLINE void CLASS::
prepare(xquad_t& message0, xquad_t message1, xquad_t message2) NOEXCEPT
{
    sha::schedule_512(message0, message1, message2);
}

TEMPLATE
template <size_t Round>
INLINE void CLASS::
round_4(xquad_t& state0, xquad_t& state1, xquad_t message) NOEXCEPT
{
    constexpr auto r = Round * 4;
    sha::compress_512(state0, state1, sha::add_512(message, sha::set_512(
        K::get[r + 0], K::get[r + 1], K::get[r + 2], K::get[r + 3])));
}

TEMPLATE
INLINE void CLASS::
native_rounds(xquad_t& lo, xquad_t& hi, xquad_t message0, xquad_t message1,
    xquad_t message2, xquad_t message3) NOEXCEPT
{
    const auto start_lo = lo;
    const auto start_hi = hi;

    round_4<0>(lo, hi, message0);
    round_4<1>(lo, hi, message1);
    round_4<2>(lo, hi, message2);
    round_4<3>(lo, hi, message3);

    prepare(message0, message1);
    prepare(message0, message2, message3);
    round_4<4>(lo, hi, message0);

    prepare(message1, message2);
    prepare(message1, message3, message0);
    round_4<5>(lo, hi, message1);

    prepare(message2, message3);
    prepare(message2, message0, message1);
    round_4<6>(lo, hi, message2);

    prepare(message3, message0);
    prepare(message3, message1, message2);
    round_4<7>(lo, hi, message3);

    prepare(message0, message1);
    prepare(message0, message2, message3);
    round_4<8>(lo, hi, message0);

    prepare(message1, message2);
    prepare(message1, message3, message0);
    round_4<9>(lo, hi, message1);

    prepare(message2, message3);
    prepare(message2, message0, message1);
    round_4<10>(lo, hi, message2);

    prepare(message3, message0);
    prepare(message3, message1, message2);
    round_4<11>(lo, hi, message3);

    prepare(message0, message1);
    prepare(message0, message2, message3);
    round_4<12>(lo, hi, message0);

    prepare(message1, message2);
    prepare(message1, message3, message0);
    round_4<13>(lo, hi, message1);

    prepare(message2, message3);
    prepare(message2, message0, message1);
    round_4<14>(lo, hi, message2);

    prepare(message3, message0);
    prepare(message3, message1, message2);
    round_4<15>(lo, hi, message3);

    prepare(message0, message1);
    prepare(message0, message2, message3);
    round_4<16>(lo, hi, message0);

    prepare(message1, message2);
    prepare(message1, message3, message0);
    round_4<17>(lo, hi, message1);

    prepare(message2, message3);
    prepare(message2, message0, message1);
    round_4<18>(lo, hi, message2);

    prepare(message3, message0);
    prepare(message3, message1, message2);
    round_4<19>(lo, hi, message3);

    lo = sha::add_512(lo, start_lo);
    hi = sha::add_512(hi, start_hi);
}

TEMPLATE
template <bool Swap>
INLINE void CLASS::
native_rounds(xquad_t& lo, xquad_t& hi, const block_t& block) NOEXCEPT
{
    const auto& wblock = array_cast<xquad_t>(block);
    const auto message0 = endian<Swap>(sha::load_512(wblock[0]));
    const auto message1 = endian<Swap>(sha::load_512(wblock[1]));
    const auto message2 = endian<Swap>(sha::load_512(wblock[2]));
    const auto message3 = endian<Swap>(sha::load_512(wblock[3]));
    native_rounds(lo, hi, message0, message1, message2, message3);
}

TEMPLATE
template <size_t Strength, bool_if<Strength == 512>>
void CLASS::
native_transform(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    auto& wstate = array_cast<xquad_t>(state);
    auto lo = sha::load_512(wstate[0]);
    auto hi = sha::load_512(wstate[1]);
    shuffle(lo, hi);

    // native_rounds must be inlined here (register boundary).
    for (auto& block: blocks)
        native_rounds<true>(lo, hi, block);

    unshuffle(lo, hi);
    sha::store_512(wstate[0], lo);
    sha::store_512(wstate[1], hi);
}

TEMPLATE
template <bool Swap, size_t Strength, bool_if<Strength == 512>>
void CLASS::
native_transform(state_t& state, const auto& block) NOEXCEPT
{
    auto& wstate = array_cast<xquad_t>(state);
    auto lo = sha::load_512(wstate[0]);
    auto hi = sha::load_512(wstate[1]);
    shuffle(lo, hi);

    // native_rounds must be inlined here (register boundary).
    native_rounds<Swap>(lo, hi, array_cast<byte_t>(block));

    unshuffle(lo, hi);
    sha::store_512(wstate[0], lo);
    sha::store_512(wstate[1], hi);
}

TEMPLATE
template <size_t Strength, bool_if<Strength == 512>>
typename CLASS::digest_t CLASS::
native_finalize(state_t& state, const words_t& pad) NOEXCEPT
{
    auto& wstate = array_cast<xquad_t>(state);
    auto lo = sha::load_512(wstate[0]);
    auto hi = sha::load_512(wstate[1]);
    shuffle(lo, hi);

    // native_rounds must be inlined here (register boundary).
    native_rounds<false>(lo, hi, array_cast<byte_t>(pad));
    unshuffle(lo, hi);

    // digest is copied so that state remains valid (LE).
    state_t out{};
    auto& wout = array_cast<xquad_t>(out);
    sha::store_512(wout[0], lo);
    sha::store_512(wout[1], hi);
    return output(out);
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

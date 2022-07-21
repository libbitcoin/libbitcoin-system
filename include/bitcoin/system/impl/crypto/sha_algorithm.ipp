/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SHA_ALGORITHM_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SHA_ALGORITHM_IPP

#include <bit>
#include <type_traits>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace sha {

template <typename SHA>
constexpr auto algorithm<SHA>::
choice(auto a, auto b, auto c) NOEXCEPT
{
    return (a & (b ^ c)) ^ c;
}

template <typename SHA>
constexpr auto algorithm<SHA>::
majory(auto a, auto b, auto c) NOEXCEPT
{
    return (a & (b | c)) | (b & c);
}

template <typename SHA>
constexpr auto algorithm<SHA>::
SIGMA0(auto a) NOEXCEPT
{
    return std::rotr(a, 2) ^ std::rotr(a, 13) ^ std::rotr(a, 22);
}

template <typename SHA>
constexpr auto algorithm<SHA>::
SIGMA1(auto a) NOEXCEPT
{
    return std::rotr(a, 6) ^ std::rotr(a, 11) ^ std::rotr(a, 25);
}

template <typename SHA>
constexpr auto algorithm<SHA>::
sigma0(auto a) NOEXCEPT
{
    return std::rotr(a, 7) ^ std::rotr(a, 18) ^ (a >> 3);
}

template <typename SHA>
constexpr auto algorithm<SHA>::
sigma1(auto a) NOEXCEPT
{
    return std::rotr(a, 17) ^ std::rotr(a, 19) ^ (a >> 10);
}

template <typename SHA>
constexpr void algorithm<SHA>::
round1(auto a, auto b, auto c, auto& d, auto e, auto f, auto g,
    auto& h, auto k) NOEXCEPT
{
    const auto x = SIGMA0(a) + majory(a, b, c);
    const auto y = SIGMA1(e) + choice(e, f, g) + h + k;
    d += y;
    h  = x + y;
}

template <typename SHA>
template<size_t Round>
constexpr void algorithm<SHA>::
round(state_t& out, const buffer_t& in) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    round1(
        out[(block_bytes + 0 - Round) % state_words],
        out[(block_bytes + 1 - Round) % state_words],
        out[(block_bytes + 2 - Round) % state_words],
        out[(block_bytes + 3 - Round) % state_words], // in/out
        out[(block_bytes + 4 - Round) % state_words],
        out[(block_bytes + 5 - Round) % state_words],
        out[(block_bytes + 6 - Round) % state_words],
        out[(block_bytes + 7 - Round) % state_words], // in/out
        in[Round] + SHA::K::get[Round]);
    BC_POP_WARNING()
}

template <typename SHA>
template<size_t Word>
constexpr void algorithm<SHA>::
expand(buffer_t& out) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    out[Word] =
        sigma1(out[Word -  2]) + out[Word -  7] +
        sigma0(out[Word - 15]) + out[Word - 16];
    BC_POP_WARNING()
}

template <typename SHA>
constexpr void algorithm<SHA>::
expand48(buffer_t& out) NOEXCEPT
{
    // TODO: change to meta-unrolled loop.
    expand<16>(out);
    expand<17>(out);
    expand<18>(out);
    expand<19>(out);
    expand<20>(out);
    expand<21>(out);
    expand<22>(out);
    expand<23>(out);
    expand<24>(out);
    expand<25>(out);
    expand<26>(out);
    expand<27>(out);
    expand<28>(out);
    expand<29>(out);
    expand<30>(out);
    expand<31>(out);
    expand<32>(out);
    expand<33>(out);
    expand<34>(out);
    expand<35>(out);
    expand<36>(out);
    expand<37>(out);
    expand<38>(out);
    expand<39>(out);
    expand<40>(out);
    expand<41>(out);
    expand<42>(out);
    expand<43>(out);
    expand<44>(out);
    expand<45>(out);
    expand<46>(out);
    expand<47>(out);
    expand<48>(out);
    expand<49>(out);
    expand<50>(out);
    expand<51>(out);
    expand<52>(out);
    expand<53>(out);
    expand<54>(out);
    expand<55>(out);
    expand<56>(out);
    expand<57>(out);
    expand<58>(out);
    expand<59>(out);
    expand<60>(out);
    expand<61>(out);
    expand<62>(out);
    expand<63>(out);
}

template <typename SHA>
constexpr void algorithm<SHA>::
rounds(state_t& out, const buffer_t& in) NOEXCEPT
{
    // TODO: change to meta-unrolled loop.
    // At least 64 rounds for both sha256/512.
    round<0>(out, in);
    round<1>(out, in);
    round<2>(out, in);
    round<3>(out, in);
    round<4>(out, in);
    round<5>(out, in);
    round<6>(out, in);
    round<7>(out, in);
    round<8>(out, in);
    round<9>(out, in);
    round<10>(out, in);
    round<11>(out, in);
    round<12>(out, in);
    round<13>(out, in);
    round<14>(out, in);
    round<15>(out, in);
    round<16>(out, in);
    round<17>(out, in);
    round<18>(out, in);
    round<19>(out, in);
    round<20>(out, in);
    round<21>(out, in);
    round<22>(out, in);
    round<23>(out, in);
    round<24>(out, in);
    round<25>(out, in);
    round<26>(out, in);
    round<27>(out, in);
    round<28>(out, in);
    round<29>(out, in);
    round<30>(out, in);
    round<31>(out, in);
    round<32>(out, in);
    round<33>(out, in);
    round<34>(out, in);
    round<35>(out, in);
    round<36>(out, in);
    round<37>(out, in);
    round<38>(out, in);
    round<39>(out, in);
    round<40>(out, in);
    round<41>(out, in);
    round<42>(out, in);
    round<43>(out, in);
    round<44>(out, in);
    round<45>(out, in);
    round<46>(out, in);
    round<47>(out, in);
    round<48>(out, in);
    round<49>(out, in);
    round<50>(out, in);
    round<51>(out, in);
    round<52>(out, in);
    round<53>(out, in);
    round<54>(out, in);
    round<55>(out, in);
    round<56>(out, in);
    round<57>(out, in);
    round<58>(out, in);
    round<59>(out, in);
    round<60>(out, in);
    round<61>(out, in);
    round<62>(out, in);
    round<63>(out, in);

    // 16 more rounds for sha512
    if constexpr (SHA::K::rounds == 80)
    {
        round<64>(out, in);
        round<65>(out, in);
        round<66>(out, in);
        round<67>(out, in);
        round<68>(out, in);
        round<69>(out, in);
        round<70>(out, in);
        round<71>(out, in);
        round<72>(out, in);
        round<73>(out, in);
        round<74>(out, in);
        round<75>(out, in);
        round<76>(out, in);
        round<77>(out, in);
        round<78>(out, in);
        round<79>(out, in);
    }
}

template <typename SHA>
constexpr void algorithm<SHA>::
copying8(buffer_t& out, const state_t& in) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = in[3];
        out[4] = in[4];
        out[5] = in[5];
        out[6] = in[6];
        out[7] = in[7];
        BC_POP_WARNING()
    }
    else
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        auto& to = narrowing_array_cast<word_t, in.size()>(out);
        BC_POP_WARNING()
        to = in;
    }
}

template <typename SHA>
constexpr void algorithm<SHA>::
summary8(state_t& out, const state_t& in) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    out[0] += in[0];
    out[1] += in[1];
    out[2] += in[2];
    out[3] += in[3];
    out[4] += in[4];
    out[5] += in[5];
    out[6] += in[6];
    out[7] += in[7];
    BC_POP_WARNING()
}

template <typename SHA>
constexpr void algorithm<SHA>::
paddin64(buffer_t& out) NOEXCEPT
{
    // SHA::pad::buffer is the output of expand48(SHA::pad::block).
    out = SHA::pad::buffer;
}

template <typename SHA>
constexpr void algorithm<SHA>::
padding8(buffer_t& out) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        out[ 8] = SHA::pad::chunk[0];
        out[ 9] = SHA::pad::chunk[1];
        out[10] = SHA::pad::chunk[2];
        out[11] = SHA::pad::chunk[3];
        out[12] = SHA::pad::chunk[4];
        out[13] = SHA::pad::chunk[5];
        out[14] = SHA::pad::chunk[6];
        out[15] = SHA::pad::chunk[7];
        BC_POP_WARNING()
    }
    else
    {
        // TODO: make safe offsetting array cast.
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        constexpr auto pad_size = SHA::pad::chunk.size();
        auto& to = unsafe_array_cast<word_t, pad_size>(&out[pad_size]);
        BC_POP_WARNING()
        to = SHA::pad::chunk;
    }
}

template <typename SHA>
constexpr void algorithm<SHA>::
paddin16(buffer_t& out, count_t blocks) NOEXCEPT
{

    if (std::is_constant_evaluated())
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        out[0] = SHA::pad::stream[0];
        out[1] = SHA::pad::stream[1];
        out[2] = SHA::pad::stream[2];
        out[3] = SHA::pad::stream[3];
        out[4] = SHA::pad::stream[4];
        out[5] = SHA::pad::stream[5];
        ////out[6] = SHA::pad::stream[6];
        ////out[7] = SHA::pad::stream[7];
        BC_POP_WARNING()
    }
    else
    {
        auto& to = narrowing_array_cast<word_t, SHA::pad::stream.size()>(out);
        to = SHA::pad::stream;
    }

    // Copy in the streamed bit count (count_t is twice the size of word_t).
    const auto bits = to_bits<count_t>(blocks * block_size);

    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    out[6] = narrow_cast<word_t>(bits >> bc::bits<word_t>);
    out[7] = narrow_cast<word_t>(bits);
    BC_POP_WARNING()
}

template <typename SHA>
constexpr void algorithm<SHA>::
bigend16(buffer_t& out, const block_t& in) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        from_big< 0 * SHA::word_bytes>(out[ 0], in);
        from_big< 1 * SHA::word_bytes>(out[ 1], in);
        from_big< 2 * SHA::word_bytes>(out[ 2], in);
        from_big< 3 * SHA::word_bytes>(out[ 3], in);
        from_big< 4 * SHA::word_bytes>(out[ 4], in);
        from_big< 5 * SHA::word_bytes>(out[ 5], in);
        from_big< 6 * SHA::word_bytes>(out[ 6], in);
        from_big< 7 * SHA::word_bytes>(out[ 7], in);
        from_big< 8 * SHA::word_bytes>(out[ 8], in);
        from_big< 9 * SHA::word_bytes>(out[ 9], in);
        from_big<10 * SHA::word_bytes>(out[10], in);
        from_big<11 * SHA::word_bytes>(out[11], in);
        from_big<12 * SHA::word_bytes>(out[12], in);
        from_big<13 * SHA::word_bytes>(out[13], in);
        from_big<14 * SHA::word_bytes>(out[14], in);
        from_big<15 * SHA::word_bytes>(out[15], in);
        BC_POP_WARNING()
    }
    else
    {
        auto& from = array_cast<word_t>(in);
        auto& to = narrowing_array_cast<word_t, block_words>(out);
        from_big_endians(to, from);
    }
}

template <typename SHA>
constexpr void algorithm<SHA>::
bigend08(buffer_t& out, const half_t& in) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        from_big<0 * SHA::word_bytes>(out[0], in);
        from_big<1 * SHA::word_bytes>(out[1], in);
        from_big<2 * SHA::word_bytes>(out[2], in);
        from_big<3 * SHA::word_bytes>(out[3], in);
        from_big<4 * SHA::word_bytes>(out[4], in);
        from_big<5 * SHA::word_bytes>(out[5], in);
        from_big<6 * SHA::word_bytes>(out[6], in);
        from_big<7 * SHA::word_bytes>(out[7], in);
        BC_POP_WARNING()
    }
    else
    {
        auto& from = array_cast<SHA::word_t>(in);
        auto& to = narrowing_array_cast<word_t, chunk_words>(out);
        from_big_endians(to, from);
    }
}

template <typename SHA>
constexpr void algorithm<SHA>::
bigend08(digest_t& out, const state_t& in) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        to_big<0 * SHA::word_bytes>(out, in[0]);
        to_big<1 * SHA::word_bytes>(out, in[1]);
        to_big<2 * SHA::word_bytes>(out, in[2]);
        to_big<3 * SHA::word_bytes>(out, in[3]);
        to_big<4 * SHA::word_bytes>(out, in[4]);
        to_big<5 * SHA::word_bytes>(out, in[5]);
        to_big<6 * SHA::word_bytes>(out, in[6]);
        to_big<7 * SHA::word_bytes>(out, in[7]);
        BC_POP_WARNING()
    }
    else
    {
        auto& to = array_cast<word_t>(out);
        to_big_endians(to, in);
    }
}

// Streaming hash functions.
// ---------------------------------------------------------------------------

template <typename SHA>
VCONSTEXPR void algorithm<SHA>::
accumulate(state_t& state, const blocks& blocks) NOEXCEPT
{
    buffer_t words{};

    for (auto& block: blocks)
    {
        const state_t start{ state };
        bigend16(words, block);
        expand48(words);
        rounds(state, words);
        summary8(state, start);
    }
}

// This is a specialization of multiple blocks so that we can have constexpr
// and avoid the (trivial) overhead of iteration and a vector construction.
template <typename SHA>
constexpr void algorithm<SHA>::
accumulate(state_t& state, const block_t& block) NOEXCEPT
{
    buffer_t words{};
    const state_t start{ state };
    bigend16(words, block);
    expand48(words);
    rounds(state, words);
    summary8(state, start);
}

template <typename SHA>
constexpr typename algorithm<SHA>::digest_t algorithm<SHA>::
finalize(const state_t& state) NOEXCEPT
{
    digest_t out{};
    bigend08(out, state);
    return out;
}

// Finalized single hash functions.
// ---------------------------------------------------------------------------
// Cannot be vectorized as the hash function is cumulative. But with a full
// block can take advantage of multi-lane single block intrinsics.

template <typename SHA>
constexpr typename algorithm<SHA>::digest_t algorithm<SHA>::
hash(const blocks& blocks) NOEXCEPT
{
    // process N blocks in accumulator loop.
    auto state = SHA::H::get;
    accumulate(state, blocks);

    // full pad block for N blocks (pre-endianed, size added).
    buffer space{};
    paddin16(space, blocks.size());
    expand48(space);
    const auto save = state;
    rounds(state, space);
    summary8(state, save);
    return finalize(state);
}

template <typename SHA>
constexpr typename algorithm<SHA>::digest_t algorithm<SHA>::
hash(const block_t& block) NOEXCEPT
{
    buffer space{};

    // process 1 block in (avoid accumulate to reuse buffer).
    bigend16(space, block);
    expand48(space);
    auto state = SHA::H::get;
    rounds(state, space);
    summary8(state, SHA::H::get);

    // full pad block for 1 block (pre-sized/endianed/expanded).
    paddin64(space);
    const auto save = state;
    rounds(state, space);
    summary8(state, save);
    return finalize(state);
}

template <typename SHA>
constexpr typename algorithm<SHA>::digest_t algorithm<SHA>::
hash(const half_t& half) NOEXCEPT
{
    // process 1/2 data block with 1/2 pad block (pre-sized/endianed).
    buffer space{};
    bigend08(space, half);
    padding8(space);
    expand48(space);
    auto state = SHA::H::get;
    rounds(state, space);
    summary8(state, SHA::H::get);
    return finalize(state);
}

// Finalized merkle hash functions.
// ---------------------------------------------------------------------------
// TODO: generalize to vectorized single block hash (hash_set).
// TODO: a second round (double sha) is then just vectorized chunk hash.
// TODO: a merkle hash is then iteration over vectorized single and second.
// TODO: current optimizations limit vectorization to specialized CPUs and are
// TODO: limited to 2/4/8 concurrent blocks, but this can be fully vectorized
// TODO: on any platform by simply using std::for_each(parallel...). This can
// TODO: also be layered over CPU specific vectorizations and sha-ni.

template <typename SHA>
VCONSTEXPR typename algorithm<SHA>::digests algorithm<SHA>::
merkle(const blocks& blocks) NOEXCEPT
{
    buffer space{};
    digests out{};
    out.reserve(to_half(blocks.size()));

    for (auto& block: blocks)
    {
        bigend16(space, block);         // hash(block)[part 1]
        expand48(space);                // hash(block)[part 1]
        auto state = SHA::H::get;            // hash(block)[part 1]
        rounds(state, space);           // hash(block)[part 1]
        summary8(state, SHA::H::get);        // hash(block)[part 1]

        paddin64(space);                // hash(block)[part 2]
        const auto save = state;        // hash(block)[part 2]
        rounds(state, space);           // hash(block)[part 2]
        summary8(state, save);          // hash(block)[part 2]

        copying8(space, state);         // hash(half)
        padding8(space);                // hash(half)
        expand48(space);                // hash(half)
        state = SHA::H::get;                 // hash(half)
        rounds(state, space);           // hash(half)
        summary8(state, SHA::H::get);        // hash(half)
        out.push_back(finalize(state)); // hash(half)
    }

    return out;
}

// This is a specialization of multiple blocks so that we can have constexpr
// and avoid the (trivial) overhead of iteration and a vector construction.
template <typename SHA>
constexpr typename algorithm<SHA>::digest_t algorithm<SHA>::
merkle(const block_t& block) NOEXCEPT
{
    buffer space{};

    bigend16(space, block);             // hash(block)[part 1]
    expand48(space);                    // hash(block)[part 1]
    auto state = SHA::H::get;                // hash(block)[part 1]
    rounds(state, space);               // hash(block)[part 1]
    summary8(state, SHA::H::get);            // hash(block)[part 1]

    paddin64(space);                    // hash(block)[part 2]
    const auto save = state;            // hash(block)[part 2]
    rounds(state, space);               // hash(block)[part 2]
    summary8(state, save);              // hash(block)[part 2]

    copying8(space, state);             // hash(half)
    padding8(space);                    // hash(half)
    expand48(space);                    // hash(half)
    state = SHA::H::get;                     // hash(half)
    rounds(state, space);               // hash(half)
    summary8(state, SHA::H::get);            // hash(half)
    return finalize(state);             // hash(half)
}

#ifdef NORMALIZED_NATIVE_MERKLE
/// Bit count is encoded into message block as 64 bit big-endian.
/// This is by convention and not subject to platform endianness.
/// The padding sentinel is effectively a single byte (not a full word).
constexpr auto xxxx = 0xff_u8; // unused space
constexpr auto pppp = 0x80_u8; // pad sentinel
constexpr auto zzzz = 0x00_u8; // zeros of bit count
constexpr auto full = to_bits(block_size) >> byte_bits;  // bit count >> 8
constexpr auto half = to_bits(digest_size) >> byte_bits; // bit count >> 8

/// Padding for full block hash round (64 bytes of pad/count).
/// The buffer is prefilled with padding and a count of 512 bits.
alignas(16) constexpr block merkle_pad64
{
    pppp, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, full, zzzz
};

/// Padding for a half block hash round (32 bytes of pad/count).
/// The buffer is prefilled with padding and a count of 256 bits.
alignas(16) constexpr block merkle_pad32
{
    xxxx, xxxx, xxxx, xxxx, xxxx, xxxx, xxxx, xxxx,
    xxxx, xxxx, xxxx, xxxx, xxxx, xxxx, xxxx, xxxx,
    xxxx, xxxx, xxxx, xxxx, xxxx, xxxx, xxxx, xxxx,
    xxxx, xxxx, xxxx, xxxx, xxxx, xxxx, xxxx, xxxx,

    pppp, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, half, zzzz
};

constexpr void bigend08(block& out, const state& in) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        constexpr auto size = sizeof(uint32_t);
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        to_big<0 * size>(out, in[0]);
        to_big<1 * size>(out, in[1]);
        to_big<2 * size>(out, in[2]);
        to_big<3 * size>(out, in[3]);
        to_big<4 * size>(out, in[4]);
        to_big<5 * size>(out, in[5]);
        to_big<6 * size>(out, in[6]);
        to_big<7 * size>(out, in[7]);
        BC_POP_WARNING()
    }
    else
    {
        auto& to = narrowing_array_cast<uint8_t, digest_size>(out);
        bigend08(to, in);
    }
}

constexpr digest merkle(const block& block) NOEXCEPT
{
    auto state = sha::initial;
    accumulate(state, block);
    accumulate(state, merkle_pad64);
    auto buffer = merkle_pad32;
    bigend08(buffer, state);
    state = sha::initial;
    accumulate(state, buffer);
    return finalize(state);
}

////// Specialized for single block padding.
////template <size_t Offset,
////    if_not_greater<Offset, integers> = true>
////void constexpr pad(auto& buffer) NOEXCEPT
////{
////    constexpr auto sentinel = bit_hi<uint32_t>;
////    constexpr auto bitcount = possible_narrow_cast<uint32_t>(
////        (integers - Offset) * bits<uint32_t>);
////
////    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
////    buffer[Offset] = sentinel;
////    buffer[sub1(integers)] = bitcount;
////    std::fill(&buffer[add1(Offset)], &buffer[sub1(integers)], 0);
////    BC_POP_WARNING()
////}
#endif // NORMALIZED_NATIVE_MERKLE

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

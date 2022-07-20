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

#include <algorithm>
#include <bit>
#include <iterator>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

// sha256: movable-type.co.uk/scripts/sha256.html
// Use inline vs. constexpr to obtain intrinsic std::rotr.

namespace libbitcoin {
namespace system {

namespace sha {

constexpr auto choice(auto a, auto b, auto c) NOEXCEPT
{
    return (a & (b ^ c)) ^ c;
}

constexpr auto majory(auto a, auto b, auto c) NOEXCEPT
{
    return (a & (b | c)) | (b & c);
}

constexpr auto SIGMA0(auto a) NOEXCEPT
{
    return std::rotr(a, 2) ^ std::rotr(a, 13) ^ std::rotr(a, 22);
}

constexpr auto SIGMA1(auto a) NOEXCEPT
{
    return std::rotr(a, 6) ^ std::rotr(a, 11) ^ std::rotr(a, 25);
}

constexpr auto sigma0(auto a) NOEXCEPT
{
    return std::rotr(a, 7) ^ std::rotr(a, 18) ^ (a >> 3);
}

constexpr auto sigma1(auto a) NOEXCEPT
{
    return std::rotr(a, 17) ^ std::rotr(a, 19) ^ (a >> 10);
}

constexpr void round1(auto a, auto b, auto c, auto& d, auto e, auto f, auto g,
    auto& h, auto k) NOEXCEPT
{
    const auto x = SIGMA0(a) + majory(a, b, c);
    const auto y = SIGMA1(e) + choice(e, f, g) + h + k;
    d += y;
    h  = x + y;
}

template<uint8_t Round, word_t K>
constexpr void round(state& out, const buffer& in) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    round1(
        out[(block_size + 0 - Round) % state_words],
        out[(block_size + 1 - Round) % state_words],
        out[(block_size + 2 - Round) % state_words],
        out[(block_size + 3 - Round) % state_words], // in/out
        out[(block_size + 4 - Round) % state_words],
        out[(block_size + 5 - Round) % state_words],
        out[(block_size + 6 - Round) % state_words],
        out[(block_size + 7 - Round) % state_words], // in/out
        in[Round] + K);
    BC_POP_WARNING()
}

template<size_t Index>
constexpr void expand(buffer& out) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    out[Index] =
        sigma1(out[Index -  2]) + out[Index -  7] +
        sigma0(out[Index - 15]) + out[Index - 16];
    BC_POP_WARNING()
}

constexpr void expand48(buffer& out) NOEXCEPT
{
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

// 64 or 80 rounds.
constexpr void rounds64(state& out, const buffer& in) NOEXCEPT
{
    round< 0, 0x428a2f98>(out, in);
    round< 1, 0x71374491>(out, in);
    round< 2, 0xb5c0fbcf>(out, in);
    round< 3, 0xe9b5dba5>(out, in);
    round< 4, 0x3956c25b>(out, in);
    round< 5, 0x59f111f1>(out, in);
    round< 6, 0x923f82a4>(out, in);
    round< 7, 0xab1c5ed5>(out, in);
    round< 8, 0xd807aa98>(out, in);
    round< 9, 0x12835b01>(out, in);
    round<10, 0x243185be>(out, in);
    round<11, 0x550c7dc3>(out, in);
    round<12, 0x72be5d74>(out, in);
    round<13, 0x80deb1fe>(out, in);
    round<14, 0x9bdc06a7>(out, in);
    round<15, 0xc19bf174>(out, in);
    round<16, 0xe49b69c1>(out, in);
    round<17, 0xefbe4786>(out, in);
    round<18, 0x0fc19dc6>(out, in);
    round<19, 0x240ca1cc>(out, in);
    round<20, 0x2de92c6f>(out, in);
    round<21, 0x4a7484aa>(out, in);
    round<22, 0x5cb0a9dc>(out, in);
    round<23, 0x76f988da>(out, in);
    round<24, 0x983e5152>(out, in);
    round<25, 0xa831c66d>(out, in);
    round<26, 0xb00327c8>(out, in);
    round<27, 0xbf597fc7>(out, in);
    round<28, 0xc6e00bf3>(out, in);
    round<29, 0xd5a79147>(out, in);
    round<30, 0x06ca6351>(out, in);
    round<31, 0x14292967>(out, in);
    round<32, 0x27b70a85>(out, in);
    round<33, 0x2e1b2138>(out, in);
    round<34, 0x4d2c6dfc>(out, in);
    round<35, 0x53380d13>(out, in);
    round<36, 0x650a7354>(out, in);
    round<37, 0x766a0abb>(out, in);
    round<38, 0x81c2c92e>(out, in);
    round<39, 0x92722c85>(out, in);
    round<40, 0xa2bfe8a1>(out, in);
    round<41, 0xa81a664b>(out, in);
    round<42, 0xc24b8b70>(out, in);
    round<43, 0xc76c51a3>(out, in);
    round<44, 0xd192e819>(out, in);
    round<45, 0xd6990624>(out, in);
    round<46, 0xf40e3585>(out, in);
    round<47, 0x106aa070>(out, in);
    round<48, 0x19a4c116>(out, in);
    round<49, 0x1e376c08>(out, in);
    round<50, 0x2748774c>(out, in);
    round<51, 0x34b0bcb5>(out, in);
    round<52, 0x391c0cb3>(out, in);
    round<53, 0x4ed8aa4a>(out, in);
    round<54, 0x5b9cca4f>(out, in);
    round<55, 0x682e6ff3>(out, in);
    round<56, 0x748f82ee>(out, in);
    round<57, 0x78a5636f>(out, in);
    round<58, 0x84c87814>(out, in);
    round<59, 0x8cc70208>(out, in);
    round<60, 0x90befffa>(out, in);
    round<61, 0xa4506ceb>(out, in);
    round<62, 0xbef9a3f7>(out, in);
    round<63, 0xc67178f2>(out, in);

    // 16 more rounds for sha512
    if constexpr (256u == 512u)
    {
        round<64, 0x00000000>(out, in);
        round<65, 0x00000000>(out, in);
        round<66, 0x00000000>(out, in);
        round<67, 0x00000000>(out, in);
        round<68, 0x00000000>(out, in);
        round<69, 0x00000000>(out, in);
        round<70, 0x00000000>(out, in);
        round<71, 0x00000000>(out, in);
        round<72, 0x00000000>(out, in);
        round<73, 0x00000000>(out, in);
        round<74, 0x00000000>(out, in);
        round<76, 0x00000000>(out, in);
        round<77, 0x00000000>(out, in);
        round<78, 0x00000000>(out, in);
        round<79, 0x00000000>(out, in);
    }
}

constexpr void summary8(state& out, const state& in) NOEXCEPT
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

constexpr void copying8(buffer& out, const state& in) NOEXCEPT
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
        // TODO: safe offsetting array cast.
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        auto& to = unsafe_array_cast<word_t, state_words>(&out[state_words]);
        BC_POP_WARNING()
        to = in;
    }
}

constexpr void copyin64(buffer& out, const buffer& in) NOEXCEPT
{
    out = in;
}

constexpr void padding8(buffer& out) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        out[ 8] = pad32[0];
        out[ 9] = pad32[1];
        out[10] = pad32[2];
        out[11] = pad32[3];
        out[12] = pad32[4];
        out[13] = pad32[5];
        out[14] = pad32[6];
        out[15] = pad32[7];
        BC_POP_WARNING()
    }
    else
    {
        // TODO: make safe offsetting array cast.
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        auto& to = unsafe_array_cast<word_t, state_words>(&out[state_words]);
        BC_POP_WARNING()
        to = pad32;
    }
}

constexpr void paddin16(buffer& out, size_t blocks) NOEXCEPT
{
    constexpr auto block32 = block_size / sizeof(word_t);
    const auto bits = to_bits<uint64_t>(blocks * block_size);

    if (std::is_constant_evaluated())
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        out[0] = pad64[0];
        out[1] = pad64[1];
        out[2] = pad64[2];
        out[3] = pad64[3];
        out[4] = pad64[4];
        out[5] = pad64[5];
        ////out[6] = pad64[6];
        ////out[7] = pad64[7];
        BC_POP_WARNING()
    }
    else
    {
        auto& to = narrowing_array_cast<word_t, block32>(out);
        to = pad64;
    }

    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    out[6] = narrow_cast<word_t>(bits >> bc::bits<word_t>);
    out[7] = narrow_cast<word_t>(bits);
    BC_POP_WARNING()
}

constexpr void bigend16(buffer& out, const block& in) NOEXCEPT
{
    constexpr auto block32 = block_size / sizeof(word_t);

    if (std::is_constant_evaluated())
    {
        constexpr auto size = sizeof(word_t);
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        from_big< 0 * size>(out[ 0], in);
        from_big< 1 * size>(out[ 1], in);
        from_big< 2 * size>(out[ 2], in);
        from_big< 3 * size>(out[ 3], in);
        from_big< 4 * size>(out[ 4], in);
        from_big< 5 * size>(out[ 5], in);
        from_big< 6 * size>(out[ 6], in);
        from_big< 7 * size>(out[ 7], in);
        from_big< 8 * size>(out[ 8], in);
        from_big< 9 * size>(out[ 9], in);
        from_big<10 * size>(out[10], in);
        from_big<11 * size>(out[11], in);
        from_big<12 * size>(out[12], in);
        from_big<13 * size>(out[13], in);
        from_big<14 * size>(out[14], in);
        from_big<15 * size>(out[15], in);
        BC_POP_WARNING()
    }
    else
    {
        auto& from = array_cast<word_t>(in);
        auto& to = narrowing_array_cast<word_t, block32>(out);
        from_big_endians(to, from);
    }
}

constexpr void bigend08(buffer& out, const half& in) NOEXCEPT
{
    constexpr auto block16 = half_size / sizeof(word_t);

    if (std::is_constant_evaluated())
    {
        constexpr auto size = sizeof(word_t);
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        from_big<0 * size>(out[0], in);
        from_big<1 * size>(out[1], in);
        from_big<2 * size>(out[2], in);
        from_big<3 * size>(out[3], in);
        from_big<4 * size>(out[4], in);
        from_big<5 * size>(out[5], in);
        from_big<6 * size>(out[6], in);
        from_big<7 * size>(out[7], in);
        BC_POP_WARNING()
    }
    else
    {
        auto& from = array_cast<word_t>(in);
        auto& to = narrowing_array_cast<word_t, block16>(out);
        from_big_endians(to, from);
    }
}

constexpr void bigend08(digest& out, const state& in) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        constexpr auto size = sizeof(word_t);
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
        auto& to = array_cast<word_t>(out);
        to_big_endians(to, in);
    }
}

// Streaming hash functions.
// ---------------------------------------------------------------------------

VCONSTEXPR void accumulate(state& state, const blocks& blocks) NOEXCEPT
{
    buffer words{};

    for (auto& block: blocks)
    {
        const sha::state start{ state };
        bigend16(words, block);
        expand48(words);
        rounds64(state, words);
        summary8(state, start);
    }
}

// This is a specialization of multiple blocks so that we can have constexpr
// and avoid the (trivial) overhead of iteration and a vector construction.
// TODO: once compilers catch up to C++20 vector constexpr this can be folded.
constexpr void accumulate(state& state, const block& block) NOEXCEPT
{
    buffer words{};
    const sha::state start{ state };
    bigend16(words, block);
    expand48(words);
    rounds64(state, words);
    summary8(state, start);
}

constexpr digest finalize(const state& state) NOEXCEPT
{
    digest out{};
    bigend08(out, state);
    return out;
}

// Finalized single hash functions.
// ---------------------------------------------------------------------------

constexpr digest hash(const blocks& blocks) NOEXCEPT
{
    // process N blocks in accumulator loop.
    auto state = sha::initial;
    accumulate(state, blocks);

    // full pad block for N blocks (pre-endianed, size added).
    buffer words{};
    paddin16(words, blocks.size());
    expand48(words);
    const auto save = state;
    rounds64(state, words);
    summary8(state, save);
    return finalize(state);
}

constexpr digest hash(const block& block) NOEXCEPT
{
    buffer words{};

    // process 1 block in (avoid accumulate to reuse buffer).
    auto state = sha::initial;
    bigend16(words, block);
    expand48(words);
    rounds64(state, words);
    summary8(state, sha::initial);

    // full pad block for 1 block (pre-sized/endianed/expanded).
    copyin64(words, expanded_pad64);
    const auto save = state;
    rounds64(state, words);
    summary8(state, save);
    return finalize(state);
}

constexpr digest hash(const half& half) NOEXCEPT
{
    // process 1/2 data block with 1/2 pad block (pre-sized/endianed).
    buffer words{};
    bigend08(words, half);
    padding8(words);
    expand48(words);
    auto state = sha::initial;
    rounds64(state, words);
    summary8(state, sha::initial);
    return finalize(state);
}

// Finalized merkle hash functions.
// ---------------------------------------------------------------------------

VCONSTEXPR digests merkle(const blocks& blocks) NOEXCEPT
{
    buffer words{};
    digests out{};
    out.reserve(to_half(blocks.size()));

    for (auto& block: blocks)
    {
        bigend16(words, block);
        expand48(words);
        auto state = sha::initial;
        rounds64(state, words);
        summary8(state, sha::initial);

        copyin64(words, expanded_pad64);
        const auto save = state;
        rounds64(state, words);
        summary8(state, save);

        copying8(words, state);
        padding8(words);
        expand48(words);
        state = sha::initial;
        rounds64(state, words);
        summary8(state, sha::initial);

        out.push_back(finalize(state));
    }

    return out;
}

// This is a specialization of multiple blocks so that we can have constexpr
// and avoid the (trivial) overhead of iteration and a vector construction.
// TODO: once compilers catch up to C++20 vector constexpr this can be folded.
constexpr digest merkle(const block& block) NOEXCEPT
{
    buffer words{};

    bigend16(words, block);
    expand48(words);
    auto state = sha::initial;
    rounds64(state, words);
    summary8(state, sha::initial);

    // expanded_pad64 is the output of expand48(sha256::pad_64)
    copyin64(words, expanded_pad64);
    const auto save = state;
    rounds64(state, words);
    summary8(state, save);

    copying8(words, state);
    padding8(words);
    expand48(words);
    state = sha::initial;
    rounds64(state, words);
    summary8(state, sha::initial);

    return finalize(state);
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
constexpr alignas(16) block merkle_pad64
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
constexpr alignas(16) block merkle_pad32
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

#define TEMPLATE \
template <size_t Strength, size_t Digest, \
    bool_if<Strength == 256u || Strength == 512u> If>

#define CLASS \
algorithm<Strength, Digest, If>

TEMPLATE
constexpr CLASS::state_t CLASS::h() NOEXCEPT
{
    if constexpr (digest == strength)
    {
        if constexpr (strength == s_256)
        {
            return
            {
                0x6a09e667,
                0xbb67ae85,
                0x3c6ef372,
                0xa54ff53a,
                0x510e527f,
                0x9b05688c,
                0x1f83d9ab,
                0x5be0cd19
            };
        }

        if constexpr (strength == s_512)
        {
            return
            {
                0x6a09e667f3bcc908,
                0xbb67ae8584caa73b,
                0x3c6ef372fe94f82b,
                0xa54ff53a5f1d36f1,
                0x510e527fade682d1,
                0x9b05688c2b3e6c1f,
                0x1f83d9abfb41bd6b,
                0x5be0cd19137e2179
            };
        }
    }
    else
    {
        return {};
    }
}

TEMPLATE
constexpr CLASS::const_t CLASS::k() NOEXCEPT
{
    if constexpr (digest == strength)
    {
        if constexpr (strength == s_256)
        {
            return
            {
                0x428a2f98,
                0x71374491,
                0xb5c0fbcf,
                0xe9b5dba5,
                0x3956c25b,
                0x59f111f1,
                0x923f82a4,
                0xab1c5ed5,
                0xd807aa98,
                0x12835b01,
                0x243185be,
                0x550c7dc3,
                0x72be5d74,
                0x80deb1fe,
                0x9bdc06a7,
                0xc19bf174,
                0xe49b69c1,
                0xefbe4786,
                0x0fc19dc6,
                0x240ca1cc,
                0x2de92c6f,
                0x4a7484aa,
                0x5cb0a9dc,
                0x76f988da,
                0x983e5152,
                0xa831c66d,
                0xb00327c8,
                0xbf597fc7,
                0xc6e00bf3,
                0xd5a79147,
                0x06ca6351,
                0x14292967,
                0x27b70a85,
                0x2e1b2138,
                0x4d2c6dfc,
                0x53380d13,
                0x650a7354,
                0x766a0abb,
                0x81c2c92e,
                0x92722c85,
                0xa2bfe8a1,
                0xa81a664b,
                0xc24b8b70,
                0xc76c51a3,
                0xd192e819,
                0xd6990624,
                0xf40e3585,
                0x106aa070,
                0x19a4c116,
                0x1e376c08,
                0x2748774c,
                0x34b0bcb5,
                0x391c0cb3,
                0x4ed8aa4a,
                0x5b9cca4f,
                0x682e6ff3,
                0x748f82ee,
                0x78a5636f,
                0x84c87814,
                0x8cc70208,
                0x90befffa,
                0xa4506ceb,
                0xbef9a3f7,
                0xc67178f2
            };
        }

        if constexpr (strength == s_512)
        {
            return
            {
                0x428a2f98d728ae22,
                0x7137449123ef65cd,
                0xb5c0fbcfec4d3b2f,
                0xe9b5dba58189dbbc,
                0x3956c25bf348b538,
                0x59f111f1b605d019,
                0x923f82a4af194f9b,
                0xab1c5ed5da6d8118,
                0xd807aa98a3030242,
                0x12835b0145706fbe,
                0x243185be4ee4b28c,
                0x550c7dc3d5ffb4e2,
                0x72be5d74f27b896f,
                0x80deb1fe3b1696b1,
                0x9bdc06a725c71235,
                0xc19bf174cf692694,
                0xe49b69c19ef14ad2,
                0xefbe4786384f25e3,
                0x0fc19dc68b8cd5b5,
                0x240ca1cc77ac9c65,
                0x2de92c6f592b0275,
                0x4a7484aa6ea6e483,
                0x5cb0a9dcbd41fbd4,
                0x76f988da831153b5,
                0x983e5152ee66dfab,
                0xa831c66d2db43210,
                0xb00327c898fb213f,
                0xbf597fc7beef0ee4,
                0xc6e00bf33da88fc2,
                0xd5a79147930aa725,
                0x06ca6351e003826f,
                0x142929670a0e6e70,
                0x27b70a8546d22ffc,
                0x2e1b21385c26c926,
                0x4d2c6dfc5ac42aed,
                0x53380d139d95b3df,
                0x650a73548baf63de,
                0x766a0abb3c77b2a8,
                0x81c2c92e47edaee6,
                0x92722c851482353b,
                0xa2bfe8a14cf10364,
                0xa81a664bbc423001,
                0xc24b8b70d0f89791,
                0xc76c51a30654be30,
                0xd192e819d6ef5218,
                0xd69906245565a910,
                0xf40e35855771202a,
                0x106aa07032bbd1b8,
                0x19a4c116b8d2d0c8,
                0x1e376c085141ab53,
                0x2748774cdf8eeb99,
                0x34b0bcb5e19b48a8,
                0x391c0cb3c5c95a63,
                0x4ed8aa4ae3418acb,
                0x5b9cca4f7763e373,
                0x682e6ff3d6b2b8a3,
                0x748f82ee5defb2fc,
                0x78a5636f43172f60,
                0x84c87814a1f0ab72,
                0x8cc702081a6439ec,
                0x90befffa23631e28,
                0xa4506cebde82bde9,
                0xbef9a3f7b2c67915,
                0xc67178f2e372532b,
                0xca273eceea26619c,
                0xd186b8c721c0c207,
                0xeada7dd6cde0eb1e,
                0xf57d4f7fee6ed178,
                0x06f067aa72176fba,
                0x0a637dc5a2c898a6,
                0x113f9804bef90dae,
                0x1b710b35131c471b,
                0x28db77f523047d84,
                0x32caab7b40c72493,
                0x3c9ebe0a15c9bebc,
                0x431d67c49c100d4c,
                0x4cc5d4becb3e42b6,
                0x597f299cfc657e2a,
                0x5fcb6fab3ad6faec,
                0x6c44198c4a475817
            };
        }
    }
    else
    {
        return {};
    }
}

#undef TEMPLATE

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

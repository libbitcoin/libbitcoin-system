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
#include "../test.hpp"
#include "sha_alg1.hpp"
#include "hash_acc1.hpp"
    
BOOST_AUTO_TEST_SUITE(sha_tests)

// TODO: integrate sha-ni.
// TODO: vectorize algorithm (2/4/8/16).
// TODO: parallelize hash(blocks) and double_hash(blocks).
// TODO: implement 5.3.6 SHA-512/t initial vector derivation.
// TODO: add/derive SHA-256/224, 512/384, 512/224, 512/256 constants/types.
// TODO: test double_hash implementations and non-block accumulation.
// TODO: single/double accumulate and finalize are tested via accumulator.
// TODO: merge into /crypto directory.
// TODO: add nist test vectors.

// Implementation based on FIPS PUB 180-4 [Secure Hash Standard (SHS)].
// All aspects of FIPS180 are supported within the implmentation.
// nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf

// 4.1 Functions
// ---------------------------------------------------------------------------

template <typename SHA>
constexpr auto algorithm<SHA>::
parity(auto x, auto y, auto z) NOEXCEPT
{
    // 4.1.1 SHA-1 Functions
    return x ^ y ^ z;
}

template <typename SHA>
constexpr auto algorithm<SHA>::
choice(auto x, auto y, auto z) NOEXCEPT
{
    // 4.1.1 SHA-1 Functions
    // 4.1.2 SHA-224 and SHA-256 Functions
    // 4.1.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Functions
    // Normal form reduced.
    ////return (x & y) ^ (~x & z);
    return (x & (y ^ z)) ^ z;
}

template <typename SHA>
constexpr auto algorithm<SHA>::
majority(auto x, auto y, auto z) NOEXCEPT
{
    // 4.1.1 SHA-1 Functions
    // 4.1.2 SHA-224 and SHA-256 Functions
    // 4.1.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Functions
    // Normal form reduced.
    ////return (x & y) ^ (x & z) ^ (y & z);
    return (x & (y | z)) | (y & z);
}

template <typename SHA>
constexpr auto algorithm<SHA>::
SIGMA0(auto x) NOEXCEPT
{
    // std::rotr is one opcode when !is_constant_evaluated.
    if constexpr (SHA::rounds == 80)
    {
        // 4.1.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Functions
        return std::rotr(x, 28) ^ std::rotr(x, 34) ^ std::rotr(x, 39);
    }
    else
    {
        // 4.1.2 SHA-224 and SHA-256 Functions
        return std::rotr(x, 2) ^ std::rotr(x, 13) ^ std::rotr(x, 22);
    }
}

template <typename SHA>
constexpr auto algorithm<SHA>::
SIGMA1(auto x) NOEXCEPT
{
    // std::rotr is one opcode when !is_constant_evaluated.
    if constexpr (SHA::rounds == 80)
    {
        // 4.1.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Functions
        return std::rotr(x, 14) ^ std::rotr(x, 18) ^ std::rotr(x, 41);
    }
    else
    {
        // 4.1.2 SHA-224 and SHA-256 Functions
        return std::rotr(x, 6) ^ std::rotr(x, 11) ^ std::rotr(x, 25);
    }
}

template <typename SHA>
constexpr auto algorithm<SHA>::
sigma0(auto x) NOEXCEPT
{
    // std::rotr is one opcode when !is_constant_evaluated.
    if constexpr (SHA::rounds == 80)
    {
        // 4.1.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Functions
        return std::rotr(x, 1) ^ std::rotr(x, 8) ^ (x >> 7);
    }
    else
    {
        // 4.1.2 SHA-224 and SHA-256 Functions
        return std::rotr(x, 7) ^ std::rotr(x, 18) ^ (x >> 3);
    }
}

template <typename SHA>
constexpr auto algorithm<SHA>::
sigma1(auto x) NOEXCEPT
{
    // std::rotr is one opcode when !is_constant_evaluated.
    if constexpr (SHA::rounds == 80)
    {
        // 4.1.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Functions
        return std::rotr(x, 19) ^ std::rotr(x, 61) ^ (x >> 6);
    }
    else
    {
        // 4.1.2 SHA-224 and SHA-256 Functions
        return std::rotr(x, 17) ^ std::rotr(x, 19) ^ (x >> 10);
    }
}

// Rounds
// ---------------------------------------------------------------------------
// 6.1.2 SHA-1   Hash Computation (1 to N)
// 6.2.2 SHA-256 Hash Computation (1 to N)
// 6.4.2 SHA-512 Hash Computation (1 to N)

template <typename SHA>
template<size_t Round>
constexpr auto algorithm<SHA>::
functor() NOEXCEPT
{
    using self = algorithm<SHA>;
    constexpr auto fn = (Round / 20u);

    // 4.1.1 SHA-1 Functions (limited to uint32_t)
    if      constexpr (fn == 0u)
        return &self::template choice<uint32_t, uint32_t, uint32_t>;
    else if constexpr (fn == 1u)
        return &self::template parity<uint32_t, uint32_t, uint32_t>;
    else if constexpr (fn == 2u)
        return &self::template majority<uint32_t, uint32_t, uint32_t>;
    else if constexpr (fn == 3u)
        return &self::template parity<uint32_t, uint32_t, uint32_t>;
}

template <typename SHA>
template<size_t Round>
constexpr void algorithm<SHA>::
round(auto a, auto& b, auto c, auto d, auto& e, auto w) NOEXCEPT
{
    // 4.2.1 SHA-1 Constants
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
    constexpr auto k = SHA::K::get[Round];
    constexpr auto f = functor<Round>();
    BC_POP_WARNING()
    BC_POP_WARNING()

    // 6.1.2.3 SHA-1 Hash Computation (t=0 to 79)
    // std::rotl is one opcode when !is_constant_evaluated.
    const auto t = std::rotl(a, 5) + f(b, c, d) + e + k + w;
    b = /*c =*/ std::rotl(b, 30);
    e = /*a =*/ t;
}

template <typename SHA>
template<size_t Round>
constexpr void algorithm<SHA>::
round(auto a, auto b, auto c, auto& d, auto e, auto f, auto g, auto& h,
    auto w) NOEXCEPT
{
    // 4.2.2 SHA-224 and SHA-256 Constants
    // 4.2.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Constants
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    constexpr auto k = SHA::K::get[Round];
    BC_POP_WARNING()

    // 6.2.2.3 SHA-256 Hash Computation (t=0 to 63)
    // 6.4.2.3 SHA-512 Hash Computation (t=0 to 79)
    const auto t1 = h + SIGMA1(e) + choice(e, f, g) + k + w;
    const auto t2 = SIGMA0(a) + majority(a, b, c);
    d = /*e =*/  d + t1;
    h = /*a =*/ t1 + t2;
}

template <typename SHA>
template<size_t Round>
constexpr void algorithm<SHA>::
round(auto& out, const auto& in) NOEXCEPT
{
    // All indexes resolved to constants by compile.
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
    if constexpr (SHA::digest == 160)
    {
        // 6.1.2.3 SHA-1 Hash Computation (t=0 to 79)
        round<Round>(
            out[(SHA::rounds + 0 - Round) % SHA::state_words],
            out[(SHA::rounds + 1 - Round) % SHA::state_words], // c->b
            out[(SHA::rounds + 2 - Round) % SHA::state_words],
            out[(SHA::rounds + 3 - Round) % SHA::state_words],
            out[(SHA::rounds + 4 - Round) % SHA::state_words], // a->e
            in[Round]);
    }
    else
    {
        // 6.2.2.3 SHA-256 Hash Computation (t=0 to 63)
        // 6.4.2.3 SHA-512 Hash Computation (t=0 to 79)
        round<Round>(
            out[(SHA::rounds + 0 - Round) % SHA::state_words],
            out[(SHA::rounds + 1 - Round) % SHA::state_words],
            out[(SHA::rounds + 2 - Round) % SHA::state_words],
            out[(SHA::rounds + 3 - Round) % SHA::state_words], // e->d
            out[(SHA::rounds + 4 - Round) % SHA::state_words],
            out[(SHA::rounds + 5 - Round) % SHA::state_words],
            out[(SHA::rounds + 6 - Round) % SHA::state_words],
            out[(SHA::rounds + 7 - Round) % SHA::state_words], // a->h
            in[Round]);
    }
    BC_POP_WARNING()
    BC_POP_WARNING()
}

template <typename SHA>
constexpr void algorithm<SHA>::
rounding(state_t& out, const buffer_t& in) NOEXCEPT
{
    const state_t start{ out };

    // Templated constant reduces ops per iteration by 35% (vs. parameter).
    // Pointer indexing reduces ops per iteration by 43% (vs. std::array[]).
    // Unrolled/inlined loop reduces ops per iteration by 23% (vs. for loop).
    BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
    auto pin = in.data();
    auto pout = out.data();
    BC_POP_WARNING()

    // 6.1.2.3 SHA-1   Hash Computation (t=0 to 79)
    // 6.2.2.3 SHA-256 Hash Computation (t=0 to 63)
    // 6.4.2.3 SHA-512 Hash Computation (t=0 to 79)
    round<0>(pout, pin);
    round<1>(pout, pin);
    round<2>(pout, pin);
    round<3>(pout, pin);
    round<4>(pout, pin);
    round<5>(pout, pin);
    round<6>(pout, pin);
    round<7>(pout, pin);
    round<8>(pout, pin);
    round<9>(pout, pin);
    round<10>(pout, pin);
    round<11>(pout, pin);
    round<12>(pout, pin);
    round<13>(pout, pin);
    round<14>(pout, pin);
    round<15>(pout, pin);

    round<16>(pout, pin);
    round<17>(pout, pin);
    round<18>(pout, pin);
    round<19>(pout, pin);
    round<20>(pout, pin);
    round<21>(pout, pin);
    round<22>(pout, pin);
    round<23>(pout, pin);
    round<24>(pout, pin);
    round<25>(pout, pin);
    round<26>(pout, pin);
    round<27>(pout, pin);
    round<28>(pout, pin);
    round<29>(pout, pin);
    round<30>(pout, pin);
    round<31>(pout, pin);

    round<32>(pout, pin);
    round<33>(pout, pin);
    round<34>(pout, pin);
    round<35>(pout, pin);
    round<36>(pout, pin);
    round<37>(pout, pin);
    round<38>(pout, pin);
    round<39>(pout, pin);
    round<40>(pout, pin);
    round<41>(pout, pin);
    round<42>(pout, pin);
    round<43>(pout, pin);
    round<44>(pout, pin);
    round<45>(pout, pin);
    round<46>(pout, pin);
    round<47>(pout, pin);

    round<48>(pout, pin);
    round<49>(pout, pin);
    round<50>(pout, pin);
    round<51>(pout, pin);
    round<52>(pout, pin);
    round<53>(pout, pin);
    round<54>(pout, pin);
    round<55>(pout, pin);
    round<56>(pout, pin);
    round<57>(pout, pin);
    round<58>(pout, pin);
    round<59>(pout, pin);
    round<60>(pout, pin);
    round<61>(pout, pin);
    round<62>(pout, pin);
    round<63>(pout, pin);

    // 6.1.2.3 SHA-1   Hash Computation (t=0 to 79)
    // 6.4.2.3 SHA-512 Hash Computation (t=0 to 79)
    if constexpr (SHA::rounds == 80)
    {
        round<64>(pout, pin);
        round<65>(pout, pin);
        round<66>(pout, pin);
        round<67>(pout, pin);
        round<68>(pout, pin);
        round<69>(pout, pin);
        round<70>(pout, pin);
        round<71>(pout, pin);
        round<72>(pout, pin);
        round<73>(pout, pin);
        round<74>(pout, pin);
        round<75>(pout, pin);
        round<76>(pout, pin);
        round<77>(pout, pin);
        round<78>(pout, pin);
        round<79>(pout, pin);
    }

    summarize(out, start);
}

template <typename SHA>
template<size_t Word>
constexpr void algorithm<SHA>::
prepare(auto& out) NOEXCEPT
{
    // All indexes resolved to constants by compile.
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
    if constexpr (SHA::digest == 160)
    {
        // 6.1.2 SHA-1 Hash Computation (16 <= t <= 79)
        out[Word] = std::rotl(
            out[Word -  3] ^
            out[Word -  8] ^ 
            out[Word - 14] ^
            out[Word - 16], 1);
    }
    else
    {
        // 6.2.2 SHA-256 Hash Computation (16 <= t <= 63)
        // 6.4.2 SHA-512 Hash Computation (16 <= t <= 79)
        out[Word] =
            sigma1(out[Word -  2]) +
                   out[Word -  7]  +
            sigma0(out[Word - 15]) +
                   out[Word - 16];
    }
    BC_POP_WARNING()
    BC_POP_WARNING()
}

template <typename SHA>
constexpr void algorithm<SHA>::
preparing(buffer_t& out) NOEXCEPT
{
    // Templated constant reduces ops per iteration by 35% (vs. parameter).
    // Pointer indexing reduces ops per iteration by 43% (vs. std::array[]).
    // Unrolled/inlined loop reduces ops per iteration by 23% (vs. for loop).
    BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
    auto pout = out.data();
    BC_POP_WARNING()

    // 6.1.2.1 SHA-1   Hash Computation (16 <= t <= 79)
    // 6.2.2.1 SHA-256 Hash Computation (16 <= t <= 63)
    // 6.4.2.1 SHA-512 Hash Computation (16 <= t <= 79)
    prepare<16>(pout);
    prepare<17>(pout);
    prepare<18>(pout);
    prepare<19>(pout);
    prepare<20>(pout);
    prepare<21>(pout);
    prepare<22>(pout);
    prepare<23>(pout);
    prepare<24>(pout);
    prepare<25>(pout);
    prepare<26>(pout);
    prepare<27>(pout);
    prepare<28>(pout);
    prepare<29>(pout);
    prepare<30>(pout);
    prepare<31>(pout);
    prepare<32>(pout);

    prepare<33>(pout);
    prepare<34>(pout);
    prepare<35>(pout);
    prepare<36>(pout);
    prepare<37>(pout);
    prepare<38>(pout);
    prepare<39>(pout);
    prepare<40>(pout);
    prepare<41>(pout);
    prepare<42>(pout);
    prepare<43>(pout);
    prepare<44>(pout);
    prepare<45>(pout);
    prepare<46>(pout);
    prepare<47>(pout);
    prepare<48>(pout);

    prepare<49>(pout);
    prepare<50>(pout);
    prepare<51>(pout);
    prepare<52>(pout);
    prepare<53>(pout);
    prepare<54>(pout);
    prepare<55>(pout);
    prepare<56>(pout);
    prepare<57>(pout);
    prepare<58>(pout);
    prepare<59>(pout);
    prepare<60>(pout);
    prepare<61>(pout);
    prepare<62>(pout);
    prepare<63>(pout);

    // 6.1.2 SHA-1   Hash Computation (16 <= t <= 79)
    // 6.4.2 SHA-512 Hash Computation (16 <= t <= 79)
    if constexpr (SHA::rounds == 80)
    {
        prepare<64>(pout);
        prepare<65>(pout);
        prepare<66>(pout);
        prepare<67>(pout);
        prepare<68>(pout);
        prepare<69>(pout);
        prepare<70>(pout);
        prepare<71>(pout);
        prepare<72>(pout);
        prepare<73>(pout);
        prepare<74>(pout);
        prepare<75>(pout);
        prepare<76>(pout);
        prepare<77>(pout);
        prepare<78>(pout);
        prepare<79>(pout);
    }
}

template <typename SHA>
constexpr void algorithm<SHA>::
summarize(state_t& out, const state_t& in) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    if constexpr (SHA::digest == 160)
    {
        // 6.1.2.4 SHA-1 Hash Computation
        out[0] += in[0];
        out[1] += in[1];
        out[2] += in[2];
        out[3] += in[3];
        out[4] += in[4];
    }
    else
    {
        // 6.2.2.4 SHA-256 Hash Computation
        // 6.4.2.4 SHA-512 Hash Computation
        out[0] += in[0];
        out[1] += in[1];
        out[2] += in[2];
        out[3] += in[3];
        out[4] += in[4];
        out[5] += in[5];
        out[6] += in[6];
        out[7] += in[7];
    }
    BC_POP_WARNING()
}

template <typename SHA>
constexpr void algorithm<SHA>::
push_state(buffer_t& out, const state_t& in) NOEXCEPT
{
    // 5.3 Setting the Initial Hash Value
    // This is a double hash optimization.
    if (std::is_constant_evaluated())
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        if constexpr (SHA::digest == 160)
        {
            // 6.1.2.1 SHA-1 Hash Computation (0 <= t <= 15)
            out[0] = in[0];
            out[1] = in[1];
            out[2] = in[2];
            out[3] = in[3];
            out[4] = in[4];
        }
        else
        {
            // 6.2.2.1 SHA-256 Hash Computation (0 <= t <= 15)
            // 6.4.2.1 SHA-512 Hash Computation (0 <= t <= 15)
            out[0] = in[0];
            out[1] = in[1];
            out[2] = in[2];
            out[3] = in[3];
            out[4] = in[4];
            out[5] = in[5];
            out[6] = in[6];
            out[7] = in[7];
        }
        BC_POP_WARNING()
    }
    else
    {
        // Array cast is a runtime no-op.
        narrow_array_cast<word_t, array_count<state_t>>(out) = in;
    }
}

// 5.1 Padding the Message
// ---------------------------------------------------------------------------
// 5.1.1 SHA-1, SHA-224 and SHA-256
// 5.1.2 SHA-384, SHA-512, SHA-512/224 and SHA-512/256

template <typename SHA>
constexpr void algorithm<SHA>::
pad_one(buffer_t& out) NOEXCEPT
{
    // Pad a single whole block.
    // SHA::pad::buffer is the output of preparing(SHA::pad::block).
    out = SHA::pad::buffer;
}

template <typename SHA>
constexpr void algorithm<SHA>::
pad_half(buffer_t& out) NOEXCEPT
{
    // Pad a half block.
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
        // Array cast is a runtime no-op.
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        constexpr auto pad_size = array_count<chunk_t>;
        auto& to = unsafe_array_cast<word_t, pad_size>(&out[pad_size]);
        BC_POP_WARNING()
        to = SHA::pad::chunk;
    }
}

template <typename SHA>
constexpr void algorithm<SHA>::
pad_state(buffer_t& out) NOEXCEPT
{
    // This is a double hash optimization.
    // This is the same as pad_half unless hash is SHA-1.
    if (std::is_constant_evaluated())
    {
        if constexpr (SHA::digest == 160)
        {
            // SHA-1 padding of state is 16-5 [11] words.
            BC_PUSH_WARNING(NO_ARRAY_INDEXING)
            out[ 5] = SHA::pad::state[0];
            out[ 6] = SHA::pad::state[1];
            out[ 7] = SHA::pad::state[2];
            out[ 8] = SHA::pad::state[3];
            out[ 9] = SHA::pad::state[4];
            out[10] = SHA::pad::state[5];
            out[11] = SHA::pad::state[6];
            out[12] = SHA::pad::state[7];
            out[13] = SHA::pad::state[8];
            out[14] = SHA::pad::state[9];
            out[15] = SHA::pad::state[10];
            BC_POP_WARNING()
        }
        else
        {
            // SHA-256/512 padding of state is 16-8 [8] words.
            BC_PUSH_WARNING(NO_ARRAY_INDEXING)
            out[ 8] = SHA::pad::state[0];
            out[ 9] = SHA::pad::state[1];
            out[10] = SHA::pad::state[2];
            out[11] = SHA::pad::state[3];
            out[12] = SHA::pad::state[4];
            out[13] = SHA::pad::state[5];
            out[14] = SHA::pad::state[6];
            out[15] = SHA::pad::state[7];
            BC_POP_WARNING()
        }
    }
    else
    {
        // Array cast is a runtime no-op.
        // TODO: make safe offsetting array cast.
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        constexpr auto pad_size = array_count<statep_t>;
        auto& to = unsafe_array_cast<word_t, pad_size>(&out[pad_size]);
        BC_POP_WARNING()
        to = SHA::pad::state;
    }
}

template <typename SHA>
constexpr void algorithm<SHA>::
pad_n(buffer_t& out, count_t blocks) NOEXCEPT
{
    // Pad any number of whole blocks.
    if (std::is_constant_evaluated())
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        out[ 0] = SHA::pad::block[0];
        out[ 1] = SHA::pad::block[1];
        out[ 2] = SHA::pad::block[2];
        out[ 3] = SHA::pad::block[3];
        out[ 4] = SHA::pad::block[4];
        out[ 5] = SHA::pad::block[5];
        out[ 6] = SHA::pad::block[6];
        out[ 7] = SHA::pad::block[7];
        out[ 8] = SHA::pad::block[8];
        out[ 9] = SHA::pad::block[9];
        out[10] = SHA::pad::block[10];
        out[11] = SHA::pad::block[11];
        out[12] = SHA::pad::block[12];
        out[13] = SHA::pad::block[13];
        ////out[14] = SHA::pad::block[14]; // overwritten by count
        ////out[15] = SHA::pad::block[15]; // overwritten by count
        BC_POP_WARNING()
    }
    else
    {
        // Array cast is a runtime no-op.
        // SHA::pad::block is counted, but count will be overwritten below.
        auto& to = narrow_array_cast<word_t, array_count<words_t>>(out);
        to = SHA::pad::block;
    }

    // Copy in the streamed bit count.
    const auto bits = to_bits(blocks * block_bytes);

    // count_t is twice the size of word_t, so split to hi/low words.
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    out[14] = hi_word<word_t>(bits);
    out[15] = lo_word<word_t>(bits);
    BC_POP_WARNING()
}

// 5.2 Parsing the Message
// ---------------------------------------------------------------------------
// 5.2.1 SHA-1, SHA-224 and SHA-256
// 5.2.2 SHA-384, SHA-512, SHA-512/224 and SHA-512/256

template <typename SHA>
constexpr void algorithm<SHA>::
input(buffer_t& out, const block_t& in) NOEXCEPT
{
    // big-endian I/O is conventional.
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
        // Array cast is a runtime no-op.
        // FBE is 1 (or 0) opcode per element and loop-unrolled.
        auto& from = array_cast<word_t>(in);
        auto& to = narrow_array_cast<word_t, SHA::block_words>(out);
        from_big_endians(to, from);
    }
}

template <typename SHA>
constexpr void algorithm<SHA>::
input(buffer_t& out, const half_t& in) NOEXCEPT
{
    // big-endian I/O is conventional.
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
        // Array cast is a runtime no-op.
        // FBE is 1 (or 0) opcode per element and loop-unrolled.
        auto& from = array_cast<SHA::word_t>(in);
        auto& to = narrow_array_cast<word_t, array_count<chunk_t>>(out);
        from_big_endians(to, from);
    }
}

template <typename SHA>
constexpr typename algorithm<SHA>::digest_t algorithm<SHA>::
output(const state_t& in) NOEXCEPT
{
    // 6.1.2 SHA-1   Hash Computation
    // 6.2.2 SHA-256 Hash Computation
    // 6.4.2 SHA-512 Hash Computation

    // big-endian I/O is conventional.
    if (std::is_constant_evaluated())
    {
        digest_t out{};
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        if constexpr (SHA::digest == 160)
        {
            to_big<0 * SHA::word_bytes>(out, in[0]);
            to_big<1 * SHA::word_bytes>(out, in[1]);
            to_big<2 * SHA::word_bytes>(out, in[2]);
            to_big<3 * SHA::word_bytes>(out, in[3]);
            to_big<4 * SHA::word_bytes>(out, in[4]);
        }
        else
        {
            to_big<0 * SHA::word_bytes>(out, in[0]);
            to_big<1 * SHA::word_bytes>(out, in[1]);
            to_big<2 * SHA::word_bytes>(out, in[2]);
            to_big<3 * SHA::word_bytes>(out, in[3]);
            to_big<4 * SHA::word_bytes>(out, in[4]);
            to_big<5 * SHA::word_bytes>(out, in[5]);
            to_big<6 * SHA::word_bytes>(out, in[6]);
            to_big<7 * SHA::word_bytes>(out, in[7]);
        }
        BC_POP_WARNING()
        return out;
    }
    else
    {
        // Array cast is a runtime no-op.
        // TBE is 1 (or 0) opcode per element and loop-unrolled.
        return array_cast<byte_t>(to_big_endians(in));
    }
}

// Streaming single hash functions.
// ---------------------------------------------------------------------------

template <typename SHA>
constexpr void algorithm<SHA>::
accumulate(state_t& state, const block_t& block) NOEXCEPT
{
    buffer_t space{};
    input(space, block);
    preparing(space);
    rounding(state, space);
}

template <typename SHA>
VCONSTEXPR void algorithm<SHA>::
accumulate(state_t& state, const blocks_t& blocks) NOEXCEPT
{
    buffer_t space{};

    for (auto& block: blocks)
    {
        input(space, block);
        preparing(space);
        rounding(state, space);
    }
}

template <typename SHA>
constexpr typename algorithm<SHA>::digest_t algorithm<SHA>::
finalize(const state_t& state) NOEXCEPT
{
    return output(state);
}

// Finalized single hash functions.
// ---------------------------------------------------------------------------

template <typename SHA>
constexpr typename algorithm<SHA>::digest_t algorithm<SHA>::
hash(const half_t& half) NOEXCEPT
{
    buffer_t space{};
    auto state = SHA::H::get;

    // process 1/2 data block with 1/2 pad block (pre-sized/endianed).
    input(space, half);
    pad_half(space);
    preparing(space);
    rounding(state, space);
    return finalize(state);
}

template <typename SHA>
constexpr typename algorithm<SHA>::digest_t algorithm<SHA>::
hash(const block_t& block) NOEXCEPT
{
    buffer_t space{};
    auto state = SHA::H::get;

    // process block 1 (avoid accumulate to reuse buffer).
    // full pad for block 2 (pre-sized/endianed/expanded).
    input(space, block);
    preparing(space);
    rounding(state, space);
    pad_one(space);
    rounding(state, space);
    return finalize(state);
}

template <typename SHA>
constexpr typename algorithm<SHA>::digest_t algorithm<SHA>::
hash(const blocks_t& blocks) NOEXCEPT
{
    buffer_t space{};
    auto state = SHA::H::get;

    // process N blocks (inlined accumulator).
    for (auto& block: blocks)
    {
        input(space, block);
        preparing(space);
        rounding(state, space);
    }

    // full pad block for N blocks (pre-endianed, size added).
    pad_n(space, blocks.size());
    preparing(space);
    rounding(state, space);
    return finalize(state);
}

// Finalized double hash functions.
// ---------------------------------------------------------------------------

template <typename SHA>
constexpr typename algorithm<SHA>::digest_t algorithm<SHA>::
double_hash(const block_t& block) NOEXCEPT
{
    buffer_t space{};
    auto state = SHA::H::get;

    input(space, block);                // hash(full)
    preparing(space);                   // hash(full)
    rounding(state, space);             // hash(full)

    pad_one(space);                     // hash(half)
    preparing(space);                   // hash(half)
    rounding(state, space);             // hash(half)

    push_state(space, state);           // hash(state)
    pad_state(space);                   // hash(state)
    preparing(space);                   // hash(state)
    rounding(state, space);             // hash(state)
    return finalize(state);             // hash(state)
}

template <typename SHA>
VCONSTEXPR typename algorithm<SHA>::digests_t algorithm<SHA>::
double_hash(const blocks_t& blocks) NOEXCEPT
{
    buffer_t space{};
    digests_t out{};
    out.reserve(to_half(blocks.size()));
    auto state = SHA::H::get;

    for (auto& block: blocks)
    {
        input(space, block);            // hash(full)
        preparing(space);               // hash(full)
        rounding(state, space);         // hash(full)

        pad_one(space);                 // hash(half)
        preparing(space);               // hash(half)
        rounding(state, space);         // hash(half)

        push_state(space, state);       // hash(state)
        pad_state(space);               // hash(state)
        preparing(space);               // hash(state)
        rounding(state, space);         // hash(state)
        out.push_back(finalize(state)); // hash(state)
    }

    return out;
}

// TESTS
// ============================================================================

// sha160
// ----------------------------------------------------------------------------

using sha_160 = algorithm<h160>;
using sha_160_accumulator = accumulator<sha_160>;

short_hash sha160_hash(const data_slice& data) NOEXCEPT
{
    short_hash hash{};
    sha_160_accumulator accumulator;
    accumulator.write(data.size(), data.data());
    accumulator.flush(hash.data());
    return hash;
}

short_hash sha160_hash(const data_slice& left, const data_slice& right) NOEXCEPT
{
    short_hash hash{};
    sha_160_accumulator accumulator;
    accumulator.write(left.size(), left.data());
    accumulator.write(right.size(), right.data());
    accumulator.flush(hash.data());
    return hash;
}

constexpr auto half160 = sha_160::half_t{};
constexpr auto full160 = sha_160::block_t{};
constexpr auto twin160 = std_array<uint8_t, sha_160::block_bytes * 2>{};
constexpr auto expected_half160 = base16_array("de8a847bff8c343d69b853a215e6ee775ef2ef96");
constexpr auto expected_full160 = base16_array("c8d7d0ef0eedfa82d2ea1aa592845b9a6d4b02b7");
constexpr auto expected_twin160 = base16_array("0ae4f711ef5d6e9d26c611fd2c8c8ac45ecbf9e7");
constexpr auto expected_merk160 = base16_array("a8782546751527a4a1ad999058165d6af22421c8");

// algorithm

BOOST_AUTO_TEST_CASE(algorithm__merk160_null_hash__expected)
{
    static_assert(sha_160::double_hash(full160) != expected_merk160);
    BOOST_CHECK_NE(sha_160::double_hash(full160), expected_merk160);
    BOOST_CHECK_EQUAL(system::sha1_hash(system::sha1_hash(full160)), expected_merk160);
}

BOOST_AUTO_TEST_CASE(algorithm__hash__half160_block_null_hash__expected)
{
    static_assert(sha_160::hash(half160) == expected_half160);
    BOOST_CHECK_EQUAL(sha_160::hash(half160), expected_half160);
    BOOST_CHECK_EQUAL(system::sha1_hash(half160), expected_half160);
}

BOOST_AUTO_TEST_CASE(algorithm__hash__full160_block_null_hash__expected)
{
    static_assert(sha_160::hash(full160) == expected_full160);
    BOOST_CHECK_EQUAL(sha_160::hash(full160), expected_full160);
    BOOST_CHECK_EQUAL(system::sha1_hash(full160), expected_full160);
}

BOOST_AUTO_TEST_CASE(algorithm__hash__twin160_block_null_hash__expected)
{
    const std_vector<cref<sha_160::block_t>> twin{ std::cref(full160), std::cref(full160) };
    BOOST_CHECK_EQUAL(sha_160::hash(twin), expected_twin160);
    BOOST_CHECK_EQUAL(system::sha1_hash(twin160), expected_twin160);
}

// accumulator

BOOST_AUTO_TEST_CASE(accumulator__hash__half160_block_null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha160_hash(half160), expected_half160);
    BOOST_CHECK_EQUAL(system::sha1_hash(half160), expected_half160);
}

BOOST_AUTO_TEST_CASE(accumulator__hash__full160_block_null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha160_hash(full160), expected_full160);
    BOOST_CHECK_EQUAL(system::sha1_hash(full160), expected_full160);
}

BOOST_AUTO_TEST_CASE(accumulator__hash__twin160_block_null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha160_hash(full160, full160), expected_twin160);
    BOOST_CHECK_EQUAL(system::sha1_hash(twin160), expected_twin160);
}

// double_hash(full160)
// double_hash(twin160)

// sha256
// ----------------------------------------------------------------------------

using sha_256 = algorithm<h256<>>;
using sha_256_accumulator = accumulator<sha_256>;

hash_digest sha256_hash(const data_slice& data) NOEXCEPT
{
    hash_digest hash{};
    sha_256_accumulator accumulator;
    accumulator.write(data.size(), data.data());
    accumulator.flush(hash.data());
    return hash;
}

hash_digest sha256_hash(const data_slice& left, const data_slice& right) NOEXCEPT
{
    hash_digest hash{};
    sha_256_accumulator accumulator;
    accumulator.write(left.size(), left.data());
    accumulator.write(right.size(), right.data());
    accumulator.flush(hash.data());
    return hash;
}

constexpr auto half256 = sha_256::half_t{};
constexpr auto full256 = sha_256::block_t{};
constexpr auto twin256 = std_array<uint8_t, sha_256::block_bytes * 2>{};
constexpr auto expected_half256 = base16_array("66687aadf862bd776c8fc18b8e9f8e20089714856ee233b3902a591d0d5f2925");
constexpr auto expected_full256 = base16_array("f5a5fd42d16a20302798ef6ed309979b43003d2320d9f0e8ea9831a92759fb4b");
constexpr auto expected_twin256 = base16_array("38723a2e5e8a17aa7950dc008209944e898f69a7bd10a23c839d341e935fd5ca");
constexpr auto expected_merk256 = base16_array("e2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf9");

// algorithm

BOOST_AUTO_TEST_CASE(algorithm__merk256_null_hash__expected)
{
    static_assert(sha_256::double_hash(full256) != expected_merk256);
    BOOST_CHECK_NE(sha_256::double_hash(full256), expected_merk256);
    BOOST_CHECK_EQUAL(system::sha256_hash(system::sha256_hash(full256)), expected_merk256);
}

BOOST_AUTO_TEST_CASE(algorithm__hash__half256_block_null_hash__expected)
{
    static_assert(sha_256::hash(half256) == expected_half256);
    BOOST_CHECK_EQUAL(sha_256::hash(half256), expected_half256);
    BOOST_CHECK_EQUAL(system::sha256_hash(half256), expected_half256);
}

BOOST_AUTO_TEST_CASE(algorithm__hash__full256_block_null_hash__expected)
{
    static_assert(sha_256::hash(full256) == expected_full256);
    BOOST_CHECK_EQUAL(sha_256::hash(full256), expected_full256);
    BOOST_CHECK_EQUAL(system::sha256_hash(full256), expected_full256);
}

BOOST_AUTO_TEST_CASE(algorithm__hash__twin256_block_null_hash__expected)
{
    const std_vector<cref<sha_256::block_t>> twin{ std::cref(full256), std::cref(full256) };
    BOOST_CHECK_EQUAL(sha_256::hash(twin), expected_twin256);
    BOOST_CHECK_EQUAL(system::sha256_hash(twin256), expected_twin256);
}

// accumulator

BOOST_AUTO_TEST_CASE(accumulator__hash__half256_block_null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha256_hash(half256), expected_half256);
    BOOST_CHECK_EQUAL(system::sha256_hash(half256), expected_half256);
}

BOOST_AUTO_TEST_CASE(accumulator__hash__full256_block_null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha256_hash(full256), expected_full256);
    BOOST_CHECK_EQUAL(system::sha256_hash(full256), expected_full256);
}

BOOST_AUTO_TEST_CASE(accumulator__hash__twin256_block_null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha256_hash(full256, full256), expected_twin256);
    BOOST_CHECK_EQUAL(system::sha256_hash(twin256), expected_twin256);

    // dedicated doubler in bc lib for sha256.
    BOOST_CHECK_EQUAL(system::sha256_hash(full256, full256), system::sha256_hash(twin256));
}

// double_hash(full256)
// double_hash(twin256)

// sha512
// ----------------------------------------------------------------------------

using sha_512 = algorithm<h512<>>;
using sha_512_accumulator = accumulator<sha_512>;

long_hash sha512_hash(const data_slice& data) NOEXCEPT
{
    long_hash hash{};
    sha_512_accumulator accumulator;
    accumulator.write(data.size(), data.data());
    accumulator.flush(hash.data());
    return hash;
}

long_hash sha512_hash(const data_slice& left, const data_slice& right) NOEXCEPT
{
    long_hash hash{};
    sha_512_accumulator accumulator;
    accumulator.write(left.size(), left.data());
    accumulator.write(right.size(), right.data());
    accumulator.flush(hash.data());
    return hash;
}

constexpr auto half512 = sha_512::half_t{};
constexpr auto full512 = sha_512::block_t{};
constexpr auto twin512 = std_array<uint8_t, sha_512::block_bytes * 2>{};
constexpr auto expected_half512 = base16_array("7be9fda48f4179e611c698a73cff09faf72869431efee6eaad14de0cb44bbf66503f752b7a8eb17083355f3ce6eb7d2806f236b25af96a24e22b887405c20081");
constexpr auto expected_full512 = base16_array("ab942f526272e456ed68a979f50202905ca903a141ed98443567b11ef0bf25a552d639051a01be58558122c58e3de07d749ee59ded36acf0c55cd91924d6ba11");
constexpr auto expected_twin512 = base16_array("693f95d58383a6162d2aab49eb60395dcc4bb22295120caf3f21e3039003230b287c566a03c7a0ca5accaed2133c700b1cb3f82edf8adcbddc92b4f9fb9910c6");
constexpr auto expected_merk512 = base16_array("56d3e5825edf06e467e50dfeb09c1df2d9940121c05d61a162bfcb80aea3aa5fe958d917ac993d76cd3ea86240fedbb79520ce7b9c275793e3c75a82116cc320");

// algorithm

BOOST_AUTO_TEST_CASE(algorithm__merk512_null_hash__expected)
{
    static_assert(sha_512::double_hash(full512) != expected_merk512);
    BOOST_CHECK_NE(sha_512::double_hash(full512), expected_merk512);
    BOOST_CHECK_EQUAL(system::sha512_hash(system::sha512_hash(full512)), expected_merk512);
}

BOOST_AUTO_TEST_CASE(algorithm__hash__half512_block_null_hash__expected)
{
    static_assert(sha_512::hash(half512) == expected_half512);
    BOOST_CHECK_EQUAL(sha_512::hash(half512), expected_half512);
    BOOST_CHECK_EQUAL(system::sha512_hash(half512), expected_half512);
}

BOOST_AUTO_TEST_CASE(algorithm__hash__full512_block_null_hash__expected)
{
    static_assert(sha_512::hash(full512) == expected_full512);
    BOOST_CHECK_EQUAL(sha_512::hash(full512), expected_full512);
    BOOST_CHECK_EQUAL(system::sha512_hash(full512), expected_full512);
}

BOOST_AUTO_TEST_CASE(algorithm__hash__twin512_block_null_hash__expected)
{
    const std_vector<cref<sha_512::block_t>> twin{ std::cref(full512), std::cref(full512) };
    BOOST_CHECK_EQUAL(sha_512::hash(twin), expected_twin512);
    BOOST_CHECK_EQUAL(system::sha512_hash(twin512), expected_twin512);
}

// accumulator

BOOST_AUTO_TEST_CASE(accumulator__hash__half512_block_null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha512_hash(half512), expected_half512);
    BOOST_CHECK_EQUAL(system::sha512_hash(half512), expected_half512);
}

BOOST_AUTO_TEST_CASE(accumulator__hash__full512_block_null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha512_hash(full512), expected_full512);
    BOOST_CHECK_EQUAL(system::sha512_hash(full512), expected_full512);
}

BOOST_AUTO_TEST_CASE(accumulator__hash__twin512_block_null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha512_hash(full512, full512), expected_twin512);
    BOOST_CHECK_EQUAL(system::sha512_hash(twin512), expected_twin512);
}

// double_hash(full512)
// double_hash(twin512)

// Dumper
// ----------------------------------------------------------------------------

template <typename SHA>
struct accessor
  : public algorithm<SHA>
{
    using base = algorithm<SHA>;
    static constexpr void preparing(base::buffer_t& out) NOEXCEPT
    {
        base::preparing(out);
    }
};

template <typename SHA>
struct dumper
{
    static constexpr void dump(std::ostream& out)
    {
        using base = accessor<SHA>;

        typename base::buffer_t space{};
        constexpr auto size = base::pad::block.size();
        narrow_array_cast<base::word_t, size>(space) = base::pad::block;
        base::preparing(space);

        std::for_each(space.begin(), space.end(), [&](auto& word) noexcept
        {
            out << "0x" << encode_base16(to_big_endian(word)) << "," << std::endl;
        });
    }
};

////BOOST_AUTO_TEST_CASE(algorithm__dump__expanded_block_pad__to_console)
////{
////    dumper<h160>::dump(std::cout);
////    dumper<h256<>>::dump(std::cout);
////    dumper<h512<>>::dump(std::cout);
////}

BOOST_AUTO_TEST_SUITE_END()

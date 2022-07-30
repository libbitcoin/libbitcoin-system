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
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace sha {

// TODO: integrate sha-ni.
// TODO: vectorize algorithm (2/4/8/16).
// TODO: implement 5.3.6 SHA-512/t initial vector derivation.
// TODO: add/derive SHA-256/224, 512/384, 512/224, 512/256 constants/types.
// TODO: add nist test vectors.

// Implementation based on FIPS PUB 180-4 [Secure Hash Standard (SHS)].
// All aspects of FIPS180 are supported within the implmentation.
// nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf

#define TEMPLATE template <typename SHA, bool Concurrent>
#define CLASS algorithm<SHA, Concurrent>

// Bogus warning suggests constexpr when declared consteval.
BC_PUSH_WARNING(USE_CONSTEXPR_FOR_FUNCTION)
BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

// private
// ---------------------------------------------------------------------------

TEMPLATE
CONSTEVAL auto& CLASS::
concurrency() NOEXCEPT
{
    if constexpr (Concurrent)
        return bc::par_unseq;
    else
        return bc::seq;
}

TEMPLATE
CONSTEVAL typename CLASS::chunk_t CLASS::
chunk_pad() NOEXCEPT
{
    constexpr auto bytes = possible_narrow_cast<word_t>(array_count<half_t>);

    chunk_t out{};
    out.front() = bit_hi<word_t>;
    out.back() = to_bits(bytes);
    return out;
}

TEMPLATE
CONSTEVAL typename CLASS::state_pad_t CLASS::
state_pad() NOEXCEPT
{
    constexpr auto bytes = possible_narrow_cast<word_t>(array_count<digest_t>);

    state_pad_t out{};
    out.front() = bit_hi<word_t>;
    out.back() = to_bits(bytes);
    return out;
}

TEMPLATE
CONSTEVAL typename CLASS::blocks_pad_t CLASS::
blocks_pad() NOEXCEPT
{
    blocks_pad_t out{};
    out.front() = bit_hi<word_t>;
    return out;
}

TEMPLATE
CONSTEVAL typename CLASS::buffer_t CLASS::
block_pad() NOEXCEPT
{
    constexpr auto index = sub1(array_count<words_t>);
    constexpr auto bytes = possible_narrow_cast<word_t>(array_count<block_t>);

    buffer_t out{};
    out.front() = bit_hi<word_t>;
    out.at(index) = to_bits(bytes);
    preparing(out);
    return out;
}

// 4.1 Functions
// ---------------------------------------------------------------------------

TEMPLATE
constexpr auto CLASS::
parity(auto x, auto y, auto z) NOEXCEPT
{
    // 4.1.1 SHA-1 Functions
    return x ^ y ^ z;
}

TEMPLATE
constexpr auto CLASS::
choice(auto x, auto y, auto z) NOEXCEPT
{
    // 4.1.1 SHA-1 Functions
    // 4.1.2 SHA-224 and SHA-256 Functions
    // 4.1.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Functions
    // Normal form reduced.
    ////return (x & y) ^ (~x & z);
    return (x & (y ^ z)) ^ z;
}

TEMPLATE
constexpr auto CLASS::
majority(auto x, auto y, auto z) NOEXCEPT
{
    // 4.1.1 SHA-1 Functions
    // 4.1.2 SHA-224 and SHA-256 Functions
    // 4.1.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Functions
    // Normal form reduced.
    ////return (x & y) ^ (x & z) ^ (y & z);
    return (x & (y | z)) | (y & z);
}

TEMPLATE
constexpr auto CLASS::
SIGMA0(auto x) NOEXCEPT
{
    // 4.1.2 SHA-224 and SHA-256 Functions
    // 4.1.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Functions
    if constexpr (SHA::rounds == 80)
        return std::rotr(x, 28) ^ std::rotr(x, 34) ^ std::rotr(x, 39);
    else
        return std::rotr(x,  2) ^ std::rotr(x, 13) ^ std::rotr(x, 22);
}

TEMPLATE
constexpr auto CLASS::
SIGMA1(auto x) NOEXCEPT
{
    // 4.1.2 SHA-224 and SHA-256 Functions
    // 4.1.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Functions
    if constexpr (SHA::rounds == 80)
        return std::rotr(x, 14) ^ std::rotr(x, 18) ^ std::rotr(x, 41);
    else
        return std::rotr(x,  6) ^ std::rotr(x, 11) ^ std::rotr(x, 25);
}

TEMPLATE
constexpr auto CLASS::
sigma0(auto x) NOEXCEPT
{
    // 4.1.2 SHA-224 and SHA-256 Functions
    // 4.1.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Functions
    if constexpr (SHA::rounds == 80)
        return std::rotr(x, 1) ^ std::rotr(x,  8) ^ (x >> 7);
    else
        return std::rotr(x, 7) ^ std::rotr(x, 18) ^ (x >> 3);
}

TEMPLATE
constexpr auto CLASS::
sigma1(auto x) NOEXCEPT
{
    // 4.1.2 SHA-224 and SHA-256 Functions
    // 4.1.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Functions
    if constexpr (SHA::rounds == 80)
        return std::rotr(x, 19) ^ std::rotr(x, 61) ^ (x >>  6);
    else
        return std::rotr(x, 17) ^ std::rotr(x, 19) ^ (x >> 10);
}

// Rounds
// ---------------------------------------------------------------------------
// 6.1.2 SHA-1   Hash Computation (1 to N)
// 6.2.2 SHA-256 Hash Computation (1 to N)
// 6.4.2 SHA-512 Hash Computation (1 to N)

TEMPLATE
template<size_t Round>
CONSTEVAL auto CLASS::
functor() NOEXCEPT
{
    using self = CLASS;
    constexpr auto fn = (Round / 20u);

    // 4.1.1 SHA-1 Functions (limited to uint32_t)
    if constexpr (fn == 0u)
        return &self::template choice<uint32_t, uint32_t, uint32_t>;
    else if constexpr (fn == 1u)
        return &self::template parity<uint32_t, uint32_t, uint32_t>;
    else if constexpr (fn == 2u)
        return &self::template majority<uint32_t, uint32_t, uint32_t>;
    else if constexpr (fn == 3u)
        return &self::template parity<uint32_t, uint32_t, uint32_t>;
}

TEMPLATE
template<size_t Round>
FORCE_INLINE constexpr void CLASS::
round(auto a, auto& b, auto c, auto d, auto& e, auto w) NOEXCEPT
{
    // 4.2.1 SHA-1 Constants
    constexpr auto k = K::get[Round];
    constexpr auto f = functor<Round>();

    // 6.1.2.3 SHA-1 Hash Computation (t=0 to 79)
    e = /*a =*/ std::rotl(a, 5) + f(b, c, d) + e + k + w;
    b = /*c =*/ std::rotl(b, 30);

    // SHA-NI
    // Four rounds (total rounds 80/4).
    // First round is add(e, w), then sha1nexte(e, w).
    // fk is round-based enumeration implying f selection and k value.
    //     e1 = sha1nexte(e0, w);
    //     abcd = sha1rnds4(abcd, e0, fk);
    // NEON
    // f is implied by k in wk.
    //     e1 = vsha1h(vgetq_lane(abcd, 0);
    //     vsha1cq(abcd, e0, vaddq(w, k));
}

TEMPLATE
template<size_t Round>
FORCE_INLINE constexpr void CLASS::
round(auto a, auto b, auto c, auto& d, auto e, auto f, auto g, auto& h,
    auto w) NOEXCEPT
{
    // 4.2.2 SHA-224 and SHA-256 Constants
    // 4.2.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Constants
    constexpr auto k = K::get[Round];

    // 6.2.2.3 SHA-256 Hash Computation (t=0 to 63)
    // 6.4.2.3 SHA-512 Hash Computation (t=0 to 79)
    const auto t1 = h + SIGMA1(e) + choice(e, f, g) + k + w;
    const auto t2 = SIGMA0(a) + majority(a, b, c);
    d = /*e =*/  d + t1;
    h = /*a =*/ t1 + t2;

    // Rounds can be cut in half and this round doubled (intel paper).
    // Avoids the need for a temporary variable and aligns with SHA-NI.
    // Removing the temporary eliminates 2x64 instructions from the assembly.
    // h = /*a =*/ SIGMA0(a) + SIGMA1(e) + majority(a, b, c) +
    //             choice(e, f, g) + (k + w) + h;
    // d = /*e =*/ SIGMA1(e) + choice(e, f, g) + (k + w) + h + d;
    //
    // Each call is 2 rounds, 4 rounds total.
    // s, w and k are 128 (4 words each, s1/s2 is 8 word state).
    // SHA-NI
    //     const auto value = add(w, k);
    //     abcd = sha256rnds2(abcd, efgh, value);
    //     efgh = sha256rnds2(efgh, abcd, shuffle(value));
    // NEON
    //     const auto value = vaddq(w, k);
    //     abcd = vsha256hq(abcd, efgh, value);
    //     efgh = vsha256h2q(efgh, abcd, value);
}

TEMPLATE
template<size_t Round>
FORCE_INLINE constexpr void CLASS::
round(auto& out, const auto& in) NOEXCEPT
{
    constexpr auto words  = SHA::state_words;
    constexpr auto rounds = SHA::rounds;

    if constexpr (SHA::digest == 160)
    {
        // 6.1.2.3 SHA-1 Hash Computation (t=0 to 79)
        round<Round>(
            out[(rounds + 0 - Round) % words],
            out[(rounds + 1 - Round) % words], // c->b
            out[(rounds + 2 - Round) % words],
            out[(rounds + 3 - Round) % words],
            out[(rounds + 4 - Round) % words], // a->e
            in[Round]);

        // SNA-NI/NEON
        // State packs in 128 (one state variable), reduces above to 1 out[].
        // Input value is 128 (w). Constants (k) statically initialized as 128.
    }
    else
    {
        // 6.2.2.3 SHA-256 Hash Computation (t=0 to 63)
        // 6.4.2.3 SHA-512 Hash Computation (t=0 to 79)
        round<Round>(
            out[(rounds + 0 - Round) % words],
            out[(rounds + 1 - Round) % words],
            out[(rounds + 2 - Round) % words],
            out[(rounds + 3 - Round) % words], // e->d
            out[(rounds + 4 - Round) % words],
            out[(rounds + 5 - Round) % words],
            out[(rounds + 6 - Round) % words],
            out[(rounds + 7 - Round) % words], // a->h
            in[Round]);

        // SHA-NI/NEON
        // Each element is 128 (vs. 32), reduces above to 2 out[] (s0/s1).
        // Input value is 128 (w). Constants (k) statically initialized as 128.
    }
}

TEMPLATE
constexpr void CLASS::
rounding(state_t& out, const buffer_t& in) NOEXCEPT
{
    // SHA-NI/256: 64/4 = 16 quad rounds, 8/4 = 2 state elements.
    const state_t start{ out };

    // Templated constant reduces ops per iteration by 35% (vs. parameter).
    // Pointer indexing reduces ops per iteration by 43% (vs. std::array[]).
    // Unrolled/inlined loop reduces ops per iteration by 23% (vs. for loop).
    auto pin = in.data();
    auto pout = out.data();

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

TEMPLATE
template<size_t Word>
FORCE_INLINE constexpr void CLASS::
prepare(auto& out) NOEXCEPT
{
    if constexpr (SHA::digest == 160)
    {
        // 6.1.2 SHA-1 Hash Computation (16 <= t <= 79)
        out[Word] = std::rotl(
            out[Word - 16] ^ out[Word - 14] ^
            out[Word -  8] ^ out[Word -  3], 1);

        // SHA-NI
        //     out[Word] = sha1msg2 // xor and rotl1
        //     (
        //         xor              // not using sha1msg1
        //         (
        //             sha1msg1     // xor (why not xor?)
        //             (
        //                 out[Word - 16],
        //                 out[Word - 14]
        //             ),
        //             out[Word -  8]
        //          ),
        //          out[Word -  3]
        //     );
        // NEON
        //     vsha1su1q/vsha1su0q
    }
    else
    {
        // 6.2.2 SHA-256 Hash Computation (16 <= t <= 63)
        // 6.4.2 SHA-512 Hash Computation (16 <= t <= 79)
        out[Word] =
            out[Word - 16] + sigma0(out[Word - 15]) +
            out[Word -  7] + sigma1(out[Word -  2]);

        // Each word is 128, buffer goes from 64 to 16 words.
        // SHA-NI
        // out[Word] =
        //     sha256msg1(out[Word - 16], out[Word - 15]) +
        //     sha256msg2(out[Word -  7], out[Word -  2]);
        // NEON
        // Not sure about these indexes.
        // mijailovic.net/2018/06/06/sha256-armv8
        // out[Word] =
        //     vsha256su0q(out[Word - 13], out[Word - 9])
        //     vsha256su1q(out[Word - 13], out[Word - 5], out[Word - 1]);
    }
}

TEMPLATE
constexpr void CLASS::
preparing(buffer_t& out) NOEXCEPT
{
    auto pout = out.data();

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

TEMPLATE
constexpr void CLASS::
summarize(state_t& out, const state_t& in) NOEXCEPT
{
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
}

TEMPLATE
constexpr void CLASS::
input(buffer_t& out, const state_t& in) NOEXCEPT
{
    // 5.3 Setting the Initial Hash Value

    // This is a double hash optimization.
    if (std::is_constant_evaluated())
    {
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
    }
    else
    {
        narrow_array_cast<word_t, array_count<state_t>>(out) = in;
    }
}

// 5.1 Padding the Message
// ---------------------------------------------------------------------------
// 5.1.1 SHA-1, SHA-224 and SHA-256
// 5.1.2 SHA-384, SHA-512, SHA-512/224 and SHA-512/256

TEMPLATE
constexpr void CLASS::
pad_one(buffer_t& out) NOEXCEPT
{
    // Pad a single whole block with pre-prepared buffer.
    constexpr auto pad = block_pad();

    out = pad;
}

TEMPLATE
constexpr void CLASS::
pad_half(buffer_t& out) NOEXCEPT
{
    // Pad a half block.
    constexpr auto pad = chunk_pad();

    if (std::is_constant_evaluated())
    {
        out.at(8)  = pad.at(0);
        out.at(9)  = pad.at(1);
        out.at(10) = pad.at(2);
        out.at(11) = pad.at(3);
        out.at(12) = pad.at(4);
        out.at(13) = pad.at(5);
        out.at(14) = pad.at(6);
        out.at(15) = pad.at(7);
    }
    else
    {
        constexpr auto size = array_count<chunk_t>;
        unsafe_array_cast<word_t, size>(&out[size]) = pad;
    }
}

TEMPLATE
constexpr void CLASS::
pad_state(buffer_t& out) NOEXCEPT
{
    // Pad state as buffer input.
    // This is a double hash optimization.
    // This is the same as pad_half unless SHA is SHA-1.
    constexpr auto pad = state_pad();

    if (std::is_constant_evaluated())
    {
        if constexpr (SHA::digest == 160)
        {
            // SHA-1 padding of state is 16-5 [11] words.
            out.at(5)  = pad.at(0);
            out.at(6)  = pad.at(1);
            out.at(7)  = pad.at(2);
            out.at(8)  = pad.at(3);
            out.at(9)  = pad.at(4);
            out.at(10) = pad.at(5);
            out.at(11) = pad.at(6);
            out.at(12) = pad.at(7);
            out.at(13) = pad.at(8);
            out.at(14) = pad.at(9);
            out.at(15) = pad.at(10);
        }
        else
        {
            // SHA-256/512 padding of state is 16-8 [8] words.
            out.at(8)  = pad.at(0);
            out.at(9)  = pad.at(1);
            out.at(10) = pad.at(2);
            out.at(11) = pad.at(3);
            out.at(12) = pad.at(4);
            out.at(13) = pad.at(5);
            out.at(14) = pad.at(6);
            out.at(15) = pad.at(7);
        }
    }
    else
    {
        constexpr auto size = SHA::block_words - SHA::state_words;
        unsafe_array_cast<word_t, size>(&out[SHA::state_words]) = pad;
    }
}

TEMPLATE
constexpr void CLASS::
pad_n(buffer_t& out, count_t blocks) NOEXCEPT
{
    // Pad any number of whole blocks.
    constexpr auto pad = blocks_pad();
    const auto bits = to_bits(blocks * array_count<block_t>);

    if (std::is_constant_evaluated())
    {
        out.at(0)  = pad.at(0);
        out.at(1)  = pad.at(1);
        out.at(2)  = pad.at(2);
        out.at(3)  = pad.at(3);
        out.at(4)  = pad.at(4);
        out.at(5)  = pad.at(5);
        out.at(6)  = pad.at(6);
        out.at(7)  = pad.at(7);
        out.at(8)  = pad.at(8);
        out.at(9)  = pad.at(9);
        out.at(10) = pad.at(10);
        out.at(11) = pad.at(11);
        out.at(12) = pad.at(12);
        out.at(13) = pad.at(13);
        out.at(14) = hi_word<word_t>(bits);
        out.at(15) = lo_word<word_t>(bits);
    }
    else
    {
        narrow_array_cast<word_t, array_count<blocks_pad_t>>(out) = pad;

        // Split count into hi/low words and assign end of padded buffer.
        out[14] = hi_word<word_t>(bits);
        out[15] = lo_word<word_t>(bits);
    }
}

// 5.2 Parsing the Message
// ---------------------------------------------------------------------------
// 5.2.1 SHA-1, SHA-224 and SHA-256
// 5.2.2 SHA-384, SHA-512, SHA-512/224 and SHA-512/256
// big-endian I/O is conventional for SHA.

TEMPLATE
constexpr void CLASS::
input(buffer_t& out, const block_t& in) NOEXCEPT
{
    constexpr auto size = SHA::word_bytes;

    if (std::is_constant_evaluated())
    {
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
    }
    else
    {
        auto& to = narrow_array_cast<word_t, SHA::block_words>(out);
        from_big_endians(to, array_cast<word_t>(in));
    }
}

TEMPLATE
constexpr void CLASS::
input1(buffer_t& out, const half_t& in) NOEXCEPT
{
    constexpr auto size = SHA::word_bytes;

    if (std::is_constant_evaluated())
    {
        from_big<0 * size>(out[0], in);
        from_big<1 * size>(out[1], in);
        from_big<2 * size>(out[2], in);
        from_big<3 * size>(out[3], in);
        from_big<4 * size>(out[4], in);
        from_big<5 * size>(out[5], in);
        from_big<6 * size>(out[6], in);
        from_big<7 * size>(out[7], in);
    }
    else
    {
        auto& to = narrow_array_cast<word_t, array_count<chunk_t>>(out);
        from_big_endians(to, array_cast<word_t>(in));
    }
}

TEMPLATE
constexpr void CLASS::
input2(buffer_t& out, const half_t& in) NOEXCEPT
{
    constexpr auto size = SHA::word_bytes;

    if (std::is_constant_evaluated())
    {
        from_big< 8 * size>(out[8], in);
        from_big< 9 * size>(out[9], in);
        from_big<10 * size>(out[10], in);
        from_big<11 * size>(out[11], in);
        from_big<12 * size>(out[12], in);
        from_big<13 * size>(out[13], in);
        from_big<14 * size>(out[14], in);
        from_big<15 * size>(out[15], in);
    }
    else
    {
        constexpr auto size = SHA::state_words;
        auto& to = unsafe_array_cast<word_t, size>(&out[size]);
        from_big_endians(to, array_cast<word_t>(in));
    }
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
output(const state_t& in) NOEXCEPT
{
    // 6.1.2 SHA-1   Hash Computation
    // 6.2.2 SHA-256 Hash Computation
    // 6.4.2 SHA-512 Hash Computation
    constexpr auto size = SHA::word_bytes;

    if (std::is_constant_evaluated())
    {
        digest_t out{};

        if constexpr (SHA::digest == 160)
        {
            to_big<0 * size>(out, in[0]);
            to_big<1 * size>(out, in[1]);
            to_big<2 * size>(out, in[2]);
            to_big<3 * size>(out, in[3]);
            to_big<4 * size>(out, in[4]);
        }
        else
        {
            to_big<0 * size>(out, in[0]);
            to_big<1 * size>(out, in[1]);
            to_big<2 * size>(out, in[2]);
            to_big<3 * size>(out, in[3]);
            to_big<4 * size>(out, in[4]);
            to_big<5 * size>(out, in[5]);
            to_big<6 * size>(out, in[6]);
            to_big<7 * size>(out, in[7]);
        }

        return out;
    }
    else
    {
        return array_cast<byte_t>(to_big_endians(in));
    }
}

// Single hash functions.
// ---------------------------------------------------------------------------

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const half_t& half) NOEXCEPT
{
    buffer_t space{};
    auto state = H::get;

    input1(space, half);
    pad_half(space);
    preparing(space);
    rounding(state, space);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const block_t& block) NOEXCEPT
{
    buffer_t space{};
    auto state = H::get;

    input(space, block);
    preparing(space);
    rounding(state, space);

    // pad_one is prepared
    pad_one(space);
    rounding(state, space);
    return output(state);
}

TEMPLATE
VCONSTEXPR typename CLASS::digest_t CLASS::
hash(const set_t& blocks) NOEXCEPT
{
    buffer_t space{};
    auto state = H::get;

    // The set is ordered (accumulated).
    for (auto& block: blocks)
    {
        input(space, block);
        preparing(space);
        rounding(state, space);
    }

    pad_n(space, blocks.size());
    preparing(space);
    rounding(state, space);
    return output(state);
}

// Double hash functions.
// ---------------------------------------------------------------------------
// TODO: provide double hash of half block (common scenario).

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
double_hash(const block_t& block) NOEXCEPT
{
    buffer_t space{};

    // first hash
    // --------------------
    auto state = H::get;

    // hash one block
    input(space, block);
    preparing(space);
    rounding(state, space);

    // pad_one is prepared
    // input state before resetting
    // hash one block of single block padding
    pad_one(space);
    rounding(state, space);
    input(space, state);

    // second hash
    // --------------------
    state = H::get;

    // hash padded state
    pad_state(space);
    preparing(space);
    rounding(state, space);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
double_hash(const digest_t& left, const digest_t& right) NOEXCEPT
{
    buffer_t space{};
    auto state = H::get;

    input1(space, left);
    input2(space, right);
    preparing(space);
    rounding(state, space);

    pad_one(space);
    rounding(state, space);
    input(space, state);

    state = H::get;
    pad_state(space);
    preparing(space);
    rounding(state, space);
    return output(state);
}

// TODO: test.
TEMPLATE
VCONSTEXPR typename CLASS::digests_t CLASS::
double_hash(const set_t& blocks) NOEXCEPT
{
    digests_t digests(blocks.size());

    // A double_hash set is independent blocks (vectorizable).
    std_transform(concurrency(), blocks.begin(), blocks.end(), digests.begin(),
        [&](const block_t& block)
        {
            buffer_t space{};

            auto state = H::get;
            input(space, block);
            preparing(space);
            rounding(state, space);
            pad_one(space);
            rounding(state, space);
            input(space, state);

            state = H::get;
            pad_state(space);
            preparing(space);
            rounding(state, space);
            return output(state);
        });

    return digests;
}

// Streaming hash functions.
// ---------------------------------------------------------------------------

TEMPLATE
constexpr void CLASS::
accumulate(state_t& state, const block_t& block) NOEXCEPT
{
    buffer_t space{};
    input(space, block);
    preparing(space);
    rounding(state, space);
}

TEMPLATE
VCONSTEXPR void CLASS::
accumulate(state_t& state, const set_t& blocks) NOEXCEPT
{
    buffer_t space{};

    for (auto& block: blocks)
    {
        input(space, block);
        preparing(space);
        rounding(state, space);
    }
}

// TODO: test.
TEMPLATE
template <size_t Size>
VCONSTEXPR typename CLASS::states_t CLASS::
accumulate(const sets_t<Size>& sets) NOEXCEPT
{
    states_t states(sets.size());

    // The set of sets is independent (vectorizable).
    std_transform(concurrency(), sets.begin(), sets.end(), states.begin(),
        [&](const set_t& blocks)
        {
            buffer_t space{};
            auto state = H::get;

            // Each set is ordered (accumulated).
            for (auto& block: blocks)
            {
                input(space, block);
                preparing(space);
                rounding(state, space);
            }
        });

    // States are not finalized as there may be more data in each set.
    // For each set use accumulator(blocks.size(), state) to continue, and
    // then finalize(state) to complete (or just finalize(state) if no more).
    return states;
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
finalize(const state_t& state) NOEXCEPT
{
    return output(state);
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

#undef CLASS
#undef TEMPLATE

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

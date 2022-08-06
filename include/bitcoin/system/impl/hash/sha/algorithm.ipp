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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_IPP

#include <bit>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

// Based on:
// FIPS PUB 180-4 [Secure Hash Standard (SHS)].
// All aspects of FIPS180 are supported within the implmentation.
// nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf

namespace libbitcoin {
namespace system {
namespace sha {

// TODO: integrate sha-ni/neon.
// TODO: vectorize (2/4/8/16 lanes).
// TODO: implement 5.3.6 SHA-512/t initial vector derivation.
// TODO: add SHA-256/224, 512/384, 512/224, 512/256 constants/types.

#define TEMPLATE template <typename SHA, bool Concurrent, \
    if_same<typename SHA::T, shah_t> If>
#define CLASS algorithm<SHA, Concurrent, If>

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
    // See comments in accumulator regarding padding endianness.
    constexpr auto bytes = possible_narrow_cast<word_t>(array_count<half_t>);

    chunk_t out{};
    out.front() = bit_hi<word_t>;
    out.back() = to_bits(bytes);
    return out;
}

TEMPLATE
CONSTEVAL typename CLASS::blocks_pad_t CLASS::
blocks_pad() NOEXCEPT
{
    // See comments in accumulator regarding padding endianness.
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
    schedule(out);
    return out;
}

// Primitives
// ---------------------------------------------------------------------------

template<auto B>
INLINE constexpr auto shr_(auto a) NOEXCEPT
{
    return a >> B;
}

template<auto B>
INLINE constexpr auto ror_(auto a) NOEXCEPT
{
    return std::rotr(a, B);
}

template<auto B>
INLINE constexpr auto rol_(auto a) NOEXCEPT
{
    return std::rotl(a, B);
}

template<auto B>
INLINE constexpr auto add_(auto a) NOEXCEPT
{
    return a + B;
}

INLINE constexpr auto add_(auto a, auto b) NOEXCEPT
{
    return a + b;
}

INLINE constexpr auto and_(auto a, auto b) NOEXCEPT
{
    return a & b;
}

INLINE constexpr auto or_(auto a, auto b) NOEXCEPT
{
    return a | b;
}

INLINE constexpr auto xor_(auto a, auto b) NOEXCEPT
{
    return a ^ b;
}

// 4.1 Functions
// ---------------------------------------------------------------------------

TEMPLATE
INLINE constexpr auto CLASS::
parity(auto x, auto y, auto z) NOEXCEPT
{
    // FIPS.180
    // 4.1.1 SHA-1 Functions
    return xor_(xor_(x, y), z);
}

TEMPLATE
INLINE constexpr auto CLASS::
choice(auto x, auto y, auto z) NOEXCEPT
{
    // FIPS.180
    // 4.1.1 SHA-1 Functions
    // 4.1.2 SHA-224 and SHA-256 Functions
    // 4.1.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Functions
    // Normal form reduced.
    ////return (x & y) ^ (~x & z);
    return xor_(and_(x, xor_(y, z)), z);
}

TEMPLATE
INLINE constexpr auto CLASS::
majority(auto x, auto y, auto z) NOEXCEPT
{
    // FIPS.180
    // 4.1.1 SHA-1 Functions
    // 4.1.2 SHA-224 and SHA-256 Functions
    // 4.1.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Functions
    // Normal form reduced.
    ////return (x & y) ^ (x & z) ^ (y & z);
    return or_(and_(x, or_(y, z)), and_(y, z));
}

TEMPLATE
template <size_t A, size_t B, size_t C>
INLINE constexpr auto CLASS::
sigma(auto x) NOEXCEPT
{
    // FIPS.180
    // 4.1.2 SHA-224 and SHA-256 Functions
    // 4.1.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Functions
    return xor_(xor_(ror_<A>(x), ror_<B>(x)), shr_<C>(x));
}

TEMPLATE
template <size_t A, size_t B, size_t C>
INLINE constexpr auto CLASS::
Sigma(auto x) NOEXCEPT
{
    // FIPS.180
    // 4.1.2 SHA-224 and SHA-256 Functions
    // 4.1.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Functions
    return xor_(xor_(ror_<A>(x), ror_<B>(x)), ror_<C>(x));
}

// Wrappers
// ---------------------------------------------------------------------------

TEMPLATE
INLINE constexpr auto CLASS::
Sigma0(auto x) NOEXCEPT
{
    if constexpr (SHA::rounds == 80)
        return Sigma<28, 34, 39>(x);
    else
        return Sigma<2, 13, 22>(x);
}

TEMPLATE
INLINE constexpr auto CLASS::
Sigma1(auto x) NOEXCEPT
{
    if constexpr (SHA::rounds == 80)
        return Sigma<14, 18, 41>(x);
    else
        return Sigma<6, 11, 25>(x);
}

TEMPLATE
INLINE constexpr auto CLASS::
sigma0(auto x) NOEXCEPT
{
    if constexpr (SHA::rounds == 80)
        return sigma<1, 8, 7>(x);
    else
        return sigma<7, 18, 3>(x);
}

TEMPLATE
INLINE constexpr auto CLASS::
sigma1(auto x) NOEXCEPT
{
    if constexpr (SHA::rounds == 80)
        return sigma<19, 61, 6>(x);
    else
        return sigma<17, 19, 10>(x);
}

// Rounds
// ---------------------------------------------------------------------------
// FIPS.180
// 6.1.2 SHA-1   Hash Computation (1 to N)
// 6.2.2 SHA-256 Hash Computation (1 to N)
// 6.4.2 SHA-512 Hash Computation (1 to N)

TEMPLATE
template<size_t Round, typename Auto>
CONSTEVAL auto CLASS::
functor() NOEXCEPT
{
    using self = CLASS;
    constexpr auto fn = Round / K::columns;

    // FIPS.180
    // 4.1.1 SHA-1 Functions (limited to uint32_t)
    // Select function by column.
    if constexpr (fn == 0u)
        return &self::template choice<Auto, Auto, Auto>;
    else if constexpr (fn == 1u)
        return &self::template parity<Auto, Auto, Auto>;
    else if constexpr (fn == 2u)
        return &self::template majority<Auto, Auto, Auto>;
    else if constexpr (fn == 3u)
        return &self::template parity<Auto, Auto, Auto>;
}

TEMPLATE
template<size_t Round>
INLINE constexpr void CLASS::
round(auto a, auto& b, auto c, auto d, auto& e, auto w) NOEXCEPT
{
    // FIPS.180
    // 4.2.1 SHA-1 Constants
    constexpr auto k = K::get[Round];
    constexpr auto f = functor<Round, decltype(a)>();

    // FIPS.180
    // 6.1.2.3 SHA-1 Hash Computation (t=0 to 79)
    e = /*a =*/ add_<k>(add_(add_(add_(rol_<5>(a), f(b, c, d)), e), w));
    b = /*c =*/ rol_<30>(b);

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
INLINE constexpr void CLASS::
round(auto a, auto b, auto c, auto& d, auto e, auto f, auto g, auto& h,
    auto w) NOEXCEPT
{
    // FIPS.180
    // 4.2.2 SHA-224 and SHA-256 Constants
    // 4.2.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Constants
    constexpr auto k = K::get[Round];

    // FIPS.180
    // 6.2.2.3 SHA-256 Hash Computation (t=0 to 63)
    // 6.4.2.3 SHA-512 Hash Computation (t=0 to 79)
    const auto t = add_<k>(add_(add_(add_(Sigma1(e), choice(e, f, g)), h), w));
    d = /*e =*/    add_(t, d);
    h = /*a =*/    add_(add_(t, Sigma0(a)), majority(a, b, c));

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
INLINE constexpr void CLASS::
round(auto& state, const auto& buffer) NOEXCEPT
{
    if constexpr (SHA::digest == 160)
    {
        // FIPS.180
        // 6.1.2.3 SHA-1 Hash Computation (t=0 to 79)
        round<Round>(
            state[(SHA::rounds + 0 - Round) % SHA::state_words],
            state[(SHA::rounds + 1 - Round) % SHA::state_words], // c->b
            state[(SHA::rounds + 2 - Round) % SHA::state_words],
            state[(SHA::rounds + 3 - Round) % SHA::state_words],
            state[(SHA::rounds + 4 - Round) % SHA::state_words], // a->e
            buffer[Round]);

        // SNA-NI/NEON
        // State packs in 128 (one state variable), reduces above to 1 out[].
        // Input value is 128 (w). Constants (k) statically initialized as 128.
    }
    else
    {
        // FIPS.180
        // 6.2.2.3 SHA-256 Hash Computation (t=0 to 63)
        // 6.4.2.3 SHA-512 Hash Computation (t=0 to 79)
        round<Round>(
            state[(SHA::rounds + 0 - Round) % SHA::state_words],
            state[(SHA::rounds + 1 - Round) % SHA::state_words],
            state[(SHA::rounds + 2 - Round) % SHA::state_words],
            state[(SHA::rounds + 3 - Round) % SHA::state_words], // e->d
            state[(SHA::rounds + 4 - Round) % SHA::state_words],
            state[(SHA::rounds + 5 - Round) % SHA::state_words],
            state[(SHA::rounds + 6 - Round) % SHA::state_words],
            state[(SHA::rounds + 7 - Round) % SHA::state_words], // a->h
            buffer[Round]);

        // SHA-NI/NEON
        // Each element is 128 (vs. 32), reduces above to 2 out[] (s0/s1).
        // Input value is 128 (w). Constants (k) statically initialized as 128.
    }
}

TEMPLATE
constexpr void CLASS::
compress(state_t& state, const buffer_t& buffer) NOEXCEPT
{
    // SHA-NI/256: 64/4 = 16 quad rounds, 8/4 = 2 state elements.
    const state_t start{ state };

    // Templated constant reduces ops per iteration by 35% (vs. parameter).
    // Pointer indexing reduces ops per iteration by 43% (vs. std::array[]).
    // Unrolled/inlined loop reduces ops per iteration by 23% (vs. for loop).
    // Pointer degradation here (optimization), use auto typing in round().
    auto pbuffer = buffer.data();
    auto pstate = state.data();

    // FIPS.180
    // 6.1.2.3 SHA-1   Hash Computation (t=0 to 79)
    // 6.2.2.3 SHA-256 Hash Computation (t=0 to 63)
    // 6.4.2.3 SHA-512 Hash Computation (t=0 to 79)
    round< 0>(pstate, pbuffer);
    round< 1>(pstate, pbuffer);
    round< 2>(pstate, pbuffer);
    round< 3>(pstate, pbuffer);
    round< 4>(pstate, pbuffer);
    round< 5>(pstate, pbuffer);
    round< 6>(pstate, pbuffer);
    round< 7>(pstate, pbuffer);
    round< 8>(pstate, pbuffer);
    round< 9>(pstate, pbuffer);
    round<10>(pstate, pbuffer);
    round<11>(pstate, pbuffer);
    round<12>(pstate, pbuffer);
    round<13>(pstate, pbuffer);
    round<14>(pstate, pbuffer);
    round<15>(pstate, pbuffer);

    round<16>(pstate, pbuffer);
    round<17>(pstate, pbuffer);
    round<18>(pstate, pbuffer);
    round<19>(pstate, pbuffer);
    round<20>(pstate, pbuffer);
    round<21>(pstate, pbuffer);
    round<22>(pstate, pbuffer);
    round<23>(pstate, pbuffer);
    round<24>(pstate, pbuffer);
    round<25>(pstate, pbuffer);
    round<26>(pstate, pbuffer);
    round<27>(pstate, pbuffer);
    round<28>(pstate, pbuffer);
    round<29>(pstate, pbuffer);
    round<30>(pstate, pbuffer);
    round<31>(pstate, pbuffer);

    round<32>(pstate, pbuffer);
    round<33>(pstate, pbuffer);
    round<34>(pstate, pbuffer);
    round<35>(pstate, pbuffer);
    round<36>(pstate, pbuffer);
    round<37>(pstate, pbuffer);
    round<38>(pstate, pbuffer);
    round<39>(pstate, pbuffer);
    round<40>(pstate, pbuffer);
    round<41>(pstate, pbuffer);
    round<42>(pstate, pbuffer);
    round<43>(pstate, pbuffer);
    round<44>(pstate, pbuffer);
    round<45>(pstate, pbuffer);
    round<46>(pstate, pbuffer);
    round<47>(pstate, pbuffer);

    round<48>(pstate, pbuffer);
    round<49>(pstate, pbuffer);
    round<50>(pstate, pbuffer);
    round<51>(pstate, pbuffer);
    round<52>(pstate, pbuffer);
    round<53>(pstate, pbuffer);
    round<54>(pstate, pbuffer);
    round<55>(pstate, pbuffer);
    round<56>(pstate, pbuffer);
    round<57>(pstate, pbuffer);
    round<58>(pstate, pbuffer);
    round<59>(pstate, pbuffer);
    round<60>(pstate, pbuffer);
    round<61>(pstate, pbuffer);
    round<62>(pstate, pbuffer);
    round<63>(pstate, pbuffer);

    // FIPS.180
    // 6.1.2.3 SHA-1   Hash Computation (t=0 to 79)
    // 6.4.2.3 SHA-512 Hash Computation (t=0 to 79)
    if constexpr (SHA::rounds == 80)
    {
        round<64>(pstate, pbuffer);
        round<65>(pstate, pbuffer);
        round<66>(pstate, pbuffer);
        round<67>(pstate, pbuffer);
        round<68>(pstate, pbuffer);
        round<69>(pstate, pbuffer);
        round<70>(pstate, pbuffer);
        round<71>(pstate, pbuffer);
        round<72>(pstate, pbuffer);
        round<73>(pstate, pbuffer);
        round<74>(pstate, pbuffer);
        round<75>(pstate, pbuffer);
        round<76>(pstate, pbuffer);
        round<77>(pstate, pbuffer);
        round<78>(pstate, pbuffer);
        round<79>(pstate, pbuffer);
    }

    summarize(state, start);
}

TEMPLATE
template<size_t Round>
INLINE constexpr void CLASS::
prepare(auto& buffer) NOEXCEPT
{
    if constexpr (SHA::digest == 160)
    {
        // FIPS.180
        // 6.1.2 SHA-1 Hash Computation (16 <= t <= 79)
        buffer[Round] = rol_<1>(xor_(
            xor_(buffer[Round - 16], buffer[Round - 14]),
            xor_(buffer[Round -  8], buffer[Round -  3])));

        // SHA-NI
        //     buffer[Round] = sha1msg2 // xor and rotl1
        //     (
        //         xor                // not using sha1msg1
        //         (
        //             sha1msg1       // xor (specialized)
        //             (
        //                 buffer[Round - 16],
        //                 buffer[Round - 14]
        //             ),
        //             buffer[Round -  8]
        //          ),
        //          buffer[Round -  3]
        //     );
        // NEON
        //     vsha1su1q/vsha1su0q
    }
    else
    {
        // FIPS.180
        // 6.2.2 SHA-256 Hash Computation (16 <= t <= 63)
        // 6.4.2 SHA-512 Hash Computation (16 <= t <= 79)
        buffer[Round] = add_(
            add_(buffer[Round - 16], sigma0(buffer[Round - 15])),
            add_(buffer[Round -  7], sigma1(buffer[Round -  2])));

        // Each word is 128, buffer goes from 64 to 16 words.
        // SHA-NI
        // buffer[Round] =
        //     sha256msg1(buffer[Round - 16], buffer[Round - 15]) +
        //     sha256msg2(buffer[Round -  7], buffer[Round -  2]);
        // NEON
        // Not sure about these indexes.
        // mijailovic.net/2018/06/06/sha256-armv8
        // buffer[Round] =
        //     vsha256su0q(buffer[Round - 13], buffer[Round - 9])
        //     vsha256su1q(buffer[Round - 13], buffer[Round - 5],
        //                 buffer[Round - 1]);
    }
}

TEMPLATE
constexpr void CLASS::
schedule(buffer_t& buffer) NOEXCEPT
{
    // Pointer degradation here (optimization), use auto typing in prepare().
    auto pbuffer = buffer.data();

    // FIPS.180
    // 6.1.2.1 SHA-1   Hash Computation (16 <= t <= 79)
    // 6.2.2.1 SHA-256 Hash Computation (16 <= t <= 63)
    // 6.4.2.1 SHA-512 Hash Computation (16 <= t <= 79)
    prepare<16>(pbuffer);
    prepare<17>(pbuffer);
    prepare<18>(pbuffer);
    prepare<19>(pbuffer);
    prepare<20>(pbuffer);
    prepare<21>(pbuffer);
    prepare<22>(pbuffer);
    prepare<23>(pbuffer);
    prepare<24>(pbuffer);
    prepare<25>(pbuffer);
    prepare<26>(pbuffer);
    prepare<27>(pbuffer);
    prepare<28>(pbuffer);
    prepare<29>(pbuffer);
    prepare<30>(pbuffer);
    prepare<31>(pbuffer);
    prepare<32>(pbuffer);

    prepare<33>(pbuffer);
    prepare<34>(pbuffer);
    prepare<35>(pbuffer);
    prepare<36>(pbuffer);
    prepare<37>(pbuffer);
    prepare<38>(pbuffer);
    prepare<39>(pbuffer);
    prepare<40>(pbuffer);
    prepare<41>(pbuffer);
    prepare<42>(pbuffer);
    prepare<43>(pbuffer);
    prepare<44>(pbuffer);
    prepare<45>(pbuffer);
    prepare<46>(pbuffer);
    prepare<47>(pbuffer);
    prepare<48>(pbuffer);

    prepare<49>(pbuffer);
    prepare<50>(pbuffer);
    prepare<51>(pbuffer);
    prepare<52>(pbuffer);
    prepare<53>(pbuffer);
    prepare<54>(pbuffer);
    prepare<55>(pbuffer);
    prepare<56>(pbuffer);
    prepare<57>(pbuffer);
    prepare<58>(pbuffer);
    prepare<59>(pbuffer);
    prepare<60>(pbuffer);
    prepare<61>(pbuffer);
    prepare<62>(pbuffer);
    prepare<63>(pbuffer);

    // FIPS.180
    // 6.1.2 SHA-1   Hash Computation (16 <= t <= 79)
    // 6.4.2 SHA-512 Hash Computation (16 <= t <= 79)
    if constexpr (SHA::rounds == 80)
    {
        prepare<64>(pbuffer);
        prepare<65>(pbuffer);
        prepare<66>(pbuffer);
        prepare<67>(pbuffer);
        prepare<68>(pbuffer);
        prepare<69>(pbuffer);
        prepare<70>(pbuffer);
        prepare<71>(pbuffer);
        prepare<72>(pbuffer);
        prepare<73>(pbuffer);
        prepare<74>(pbuffer);
        prepare<75>(pbuffer);
        prepare<76>(pbuffer);
        prepare<77>(pbuffer);
        prepare<78>(pbuffer);
        prepare<79>(pbuffer);
    }
}

TEMPLATE
constexpr void CLASS::
summarize(state_t& out, const state_t& in) NOEXCEPT
{
    // FIPS.180
    // 6.1.2.4 SHA-1 Hash Computation
    out[0] = add_(out[0], in[0]);
    out[1] = add_(out[1], in[1]);
    out[2] = add_(out[2], in[2]);
    out[3] = add_(out[3], in[3]);
    out[4] = add_(out[4], in[4]);

    if constexpr (SHA::digest != 160)
    {
        // FIPS.180
        // 6.2.2.4 SHA-256 Hash Computation
        // 6.4.2.4 SHA-512 Hash Computation
        out[5] = add_(out[5], in[5]);
        out[6] = add_(out[6], in[6]);
        out[7] = add_(out[7], in[7]);
    }
}

TEMPLATE
constexpr void CLASS::
input(buffer_t& buffer, const state_t& state) NOEXCEPT
{
    // FIPS.180
    // 5.3 Setting the Initial Hash Value

    // This is a double hash optimization.
    if (std::is_constant_evaluated())
    {
        // FIPS.180
        // 6.1.2.1 SHA-1 Hash Computation (0 <= t <= 15)
        buffer[0] = state[0];
        buffer[1] = state[1];
        buffer[2] = state[2];
        buffer[3] = state[3];
        buffer[4] = state[4];

        if constexpr (SHA::digest != 160)
        {
            // FIPS.180
            // 6.2.2.1 SHA-256 Hash Computation (0 <= t <= 15)
            // 6.4.2.1 SHA-512 Hash Computation (0 <= t <= 15)
            buffer[5] = state[5];
            buffer[6] = state[6];
            buffer[7] = state[7];
        }
    }
    else
    {
        array_cast<word_t, array_count<state_t>>(buffer) = state;
    }
}

// FIPS.180
// 5.1 Padding the Message
// ---------------------------------------------------------------------------
// 5.1.1 SHA-1, SHA-224 and SHA-256
// 5.1.2 SHA-384, SHA-512, SHA-512/224 and SHA-512/256

TEMPLATE
constexpr void CLASS::
pad_one(buffer_t& buffer) NOEXCEPT
{
    // Pad a single whole block with pre-prepared buffer.
    constexpr auto pad = block_pad();

    buffer = pad;
}

TEMPLATE
constexpr void CLASS::
pad_half(buffer_t& buffer) NOEXCEPT
{
    // Pad a half block.
    constexpr auto pad = chunk_pad();

    if (std::is_constant_evaluated())
    {
        buffer.at(8)  = pad.at(0);
        buffer.at(9)  = pad.at(1);
        buffer.at(10) = pad.at(2);
        buffer.at(11) = pad.at(3);
        buffer.at(12) = pad.at(4);
        buffer.at(13) = pad.at(5);
        buffer.at(14) = pad.at(6);
        buffer.at(15) = pad.at(7);
    }
    else
    {
        constexpr auto size = array_count<chunk_t>;
        array_cast<word_t, size, size>(buffer) = pad;
    }
}

TEMPLATE
constexpr void CLASS::
pad_n(buffer_t& buffer, count_t blocks) NOEXCEPT
{
    // Pad any number of whole blocks.
    constexpr auto pad = blocks_pad();
    const auto bits = to_bits(blocks * array_count<block_t>);

    if (std::is_constant_evaluated())
    {
        buffer.at(0)  = pad.at(0);
        buffer.at(1)  = pad.at(1);
        buffer.at(2)  = pad.at(2);
        buffer.at(3)  = pad.at(3);
        buffer.at(4)  = pad.at(4);
        buffer.at(5)  = pad.at(5);
        buffer.at(6)  = pad.at(6);
        buffer.at(7)  = pad.at(7);
        buffer.at(8)  = pad.at(8);
        buffer.at(9)  = pad.at(9);
        buffer.at(10) = pad.at(10);
        buffer.at(11) = pad.at(11);
        buffer.at(12) = pad.at(12);
        buffer.at(13) = pad.at(13);
        buffer.at(14) = hi_word<word_t>(bits);
        buffer.at(15) = lo_word<word_t>(bits);
    }
    else
    {
        array_cast<word_t, array_count<blocks_pad_t>>(buffer) = pad;

        // Split count into hi/low words and assign end of padded buffer.
        buffer[14] = hi_word<word_t>(bits);
        buffer[15] = lo_word<word_t>(bits);
    }
}

// FIPS.180
// 5.2 Parsing the Message
// ---------------------------------------------------------------------------
// 5.2.1 SHA-1, SHA-224 and SHA-256
// 5.2.2 SHA-384, SHA-512, SHA-512/224 and SHA-512/256
// big-endian I/O is conventional for SHA.

TEMPLATE
constexpr void CLASS::
input(buffer_t& buffer, const block_t& block) NOEXCEPT
{
    constexpr auto size = SHA::word_bytes;

    if (std::is_constant_evaluated())
    {
        from_big< 0 * size>(buffer.at( 0), block);
        from_big< 1 * size>(buffer.at( 1), block);
        from_big< 2 * size>(buffer.at( 2), block);
        from_big< 3 * size>(buffer.at( 3), block);
        from_big< 4 * size>(buffer.at( 4), block);
        from_big< 5 * size>(buffer.at( 5), block);
        from_big< 6 * size>(buffer.at( 6), block);
        from_big< 7 * size>(buffer.at( 7), block);
        from_big< 8 * size>(buffer.at( 8), block);
        from_big< 9 * size>(buffer.at( 9), block);
        from_big<10 * size>(buffer.at(10), block);
        from_big<11 * size>(buffer.at(11), block);
        from_big<12 * size>(buffer.at(12), block);
        from_big<13 * size>(buffer.at(13), block);
        from_big<14 * size>(buffer.at(14), block);
        from_big<15 * size>(buffer.at(15), block);
    }
    else
    {
        auto& to = array_cast<word_t, SHA::block_words>(buffer);
        from_big_endians(to, array_cast<word_t>(block));
    }
}

TEMPLATE
constexpr void CLASS::
input1(buffer_t& buffer, const half_t& half) NOEXCEPT
{
    constexpr auto size = SHA::word_bytes;

    if (std::is_constant_evaluated())
    {
        from_big<0 * size>(buffer.at(0), half);
        from_big<1 * size>(buffer.at(1), half);
        from_big<2 * size>(buffer.at(2), half);
        from_big<3 * size>(buffer.at(3), half);
        from_big<4 * size>(buffer.at(4), half);
        from_big<5 * size>(buffer.at(5), half);
        from_big<6 * size>(buffer.at(6), half);
        from_big<7 * size>(buffer.at(7), half);
    }
    else
    {
        auto& to = array_cast<word_t, array_count<chunk_t>>(buffer);
        from_big_endians(to, array_cast<word_t>(half));
    }
}

TEMPLATE
constexpr void CLASS::
input2(buffer_t& buffer, const half_t& half) NOEXCEPT
{
    constexpr auto size = SHA::word_bytes;

    if (std::is_constant_evaluated())
    {
        from_big<0 * size>(buffer.at( 8), half);
        from_big<1 * size>(buffer.at( 9), half);
        from_big<2 * size>(buffer.at(10), half);
        from_big<3 * size>(buffer.at(11), half);
        from_big<4 * size>(buffer.at(12), half);
        from_big<5 * size>(buffer.at(13), half);
        from_big<6 * size>(buffer.at(14), half);
        from_big<7 * size>(buffer.at(15), half);
    }
    else
    {
        constexpr auto words = SHA::state_words;
        auto& to = array_cast<word_t, words, words>(buffer);
        from_big_endians(to, array_cast<word_t>(half));
    }
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
output(const state_t& state) NOEXCEPT
{
    // FIPS.180
    // 6.1.2 SHA-1   Hash Computation
    // 6.2.2 SHA-256 Hash Computation
    // 6.4.2 SHA-512 Hash Computation
    constexpr auto size = SHA::word_bytes;

    if (std::is_constant_evaluated())
    {
        digest_t digest{};

        to_big<0 * size>(digest, state.at(0));
        to_big<1 * size>(digest, state.at(1));
        to_big<2 * size>(digest, state.at(2));
        to_big<3 * size>(digest, state.at(3));
        to_big<4 * size>(digest, state.at(4));

        if constexpr (SHA::digest != 160)
        {
            to_big<5 * size>(digest, state.at(5));
            to_big<6 * size>(digest, state.at(6));
            to_big<7 * size>(digest, state.at(7));
        }

        return digest;
    }
    else
    {
        return array_cast<byte_t>(to_big_endians(state));
    }
}

// Hashing.
// ---------------------------------------------------------------------------

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const half_t& half) NOEXCEPT
{
    buffer_t buffer{};
    auto state = H::get;

    input1(buffer, half);
    pad_half(buffer);
    schedule(buffer);
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const block_t& block) NOEXCEPT
{
    buffer_t buffer{};
    auto state = H::get;

    input(buffer, block);
    schedule(buffer);
    compress(state, buffer);

    // pad_one is fully precomputed and buffer prepared.
    pad_one(buffer);
    compress(state, buffer);
    return output(state);
}

// Double Hashing.
// ---------------------------------------------------------------------------
// No double_hash optimizations for sha160 (double_hash requires half_t).

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
double_hash(const block_t& block) NOEXCEPT
{
    static_assert(is_same_type<digest_t, half_t>);

    buffer_t buffer{};

    // hash one block
    auto state = H::get;
    input(buffer, block);
    schedule(buffer);
    compress(state, buffer);

    // pad_one is prepared
    // input state before resetting
    // hash one block of single block padding
    pad_one(buffer);
    compress(state, buffer);
    input(buffer, state);

    // hash padded state
    state = H::get;
    pad_half(buffer);
    schedule(buffer);
    compress(state, buffer);
    return output(state);
}

// [witness commitment, merkle root]
TEMPLATE
constexpr typename CLASS::digest_t CLASS::
double_hash(const half_t& left, const half_t& right) NOEXCEPT
{
    static_assert(is_same_type<digest_t, half_t>);

    buffer_t buffer{};
    auto state = H::get;

    // Combine halves into buffer.
    input1(buffer, left);
    input2(buffer, right);
    schedule(buffer);
    compress(state, buffer);

    pad_one(buffer);
    compress(state, buffer);
    input(buffer, state);

    state = H::get;
    pad_half(buffer);
    schedule(buffer);
    compress(state, buffer);
    return output(state);
}

// Merkle Hashing (sha256/512).
// ------------------------------------------------------------------------
// No merkle_hash optimizations for sha160 (double_hash requires half_t).

// TODO: this is the boundary of vectorization.
// TODO: this will return lane hashes and increment by lanes.
// TODO: just need to double hash a range and assign the returned range.
TEMPLATE
VCONSTEXPR typename CLASS::digests_t& CLASS::
merkle_hash(digests_t& digests) NOEXCEPT
{
    static_assert(is_same_type<digest_t, half_t>);
    const auto half = to_half(digests.size());

    for (size_t i = 0, j = 0; i < half; ++i, j += 2)
        digests[i] = double_hash(digests[j], digests[add1(j)]);

    digests.resize(half);
    return digests;
};

TEMPLATE
VCONSTEXPR typename CLASS::digest_t CLASS::
merkle_root(digests_t&& digests) NOEXCEPT
{
    static_assert(is_same_type<digest_t, half_t>);
    const auto count = digests.size();

    if (is_zero(count))
        return {};

    if (is_one(count))
        return std::move(digests.front());

    if (is_odd(count))
        digests.push_back(digests.back());

    while (merkle_hash(digests).size() > one);
    return std::move(digests.front());
}

// Block.txs skip-parsed hash set.
////// Bitcoin hash set from an ordered set of ptrs [header commitment].
////INLINE hashes bitcoin_hash(std_vector<uint8_t*>&&) NOEXCEPT
////
////template <size_t Size>
////using set_t = std_vector<cref<std_array<block_t, Size>>>;
////using states_t = std_vector<state_t>;
////
////TEMPLATE
////template <size_t Size>
////VCONSTEXPR typename CLASS::states_t CLASS::
////accumulate(const set_t<Size>& sets) NOEXCEPT
////{
////    states_t states(sets.size());
////
////    // The set of sets is order independent across the sets (vectorizable).
////    std_transform(concurrency(), sets.begin(), sets.end(), states.begin(),
////        [&](const blocks_t& blocks)
////        {
////            buffer_t buffer{};
////            auto state = H::get;
////
////            // Each set is ordered (accumulated).
////            for (auto& block: blocks)
////            {
////                input(buffer, block);
////                schedule(buffer);
////                rounding(state, buffer);
////            }
////        });
////
////    return states;
////}

// Streaming (unfinalized).
// ---------------------------------------------------------------------------

TEMPLATE
constexpr void CLASS::
accumulate(state_t& state, const block_t& block) NOEXCEPT
{
    buffer_t buffer{};
    input(buffer, block);
    schedule(buffer);
    compress(state, buffer);
}

TEMPLATE
VCONSTEXPR void CLASS::
accumulate(state_t& state, const blocks_t& blocks) NOEXCEPT
{
    // Synchronization cost is prohibitive (material net loss) in all test
    // scenarios with less than a one GiB buffer (breakeven).
    if constexpr (Concurrent)
    {
        // Due to the memory allocation, avoid when not concurrent.
        // No measurable difference between heap vs. stack alloc here.
        auto buffers = std::make_shared<buffers_t>(blocks.size());

        // eprint.iacr.org/2012/067.pdf
        // Message schedules/endianness are order independent.
        std_transform(concurrency(), blocks.begin(), blocks.end(),
            buffers->begin(), [](const block_t& block) NOEXCEPT
            {
                buffer_t space{};
                input(space, block);
                schedule(space);
                return space;
            });

        for (auto& space: *buffers)
            compress(state, space);
    }
    else
    {
        buffer_t buffer{};

        for (auto& block: blocks)
        {
            input(buffer, block);
            schedule(buffer);
            compress(state, buffer);
        }
    }
}

TEMPLATE
constexpr void CLASS::
finalize(digest_t& digest, const state_t& state) NOEXCEPT
{
    digest = output(state);
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

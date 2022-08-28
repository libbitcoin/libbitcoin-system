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

#include <utility>
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

// Bogus warning suggests constexpr when declared consteval.
BC_PUSH_WARNING(USE_CONSTEXPR_FOR_FUNCTION)
BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

// 4.1 Functions
// ---------------------------------------------------------------------------

TEMPLATE
INLINE constexpr auto CLASS::
parity(auto x, auto y, auto z) NOEXCEPT
{
    return f::xor_(f::xor_(x, y), z);
}

TEMPLATE
INLINE constexpr auto CLASS::
choice(auto x, auto y, auto z) NOEXCEPT
{
    // Normal form reduced.
    ////return (x & y) ^ (~x & z);
    return f::xor_(f::and_(x, f::xor_(y, z)), z);
}

TEMPLATE
INLINE constexpr auto CLASS::
majority(auto x, auto y, auto z) NOEXCEPT
{
    // Normal form reduced.
    ////return (x & y) ^ (x & z) ^ (y & z);
    return f::or_(f::and_(x, f::or_(y, z)), f::and_(y, z));
}

TEMPLATE
template <unsigned int A, unsigned int B, unsigned int C>
INLINE constexpr auto CLASS::
sigma(auto x) NOEXCEPT
{
    constexpr auto s = SHA::word_bits;
    return f::xor_(f::xor_(f::ror<A, s>(x), f::ror<B, s>(x)), f::shr<C, s>(x));
}

TEMPLATE
template <unsigned int A, unsigned int B, unsigned int C>
INLINE constexpr auto CLASS::
Sigma(auto x) NOEXCEPT
{
    // Specialized for non-vector destructive ror.
    // "Because the ror instruction is destructive (that is, it overwrites the
    // operand), implementing the above as written would involve a number of
    // source register copy operations. If, however, the expressions are
    // rewritten as... Then the number of register copies can be minimized."
    // intel.com/content/dam/www/public/us/en/documents/white-papers/
    // sha-256-implementations-paper.pdf
    // AVX optimal (sha256)
    //
    // Examples:
    // S0(a) = (a >>> 2) ^ (a >>> 13) ^ (a >>> 22)
    // S1(e) = (e >>> 6) ^ (e >>> 11) ^ (e >>> 25)
    // S0(a) = ((((a >>>  9) ^ a) >>> 11) ^ a) >>> 2
    // S1(e) = ((((e >>> 14) ^ e) >>>  5) ^ e) >>> 6
    //
    // Generalization:
    // S (n) = (n >>> x) ^ (n >>> y) ^ (n >>> z)
    // S'(n) = ((((n >>> X) ^ n) >>> Y) ^ n) >>> Z
    // X = z - y
    // Y = y - x
    // Z = x
    // S'(n) = ((((n >>> (z-y)) ^ n) >>> (y-x)) ^ n) >>> x
    ////return f::xor_(f::xor_(f::ror<A, s>(x), f::ror<B, s>(x)), f::ror<C, s>(x));

    // This Sigma refactoring reduces native processing time by ~10%.
    constexpr auto s = SHA::word_bits;
    return f::ror<A, s>(f::xor_(f::ror<B - A, s>(f::xor_(f::ror<C - B, s>(x), x)), x));
}

// Sigma dispatch.
// ---------------------------------------------------------------------------

TEMPLATE
INLINE constexpr auto CLASS::
sigma0(auto x) NOEXCEPT
{
    if constexpr (SHA::strength == 256)
        return sigma<7, 18, 3>(x);
    else
        return sigma<1, 8, 7>(x);
}

TEMPLATE
INLINE constexpr auto CLASS::
sigma1(auto x) NOEXCEPT
{
    if constexpr (SHA::strength == 256)
        return sigma<17, 19, 10>(x);
    else
        return sigma<19, 61, 6>(x);
}

TEMPLATE
INLINE constexpr auto CLASS::
Sigma0(auto x) NOEXCEPT
{
    if constexpr (SHA::strength == 256)
        return Sigma<2, 13, 22>(x);
    else
        return Sigma<28, 34, 39>(x);
}

TEMPLATE
INLINE constexpr auto CLASS::
Sigma1(auto x) NOEXCEPT
{
    if constexpr (SHA::strength == 256)
        return Sigma<6, 11, 25>(x);
    else
        return Sigma<14, 18, 41>(x);
}

// Rounds
// ---------------------------------------------------------------------------

TEMPLATE
template<size_t Round, typename Auto>
CONSTEVAL auto CLASS::
functor() NOEXCEPT
{
    using self = CLASS;
    constexpr auto fn = Round / K::columns;

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
round(auto a, auto& b, auto c, auto d, auto& e, auto wk) NOEXCEPT
{
    constexpr auto s = SHA::word_bits;
    constexpr auto fn = functor<Round, decltype(a)>();
    e = /*a =*/ f::add<s>(f::add<s>(f::add<s>(f::rol<5, s>(a), fn(b, c, d)), e), wk);
    b = /*c =*/ f::rol<30, s>(b);

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
    auto wk) NOEXCEPT
{
    // TODO: if open lanes, vectorize Sigma0 and Sigma1 (see Intel).
    constexpr auto s = SHA::word_bits;
    const auto t = f::add<s>(f::add<s>(f::add<s>(Sigma1(e), choice(e, f, g)), h), wk);
    d = /*e =*/    f::add<s>(d, t);
    h = /*a =*/    f::add<s>(f::add<s>(Sigma0(a), majority(a, b, c)), t);

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
template <typename Word, size_t Lane>
INLINE constexpr auto CLASS::
extract(Word a) NOEXCEPT
{
    // Compress vectorization and non-vectorization require no extraction.
    static_assert(Lane == zero);
    return a;
}

TEMPLATE
template <typename Word, size_t Lane, typename xWord,
    if_not_same<Word, xWord>>
INLINE Word CLASS::
extract(xWord a) NOEXCEPT
{
    // Schedule vectorization (with compress non-vectorization), extract word.
    return get<Word, Lane>(a);
}

TEMPLATE
template<size_t Round, size_t Lane>
INLINE constexpr void CLASS::
round(auto& state, const auto& wk) NOEXCEPT
{
    using word = nocvref<decltype(state.front())>;

    if constexpr (SHA::strength == 160)
    {
        // msvc++ not inlined in x32 for vectorized state.
        BC_PUSH_WARNING(NOT_INLINED)
        round<Round>(
            state[(SHA::rounds + 0 - Round) % SHA::state_words],
            state[(SHA::rounds + 1 - Round) % SHA::state_words], // c->b
            state[(SHA::rounds + 2 - Round) % SHA::state_words],
            state[(SHA::rounds + 3 - Round) % SHA::state_words],
            state[(SHA::rounds + 4 - Round) % SHA::state_words], // a->e
            extract<word, Lane>(wk[Round]));
        BC_POP_WARNING()

        // SNA-NI/NEON
        // State packs in 128 (one state variable), reduces above to 1 out[].
        // Input value is 128 (w). Constants (k) statically initialized as 128.
    }
    else
    {
        // msvc++ not inlined in x32 for vectorized state.
        BC_PUSH_WARNING(NOT_INLINED)
        round<Round>(
            state[(SHA::rounds + 0 - Round) % SHA::state_words],
            state[(SHA::rounds + 1 - Round) % SHA::state_words],
            state[(SHA::rounds + 2 - Round) % SHA::state_words],
            state[(SHA::rounds + 3 - Round) % SHA::state_words], // e->d
            state[(SHA::rounds + 4 - Round) % SHA::state_words],
            state[(SHA::rounds + 5 - Round) % SHA::state_words],
            state[(SHA::rounds + 6 - Round) % SHA::state_words],
            state[(SHA::rounds + 7 - Round) % SHA::state_words], // a->h
            extract<word, Lane>(wk[Round]));
        BC_POP_WARNING()

        // SHA-NI/NEON
        // Each element is 128 (vs. 32), reduces above to 2 out[] (s0/s1).
        // Input value is 128 (w). Constants (k) statically initialized as 128.
    }
}

TEMPLATE
template <size_t Lane>
constexpr void CLASS::
compress(auto& state, const auto& buffer) NOEXCEPT
{
    // SHA-NI/256: 64/4 = 16 quad rounds, 8/4 = 2 state elements.
    // This is a copy (state type varies due to vectorization).
    const auto start = state;

    round< 0, Lane>(state, buffer);
    round< 1, Lane>(state, buffer);
    round< 2, Lane>(state, buffer);
    round< 3, Lane>(state, buffer);
    round< 4, Lane>(state, buffer);
    round< 5, Lane>(state, buffer);
    round< 6, Lane>(state, buffer);
    round< 7, Lane>(state, buffer);
    round< 8, Lane>(state, buffer);
    round< 9, Lane>(state, buffer);
    round<10, Lane>(state, buffer);
    round<11, Lane>(state, buffer);
    round<12, Lane>(state, buffer);
    round<13, Lane>(state, buffer);
    round<14, Lane>(state, buffer);
    round<15, Lane>(state, buffer);

    round<16, Lane>(state, buffer);
    round<17, Lane>(state, buffer);
    round<18, Lane>(state, buffer);
    round<19, Lane>(state, buffer);
    round<20, Lane>(state, buffer);
    round<21, Lane>(state, buffer);
    round<22, Lane>(state, buffer);
    round<23, Lane>(state, buffer);
    round<24, Lane>(state, buffer);
    round<25, Lane>(state, buffer);
    round<26, Lane>(state, buffer);
    round<27, Lane>(state, buffer);
    round<28, Lane>(state, buffer);
    round<29, Lane>(state, buffer);
    round<30, Lane>(state, buffer);
    round<31, Lane>(state, buffer);

    round<32, Lane>(state, buffer);
    round<33, Lane>(state, buffer);
    round<34, Lane>(state, buffer);
    round<35, Lane>(state, buffer);
    round<36, Lane>(state, buffer);
    round<37, Lane>(state, buffer);
    round<38, Lane>(state, buffer);
    round<39, Lane>(state, buffer);
    round<40, Lane>(state, buffer);
    round<41, Lane>(state, buffer);
    round<42, Lane>(state, buffer);
    round<43, Lane>(state, buffer);
    round<44, Lane>(state, buffer);
    round<45, Lane>(state, buffer);
    round<46, Lane>(state, buffer);
    round<47, Lane>(state, buffer);

    round<48, Lane>(state, buffer);
    round<49, Lane>(state, buffer);
    round<50, Lane>(state, buffer);
    round<51, Lane>(state, buffer);
    round<52, Lane>(state, buffer);
    round<53, Lane>(state, buffer);
    round<54, Lane>(state, buffer);
    round<55, Lane>(state, buffer);
    round<56, Lane>(state, buffer);
    round<57, Lane>(state, buffer);
    round<58, Lane>(state, buffer);
    round<59, Lane>(state, buffer);
    round<60, Lane>(state, buffer);
    round<61, Lane>(state, buffer);
    round<62, Lane>(state, buffer);
    round<63, Lane>(state, buffer);

    if constexpr (SHA::rounds == 80)
    {
        round<64, Lane>(state, buffer);
        round<65, Lane>(state, buffer);
        round<66, Lane>(state, buffer);
        round<67, Lane>(state, buffer);
        round<68, Lane>(state, buffer);
        round<69, Lane>(state, buffer);
        round<70, Lane>(state, buffer);
        round<71, Lane>(state, buffer);
        round<72, Lane>(state, buffer);
        round<73, Lane>(state, buffer);
        round<74, Lane>(state, buffer);
        round<75, Lane>(state, buffer);
        round<76, Lane>(state, buffer);
        round<77, Lane>(state, buffer);
        round<78, Lane>(state, buffer);
        round<79, Lane>(state, buffer);
    }

    summarize(state, start);
}

TEMPLATE
template<size_t Round>
INLINE constexpr void CLASS::
prepare(auto& buffer) NOEXCEPT
{
    // K is added to schedule words because schedule is vectorizable.
    // K-adding is shifted -16, with last 16 added after scheduling.
    constexpr auto s = SHA::word_bits;

    if constexpr (SHA::strength == 160)
    {
        constexpr auto r03 = Round - 3;
        constexpr auto r08 = Round - 8;
        constexpr auto r14 = Round - 14;
        constexpr auto r16 = Round - 16;

        buffer[Round] = f::rol<1, s>(f::xor_(
            f::xor_(buffer[r16], buffer[r14]),
            f::xor_(buffer[r08], buffer[r03])));

        buffer[r16] = f::addc<K::get[r16], s>(buffer[r16]);

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
        constexpr auto r02 = Round - 2;
        constexpr auto r07 = Round - 7;
        constexpr auto r15 = Round - 15;
        constexpr auto r16 = Round - 16;

        buffer[Round] = f::add<s>(
            f::add<s>(buffer[r16], sigma0(buffer[r15])),
            f::add<s>(buffer[r07], sigma1(buffer[r02])));

        buffer[r16] = f::addc<K::get[r16], s>(buffer[r16]);

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
INLINE constexpr void CLASS::
add_k(auto& buffer) NOEXCEPT
{
    // Add K to last 16 words.
    constexpr auto s = SHA::word_bits;
    constexpr auto r = SHA::rounds - array_count<words_t>;
    buffer[r + 0] = f::addc<K::get[r + 0], s>(buffer[r + 0]);
    buffer[r + 1] = f::addc<K::get[r + 1], s>(buffer[r + 1]);
    buffer[r + 2] = f::addc<K::get[r + 2], s>(buffer[r + 2]);
    buffer[r + 3] = f::addc<K::get[r + 3], s>(buffer[r + 3]);
    buffer[r + 4] = f::addc<K::get[r + 4], s>(buffer[r + 4]);
    buffer[r + 5] = f::addc<K::get[r + 5], s>(buffer[r + 5]);
    buffer[r + 6] = f::addc<K::get[r + 6], s>(buffer[r + 6]);
    buffer[r + 7] = f::addc<K::get[r + 7], s>(buffer[r + 7]);
    buffer[r + 8] = f::addc<K::get[r + 8], s>(buffer[r + 8]);
    buffer[r + 9] = f::addc<K::get[r + 9], s>(buffer[r + 9]);
    buffer[r + 10] = f::addc<K::get[r + 10], s>(buffer[r + 10]);
    buffer[r + 11] = f::addc<K::get[r + 11], s>(buffer[r + 11]);
    buffer[r + 12] = f::addc<K::get[r + 12], s>(buffer[r + 12]);
    buffer[r + 13] = f::addc<K::get[r + 13], s>(buffer[r + 13]);
    buffer[r + 14] = f::addc<K::get[r + 14], s>(buffer[r + 14]);
    buffer[r + 15] = f::addc<K::get[r + 15], s>(buffer[r + 15]);
}

TEMPLATE
INLINE constexpr void CLASS::
schedule_(auto& buffer) NOEXCEPT
{
    prepare<16>(buffer);
    prepare<17>(buffer);
    prepare<18>(buffer);
    prepare<19>(buffer);
    prepare<20>(buffer);
    prepare<21>(buffer);
    prepare<22>(buffer);
    prepare<23>(buffer);
    prepare<24>(buffer);
    prepare<25>(buffer);
    prepare<26>(buffer);
    prepare<27>(buffer);
    prepare<28>(buffer);
    prepare<29>(buffer);
    prepare<30>(buffer);
    prepare<31>(buffer);

    prepare<32>(buffer);
    prepare<33>(buffer);
    prepare<34>(buffer);
    prepare<35>(buffer);
    prepare<36>(buffer);
    prepare<37>(buffer);
    prepare<38>(buffer);
    prepare<39>(buffer);
    prepare<40>(buffer);
    prepare<41>(buffer);
    prepare<42>(buffer);
    prepare<43>(buffer);
    prepare<44>(buffer);
    prepare<45>(buffer);
    prepare<46>(buffer);
    prepare<47>(buffer);

    prepare<48>(buffer);
    prepare<49>(buffer);
    prepare<50>(buffer);
    prepare<51>(buffer);
    prepare<52>(buffer);
    prepare<53>(buffer);
    prepare<54>(buffer);
    prepare<55>(buffer);
    prepare<56>(buffer);
    prepare<57>(buffer);
    prepare<58>(buffer);
    prepare<59>(buffer);
    prepare<60>(buffer);
    prepare<61>(buffer);
    prepare<62>(buffer);
    prepare<63>(buffer);

    if constexpr (SHA::rounds == 80)
    {
        prepare<64>(buffer);
        prepare<65>(buffer);
        prepare<66>(buffer);
        prepare<67>(buffer);
        prepare<68>(buffer);
        prepare<69>(buffer);
        prepare<70>(buffer);
        prepare<71>(buffer);
        prepare<72>(buffer);
        prepare<73>(buffer);
        prepare<74>(buffer);
        prepare<75>(buffer);
        prepare<76>(buffer);
        prepare<77>(buffer);
        prepare<78>(buffer);
        prepare<79>(buffer);
    }

    add_k(buffer);
}

TEMPLATE
constexpr void CLASS::
schedule(auto& buffer) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        schedule_(buffer);
    }
    else if constexpr (vectorization)
    {
        schedule_v(buffer);
    }
    else
    {
        schedule_(buffer);
    }
}

TEMPLATE
INLINE constexpr void CLASS::
summarize(auto& out, const auto& in) NOEXCEPT
{
    constexpr auto s = SHA::word_bits;
    out[0] = f::add<s>(out[0], in[0]);
    out[1] = f::add<s>(out[1], in[1]);
    out[2] = f::add<s>(out[2], in[2]);
    out[3] = f::add<s>(out[3], in[3]);
    out[4] = f::add<s>(out[4], in[4]);

    if constexpr (SHA::strength != 160)
    {
        out[5] = f::add<s>(out[5], in[5]);
        out[6] = f::add<s>(out[6], in[6]);
        out[7] = f::add<s>(out[7], in[7]);
    }
}

TEMPLATE
INLINE constexpr void CLASS::
input(auto& buffer, const auto& state) NOEXCEPT
{
    // This is a double hash optimization.
    if (std::is_constant_evaluated())
    {
        buffer[0] = state[0];
        buffer[1] = state[1];
        buffer[2] = state[2];
        buffer[3] = state[3];
        buffer[4] = state[4];

        if constexpr (SHA::strength != 160)
        {
            buffer[5] = state[5];
            buffer[6] = state[6];
            buffer[7] = state[7];
        }
    }
    else
    {
        using word = array_element<decltype(state)>;
        array_cast<word, SHA::state_words>(buffer) = state;
    }
}

// 5.2 Parsing the Message
// ---------------------------------------------------------------------------
// big-endian I/O is conventional for SHA.

TEMPLATE
INLINE constexpr void CLASS::
input(buffer_t& buffer, const block_t& block) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        constexpr auto size = SHA::word_bytes;
        from_big<0 * size>(buffer.at(0), block);
        from_big<1 * size>(buffer.at(1), block);
        from_big<2 * size>(buffer.at(2), block);
        from_big<3 * size>(buffer.at(3), block);
        from_big<4 * size>(buffer.at(4), block);
        from_big<5 * size>(buffer.at(5), block);
        from_big<6 * size>(buffer.at(6), block);
        from_big<7 * size>(buffer.at(7), block);
        from_big<8 * size>(buffer.at(8), block);
        from_big<9 * size>(buffer.at(9), block);
        from_big<10 * size>(buffer.at(10), block);
        from_big<11 * size>(buffer.at(11), block);
        from_big<12 * size>(buffer.at(12), block);
        from_big<13 * size>(buffer.at(13), block);
        from_big<14 * size>(buffer.at(14), block);
        from_big<15 * size>(buffer.at(15), block);
    }
    else if constexpr (bc::is_little_endian)
    {
        const auto& in = array_cast<word_t>(block);
        buffer[0] = native_from_big_end(in[0]);
        buffer[1] = native_from_big_end(in[1]);
        buffer[2] = native_from_big_end(in[2]);
        buffer[3] = native_from_big_end(in[3]);
        buffer[4] = native_from_big_end(in[4]);
        buffer[5] = native_from_big_end(in[5]);
        buffer[6] = native_from_big_end(in[6]);
        buffer[7] = native_from_big_end(in[7]);
        buffer[8] = native_from_big_end(in[8]);
        buffer[9] = native_from_big_end(in[9]);
        buffer[10] = native_from_big_end(in[10]);
        buffer[11] = native_from_big_end(in[11]);
        buffer[12] = native_from_big_end(in[12]);
        buffer[13] = native_from_big_end(in[13]);
        buffer[14] = native_from_big_end(in[14]);
        buffer[15] = native_from_big_end(in[15]);
    }
    else
    {
        array_cast<word_t, SHA::block_words>(buffer) =
            array_cast<word_t>(block);
    }
}

TEMPLATE
INLINE constexpr void CLASS::
input1(buffer_t& buffer, const half_t& half) NOEXCEPT
{
    using word = array_element<buffer_t>;

    if (std::is_constant_evaluated())
    {
        constexpr auto size = SHA::word_bytes;
        from_big<0 * size>(buffer.at(0), half);
        from_big<1 * size>(buffer.at(1), half);
        from_big<2 * size>(buffer.at(2), half);
        from_big<3 * size>(buffer.at(3), half);
        from_big<4 * size>(buffer.at(4), half);
        from_big<5 * size>(buffer.at(5), half);
        from_big<6 * size>(buffer.at(6), half);
        from_big<7 * size>(buffer.at(7), half);
    }
    else if constexpr (bc::is_little_endian)
    {
        const auto& in = array_cast<word>(half);
        buffer[0] = native_from_big_end(in[0]);
        buffer[1] = native_from_big_end(in[1]);
        buffer[2] = native_from_big_end(in[2]);
        buffer[3] = native_from_big_end(in[3]);
        buffer[4] = native_from_big_end(in[4]);
        buffer[5] = native_from_big_end(in[5]);
        buffer[6] = native_from_big_end(in[6]);
        buffer[7] = native_from_big_end(in[7]);
    }
    else
    {
        array_cast<word, SHA::chunk_words>(buffer) = array_cast<word>(half);
    }
}

TEMPLATE
INLINE constexpr void CLASS::
input2(buffer_t& buffer, const half_t& half) NOEXCEPT
{
    using word = array_element<buffer_t>;

    if (std::is_constant_evaluated())
    {
        constexpr auto size = SHA::word_bytes;
        from_big<0 * size>(buffer.at(8), half);
        from_big<1 * size>(buffer.at(9), half);
        from_big<2 * size>(buffer.at(10), half);
        from_big<3 * size>(buffer.at(11), half);
        from_big<4 * size>(buffer.at(12), half);
        from_big<5 * size>(buffer.at(13), half);
        from_big<6 * size>(buffer.at(14), half);
        from_big<7 * size>(buffer.at(15), half);
    }
    else if constexpr (bc::is_little_endian)
    {
        const auto& in = array_cast<word>(half);
        buffer[8] = native_from_big_end(in[0]);
        buffer[9] = native_from_big_end(in[1]);
        buffer[10] = native_from_big_end(in[2]);
        buffer[11] = native_from_big_end(in[3]);
        buffer[12] = native_from_big_end(in[4]);
        buffer[13] = native_from_big_end(in[5]);
        buffer[14] = native_from_big_end(in[6]);
        buffer[15] = native_from_big_end(in[7]);
    }
    else
    {
        array_cast<word, SHA::chunk_words, SHA::chunk_words>(buffer) =
            array_cast<word>(half);
    }
}

TEMPLATE
INLINE constexpr typename CLASS::digest_t CLASS::
output(const state_t& state) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        digest_t digest{};

        constexpr auto size = SHA::word_bytes;
        to_big<0 * size>(digest, state.at(0));
        to_big<1 * size>(digest, state.at(1));
        to_big<2 * size>(digest, state.at(2));
        to_big<3 * size>(digest, state.at(3));
        to_big<4 * size>(digest, state.at(4));

        if constexpr (SHA::strength != 160)
        {
            to_big<5 * size>(digest, state.at(5));
            to_big<6 * size>(digest, state.at(6));
            to_big<7 * size>(digest, state.at(7));
        }

        return digest;
    }
    else if constexpr (bc::is_little_endian)
    {
        if constexpr (SHA::strength == 160)
        {
            return array_cast<byte_t>(state_t
            {
                native_to_big_end(state[0]),
                native_to_big_end(state[1]),
                native_to_big_end(state[2]),
                native_to_big_end(state[3]),
                native_to_big_end(state[4])
            });
        }
        else
        {
            return array_cast<byte_t>(state_t
            {
                native_to_big_end(state[0]),
                native_to_big_end(state[1]),
                native_to_big_end(state[2]),
                native_to_big_end(state[3]),
                native_to_big_end(state[4]),
                native_to_big_end(state[5]),
                native_to_big_end(state[6]),
                native_to_big_end(state[7])
            });
        }
    }
    else
    {
        return array_cast<byte_t>(state);
    }
}

// 5.1 Padding the Message
// ---------------------------------------------------------------------------

TEMPLATE
template<size_t Blocks>
CONSTEVAL typename CLASS::buffer_t CLASS::
scheduled_pad() NOEXCEPT
{
    // This precomputed padding is limited to one word of counter.
    static_assert(Blocks <= maximum<word_t> / byte_bits);

    // See comments in accumulator regarding padding endianness.
    constexpr auto index = sub1(array_count<words_t>);
    constexpr auto bytes = safe_multiply(Blocks, array_count<block_t>);

    buffer_t out{};
    out.front() = bit_hi<word_t>;
    out.at(index) = possible_narrow_cast<word_t>(to_bits(bytes));
    schedule(out);
    return out;
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
CONSTEVAL typename CLASS::pad_t CLASS::
stream_pad() NOEXCEPT
{
    // See comments in accumulator regarding padding endianness.
    pad_t out{};
    out.front() = bit_hi<word_t>;
    return out;
}

TEMPLATE
template<size_t Blocks>
constexpr void CLASS::
schedule_n(buffer_t& buffer) NOEXCEPT
{
    // Scheduled padding for n whole blocks.
    // This will compile in 4*64 (sha256) or 4*128 (sha512) bytes for each
    // unique size of blocks array hashed by callers (by template expansion).
    // and one for each that is cached for block vectors in schedule_n().
    constexpr auto pad = scheduled_pad<Blocks>();
    buffer = pad;
}

TEMPLATE
constexpr void CLASS::
schedule_n(buffer_t& buffer, size_t blocks) NOEXCEPT
{
    // This optimization is saves ~30% in message scheduling for one out of
    // N blocks: (N + 70%)/(N + 100%). So the proportional benefit decreases
    // exponentially with increasing N. For arbitrary data lengths this will
    // benefit 1/64 hashes on average. All array-sized n-block hashes have
    // precomputed schedules - this benefits only finalized chunk hashing.
    // Testing shows a 5% performance improvement for 128 byte chunk hashes.
    // Accumulator passes all write() of more than one block here.
    if constexpr (caching)
    {
        switch (blocks)
        {
            case 1: schedule_n<1>(buffer); return;
            case 2: schedule_n<2>(buffer); return;
            case 3: schedule_n<3>(buffer); return;
            case 4: schedule_n<4>(buffer); return;
            default:
            {
                pad_n(buffer, blocks);
                schedule(buffer);
                return;
            }
        }
    }
    else
    {
        pad_n(buffer, blocks);
        schedule(buffer);
        return;
    }
}

TEMPLATE
constexpr void CLASS::
schedule_1(buffer_t& buffer) NOEXCEPT
{
    // This ensures single block padding is always prescheduled.
    schedule_n<one>(buffer);
}

TEMPLATE
constexpr void CLASS::
pad_half(buffer_t& buffer) NOEXCEPT
{
    // Pad for any half block, unscheduled buffer.
    constexpr auto pad = chunk_pad();

    if (std::is_constant_evaluated())
    {
        buffer.at(8) = pad.at(0);
        buffer.at(9) = pad.at(1);
        buffer.at(10) = pad.at(2);
        buffer.at(11) = pad.at(3);
        buffer.at(12) = pad.at(4);
        buffer.at(13) = pad.at(5);
        buffer.at(14) = pad.at(6);
        buffer.at(15) = pad.at(7);
    }
    else
    {
        array_cast<word_t, SHA::chunk_words, SHA::chunk_words>(buffer) = pad;
    }
}

TEMPLATE
constexpr void CLASS::
pad_n(buffer_t& buffer, count_t blocks) NOEXCEPT
{
    // Pad any number of whole blocks, unscheduled buffer.
    constexpr auto pad = stream_pad();
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
        array_cast<word_t, array_count<pad_t>>(buffer) = pad;

        // Split count into hi/low words and assign end of padded buffer.
        buffer[14] = hi_word<word_t>(bits);
        buffer[15] = lo_word<word_t>(bits);
    }
}

// Hashing.
// ---------------------------------------------------------------------------
// No hash(state_t) optimizations for sha160 (requires chunk_t/half_t).

TEMPLATE
template <size_t Size>
constexpr typename CLASS::digest_t CLASS::
hash(const ablocks_t<Size>& blocks) NOEXCEPT
{
    buffer_t buffer{};
    auto state = H::get;
    iterate(state, blocks);
    schedule_n<Size>(buffer);
    compress(state, buffer);
    return output(state);
}

TEMPLATE
typename CLASS::digest_t CLASS::
hash(iblocks_t&& blocks) NOEXCEPT
{
    // Save block count, as iterable decrements.
    const auto count = blocks.size();

    buffer_t buffer{};
    auto state = H::get;
    iterate(state, blocks);
    schedule_n(buffer, count);
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
    schedule_1(buffer);
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const state_t& state) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    buffer_t buffer{};
    auto state2 = H::get;
    input(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    compress(state2, buffer);
    return output(state2);
}

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
hash(const half_t& left, const half_t& right) NOEXCEPT
{
    buffer_t buffer{};
    auto state = H::get;
    input1(buffer, left);
    input2(buffer, right);
    schedule(buffer);
    compress(state, buffer);
    schedule_1(buffer);
    compress(state, buffer);
    return output(state);
}

// Double Hashing.
// ---------------------------------------------------------------------------
// No double_hash optimizations for sha160 (requires chunk_t/half_t).

TEMPLATE
template <size_t Size>
constexpr typename CLASS::digest_t CLASS::
double_hash(const ablocks_t<Size>& blocks) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    buffer_t buffer{};
    auto state = H::get;
    iterate(state, blocks);
    schedule_n<Size>(buffer);
    compress(state, buffer);

    // Second hash
    input(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    state = H::get;
    compress(state, buffer);
    return output(state);
}

TEMPLATE
typename CLASS::digest_t CLASS::
double_hash(iblocks_t&& blocks) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    // Save block count, as iterable decrements.
    const auto count = blocks.size();

    buffer_t buffer{};
    auto state = H::get;
    iterate(state, blocks);
    schedule_n(buffer, count);
    compress(state, buffer);

    // Second hash
    input(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    state = H::get;
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
double_hash(const block_t& block) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    buffer_t buffer{};

    auto state = H::get;
    input(buffer, block);
    schedule(buffer);
    compress(state, buffer);
    schedule_1(buffer);
    compress(state, buffer);

    // Second hash
    input(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    state = H::get;
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
double_hash(const half_t& half) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    buffer_t buffer{};
    auto state = H::get;
    input1(buffer, half);
    pad_half(buffer);
    schedule(buffer);
    compress(state, buffer);

    // Second hash
    input(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    state = H::get;
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
double_hash(const half_t& left, const half_t& right) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    buffer_t buffer{};
    auto state = H::get;
    input1(buffer, left);
    input2(buffer, right);
    schedule(buffer);
    compress(state, buffer);
    schedule_1(buffer);
    compress(state, buffer);

    // Second hash
    input(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    state = H::get;
    compress(state, buffer);
    return output(state);
}

// Block iteration.
// ------------------------------------------------------------------------

TEMPLATE
template <size_t Size>
INLINE constexpr void CLASS::
iterate(state_t& state, const ablocks_t<Size>& blocks) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        iterate_(state, blocks);
    }
    else if constexpr (vectorization)
    {
        iterate_v(state, blocks);
    }
    else
    {
        iterate_(state, blocks);
    }
}

TEMPLATE
INLINE void CLASS::
iterate(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    if constexpr (vectorization)
    {
        iterate_v(state, blocks);
    }
    else
    {
        iterate_(state, blocks);
    }
}

TEMPLATE
template <size_t Size>
INLINE constexpr void CLASS::
iterate_(state_t& state, const ablocks_t<Size>& blocks) NOEXCEPT
{
    buffer_t buffer{};
    for (auto& block: blocks)
    {
        input(buffer, block);
        schedule(buffer);
        compress(state, buffer);
    }
}

TEMPLATE
INLINE void CLASS::
iterate_(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    buffer_t buffer{};
    for (auto& block: blocks)
    {
        input(buffer, block);
        schedule(buffer);
        compress(state, buffer);
    }
}


// Merkle Hashing (sha256/512).
// ------------------------------------------------------------------------
// No merkle_hash optimizations for sha160 (double_hash requires half_t).

TEMPLATE
VCONSTEXPR typename CLASS::digest_t CLASS::
merkle_root(digests_t&& digests) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    if (is_zero(digests.size()))
        return {};

    while (!is_one(digests.size()))
    {
        if (is_odd(digests.size()))
            digests.push_back(digests.back());

        merkle_hash(digests);
    }

    return std::move(digests.front());
}

TEMPLATE
VCONSTEXPR typename CLASS::digests_t& CLASS::
merkle_hash(digests_t& digests) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    if (std::is_constant_evaluated())
    {
        merkle_hash_(digests);
    }
    else if constexpr (vectorization)
    {
        merkle_hash_v(digests);
    }
    else
    {
        merkle_hash_(digests);
    }

    return digests;
};

TEMPLATE
VCONSTEXPR void CLASS::
merkle_hash_(digests_t& digests, size_t offset) NOEXCEPT
{
    const auto blocks = to_half(digests.size());
    for (auto i = offset, j = offset * two; i < blocks; ++i, j += two)
        digests[i] = double_hash(digests[j], digests[add1(j)]);

    digests.resize(blocks);
}

// Streaming (unfinalized).
// ---------------------------------------------------------------------------

TEMPLATE
void CLASS::
accumulate(state_t& state, iblocks_t&& blocks) NOEXCEPT
{
    iterate(state, blocks);
}

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
constexpr typename CLASS::digest_t CLASS::
finalize(state_t& state, size_t blocks) NOEXCEPT
{
    buffer_t buffer{};
    schedule_n(buffer, blocks);
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
finalize_double(state_t& state, size_t blocks) NOEXCEPT
{
    // The state out parameter is updated for first hash.
    buffer_t buffer{};
    schedule_n(buffer, blocks);
    compress(state, buffer);

    // Second hash
    input(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    auto state2 = H::get;
    compress(state2, buffer);
    return output(state2);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
normalize(const state_t& state) NOEXCEPT
{
    return output(state);
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

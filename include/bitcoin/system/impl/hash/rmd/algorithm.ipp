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
#ifndef LIBBITCOIN_SYSTEM_HASH_RMD_ALGORITHM_IPP
#define LIBBITCOIN_SYSTEM_HASH_RMD_ALGORITHM_IPP

#include <bit>
#include <iostream>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

// Based on:
// homes.esat.kuleuven.be/~bosselae/ripemd160/pdf/AB-9601/AB-9601.pdf
// [Dobbertin, Bosselaers, Preneel]

namespace libbitcoin {
namespace system {
namespace rmd {

#define TEMPLATE template <typename RMD, if_same<typename RMD::T, rmdh_t> If>
#define CLASS algorithm<RMD, If>

// Bogus warning suggests constexpr when declared consteval.
BC_PUSH_WARNING(USE_CONSTEXPR_FOR_FUNCTION)
BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

// Primitives
// ---------------------------------------------------------------------------

template<auto B>
INLINE constexpr auto rol_(auto a) NOEXCEPT
{
    return rotl<B>(a);
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

INLINE constexpr auto not_(auto a) NOEXCEPT
{
    return ~a;
}

// Functions.
// ----------------------------------------------------------------------------

TEMPLATE
INLINE constexpr auto CLASS::
f0(auto x, auto y, auto z) NOEXCEPT
{
    return xor_(xor_(x, y), z);
}

TEMPLATE
INLINE constexpr auto CLASS::
f1(auto x, auto y, auto z) NOEXCEPT
{
    return or_(and_(x, y), and_(not_(x), z));
}

TEMPLATE
INLINE constexpr auto CLASS::
f2(auto x, auto y, auto z) NOEXCEPT
{
    return xor_(or_(x, not_(y)), z);
}

TEMPLATE
INLINE constexpr auto CLASS::
f3(auto x, auto y, auto z) NOEXCEPT
{
    return or_(and_(x, z), and_(y, not_(z)));
}

TEMPLATE
INLINE constexpr auto CLASS::
f4(auto x, auto y, auto z) NOEXCEPT
{
    return xor_(x, or_(y, not_(z)));
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

    // Select function by column.
    if constexpr (RMD::strength == 128)
    {
        if constexpr (fn == 0u || fn == 7u)
            return &self::template f0<Auto, Auto, Auto>;
        else if constexpr (fn == 1u || fn == 6u)
            return &self::template f1<Auto, Auto, Auto>;
        else if constexpr (fn == 2u || fn == 5u)
            return &self::template f2<Auto, Auto, Auto>;
        else if constexpr (fn == 3u || fn == 4u)
            return &self::template f3<Auto, Auto, Auto>;
    }
    else
    {
        if constexpr (fn == 0u || fn == 9u)
            return &self::template f0<Auto, Auto, Auto>;
        else if constexpr (fn == 1u || fn == 8u)
            return &self::template f1<Auto, Auto, Auto>;
        else if constexpr (fn == 2u || fn == 7u)
            return &self::template f2<Auto, Auto, Auto>;
        else if constexpr (fn == 3u || fn == 6u)
            return &self::template f3<Auto, Auto, Auto>;
        else if constexpr (fn == 4u || fn == 5u)
            return &self::template f4<Auto, Auto, Auto>;
    }
}

TEMPLATE
template<size_t Round>
INLINE constexpr auto CLASS::
round(auto& a, auto b, auto c, auto d, auto x) NOEXCEPT
{
    constexpr auto s = K::rot[Round];
    constexpr auto k = K::get[Round / K::columns];
    constexpr auto f = functor<Round, decltype(a)>();

    a = /*b =*/ rol_<s>(add_<k>(add_(add_(a, f(b, c, d)), x)));
}

TEMPLATE
template<size_t Round>
INLINE constexpr auto CLASS::
round(auto& a, auto b, auto& c, auto d, auto e, auto x) NOEXCEPT
{
    constexpr auto s = K::rot[Round];
    constexpr auto k = K::get[Round / K::columns];
    constexpr auto f = functor<Round, decltype(a)>();

    a = /*b =*/ add_(rol_<s>(add_<k>(add_(add_(a, f(b, c, d)), x))), e);
    c = /*d =*/ rol_<10>(c);
}

TEMPLATE
template<size_t Round>
INLINE constexpr void CLASS::
round(auto& state, const auto& words) NOEXCEPT
{
    if constexpr (RMD::strength == 128)
    {
        round<Round>(
            state[(RMD::rounds + 0 - Round) % RMD::state_words], // b->a
            state[(RMD::rounds + 1 - Round) % RMD::state_words],
            state[(RMD::rounds + 2 - Round) % RMD::state_words],
            state[(RMD::rounds + 3 - Round) % RMD::state_words],
            words[K::word[Round]]);
    }
    else
    {
        round<Round>(
            state[(RMD::rounds + 0 - Round) % RMD::state_words], // b->a
            state[(RMD::rounds + 1 - Round) % RMD::state_words],
            state[(RMD::rounds + 2 - Round) % RMD::state_words], // d->c
            state[(RMD::rounds + 3 - Round) % RMD::state_words],
            state[(RMD::rounds + 4 - Round) % RMD::state_words],
            words[K::word[Round]]);
    }
}

TEMPLATE
constexpr void CLASS::
compress(state_t& state, const words_t& words) NOEXCEPT
{
    constexpr auto offset = to_half(RMD::rounds);

    state_t left{ state };
    state_t right{ state };

    // RMD160:f0/f4, RMD128:f0/f3
    round< 0>(left, words);	round< 0 + offset>(right, words);
    round< 1>(left, words);	round< 1 + offset>(right, words);
    round< 2>(left, words);	round< 2 + offset>(right, words);
    round< 3>(left, words);	round< 3 + offset>(right, words);
    round< 4>(left, words);	round< 4 + offset>(right, words);
    round< 5>(left, words);	round< 5 + offset>(right, words);
    round< 6>(left, words);	round< 6 + offset>(right, words);
    round< 7>(left, words);	round< 7 + offset>(right, words);
    round< 8>(left, words);	round< 8 + offset>(right, words);
    round< 9>(left, words);	round< 9 + offset>(right, words);
    round<10>(left, words);	round<10 + offset>(right, words);
    round<11>(left, words);	round<11 + offset>(right, words);
    round<12>(left, words);	round<12 + offset>(right, words);
    round<13>(left, words);	round<13 + offset>(right, words);
    round<14>(left, words);	round<14 + offset>(right, words);
    round<15>(left, words);	round<15 + offset>(right, words);

    // RMD160:f1/f3, RMD128:f1/f2
    round<16>(left, words);	round<16 + offset>(right, words);
    round<17>(left, words);	round<17 + offset>(right, words);
    round<18>(left, words);	round<18 + offset>(right, words);
    round<19>(left, words);	round<19 + offset>(right, words);
    round<20>(left, words);	round<20 + offset>(right, words);
    round<21>(left, words);	round<21 + offset>(right, words);
    round<22>(left, words);	round<22 + offset>(right, words);
    round<23>(left, words);	round<23 + offset>(right, words);
    round<24>(left, words);	round<24 + offset>(right, words);
    round<25>(left, words);	round<25 + offset>(right, words);
    round<26>(left, words);	round<26 + offset>(right, words);
    round<27>(left, words);	round<27 + offset>(right, words);
    round<28>(left, words);	round<28 + offset>(right, words);
    round<29>(left, words);	round<29 + offset>(right, words);
    round<30>(left, words);	round<30 + offset>(right, words);
    round<31>(left, words);	round<31 + offset>(right, words);

    // RMD160:f2/f2, RMD128:f2/f1
    round<32>(left, words);	round<32 + offset>(right, words);
    round<33>(left, words);	round<33 + offset>(right, words);
    round<34>(left, words);	round<34 + offset>(right, words);
    round<35>(left, words);	round<35 + offset>(right, words);
    round<36>(left, words);	round<36 + offset>(right, words);
    round<37>(left, words);	round<37 + offset>(right, words);
    round<38>(left, words);	round<38 + offset>(right, words);
    round<39>(left, words);	round<39 + offset>(right, words);
    round<40>(left, words);	round<40 + offset>(right, words);
    round<41>(left, words);	round<41 + offset>(right, words);
    round<42>(left, words);	round<42 + offset>(right, words);
    round<43>(left, words);	round<43 + offset>(right, words);
    round<44>(left, words);	round<44 + offset>(right, words);
    round<45>(left, words);	round<45 + offset>(right, words);
    round<46>(left, words);	round<46 + offset>(right, words);
    round<47>(left, words);	round<47 + offset>(right, words);

    // RMD160:f3/f1, RMD128:f3/f0
    round<48>(left, words);	round<48 + offset>(right, words);
    round<49>(left, words);	round<49 + offset>(right, words);
    round<50>(left, words);	round<50 + offset>(right, words);
    round<51>(left, words);	round<51 + offset>(right, words);
    round<52>(left, words);	round<52 + offset>(right, words);
    round<53>(left, words);	round<53 + offset>(right, words);
    round<54>(left, words);	round<54 + offset>(right, words);
    round<55>(left, words);	round<55 + offset>(right, words);
    round<56>(left, words);	round<56 + offset>(right, words);
    round<57>(left, words);	round<57 + offset>(right, words);
    round<58>(left, words);	round<58 + offset>(right, words);
    round<59>(left, words);	round<59 + offset>(right, words);
    round<60>(left, words);	round<60 + offset>(right, words);
    round<61>(left, words);	round<61 + offset>(right, words);
    round<62>(left, words);	round<62 + offset>(right, words);
    round<63>(left, words);	round<63 + offset>(right, words);

    // RMD160:f4/f0
    if constexpr (RMD::strength == 160)
    {
        round<64>(left, words);	round<64 + offset>(right, words);
        round<65>(left, words);	round<65 + offset>(right, words);
        round<66>(left, words);	round<66 + offset>(right, words);
        round<67>(left, words);	round<67 + offset>(right, words);
        round<68>(left, words);	round<68 + offset>(right, words);
        round<69>(left, words);	round<69 + offset>(right, words);
        round<70>(left, words);	round<70 + offset>(right, words);
        round<71>(left, words);	round<71 + offset>(right, words);
        round<72>(left, words);	round<72 + offset>(right, words);
        round<73>(left, words);	round<73 + offset>(right, words);
        round<74>(left, words);	round<74 + offset>(right, words);
        round<75>(left, words);	round<75 + offset>(right, words);
        round<76>(left, words);	round<76 + offset>(right, words);
        round<77>(left, words);	round<77 + offset>(right, words);
        round<78>(left, words);	round<78 + offset>(right, words);
        round<79>(left, words);	round<79 + offset>(right, words);
    }

    summarize(state, left, right);
}

TEMPLATE
INLINE constexpr void CLASS::
summarize(state_t& state, const state_t& batch1,
    const state_t& batch2) NOEXCEPT
{
    if constexpr (RMD::strength == 128)
    {
        const auto state_0_ = state[0];
        state[0] = add_(add_(state[1], batch1[2]), batch2[3]);
        state[1] = add_(add_(state[2], batch1[3]), batch2[0]);
        state[2] = add_(add_(state[3], batch1[0]), batch2[1]);
        state[3] = add_(add_(state_0_, batch1[1]), batch2[2]);
    }
    else
    {
        const auto state_0_ = state[0];
        state[0] = add_(add_(state[1], batch1[2]), batch2[3]);
        state[1] = add_(add_(state[2], batch1[3]), batch2[4]);
        state[2] = add_(add_(state[3], batch1[4]), batch2[0]);
        state[3] = add_(add_(state[4], batch1[0]), batch2[1]);
        state[4] = add_(add_(state_0_, batch1[1]), batch2[2]);
    }
}

// Parsing
// ---------------------------------------------------------------------------
// little-endian I/O is conventional for RMD.

TEMPLATE
INLINE constexpr void CLASS::
input(words_t& words, const block_t& block) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        constexpr auto size = RMD::word_bytes;
        from_little< 0 * size>(words.at( 0), block);
        from_little< 1 * size>(words.at( 1), block);
        from_little< 2 * size>(words.at( 2), block);
        from_little< 3 * size>(words.at( 3), block);
        from_little< 4 * size>(words.at( 4), block);
        from_little< 5 * size>(words.at( 5), block);
        from_little< 6 * size>(words.at( 6), block);
        from_little< 7 * size>(words.at( 7), block);
        from_little< 8 * size>(words.at( 8), block);
        from_little< 9 * size>(words.at( 9), block);
        from_little<10 * size>(words.at(10), block);
        from_little<11 * size>(words.at(11), block);
        from_little<12 * size>(words.at(12), block);
        from_little<13 * size>(words.at(13), block);
        from_little<14 * size>(words.at(14), block);
        from_little<15 * size>(words.at(15), block);
    }
    else if constexpr (bc::is_big_endian)
    {
        const auto& in = array_cast<word_t>(block);
        words[0] = native_from_little_end(in[0]);
        words[1] = native_from_little_end(in[1]);
        words[2] = native_from_little_end(in[2]);
        words[3] = native_from_little_end(in[3]);
        words[4] = native_from_little_end(in[4]);
        words[5] = native_from_little_end(in[5]);
        words[6] = native_from_little_end(in[6]);
        words[7] = native_from_little_end(in[7]);
        words[8] = native_from_little_end(in[8]);
        words[9] = native_from_little_end(in[9]);
        words[10] = native_from_little_end(in[10]);
        words[11] = native_from_little_end(in[11]);
        words[12] = native_from_little_end(in[12]);
        words[13] = native_from_little_end(in[13]);
        words[14] = native_from_little_end(in[14]);
        words[15] = native_from_little_end(in[15]);
    }
    else
    {
        words = array_cast<word_t>(block);
    }
}

TEMPLATE
INLINE constexpr void CLASS::
input(words_t& words, const half_t& half) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        constexpr auto size = RMD::word_bytes;
        from_little<0 * size>(words.at(0), half);
        from_little<1 * size>(words.at(1), half);
        from_little<2 * size>(words.at(2), half);
        from_little<3 * size>(words.at(3), half);
        from_little<4 * size>(words.at(4), half);
        from_little<5 * size>(words.at(5), half);
        from_little<6 * size>(words.at(6), half);
        from_little<7 * size>(words.at(7), half);
    }
    else if constexpr (bc::is_big_endian)
    {
        const auto& in = array_cast<word_t>(half);
        words[0] = native_from_little_end(in[0]);
        words[1] = native_from_little_end(in[1]);
        words[2] = native_from_little_end(in[2]);
        words[3] = native_from_little_end(in[3]);
        words[4] = native_from_little_end(in[4]);
        words[5] = native_from_little_end(in[5]);
        words[6] = native_from_little_end(in[6]);
        words[7] = native_from_little_end(in[7]);
    }
    else
    {
        array_cast<word_t, array_count<chunk_t>>(words) =
            array_cast<word_t>(half);
    }
}

TEMPLATE
INLINE constexpr typename CLASS::digest_t CLASS::
output(const state_t& state) NOEXCEPT
{
    digest_t digest{};

    if (std::is_constant_evaluated())
    {
        constexpr auto size = RMD::word_bytes;
        to_little<0 * size>(digest, state.at(0));
        to_little<1 * size>(digest, state.at(1));
        to_little<2 * size>(digest, state.at(2));
        to_little<3 * size>(digest, state.at(3));

        if constexpr (RMD::strength == 160)
        {
            to_little<4 * size>(digest, state.at(4));
        }
    }
    else if constexpr (bc::is_big_endian)
    {
        auto& out = array_cast<word_t>(digest);
        out[0] = native_to_little_end(state[0]);
        out[1] = native_to_little_end(state[1]);
        out[2] = native_to_little_end(state[2]);
        out[3] = native_to_little_end(state[3]);

        if constexpr (RMD::strength == 160)
        {
            out[4] = native_to_little_end(state[4]);
        }
    }
    else
    {
        array_cast<word_t>(digest) = state;
    }

    return digest;
}

// Padding
// ---------------------------------------------------------------------------

TEMPLATE
CONSTEVAL typename CLASS::words_t CLASS::
block_pad() NOEXCEPT
{
    // See comments in accumulator regarding padding endianness.
    constexpr auto bytes = possible_narrow_cast<word_t>(array_count<block_t>);
    constexpr auto hi = sub1(array_count<words_t>);
    constexpr auto lo = sub1(hi);

    words_t words{};
    words.front() = bit_hi<byte_t>;
    words[lo] = to_bits(bytes);
    return words;
}

TEMPLATE
CONSTEVAL typename CLASS::chunk_t CLASS::
chunk_pad() NOEXCEPT
{
    // See comments in accumulator regarding padding endianness.
    constexpr auto bytes = possible_narrow_cast<word_t>(array_count<half_t>);
    constexpr auto hi = sub1(array_count<chunk_t>);
    constexpr auto lo = sub1(hi);

    chunk_t words{};
    words.front() = bit_hi<byte_t>;
    words[lo] = to_bits(bytes);
    return words;
}

TEMPLATE
CONSTEVAL typename CLASS::pad_t CLASS::
stream_pad() NOEXCEPT
{
    // See comments in accumulator regarding padding endianness.
    pad_t words{};
    words.front() = bit_hi<byte_t>;
    return words;
}

TEMPLATE
constexpr void CLASS::
pad_one(words_t& words) NOEXCEPT
{
    // Pad a single whole block with pre-prepared buffer.
    constexpr auto pad = block_pad();
    words = pad;
}

TEMPLATE
constexpr void CLASS::
pad_half(words_t& words) NOEXCEPT
{
    // Pad a half block.
    constexpr auto pad = chunk_pad();

    if (std::is_constant_evaluated())
    {
        words.at(8)  = pad.at(0);
        words.at(9)  = pad.at(1);
        words.at(10) = pad.at(2);
        words.at(11) = pad.at(3);
        words.at(12) = pad.at(4);
        words.at(13) = pad.at(5);
        words.at(14) = pad.at(6);
        words.at(15) = pad.at(7);
    }
    else
    {
        constexpr auto size = array_count<chunk_t>;
        array_cast<word_t, size, size>(words) = pad;
    }
}

TEMPLATE
constexpr void CLASS::
pad_n(words_t& words, count_t blocks) NOEXCEPT
{
    // Pad any number of whole blocks.
    constexpr auto pad = stream_pad();
    const auto bits = to_bits(blocks * array_count<block_t>);

    if (std::is_constant_evaluated())
    {
        words.at(0)  = pad.at(0);
        words.at(1)  = pad.at(1);
        words.at(2)  = pad.at(2);
        words.at(3)  = pad.at(3);
        words.at(4)  = pad.at(4);
        words.at(5)  = pad.at(5);
        words.at(6)  = pad.at(6);
        words.at(7)  = pad.at(7);
        words.at(8)  = pad.at(8);
        words.at(9)  = pad.at(9);
        words.at(10) = pad.at(10);
        words.at(11) = pad.at(11);
        words.at(12) = pad.at(12);
        words.at(13) = pad.at(13);
        words.at(14) = lo_word<word_t>(bits);
        words.at(15) = hi_word<word_t>(bits);
    }
    else
    {
        array_cast<word_t, array_count<pad_t>>(words) = pad;

        // Split count into hi/low words and assign end of padded buffer (LE).
        words[14] = lo_word<word_t>(bits);
        words[15] = hi_word<word_t>(bits);
    }
}

// Finalized hash functions.
// ---------------------------------------------------------------------------

TEMPLATE
typename CLASS::digest_t CLASS::
hash(const iblocks_t& blocks) NOEXCEPT
{
    words_t words{};
    auto state = H::get;

    for (auto& block: blocks)
    {
        input(words, block);
        compress(state, words);
    }

    pad_n(words, blocks.size());
    compress(state, words);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const vblocks_t& blocks) NOEXCEPT
{
    words_t words{};
    auto state = H::get;

    for (auto& block: blocks)
    {
        input(words, block);
        compress(state, words);
    }

    pad_n(words, blocks.size());
    compress(state, words);
    return output(state);
}

TEMPLATE
template <size_t Size>
constexpr typename CLASS::digest_t CLASS::
hash(const ablocks_t<Size>& blocks) NOEXCEPT
{
    words_t words{};
    auto state = H::get;

    for (auto& block: blocks)
    {
        input(words, block);
        compress(state, words);
    }

    pad_n(words, blocks.size());
    compress(state, words);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const block_t& block) NOEXCEPT
{
    words_t words{};
    auto state = H::get;
    input(words, block);
    compress(state, words);
    pad_one(words);
    compress(state, words);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const half_t& half) NOEXCEPT
{
    words_t words{};
    auto state = H::get;
    input(words, half);
    pad_half(words);
    compress(state, words);
    return output(state);
}

// Streaming hash functions and finalizers.
// ---------------------------------------------------------------------------

TEMPLATE
void CLASS::
accumulate(state_t& state, const iblocks_t& blocks) NOEXCEPT
{
    words_t words{};
    for (auto& block: blocks)
    {
        input(words, block);
        compress(state, words);
    }
}

TEMPLATE
VCONSTEXPR void CLASS::
accumulate(state_t& state, const vblocks_t& blocks) NOEXCEPT
{
    words_t words{};
    for (auto& block: blocks)
    {
        input(words, block);
        compress(state, words);
    }
}

TEMPLATE
constexpr void CLASS::
accumulate(state_t& state, const block_t& block) NOEXCEPT
{
    words_t words{};
    input(words, block);
    compress(state, words);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
finalize(state_t& state, size_t blocks) NOEXCEPT
{
    words_t words{};
    pad_n(words, blocks);
    compress(state, words);
    return output(state);
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

#undef CLASS
#undef TEMPLATE

} // namespace rmd
} // namespace system
} // namespace libbitcoin

#endif

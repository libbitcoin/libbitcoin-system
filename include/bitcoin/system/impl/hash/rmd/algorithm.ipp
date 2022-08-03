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
#include <bitcoin/system/radix/radix.hpp>

namespace libbitcoin {
namespace system {
namespace rmd {

// Implementation based on:
// All aspects of RIPEMD are supported within the implmentation.
// homes.esat.kuleuven.be/~bosselae/ripemd160/pdf/AB-9601/AB-9601.pdf

#define TEMPLATE template <typename RMD, bool Concurrent, \
    if_same<typename RMD::T, rmdh_t> If>
#define CLASS algorithm<RMD, Concurrent, If>

// Bogus warning suggests constexpr when declared consteval.
BC_PUSH_WARNING(USE_CONSTEXPR_FOR_FUNCTION)
BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

// private
// ----------------------------------------------------------------------------

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
    constexpr auto hi = sub1(array_count<chunk_t>);
    constexpr auto lo = sub1(hi);

    chunk_t words{};
    words.front() = bit_hi<byte_t>;
    words[lo] = to_bits(bytes);
    return words;
}

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
CONSTEVAL typename CLASS::blocks_pad_t CLASS::
blocks_pad() NOEXCEPT
{
    // See comments in accumulator regarding padding endianness.
    blocks_pad_t words{};
    words.front() = bit_hi<byte_t>;
    return words;
}

// Primitives
// ---------------------------------------------------------------------------

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
    if constexpr (RMD::rounds == 128)
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
    if constexpr (RMD::rounds == 128)
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
template<bool First>
constexpr void CLASS::
batch(state_t& state, const words_t& words) NOEXCEPT
{
    // Order of execution is arbitrary.
    constexpr auto offset = First ? zero : to_half(RMD::rounds);

    // Pointer degradation here (optimization), use auto typing in round().
    auto pwords = words.data();
    auto pstate = state.data();

    // RMD256:f0/f4, RMD128:f0/f3
    round<offset +  0>(pstate, pwords);
    round<offset +  1>(pstate, pwords);
    round<offset +  2>(pstate, pwords);
    round<offset +  3>(pstate, pwords);
    round<offset +  4>(pstate, pwords);
    round<offset +  5>(pstate, pwords);
    round<offset +  6>(pstate, pwords);
    round<offset +  7>(pstate, pwords);
    round<offset +  8>(pstate, pwords);
    round<offset +  9>(pstate, pwords);
    round<offset + 10>(pstate, pwords);
    round<offset + 11>(pstate, pwords);
    round<offset + 12>(pstate, pwords);
    round<offset + 13>(pstate, pwords);
    round<offset + 14>(pstate, pwords);
    round<offset + 15>(pstate, pwords);

    // RMD256:f1/f3, RMD128:f1/f2
    round<offset + 16>(pstate, pwords);
    round<offset + 17>(pstate, pwords);
    round<offset + 18>(pstate, pwords);
    round<offset + 19>(pstate, pwords);
    round<offset + 20>(pstate, pwords);
    round<offset + 21>(pstate, pwords);
    round<offset + 22>(pstate, pwords);
    round<offset + 23>(pstate, pwords);
    round<offset + 24>(pstate, pwords);
    round<offset + 25>(pstate, pwords);
    round<offset + 26>(pstate, pwords);
    round<offset + 27>(pstate, pwords);
    round<offset + 28>(pstate, pwords);
    round<offset + 29>(pstate, pwords);
    round<offset + 30>(pstate, pwords);
    round<offset + 31>(pstate, pwords);

    // RMD256:f2/f2, RMD128:f2/f1
    round<offset + 32>(pstate, pwords);
    round<offset + 33>(pstate, pwords);
    round<offset + 34>(pstate, pwords);
    round<offset + 35>(pstate, pwords);
    round<offset + 36>(pstate, pwords);
    round<offset + 37>(pstate, pwords);
    round<offset + 38>(pstate, pwords);
    round<offset + 39>(pstate, pwords);
    round<offset + 40>(pstate, pwords);
    round<offset + 41>(pstate, pwords);
    round<offset + 42>(pstate, pwords);
    round<offset + 43>(pstate, pwords);
    round<offset + 44>(pstate, pwords);
    round<offset + 45>(pstate, pwords);
    round<offset + 46>(pstate, pwords);
    round<offset + 47>(pstate, pwords);

    // RMD256:f3/f1, RMD128:f3/f0
    round<offset + 48>(pstate, pwords);
    round<offset + 49>(pstate, pwords);
    round<offset + 50>(pstate, pwords);
    round<offset + 51>(pstate, pwords);
    round<offset + 52>(pstate, pwords);
    round<offset + 53>(pstate, pwords);
    round<offset + 54>(pstate, pwords);
    round<offset + 55>(pstate, pwords);
    round<offset + 56>(pstate, pwords);
    round<offset + 57>(pstate, pwords);
    round<offset + 58>(pstate, pwords);
    round<offset + 59>(pstate, pwords);
    round<offset + 60>(pstate, pwords);

    // msvc++ (/O2/Ob2/Ot) inlining stops here without __forceinline.
    round<offset + 61>(pstate, pwords);
    round<offset + 62>(pstate, pwords);
    round<offset + 63>(pstate, pwords);

    // RMD256:f4/f0
    if constexpr (RMD::rounds == 160)
    {
        round<offset + 64>(pstate, pwords);
        round<offset + 65>(pstate, pwords);
        round<offset + 66>(pstate, pwords);
        round<offset + 67>(pstate, pwords);
        round<offset + 68>(pstate, pwords);
        round<offset + 69>(pstate, pwords);
        round<offset + 70>(pstate, pwords);
        round<offset + 71>(pstate, pwords);
        round<offset + 72>(pstate, pwords);
        round<offset + 73>(pstate, pwords);
        round<offset + 74>(pstate, pwords);
        round<offset + 75>(pstate, pwords);
        round<offset + 76>(pstate, pwords);
        round<offset + 77>(pstate, pwords);
        round<offset + 78>(pstate, pwords);
        round<offset + 79>(pstate, pwords);
    }
}

TEMPLATE
constexpr void CLASS::
rounding(state_t& state, const words_t& words) NOEXCEPT
{
    // Two copies of state (required by RMD) are saved to jobs.
    std_array<std::pair<bool, state_t>, two> jobs
    {
        std::make_pair(true, state),
        std::make_pair(false, state)
    };

    if (std::is_constant_evaluated())
    {
        batch<true>(jobs.front().second, words);
        batch<false>(jobs.back().second, words);
    }
    else
    {
        // words is const, jobs are independent and unsequenced.
        std_for_each(concurrency(), jobs.begin(), jobs.end(),
            [&words](auto& job) NOEXCEPT
            {
                if (job.first)
                    batch<true>(job.second, words);
                else
                    batch<false>(job.second, words);
            });
    }

    // Add state from both jobs to original state.
    summarize(state, jobs.front().second, jobs.back().second);
}

TEMPLATE
constexpr void CLASS::
summarize(state_t& state, const state_t& batch1,
    const state_t& batch2) NOEXCEPT
{
    if constexpr (RMD::rounds == 128)
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

// Padding
// ---------------------------------------------------------------------------

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
    constexpr auto pad = blocks_pad();
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
        array_cast<word_t, array_count<blocks_pad_t>>(words) = pad;

        // Split count into hi/low words and assign end of padded buffer (LE).
        words[14] = lo_word<word_t>(bits);
        words[15] = hi_word<word_t>(bits);
    }
}

// Parsing
// ---------------------------------------------------------------------------
// little-endian I/O is conventional for RMD.

TEMPLATE
constexpr void CLASS::
input(words_t& words, const block_t& block) NOEXCEPT
{
    constexpr auto size = RMD::word_bytes;

    if (std::is_constant_evaluated())
    {
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
    else
    {
        from_little_endians(words, array_cast<word_t>(block));
    }
}

TEMPLATE
constexpr void CLASS::
input(words_t& words, const half_t& half) NOEXCEPT
{
    constexpr auto size = RMD::word_bytes;

    if (std::is_constant_evaluated())
    {
        from_little<0 * size>(words.at(0), half);
        from_little<1 * size>(words.at(1), half);
        from_little<2 * size>(words.at(2), half);
        from_little<3 * size>(words.at(3), half);
        from_little<4 * size>(words.at(4), half);
        from_little<5 * size>(words.at(5), half);
        from_little<6 * size>(words.at(6), half);
        from_little<7 * size>(words.at(7), half);
    }
    else
    {
        auto& to = array_cast<word_t, array_count<chunk_t>>(words);
        from_little_endians(to, array_cast<word_t>(half));
    }
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
output(const state_t& state) NOEXCEPT
{
    constexpr auto size = RMD::word_bytes;

    if (std::is_constant_evaluated())
    {
        digest_t digest{};

        to_little<0 * size>(digest, state.at(0));
        to_little<1 * size>(digest, state.at(1));
        to_little<2 * size>(digest, state.at(2));
        to_little<3 * size>(digest, state.at(3));

        if constexpr (K::strength == 160)
        {
            to_little<4 * size>(digest, state.at(4));
        }

        return digest;
    }
    else
    {
        return array_cast<byte_t>(to_little_endians(state));
    }
}

// Streaming single hash functions.
// ---------------------------------------------------------------------------

TEMPLATE
constexpr void CLASS::
accumulate(state_t& state, const block_t& block) NOEXCEPT
{
    words_t space{};
    input(space, block);
    rounding(state, space);
}

TEMPLATE
VCONSTEXPR void CLASS::
accumulate(state_t& state, const blocks_t& blocks) NOEXCEPT
{
    words_t space{};

    for (auto& block: blocks)
    {
        input(space, block);
        rounding(state, space);
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

// Finalized single hash functions.
// ---------------------------------------------------------------------------

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const half_t& half) NOEXCEPT
{
    words_t space{};
    auto state = H::get;

    input(space, half);
    pad_half(space);
    rounding(state, space);
    return finalize(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const block_t& block) NOEXCEPT
{
    words_t space{};
    auto state = H::get;

    input(space, block);
    rounding(state, space);

    pad_one(space);
    rounding(state, space);
    return finalize(state);
}

TEMPLATE
VCONSTEXPR typename CLASS::digest_t CLASS::
hash(const blocks_t& blocks) NOEXCEPT
{
    words_t space{};
    auto state = H::get;

    for (auto& block: blocks)
    {
        input(space, block);
        rounding(state, space);
    }

    pad_n(space, blocks.size());
    rounding(state, space);
    return finalize(state);
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

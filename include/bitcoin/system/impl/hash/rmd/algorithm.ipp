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
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace rmd {

// Implementation based on:
// homes.esat.kuleuven.be/~bosselae/ripemd/rmd128.txt
// homes.esat.kuleuven.be/~bosselae/ripemd/rmd256.txt
// homes.esat.kuleuven.be/~bosselae/ripemd/rmd160.txt
// homes.esat.kuleuven.be/~bosselae/ripemd/rmd320.txt
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
    constexpr auto bytes = possible_narrow_cast<word_t>(array_count<half_t>);

    chunk_t out{};
    out.front() = bit_hi<word_t>;
    out.back() = to_bits(bytes);
    return out;
}

TEMPLATE
CONSTEVAL typename CLASS::words_t CLASS::
block_pad() NOEXCEPT
{
    constexpr auto bytes = possible_narrow_cast<word_t>(array_count<block_t>);

    words_t out{};
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

// Functions.
// ----------------------------------------------------------------------------

TEMPLATE
constexpr auto CLASS::
f0(auto x, auto y, auto z) NOEXCEPT
{
    return x ^ y ^ z;
}

TEMPLATE
constexpr auto CLASS::
f1(auto x, auto y, auto z) NOEXCEPT
{
    return (x & y) | (~x & z);
}

TEMPLATE
constexpr auto CLASS::
f2(auto x, auto y, auto z) NOEXCEPT
{
    return (x | ~y) ^ z;
}

TEMPLATE
constexpr auto CLASS::
f3(auto x, auto y, auto z) NOEXCEPT
{
    return (x & z) | (y & ~z);
}

TEMPLATE
constexpr auto CLASS::
f4(auto x, auto y, auto z) NOEXCEPT
{
    return x ^ (y | ~z);
}

// Rounds
// ---------------------------------------------------------------------------

TEMPLATE
template<size_t Round>
CONSTEVAL auto CLASS::
functor() NOEXCEPT
{
    using self = CLASS;
    constexpr auto fn = Round / RMD::K::columns;

    if constexpr (RMD::rounds == 128)
    {
        if constexpr (fn == 0u || fn == 7u)
            return &self::template f0<uint32_t, uint32_t, uint32_t>;
        else if constexpr (fn == 1u || fn == 6u)
            return &self::template f1<uint32_t, uint32_t, uint32_t>;
        else if constexpr (fn == 2u || fn == 5u)
            return &self::template f2<uint32_t, uint32_t, uint32_t>;
        else if constexpr (fn == 3u || fn == 4u)
            return &self::template f3<uint32_t, uint32_t, uint32_t>;
    }
    else
    {
        if constexpr (fn == 0u || fn == 9u)
            return &self::template f0<uint32_t, uint32_t, uint32_t>;
        else if constexpr (fn == 1u || fn == 8u)
            return &self::template f1<uint32_t, uint32_t, uint32_t>;
        else if constexpr (fn == 2u || fn == 7u)
            return &self::template f2<uint32_t, uint32_t, uint32_t>;
        else if constexpr (fn == 3u || fn == 6u)
            return &self::template f3<uint32_t, uint32_t, uint32_t>;
        else if constexpr (fn == 4u || fn == 5u)
            return &self::template f4<uint32_t, uint32_t, uint32_t>;
    }
}

TEMPLATE
template<size_t Round>
FORCE_INLINE constexpr auto CLASS::
round(auto a, auto& b, auto c, auto d, auto& e, auto w) NOEXCEPT
{
    constexpr auto r = K::rot[Round];
    constexpr auto k = K::get[Round / RMD::K::columns];
    constexpr auto f = functor<Round>();

    // TODO: std::rotl(c,10) is not in RMD128.
    e = /*a =*/ std::rotl(a + f(b, c, d) + k + w, r) + e;
    b = /*c =*/ std::rotl(c, 10);
}

TEMPLATE
template<size_t Round>
FORCE_INLINE constexpr void CLASS::
round(auto& out, const auto& in) NOEXCEPT
{
    constexpr auto words  = RMD::state_words;
    constexpr auto rounds = RMD::rounds;

    round<Round>(
        out[(rounds + 0 - Round) % words],
        out[(rounds + 1 - Round) % words], // c->b
        out[(rounds + 2 - Round) % words],
        out[(rounds + 3 - Round) % words],
        out[(rounds + 4 - Round) % words], // a->e
        in[K::word[Round]]);
}

TEMPLATE
template<bool First>
constexpr void CLASS::
batch(state_t& out, const words_t& in) NOEXCEPT
{
    // Order of execution is arbitrary.
    constexpr auto offset = First ? zero : to_half(RMD::rounds);

    auto pin = in.data();
    auto pout = out.data();

    // RMD256:f0/f4, RMD128:f0/f3
    round<offset + 0>(pout, pin);
    round<offset + 1>(pout, pin);
    round<offset + 2>(pout, pin);
    round<offset + 3>(pout, pin);
    round<offset + 4>(pout, pin);
    round<offset + 5>(pout, pin);
    round<offset + 6>(pout, pin);
    round<offset + 7>(pout, pin);
    round<offset + 8>(pout, pin);
    round<offset + 9>(pout, pin);
    round<offset + 10>(pout, pin);
    round<offset + 11>(pout, pin);
    round<offset + 12>(pout, pin);
    round<offset + 13>(pout, pin);
    round<offset + 14>(pout, pin);
    round<offset + 15>(pout, pin);

    // RMD256:f1/f3, RMD128:f1/f2
    round<offset + 16>(pout, pin);
    round<offset + 17>(pout, pin);
    round<offset + 18>(pout, pin);
    round<offset + 19>(pout, pin);
    round<offset + 20>(pout, pin);
    round<offset + 21>(pout, pin);
    round<offset + 22>(pout, pin);
    round<offset + 23>(pout, pin);
    round<offset + 24>(pout, pin);
    round<offset + 25>(pout, pin);
    round<offset + 26>(pout, pin);
    round<offset + 27>(pout, pin);
    round<offset + 28>(pout, pin);
    round<offset + 29>(pout, pin);
    round<offset + 30>(pout, pin);
    round<offset + 31>(pout, pin);

    // RMD256:f2/f2, RMD128:f2/f1
    round<offset + 32>(pout, pin);
    round<offset + 33>(pout, pin);
    round<offset + 34>(pout, pin);
    round<offset + 35>(pout, pin);
    round<offset + 36>(pout, pin);
    round<offset + 37>(pout, pin);
    round<offset + 38>(pout, pin);
    round<offset + 39>(pout, pin);
    round<offset + 40>(pout, pin);
    round<offset + 41>(pout, pin);
    round<offset + 42>(pout, pin);
    round<offset + 43>(pout, pin);
    round<offset + 44>(pout, pin);
    round<offset + 45>(pout, pin);
    round<offset + 46>(pout, pin);
    round<offset + 47>(pout, pin);

    // RMD256:f3/f1, RMD128:f3/f0
    round<offset + 48>(pout, pin);
    round<offset + 49>(pout, pin);
    round<offset + 50>(pout, pin);
    round<offset + 51>(pout, pin);
    round<offset + 52>(pout, pin);
    round<offset + 53>(pout, pin);
    round<offset + 54>(pout, pin);
    round<offset + 55>(pout, pin);
    round<offset + 56>(pout, pin);
    round<offset + 57>(pout, pin);
    round<offset + 58>(pout, pin);
    round<offset + 59>(pout, pin);
    round<offset + 60>(pout, pin);

    // msvc++ (/O2/Ob2/Ot) inlining stops here without __forceinline.
    round<offset + 61>(pout, pin);
    round<offset + 62>(pout, pin);
    round<offset + 63>(pout, pin);

    // RMD256:f4/f0
    if constexpr (RMD::rounds == 160)
    {
        round<offset + 64>(pout, pin);
        round<offset + 65>(pout, pin);
        round<offset + 66>(pout, pin);
        round<offset + 67>(pout, pin);
        round<offset + 68>(pout, pin);
        round<offset + 69>(pout, pin);
        round<offset + 70>(pout, pin);
        round<offset + 71>(pout, pin);
        round<offset + 72>(pout, pin);
        round<offset + 73>(pout, pin);
        round<offset + 74>(pout, pin);
        round<offset + 75>(pout, pin);
        round<offset + 76>(pout, pin);
        round<offset + 77>(pout, pin);
        round<offset + 78>(pout, pin);
        round<offset + 79>(pout, pin);
    }
}

TEMPLATE
constexpr void CLASS::
rounding(state_t& state, const words_t& buffer) NOEXCEPT
{
    // Two copies of state (required by RMD) are saved to jobs.
    std_array<std::pair<bool, state_t>, two> jobs
    {
        std::make_pair(true, state),
        std::make_pair(false, state)
    };

    if (std::is_constant_evaluated())
    {
        batch<true>(jobs.front().second, buffer);
        batch<false>(jobs.back().second, buffer);
    }
    else
    {
        // buffer is const, jobs are independent and unsequenced.
        std_for_each(concurrency(), jobs.begin(), jobs.end(),
            [&buffer](auto& job) NOEXCEPT
            {
                if (job.first)
                    batch<true>(job.second, buffer);
                else
                    batch<false>(job.second, buffer);
            });
    }

    // Add state from both jobs to original state.
    summarize(state, jobs.front().second, jobs.back().second);
}

TEMPLATE
constexpr void CLASS::
summarize(state_t& out, const state_t& in1, const state_t& in2) NOEXCEPT
{
    out[0] += (in1[0] + in2[0]);
    out[1] += (in1[1] + in2[1]);
    out[2] += (in1[2] + in2[2]);
    out[3] += (in1[3] + in2[3]);
    out[4] += (in1[4] + in2[4]);
}

// Padding
// ---------------------------------------------------------------------------

TEMPLATE
constexpr void CLASS::
pad_one(words_t& out) NOEXCEPT
{
    // Pad a single whole block with pre-prepared buffer.
    constexpr auto pad = block_pad();

    out = pad;
}

TEMPLATE
constexpr void CLASS::
pad_half(words_t& out) NOEXCEPT
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
pad_n(words_t& out, count_t blocks) NOEXCEPT
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

// Parsing
// ---------------------------------------------------------------------------
// little-endian I/O is conventional for RMD.

TEMPLATE
constexpr void CLASS::
input(words_t& out, const block_t& in) NOEXCEPT
{
    constexpr auto size = RMD::word_bytes;

    if (std::is_constant_evaluated())
    {
        from_little< 0 * size>(out[ 0], in);
        from_little< 1 * size>(out[ 1], in);
        from_little< 2 * size>(out[ 2], in);
        from_little< 3 * size>(out[ 3], in);
        from_little< 4 * size>(out[ 4], in);
        from_little< 5 * size>(out[ 5], in);
        from_little< 6 * size>(out[ 6], in);
        from_little< 7 * size>(out[ 7], in);
        from_little< 8 * size>(out[ 8], in);
        from_little< 9 * size>(out[ 9], in);
        from_little<10 * size>(out[10], in);
        from_little<11 * size>(out[11], in);
        from_little<12 * size>(out[12], in);
        from_little<13 * size>(out[13], in);
        from_little<14 * size>(out[14], in);
        from_little<15 * size>(out[15], in);
    }
    else
    {
        from_little_endians(out, array_cast<word_t>(in));
    }
}

TEMPLATE
constexpr void CLASS::
input(words_t& out, const half_t& in) NOEXCEPT
{
    constexpr auto size = RMD::word_bytes;

    if (std::is_constant_evaluated())
    {
        from_little<0 * size>(out[0], in);
        from_little<1 * size>(out[1], in);
        from_little<2 * size>(out[2], in);
        from_little<3 * size>(out[3], in);
        from_little<4 * size>(out[4], in);
        from_little<5 * size>(out[5], in);
        from_little<6 * size>(out[6], in);
        from_little<7 * size>(out[7], in);
    }
    else
    {
        auto& to = narrow_array_cast<word_t, array_count<chunk_t>>(out);
        from_little_endians(to, array_cast<word_t>(in));
    }
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
output(const state_t& in) NOEXCEPT
{
    constexpr auto size = RMD::word_bytes;

    if (std::is_constant_evaluated())
    {
        digest_t out{};

        if constexpr (RMD::K::strength == 128)
        {
            to_little<0 * size>(out, in[0]);
            to_little<1 * size>(out, in[1]);
            to_little<2 * size>(out, in[2]);
            to_little<3 * size>(out, in[3]);
        }
        else
        {
            to_little<0 * size>(out, in[0]);
            to_little<1 * size>(out, in[1]);
            to_little<2 * size>(out, in[2]);
            to_little<3 * size>(out, in[3]);
            to_little<4 * size>(out, in[4]);
        }

        return out;
    }
    else
    {
        return array_cast<byte_t>(to_little_endians(in));
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

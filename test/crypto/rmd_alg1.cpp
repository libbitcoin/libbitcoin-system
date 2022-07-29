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
#include "rmd_alg1.hpp"
#include "hash_acc1.hpp"

// Implementation based on:
// homes.esat.kuleuven.be/~bosselae/ripemd/rmd128.txt
// homes.esat.kuleuven.be/~bosselae/ripemd/rmd256.txt
// homes.esat.kuleuven.be/~bosselae/ripemd/rmd160.txt
// homes.esat.kuleuven.be/~bosselae/ripemd/rmd320.txt
// homes.esat.kuleuven.be/~bosselae/ripemd160/pdf/AB-9601/AB-9601.pdf
    
BOOST_AUTO_TEST_SUITE(rmd_tests)

#define TEMPLATE template <typename RMD, bool Concurrent>
#define CLASS algorithm<RMD, Concurrent>

// Bogus warning suggests constexpr when declared consteval.
BC_PUSH_WARNING(USE_CONSTEXPR_FOR_FUNCTION)
BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

// private
// ----------------------------------------------------------------------------

TEMPLATE
CONSTEVAL auto CLASS::
concurrency() NOEXCEPT
{
    if constexpr (Concurrent)
        return std::execution::par_unseq;
    else
        return std::execution::seq;
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
    const auto t = std::rotl(a + f(b, c, d) + k + w, r) + e;
    b = /*c =*/ std::rotl(c, 10);
    e = /*a =*/ t;
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
        from_little_endians(to, array_cast<RMD::word_t>(in));
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
constexpr typename CLASS::digest_t CLASS::
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

#ifndef TESTS

using rmd_128 = algorithm<h128<>, false>;
using rmd_160 = algorithm<h160<>, false>;

template <typename Algorithm>
typename sha_tests::accumulator<Algorithm>::digest_t accumulated(
    const data_slice& data) NOEXCEPT
{
    sha_tests::accumulator<Algorithm> accumulator{};
    accumulator.write(data.size(), data.data());
    return accumulator.flush();
}

template <typename Algorithm>
typename sha_tests::accumulator<Algorithm>::digest_t accumulated(
    const data_slice& left, const data_slice& right) NOEXCEPT
{
    sha_tests::accumulator<Algorithm> accumulator{};
    accumulator.write(left.size(), left.data());
    accumulator.write(right.size(), right.data());
    return accumulator.flush();
}

// sha128
// ----------------------------------------------------------------------------

// sha160
// ----------------------------------------------------------------------------

constexpr auto half160 = rmd_160::half_t{};
constexpr auto full160 = rmd_160::block_t{};
constexpr auto pair160 = std_array<uint8_t, array_count<rmd_160::block_t> * two>{};
constexpr auto expected_half160 = base16_array("d1a70126ff7a149ca6f9b638db084480440ff842");
constexpr auto expected_full160 = base16_array("9b8ccc2f374ae313a914763cc9cdfb47bfe1c229");
constexpr auto expected_pair160 = base16_array("4300a157335cb7c9fc9423e011d7dd51090d093f");

// algorithm

BOOST_AUTO_TEST_CASE(algorithm__hash_half160__null_hash__expected)
{
    static_assert(rmd_160::hash(half160) != expected_half160);
    BOOST_CHECK_NE(rmd_160::hash(half160), expected_half160);
    BOOST_CHECK_EQUAL(system::ripemd160_hash(half160), expected_half160);
}

BOOST_AUTO_TEST_CASE(algorithm__hash_full160__null_hash__expected)
{
    static_assert(rmd_160::hash(full160) != expected_full160);
    BOOST_CHECK_NE(rmd_160::hash(full160), expected_full160);
    BOOST_CHECK_EQUAL(system::ripemd160_hash(full160), expected_full160);
}

BOOST_AUTO_TEST_CASE(algorithm__hash_blocks160__null_hash__expected)
{
    const std_vector<cref<rmd_160::block_t>> pair
    {
        std::cref(full160), std::cref(full160)
    };

    BOOST_CHECK_NE(rmd_160::hash(pair), expected_pair160);
    BOOST_CHECK_EQUAL(system::ripemd160_hash(pair160), expected_pair160);
}

// accumulator

BOOST_AUTO_TEST_CASE(accumulator__hash_half160__null_hashes__expected)
{
    BOOST_CHECK_NE(accumulated<rmd_160>(half160), expected_half160);
    BOOST_CHECK_EQUAL(system::ripemd160_hash(half160), expected_half160);
}

BOOST_AUTO_TEST_CASE(accumulator__hash_one160__null_hashes__expected)
{
    BOOST_CHECK_NE(accumulated<rmd_160>(full160), expected_full160);
    BOOST_CHECK_EQUAL(system::ripemd160_hash(full160), expected_full160);
}

BOOST_AUTO_TEST_CASE(accumulator__hash_two160__null_hashes__expected)
{
    BOOST_CHECK_NE(accumulated<rmd_160>(full160, full160), expected_pair160);
    BOOST_CHECK_EQUAL(system::ripemd160_hash(pair160), expected_pair160);
}

#endif // TESTS

#ifndef TESTS

// Verify indirection.

// algorithm<rmd128>
// ----------------------------------------------------------------------------

using k_128 = algorithm<rmd128>::K;

struct accessor128
  : public algorithm<rmd128>
{
    template<size_t Round>
    static constexpr auto functor()
    {
        return algorithm<rmd128>::functor<Round>();
    }

    static constexpr auto get_f0()
    {
        return &algorithm<rmd128>::f0<uint32_t, uint32_t, uint32_t>;
    }

    static constexpr auto get_f1()
    {
        return &algorithm<rmd128>::f1<uint32_t, uint32_t, uint32_t>;
    }

    static constexpr auto get_f2()
    {
        return &algorithm<rmd128>::f2<uint32_t, uint32_t, uint32_t>;
    }

    static constexpr auto get_f3()
    {
        return &algorithm<rmd128>::f3<uint32_t, uint32_t, uint32_t>;
    }
};

// Selection on round/columns (16) boundary yields the following indexes.
// batch1
static_assert( 0 / k_128::columns == 0);
static_assert(15 / k_128::columns == 0);
static_assert(16 / k_128::columns == 1);
static_assert(31 / k_128::columns == 1);
static_assert(32 / k_128::columns == 2);
static_assert(47 / k_128::columns == 2);
static_assert(48 / k_128::columns == 3);
static_assert(63 / k_128::columns == 3);
// batch2
static_assert( 64 / k_128::columns == 4);
static_assert( 79 / k_128::columns == 4);
static_assert( 80 / k_128::columns == 5);
static_assert( 95 / k_128::columns == 5);
static_assert( 96 / k_128::columns == 6);
static_assert(111 / k_128::columns == 6);
static_assert(112 / k_128::columns == 7);
static_assert(127 / k_128::columns == 7);

// Word index selection is on round boundary.
// batch1
static_assert(k_128::word[ 0] == 0);
static_assert(k_128::word[15] == 15);
static_assert(k_128::word[16] == 7);
static_assert(k_128::word[31] == 8);
static_assert(k_128::word[32] == 3);
static_assert(k_128::word[47] == 12);
static_assert(k_128::word[48] == 1);
static_assert(k_128::word[63] == 2);
// batch2
static_assert(k_128::word[ 64] == 5);
static_assert(k_128::word[ 79] == 12);
static_assert(k_128::word[ 80] == 6);
static_assert(k_128::word[ 95] == 2);
static_assert(k_128::word[ 96] == 15);
static_assert(k_128::word[111] == 13);
static_assert(k_128::word[112] == 8);
static_assert(k_128::word[127] == 14);

// Rotation selection is on round boundary.
// batch1
static_assert(k_128::rot[ 0] == 11);
static_assert(k_128::rot[15] == 8);
static_assert(k_128::rot[16] == 7);
static_assert(k_128::rot[31] == 12);
static_assert(k_128::rot[32] == 11);
static_assert(k_128::rot[47] == 5);
static_assert(k_128::rot[48] == 11);
static_assert(k_128::rot[63] == 12);
// batch2
static_assert(k_128::rot[ 64] == 8);
static_assert(k_128::rot[ 79] == 6);
static_assert(k_128::rot[ 80] == 9);
static_assert(k_128::rot[ 95] == 11);
static_assert(k_128::rot[ 96] == 9);
static_assert(k_128::rot[111] == 5);
static_assert(k_128::rot[112] == 15);
static_assert(k_128::rot[127] == 8);

// Constant selection is on round/columns boundary.
// batch1
static_assert(k_128::get[ 0 / k_128::columns] == 0x00000000);
static_assert(k_128::get[15 / k_128::columns] == 0x00000000);
static_assert(k_128::get[16 / k_128::columns] == 0x5a827999);
static_assert(k_128::get[31 / k_128::columns] == 0x5a827999);
static_assert(k_128::get[32 / k_128::columns] == 0x6ed9eba1);
static_assert(k_128::get[47 / k_128::columns] == 0x6ed9eba1);
static_assert(k_128::get[48 / k_128::columns] == 0x8f1bbcdc);
static_assert(k_128::get[63 / k_128::columns] == 0x8f1bbcdc);
// batch2
static_assert(k_128::get[ 64 / k_128::columns] == 0x50a28be6);
static_assert(k_128::get[ 79 / k_128::columns] == 0x50a28be6);
static_assert(k_128::get[ 80 / k_128::columns] == 0x5c4dd124);
static_assert(k_128::get[ 95 / k_128::columns] == 0x5c4dd124);
static_assert(k_128::get[ 96 / k_128::columns] == 0x6d703ef3);
static_assert(k_128::get[111 / k_128::columns] == 0x6d703ef3);
static_assert(k_128::get[112 / k_128::columns] == 0x00000000);
static_assert(k_128::get[127 / k_128::columns] == 0x00000000);

// Functor selection is on round/columns boundary.
// batch1
static_assert(accessor128::functor< 0>() == accessor128::get_f0());
static_assert(accessor128::functor<15>() == accessor128::get_f0());
static_assert(accessor128::functor<16>() == accessor128::get_f1());
static_assert(accessor128::functor<31>() == accessor128::get_f1());
static_assert(accessor128::functor<32>() == accessor128::get_f2());
static_assert(accessor128::functor<47>() == accessor128::get_f2());
static_assert(accessor128::functor<48>() == accessor128::get_f3());
static_assert(accessor128::functor<63>() == accessor128::get_f3());
// batch1
static_assert(accessor128::functor< 64>() == accessor128::get_f3());
static_assert(accessor128::functor< 79>() == accessor128::get_f3());
static_assert(accessor128::functor< 80>() == accessor128::get_f2());
static_assert(accessor128::functor< 95>() == accessor128::get_f2());
static_assert(accessor128::functor< 96>() == accessor128::get_f1());
static_assert(accessor128::functor<111>() == accessor128::get_f1());
static_assert(accessor128::functor<112>() == accessor128::get_f0());
static_assert(accessor128::functor<127>() == accessor128::get_f0());

// algorithm<rmd160>
// ----------------------------------------------------------------------------

using k_160 = algorithm<rmd160, true>::K;

struct accessor160
  : public algorithm<rmd160>
{
    template<size_t Round>
    static constexpr auto functor()
    {
        return algorithm<rmd160>::functor<Round>();
    }

    static constexpr auto get_f0()
    {
        return &algorithm<rmd160>::f0<uint32_t, uint32_t, uint32_t>;
    }

    static constexpr auto get_f1()
    {
        return &algorithm<rmd160>::f1<uint32_t, uint32_t, uint32_t>;
    }

    static constexpr auto get_f2()
    {
        return &algorithm<rmd160>::f2<uint32_t, uint32_t, uint32_t>;
    }

    static constexpr auto get_f3()
    {
        return &algorithm<rmd160>::f3<uint32_t, uint32_t, uint32_t>;
    }

    static constexpr auto get_f4()
    {
        return &algorithm<rmd160>::f4<uint32_t, uint32_t, uint32_t>;
    }
};

// Selection on round/columns (16) boundary yields the following indexes.
// batch1
static_assert( 0 / k_160::columns == 0);
static_assert(15 / k_160::columns == 0);
static_assert(16 / k_160::columns == 1);
static_assert(31 / k_160::columns == 1);
static_assert(32 / k_160::columns == 2);
static_assert(47 / k_160::columns == 2);
static_assert(48 / k_160::columns == 3);
static_assert(63 / k_160::columns == 3);
static_assert(64 / k_160::columns == 4);
static_assert(79 / k_160::columns == 4);
// batch2
static_assert( 80 / k_160::columns == 5);
static_assert( 95 / k_160::columns == 5);
static_assert( 96 / k_160::columns == 6);
static_assert(111 / k_160::columns == 6);
static_assert(112 / k_160::columns == 7);
static_assert(127 / k_160::columns == 7);
static_assert(128 / k_160::columns == 8);
static_assert(143 / k_160::columns == 8);
static_assert(144 / k_160::columns == 9);
static_assert(159 / k_160::columns == 9);

// Word index selection is on round boundary.
// batch1
static_assert(k_160::word[ 0] == 0);
static_assert(k_160::word[15] == 15);
static_assert(k_160::word[16] == 7);
static_assert(k_160::word[31] == 8);
static_assert(k_160::word[32] == 3);
static_assert(k_160::word[47] == 12);
static_assert(k_160::word[48] == 1);
static_assert(k_160::word[63] == 2);
static_assert(k_160::word[64] == 4);
static_assert(k_160::word[79] == 13);
// batch2
static_assert(k_160::word[ 80] == 5);
static_assert(k_160::word[ 95] == 12);
static_assert(k_160::word[ 96] == 6);
static_assert(k_160::word[111] == 2);
static_assert(k_160::word[112] == 15);
static_assert(k_160::word[127] == 13);
static_assert(k_160::word[128] == 8);
static_assert(k_160::word[143] == 14);
static_assert(k_160::word[144] == 12);
static_assert(k_160::word[159] == 11);

// Rotation selection is on round boundary.
// batch1
static_assert(k_160::rot[ 0] == 11);
static_assert(k_160::rot[15] == 8);
static_assert(k_160::rot[16] == 7);
static_assert(k_160::rot[31] == 12);
static_assert(k_160::rot[32] == 11);
static_assert(k_160::rot[47] == 5);
static_assert(k_160::rot[48] == 11);
static_assert(k_160::rot[63] == 12);
static_assert(k_160::rot[64] == 9);
static_assert(k_160::rot[79] == 6);
// batch2
static_assert(k_160::rot[ 80] == 8);
static_assert(k_160::rot[ 95] == 6);
static_assert(k_160::rot[ 96] == 9);
static_assert(k_160::rot[111] == 11);
static_assert(k_160::rot[112] == 9);
static_assert(k_160::rot[127] == 5);
static_assert(k_160::rot[128] == 15);
static_assert(k_160::rot[143] == 8);
static_assert(k_160::rot[144] == 8);
static_assert(k_160::rot[159] == 11);

// Constant selection is on round/columns boundary.
// batch1
static_assert(k_160::get[ 0 / k_160::columns] == 0x00000000);
static_assert(k_160::get[15 / k_160::columns] == 0x00000000);
static_assert(k_160::get[16 / k_160::columns] == 0x5a827999);
static_assert(k_160::get[31 / k_160::columns] == 0x5a827999);
static_assert(k_160::get[32 / k_160::columns] == 0x6ed9eba1);
static_assert(k_160::get[47 / k_160::columns] == 0x6ed9eba1);
static_assert(k_160::get[48 / k_160::columns] == 0x8f1bbcdc);
static_assert(k_160::get[63 / k_160::columns] == 0x8f1bbcdc);
static_assert(k_160::get[64 / k_160::columns] == 0xa953fd4e);
static_assert(k_160::get[79 / k_160::columns] == 0xa953fd4e);
// batch2
static_assert(k_160::get[ 80 / k_160::columns] == 0x50a28be6);
static_assert(k_160::get[ 95 / k_160::columns] == 0x50a28be6);
static_assert(k_160::get[ 96 / k_160::columns] == 0x5c4dd124);
static_assert(k_160::get[111 / k_160::columns] == 0x5c4dd124);
static_assert(k_160::get[112 / k_160::columns] == 0x6d703ef3);
static_assert(k_160::get[127 / k_160::columns] == 0x6d703ef3);
static_assert(k_160::get[128 / k_160::columns] == 0x7a6d76e9);
static_assert(k_160::get[143 / k_160::columns] == 0x7a6d76e9);
static_assert(k_160::get[144 / k_160::columns] == 0x00000000);
static_assert(k_160::get[159 / k_160::columns] == 0x00000000);

// Functor selection is on round/columns boundary.
// batch1
static_assert(accessor160::functor< 0>() == accessor160::get_f0());
static_assert(accessor160::functor<15>() == accessor160::get_f0());
static_assert(accessor160::functor<16>() == accessor160::get_f1());
static_assert(accessor160::functor<31>() == accessor160::get_f1());
static_assert(accessor160::functor<32>() == accessor160::get_f2());
static_assert(accessor160::functor<47>() == accessor160::get_f2());
static_assert(accessor160::functor<48>() == accessor160::get_f3());
static_assert(accessor160::functor<63>() == accessor160::get_f3());
static_assert(accessor160::functor<64>() == accessor160::get_f4());
static_assert(accessor160::functor<79>() == accessor160::get_f4());
// batch2
static_assert(accessor160::functor< 80>() == accessor160::get_f4());
static_assert(accessor160::functor< 95>() == accessor160::get_f4());
static_assert(accessor160::functor< 96>() == accessor160::get_f3());
static_assert(accessor160::functor<111>() == accessor160::get_f3());
static_assert(accessor160::functor<112>() == accessor160::get_f2());
static_assert(accessor160::functor<127>() == accessor160::get_f2());
static_assert(accessor160::functor<128>() == accessor160::get_f1());
static_assert(accessor160::functor<143>() == accessor160::get_f1());
static_assert(accessor160::functor<144>() == accessor160::get_f0());
static_assert(accessor160::functor<159>() == accessor160::get_f0());
#endif // TESTS

BOOST_AUTO_TEST_SUITE_END()

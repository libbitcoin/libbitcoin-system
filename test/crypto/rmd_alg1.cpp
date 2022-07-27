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

// Functions.
// ----------------------------------------------------------------------------

template <typename RMD, bool Concurrent>
constexpr auto algorithm<RMD, Concurrent>::
f0(auto x, auto y, auto z) NOEXCEPT
{
    return x ^ y ^ z;
}

template <typename RMD, bool Concurrent>
constexpr auto algorithm<RMD, Concurrent>::
f1(auto x, auto y, auto z) NOEXCEPT
{
    return (x & y) | (~x & z);
}

template <typename RMD, bool Concurrent>
constexpr auto algorithm<RMD, Concurrent>::
f2(auto x, auto y, auto z) NOEXCEPT
{
    return (x | ~y) ^ z;
}

template <typename RMD, bool Concurrent>
constexpr auto algorithm<RMD, Concurrent>::
f3(auto x, auto y, auto z) NOEXCEPT
{
    return (x & z) | (y & ~z);
}

template <typename RMD, bool Concurrent>
constexpr auto algorithm<RMD, Concurrent>::
f4(auto x, auto y, auto z) NOEXCEPT
{
    return x ^ (y | ~z);
}

// Rounds
// ---------------------------------------------------------------------------

template <typename RMD, bool Concurrent>
template<size_t Round>
constexpr auto algorithm<RMD, Concurrent>::
functor() NOEXCEPT
{
    using self = algorithm<RMD>;
    constexpr auto fn = (Round / RMD::K::columns) % RMD::K::columns;

    // RMD is limited to uint32_t.
    if      constexpr (fn == 0u || fn == 9u)
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

template <typename RMD, bool Concurrent>
template<size_t Round>
constexpr auto algorithm<RMD, Concurrent>::
round(auto a, auto& b, auto c, auto d, auto& e, auto w) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
    constexpr auto f = functor<Round>();
    constexpr auto r = K::rot[Round];
    constexpr auto k = K::get[Round / RMD::K::columns];
    BC_POP_WARNING()
    BC_POP_WARNING()

    // TODO: std::rotl(c,10) is not in RMD128.
    e = /*a =*/ std::rotl(a + f(b, c, d) + w + k, r) + e;
    b = /*c =*/ std::rotl(c, 10);
}

template <typename RMD, bool Concurrent>
template<size_t Round>
constexpr void algorithm<RMD, Concurrent>::
round(auto& out, const auto& in) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
    round<Round>(
        out[(RMD::rounds + 0 - Round) % RMD::state_words],
        out[(RMD::rounds + 1 - Round) % RMD::state_words], // c->b
        out[(RMD::rounds + 2 - Round) % RMD::state_words],
        out[(RMD::rounds + 3 - Round) % RMD::state_words],
        out[(RMD::rounds + 4 - Round) % RMD::state_words], // a->e
        in[K::word[Round]]);
    BC_POP_WARNING()
    BC_POP_WARNING()
}

template <typename RMD, bool Concurrent>
template<bool First>
constexpr void algorithm<RMD, Concurrent>::
batch(state_t& out, const words_t& in) NOEXCEPT
{
    // Order of execution is arbitrary.
    constexpr auto offset = First ? zero : to_half(RMD::rounds);

    BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
    auto pout = out.data();
    auto pin = in.data();
    BC_POP_WARNING()

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
    round<offset + 61>(pout, pin);
    round<offset + 62>(pout, pin);
    round<offset + 63>(pout, pin);

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

template <typename RMD, bool Concurrent>
constexpr void algorithm<RMD, Concurrent>::
summarize(state_t& out, const state_t& in1, const state_t& in2) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    out[0] += (in1[0] + in2[0]);
    out[1] += (in1[1] + in2[1]);
    out[2] += (in1[2] + in2[2]);
    out[3] += (in1[3] + in2[3]);
    out[4] += (in1[4] + in2[4]);
    BC_POP_WARNING()
}

template <typename RMD, bool Concurrent>
constexpr void algorithm<RMD, Concurrent>::
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
    else if constexpr (!Concurrent)
    {
        batch<true>(jobs.front().second, buffer);
        batch<false>(jobs.back().second, buffer);
    }
    else
    {
        // buffer is const, jobs are independent and unsequenced.
        std::for_each(std::execution::par_unseq, jobs.begin(), jobs.end(),
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

// Padding
// ---------------------------------------------------------------------------

template <typename RMD, bool Concurrent>
constexpr void algorithm<RMD, Concurrent>::
pad_one(words_t& out) NOEXCEPT
{
    // Pad a single whole block.
    out = RMD::pad::block;
}

template <typename RMD, bool Concurrent>
constexpr void algorithm<RMD, Concurrent>::
pad_half(words_t& out) NOEXCEPT
{
    // Pad a half block.
    if (std::is_constant_evaluated())
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        out[ 8] = RMD::pad::chunk[0];
        out[ 9] = RMD::pad::chunk[1];
        out[10] = RMD::pad::chunk[2];
        out[11] = RMD::pad::chunk[3];
        out[12] = RMD::pad::chunk[4];
        out[13] = RMD::pad::chunk[5];
        out[14] = RMD::pad::chunk[6];
        out[15] = RMD::pad::chunk[7];
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
        to = RMD::pad::chunk;
    }
}

template <typename RMD, bool Concurrent>
constexpr void algorithm<RMD, Concurrent>::
pad_count(words_t& out, count_t blocks) NOEXCEPT
{
    // Pad any number of whole blocks.
    if (std::is_constant_evaluated())
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        out[ 0] = RMD::pad::block[0];
        out[ 1] = RMD::pad::block[1];
        out[ 2] = RMD::pad::block[2];
        out[ 3] = RMD::pad::block[3];
        out[ 4] = RMD::pad::block[4];
        out[ 5] = RMD::pad::block[5];
        out[ 6] = RMD::pad::block[6];
        out[ 7] = RMD::pad::block[7];
        out[ 8] = RMD::pad::block[8];
        out[ 9] = RMD::pad::block[9];
        out[10] = RMD::pad::block[10];
        out[11] = RMD::pad::block[11];
        out[12] = RMD::pad::block[12];
        out[13] = RMD::pad::block[13];
        ////out[14] = RMD::pad::block[14]; // overwritten by count
        ////out[15] = RMD::pad::block[15]; // overwritten by count
        BC_POP_WARNING()
    }
    else
    {
        // Array cast is a runtime no-op.
        // RMD::pad::block is counted, but count will be overwritten below.
        auto& to = narrow_array_cast<word_t, array_count<words_t>>(out);
        to = RMD::pad::block;
    }

    // Copy in the streamed bit count.
    const auto bits = to_bits(blocks * block_bytes);

    // count_t is twice the size of word_t, so split to hi/low words.
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    out[14] = hi_word<word_t>(bits);
    out[15] = lo_word<word_t>(bits);
    BC_POP_WARNING()
}

// Parsing
// ---------------------------------------------------------------------------

template <typename RMD, bool Concurrent>
constexpr void algorithm<RMD, Concurrent>::
little_one(words_t& out, const block_t& in) NOEXCEPT
{
    // little-endian I/O is conventional.
    if (std::is_constant_evaluated())
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        from_big< 0 * RMD::word_bytes>(out[ 0], in);
        from_big< 1 * RMD::word_bytes>(out[ 1], in);
        from_big< 2 * RMD::word_bytes>(out[ 2], in);
        from_big< 3 * RMD::word_bytes>(out[ 3], in);
        from_big< 4 * RMD::word_bytes>(out[ 4], in);
        from_big< 5 * RMD::word_bytes>(out[ 5], in);
        from_big< 6 * RMD::word_bytes>(out[ 6], in);
        from_big< 7 * RMD::word_bytes>(out[ 7], in);
        from_big< 8 * RMD::word_bytes>(out[ 8], in);
        from_big< 9 * RMD::word_bytes>(out[ 9], in);
        from_big<10 * RMD::word_bytes>(out[10], in);
        from_big<11 * RMD::word_bytes>(out[11], in);
        from_big<12 * RMD::word_bytes>(out[12], in);
        from_big<13 * RMD::word_bytes>(out[13], in);
        from_big<14 * RMD::word_bytes>(out[14], in);
        from_big<15 * RMD::word_bytes>(out[15], in);
        BC_POP_WARNING()
    }
    else
    {
        // Array cast is a runtime no-op.
        // FBE is 1 (or 0) opcode per element and loop-unrolled.
        auto& from = array_cast<word_t>(in);
        auto& to = narrow_array_cast<word_t, RMD::block_words>(out);
        from_little_endians(to, from);
    }
}

template <typename RMD, bool Concurrent>
constexpr void algorithm<RMD, Concurrent>::
little_half(words_t& out, const half_t& in) NOEXCEPT
{
    // little-endian I/O is conventional.
    if (std::is_constant_evaluated())
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        from_big<0 * RMD::word_bytes>(out[0], in);
        from_big<1 * RMD::word_bytes>(out[1], in);
        from_big<2 * RMD::word_bytes>(out[2], in);
        from_big<3 * RMD::word_bytes>(out[3], in);
        from_big<4 * RMD::word_bytes>(out[4], in);
        from_big<5 * RMD::word_bytes>(out[5], in);
        from_big<6 * RMD::word_bytes>(out[6], in);
        from_big<7 * RMD::word_bytes>(out[7], in);
        BC_POP_WARNING()
    }
    else
    {
        // Array cast is a runtime no-op.
        // FBE is 1 (or 0) opcode per element and loop-unrolled.
        auto& from = array_cast<RMD::word_t>(in);
        auto& to = narrow_array_cast<word_t, array_count<chunk_t>>(out);
        from_little_endians(to, from);
    }
}

template <typename RMD, bool Concurrent>
constexpr typename algorithm<RMD, Concurrent>::digest_t
algorithm<RMD, Concurrent>::
little_state(const state_t& in) NOEXCEPT
{
    // little-endian I/O is conventional.
    if (std::is_constant_evaluated())
    {
        digest_t out{};
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        if constexpr (RMD::K::strength == 128)
        {
            to_big<0 * RMD::word_bytes>(out, in[0]);
            to_big<1 * RMD::word_bytes>(out, in[1]);
            to_big<2 * RMD::word_bytes>(out, in[2]);
            to_big<3 * RMD::word_bytes>(out, in[3]);
        }
        else
        {
            to_big<0 * RMD::word_bytes>(out, in[0]);
            to_big<1 * RMD::word_bytes>(out, in[1]);
            to_big<2 * RMD::word_bytes>(out, in[2]);
            to_big<3 * RMD::word_bytes>(out, in[3]);
            to_big<4 * RMD::word_bytes>(out, in[4]);
        }
        BC_POP_WARNING()
        return out;
    }
    else
    {
        // Array cast is a runtime no-op.
        // TBE is 1 (or 0) opcode per element and loop-unrolled.
        return array_cast<byte_t>(to_little_endians(in));
    }
}

// Streaming single hash functions.
// ---------------------------------------------------------------------------

template <typename RMD, bool Concurrent>
constexpr void algorithm<RMD, Concurrent>::
accumulate(state_t& state, const block_t& block) NOEXCEPT
{
    words_t space{};
    const state_t start{ state };
    little_one(space, block);
    rounding(state, space);
}

template <typename RMD, bool Concurrent>
VCONSTEXPR void algorithm<RMD, Concurrent>::
accumulate(state_t& state, const blocks_t& blocks) NOEXCEPT
{
    words_t space{};

    for (auto& block: blocks)
    {
        little_one(space, block);
        rounding(state, space);
    }
}

template <typename RMD, bool Concurrent>
constexpr typename algorithm<RMD, Concurrent>::digest_t
algorithm<RMD, Concurrent>::
finalize(const state_t& state) NOEXCEPT
{
    return little_state(state);
}

// Finalized single hash functions.
// ---------------------------------------------------------------------------

template <typename RMD, bool Concurrent>
constexpr typename algorithm<RMD, Concurrent>::digest_t
algorithm<RMD, Concurrent>::
hash(const half_t& half) NOEXCEPT
{
    // process 1/2 data block with 1/2 pad block (pre-sized/endianed).
    words_t space{};
    little_half(space, half);

    auto state = RMD::H::get;
    pad_half(space);
    rounding(state, space);

    return finalize(state);
}

template <typename RMD, bool Concurrent>
constexpr typename algorithm<RMD, Concurrent>::digest_t
algorithm<RMD, Concurrent>::
hash(const block_t& block) NOEXCEPT
{
    words_t space{};

    // process 1 block in (avoid accumulate to reuse buffer).
    auto state = RMD::H::get;
    little_one(space, block);
    rounding(state, space);
    summarize(state, RMD::H::get);

    // full pad block for 1 block (pre-sized/endianed/expanded).
    const auto start = state;
    pad_one(space);
    rounding(state, space);
    summarize(state, start);

    return finalize(state);
}

template <typename RMD, bool Concurrent>
constexpr typename algorithm<RMD, Concurrent>::digest_t
algorithm<RMD, Concurrent>::
hash(const blocks_t& blocks) NOEXCEPT
{
    words_t space{};
    auto state = RMD::H::get;
    auto start = RMD::H::get;

    // process N blocks (inlined accumulator).
    for (auto& block : blocks)
    {
        little_one(space, block);
        rounding(state, space);
        summarize(state, start);
        start = state;
    }

    // full pad block for N blocks (pre-endianed, size added).
    pad_count(space, blocks.size());
    rounding(state, space);
    summarize(state, start);

    return finalize(state);
}

////template <typename RMD>
////constexpr void algorithm<RMD>::
////accumulate(state_t& state, const block_t& block) NOEXCEPT
////{
////    // TODO: array_cast not constexpr.
////    rounding(state, to_little_endians(array_cast<word_t>(block)));
////}
////
////template <typename RMD>
////constexpr typename algorithm<RMD>::digest_t algorithm<RMD>::
////finalize(const state_t& state) NOEXCEPT
////{
////    // TODO: array_cast not constexpr.
////    return array_cast<byte_t>(to_little_endians(state));
////}

// TESTS
// ============================================================================

// sha160
// ----------------------------------------------------------------------------

using rmd_160 = algorithm<h160<>>;
using rmd_160_accumulator = sha_tests::accumulator<rmd_160>;

short_hash rmd160_hash(const data_slice& data) NOEXCEPT
{
    short_hash hash{};
    rmd_160_accumulator accumulator;
    accumulator.write(data.size(), data.data());
    accumulator.flush(hash.data());
    return hash;
}

short_hash rmd160_hash(const data_slice& left, const data_slice& right) NOEXCEPT
{
    short_hash hash{};
    rmd_160_accumulator accumulator;
    accumulator.write(left.size(), left.data());
    accumulator.write(right.size(), right.data());
    accumulator.flush(hash.data());
    return hash;
}

constexpr auto half160 = rmd_160::half_t{};
constexpr auto full160 = rmd_160::block_t{};
constexpr auto twin160 = std_array<uint8_t, rmd_160::block_bytes * 2>{};
constexpr auto expected_half160 = base16_array("de8a847bff8c343d69b853a215e6ee775ef2ef96");
constexpr auto expected_full160 = base16_array("c8d7d0ef0eedfa82d2ea1aa592845b9a6d4b02b7");
constexpr auto expected_twin160 = base16_array("0ae4f711ef5d6e9d26c611fd2c8c8ac45ecbf9e7");

// algorithm

////BOOST_AUTO_TEST_CASE(algorithm__half160_block__null_hash__expected)
////{
////    static_assert(rmd_160::hash(half160) == expected_half160);
////    BOOST_CHECK_EQUAL(rmd_160::hash(half160), expected_half160);
////    BOOST_CHECK_EQUAL(system::ripemd160_hash(half160), expected_half160);
////}

#ifndef TESTS
// Verify indirection.

// algorithm<rmd128>
// ----------------------------------------------------------------------------

using rmd_128 = algorithm<rmd128>;

static_assert(rmd_128::K::word[  0] == 0);
static_assert(rmd_128::K::word[ 64] == 5);
static_assert(rmd_128::K::word[127] == 14);

static_assert(rmd_128::K::rot[  0] == 11);
static_assert(rmd_128::K::rot[ 64] == 8);
static_assert(rmd_128::K::rot[127] == 8);

static_assert( 0 / rmd_128::K::columns == 0);
static_assert(15 / rmd_128::K::columns == 0);
static_assert(16 / rmd_128::K::columns == 1);
static_assert(31 / rmd_128::K::columns == 1);
static_assert(32 / rmd_128::K::columns == 2);
static_assert(47 / rmd_128::K::columns == 2);
static_assert(48 / rmd_128::K::columns == 3);
static_assert(63 / rmd_128::K::columns == 3);

static_assert(64 / rmd_128::K::columns == 4);
static_assert(79 / rmd_128::K::columns == 4);
static_assert( 80 / rmd_128::K::columns == 5);
static_assert( 95 / rmd_128::K::columns == 5);
static_assert( 96 / rmd_128::K::columns == 6);
static_assert(111 / rmd_128::K::columns == 6);
static_assert(112 / rmd_128::K::columns == 7);
static_assert(127 / rmd_128::K::columns == 7);

static_assert(rmd_128::K::get[ 0 / rmd_128::K::columns] == 0x00000000);
static_assert(rmd_128::K::get[15 / rmd_128::K::columns] == 0x00000000);
static_assert(rmd_128::K::get[16 / rmd_128::K::columns] == 0x5a827999);
static_assert(rmd_128::K::get[31 / rmd_128::K::columns] == 0x5a827999);
static_assert(rmd_128::K::get[32 / rmd_128::K::columns] == 0x6ed9eba1);
static_assert(rmd_128::K::get[47 / rmd_128::K::columns] == 0x6ed9eba1);
static_assert(rmd_128::K::get[48 / rmd_128::K::columns] == 0x8f1bbcdc);
static_assert(rmd_128::K::get[63 / rmd_128::K::columns] == 0x8f1bbcdc);

static_assert(rmd_128::K::get[ 64 / rmd_128::K::columns] == 0x50a28be6);
static_assert(rmd_128::K::get[ 79 / rmd_128::K::columns] == 0x50a28be6);
static_assert(rmd_128::K::get[ 80 / rmd_128::K::columns] == 0x5c4dd124);
static_assert(rmd_128::K::get[ 95 / rmd_128::K::columns] == 0x5c4dd124);
static_assert(rmd_128::K::get[ 96 / rmd_128::K::columns] == 0x6d703ef3);
static_assert(rmd_128::K::get[111 / rmd_128::K::columns] == 0x6d703ef3);
static_assert(rmd_128::K::get[112 / rmd_128::K::columns] == 0x00000000);
static_assert(rmd_128::K::get[127 / rmd_128::K::columns] == 0x00000000);

static_assert((  0 / rmd_128::K::columns) % rmd_128::K::columns == 0);
static_assert(( 15 / rmd_128::K::columns) % rmd_128::K::columns == 0);
static_assert(( 16 / rmd_128::K::columns) % rmd_128::K::columns == 1);
static_assert(( 31 / rmd_128::K::columns) % rmd_128::K::columns == 1);
static_assert(( 32 / rmd_128::K::columns) % rmd_128::K::columns == 2);
static_assert(( 47 / rmd_128::K::columns) % rmd_128::K::columns == 2);
static_assert(( 48 / rmd_128::K::columns) % rmd_128::K::columns == 3);
static_assert(( 63 / rmd_128::K::columns) % rmd_128::K::columns == 3);

static_assert(( 64 / rmd_128::K::columns) % rmd_128::K::columns == 4);
static_assert(( 79 / rmd_128::K::columns) % rmd_128::K::columns == 4);
static_assert(( 80 / rmd_128::K::columns) % rmd_128::K::columns == 5);
static_assert(( 95 / rmd_128::K::columns) % rmd_128::K::columns == 5);
static_assert(( 96 / rmd_128::K::columns) % rmd_128::K::columns == 6);
static_assert((111 / rmd_128::K::columns) % rmd_128::K::columns == 6);
static_assert((112 / rmd_128::K::columns) % rmd_128::K::columns == 7);
static_assert((127 / rmd_128::K::columns) % rmd_128::K::columns == 7);

struct accessor128
  : public rmd_128
{
    template<size_t Round>
    static constexpr auto functor()
    {
        return rmd_128::functor<Round>();
    }

    static constexpr auto get_f0()
    {
        return &rmd_128::f0<uint32_t, uint32_t, uint32_t>;
    }

    static constexpr auto get_f1()
    {
        return &rmd_128::f1<uint32_t, uint32_t, uint32_t>;
    }

    static constexpr auto get_f2()
    {
        return &rmd_128::f2<uint32_t, uint32_t, uint32_t>;
    }

    static constexpr auto get_f3()
    {
        return &rmd_128::f3<uint32_t, uint32_t, uint32_t>;
    }

    static constexpr auto get_f4()
    {
        return &rmd_128::f4<uint32_t, uint32_t, uint32_t>;
    }
};

static_assert(accessor128::functor< 0>() == accessor128::get_f0());
static_assert(accessor128::functor<15>() == accessor128::get_f0());
static_assert(accessor128::functor<16>() == accessor128::get_f1());
static_assert(accessor128::functor<31>() == accessor128::get_f1());
static_assert(accessor128::functor<32>() == accessor128::get_f2());
static_assert(accessor128::functor<47>() == accessor128::get_f2());
static_assert(accessor128::functor<48>() == accessor128::get_f3());
static_assert(accessor128::functor<63>() == accessor128::get_f3());
static_assert(accessor128::functor<64>() == accessor128::get_f4());
static_assert(accessor128::functor<79>() == accessor128::get_f4());

static_assert(accessor128::functor< 80>() == accessor128::get_f4());
static_assert(accessor128::functor< 95>() == accessor128::get_f4());
static_assert(accessor128::functor< 96>() == accessor128::get_f3());
static_assert(accessor128::functor<111>() == accessor128::get_f3());
static_assert(accessor128::functor<112>() == accessor128::get_f2());
static_assert(accessor128::functor<127>() == accessor128::get_f2());
static_assert(accessor128::functor<128>() == accessor128::get_f1());
static_assert(accessor128::functor<143>() == accessor128::get_f1());
static_assert(accessor128::functor<144>() == accessor128::get_f0());
static_assert(accessor128::functor<159>() == accessor128::get_f0());

// algorithm<rmd160>
// ----------------------------------------------------------------------------

using rmd_160 = algorithm<rmd160>;

static_assert(rmd_160::K::word[  0] == 0);
static_assert(rmd_160::K::word[ 80] == 5);
static_assert(rmd_160::K::word[159] == 11);

static_assert(rmd_160::K::rot[  0] == 11);
static_assert(rmd_160::K::rot[ 80] == 8);
static_assert(rmd_160::K::rot[159] == 11);

static_assert( 0 / rmd_160::K::columns == 0);
static_assert(15 / rmd_160::K::columns == 0);
static_assert(16 / rmd_160::K::columns == 1);
static_assert(31 / rmd_160::K::columns == 1);
static_assert(32 / rmd_160::K::columns == 2);
static_assert(47 / rmd_160::K::columns == 2);
static_assert(48 / rmd_160::K::columns == 3);
static_assert(63 / rmd_160::K::columns == 3);
static_assert(64 / rmd_160::K::columns == 4);
static_assert(79 / rmd_160::K::columns == 4);

static_assert( 80 / rmd_160::K::columns == 5);
static_assert( 95 / rmd_160::K::columns == 5);
static_assert( 96 / rmd_160::K::columns == 6);
static_assert(111 / rmd_160::K::columns == 6);
static_assert(112 / rmd_160::K::columns == 7);
static_assert(127 / rmd_160::K::columns == 7);
static_assert(128 / rmd_160::K::columns == 8);
static_assert(143 / rmd_160::K::columns == 8);
static_assert(144 / rmd_160::K::columns == 9);
static_assert(159 / rmd_160::K::columns == 9);

static_assert(rmd_160::K::get[ 0 / rmd_160::K::columns] == 0x00000000);
static_assert(rmd_160::K::get[15 / rmd_160::K::columns] == 0x00000000);
static_assert(rmd_160::K::get[16 / rmd_160::K::columns] == 0x5a827999);
static_assert(rmd_160::K::get[31 / rmd_160::K::columns] == 0x5a827999);
static_assert(rmd_160::K::get[32 / rmd_160::K::columns] == 0x6ed9eba1);
static_assert(rmd_160::K::get[47 / rmd_160::K::columns] == 0x6ed9eba1);
static_assert(rmd_160::K::get[48 / rmd_160::K::columns] == 0x8f1bbcdc);
static_assert(rmd_160::K::get[63 / rmd_160::K::columns] == 0x8f1bbcdc);
static_assert(rmd_160::K::get[64 / rmd_160::K::columns] == 0xa953fd4e);
static_assert(rmd_160::K::get[79 / rmd_160::K::columns] == 0xa953fd4e);

static_assert(rmd_160::K::get[ 80 / rmd_160::K::columns] == 0x50a28be6);
static_assert(rmd_160::K::get[ 95 / rmd_160::K::columns] == 0x50a28be6);
static_assert(rmd_160::K::get[ 96 / rmd_160::K::columns] == 0x5c4dd124);
static_assert(rmd_160::K::get[111 / rmd_160::K::columns] == 0x5c4dd124);
static_assert(rmd_160::K::get[112 / rmd_160::K::columns] == 0x6d703ef3);
static_assert(rmd_160::K::get[127 / rmd_160::K::columns] == 0x6d703ef3);
static_assert(rmd_160::K::get[128 / rmd_160::K::columns] == 0x7a6d76e9);
static_assert(rmd_160::K::get[143 / rmd_160::K::columns] == 0x7a6d76e9);
static_assert(rmd_160::K::get[144 / rmd_160::K::columns] == 0x00000000);
static_assert(rmd_160::K::get[159 / rmd_160::K::columns] == 0x00000000);

static_assert((  0 / rmd_160::K::columns) % rmd_160::K::columns == 0);
static_assert(( 15 / rmd_160::K::columns) % rmd_160::K::columns == 0);
static_assert(( 16 / rmd_160::K::columns) % rmd_160::K::columns == 1);
static_assert(( 31 / rmd_160::K::columns) % rmd_160::K::columns == 1);
static_assert(( 32 / rmd_160::K::columns) % rmd_160::K::columns == 2);
static_assert(( 47 / rmd_160::K::columns) % rmd_160::K::columns == 2);
static_assert(( 48 / rmd_160::K::columns) % rmd_160::K::columns == 3);
static_assert(( 63 / rmd_160::K::columns) % rmd_160::K::columns == 3);
static_assert(( 64 / rmd_160::K::columns) % rmd_160::K::columns == 4);
static_assert(( 79 / rmd_160::K::columns) % rmd_160::K::columns == 4);
static_assert(( 80 / rmd_160::K::columns) % rmd_160::K::columns == 5);
static_assert(( 95 / rmd_160::K::columns) % rmd_160::K::columns == 5);
static_assert(( 96 / rmd_160::K::columns) % rmd_160::K::columns == 6);
static_assert((111 / rmd_160::K::columns) % rmd_160::K::columns == 6);
static_assert((112 / rmd_160::K::columns) % rmd_160::K::columns == 7);
static_assert((127 / rmd_160::K::columns) % rmd_160::K::columns == 7);
static_assert((128 / rmd_160::K::columns) % rmd_160::K::columns == 8);
static_assert((143 / rmd_160::K::columns) % rmd_160::K::columns == 8);
static_assert((144 / rmd_160::K::columns) % rmd_160::K::columns == 9);
static_assert((159 / rmd_160::K::columns) % rmd_160::K::columns == 9);

struct accessor160
  : public rmd_160
{
    template<size_t Round>
    static constexpr auto functor()
    {
        return rmd_160::functor<Round>();
    }

    static constexpr auto get_f0()
    {
        return &rmd_160::f0<uint32_t, uint32_t, uint32_t>;
    }

    static constexpr auto get_f1()
    {
        return &rmd_160::f1<uint32_t, uint32_t, uint32_t>;
    }

    static constexpr auto get_f2()
    {
        return &rmd_160::f2<uint32_t, uint32_t, uint32_t>;
    }

    static constexpr auto get_f3()
    {
        return &rmd_160::f3<uint32_t, uint32_t, uint32_t>;
    }

    static constexpr auto get_f4()
    {
        return &rmd_160::f4<uint32_t, uint32_t, uint32_t>;
    }
};

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
#endif

BOOST_AUTO_TEST_SUITE_END()

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
#include "hash_acc1.hpp"
#include "rmd_alg1.hpp"
    
BOOST_AUTO_TEST_SUITE(rmd_tests)

#include <bitcoin/system/crypto/external/ripemd160.hpp>

#include <bit>
#include <utility>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

// homes.esat.kuleuven.be/~bosselae/ripemd160/pdf/AB-9601/AB-9601.pdf
// [Dobbertin, Bosselaers, Preneel] 18 April 1996.

// Constants.
// ----------------------------------------------------------------------------

constexpr state_t H
{
    0x67452301,
    0xefcdab89,
    0x98badcfe,
    0x10325476,
    0xc3d2e1f0
};

constexpr constants_t K
{
    0x00000000ul,
    0x5a827999ul,
    0x6ed9eba1ul,
    0x8f1bbcdcul,
    0xa953fd4eul,
    0x50a28be6ul,
    0x5c4dd124ul,
    0x6d703ef3ul,
    0x7a6d76e9ul,
    0x00000000ul
};

constexpr std_array<size_t, rounds> W
{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8,
    3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12,
    1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2,
    4, 0, 5, 9, 7, 12, 2, 10, 14, 1, 3, 8, 11, 6, 15, 13,

    5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12,
    6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2,
    15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13,
    8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14,
    12, 15, 10, 4, 1, 5, 8, 7, 6, 2, 13, 14, 0, 3, 9, 11
};


constexpr std_array<size_t, rounds> R
{
    11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8,
    7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12,
    11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5,
    11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12,
    9, 15, 5, 11, 6, 8, 13, 12, 5, 12, 13, 14, 11, 8, 5, 6,

    8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6,
    9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11,
    9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5,
    15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8,
    8, 5, 12, 9, 12, 5, 14, 6, 8, 13, 6, 5, 15, 13, 11, 11,
};

// Functions.
// ----------------------------------------------------------------------------

constexpr auto f0(uint32_t x, uint32_t y, uint32_t z) NOEXCEPT
{
    return x ^ y ^ z;
}

constexpr auto f1(uint32_t x, uint32_t y, uint32_t z) NOEXCEPT
{
    return (x & y) | (~x & z);
}

constexpr auto f2(uint32_t x, uint32_t y, uint32_t z) NOEXCEPT
{
    return (x | ~y) ^ z;
}

constexpr auto f3(uint32_t x, uint32_t y, uint32_t z) NOEXCEPT
{
    return (x & z) | (y & ~z);
}

constexpr auto f4(uint32_t x, uint32_t y, uint32_t z) NOEXCEPT
{
    return x ^ (y | ~z);
}

template<size_t Round>
constexpr auto functor() NOEXCEPT
{
    constexpr auto functor = (Round / columns) % columns;

    if constexpr      (functor == 0u || functor == 9u)
        return &f0;
    else if constexpr (functor == 1u || functor == 8u)
        return &f1;
    else if constexpr (functor == 2u || functor == 7u)
        return &f2;
    else if constexpr (functor == 3u || functor == 6u)
        return &f3;
    else if constexpr (functor == 4u || functor == 5u)
        return &f4;
}

// Rounds
// ---------------------------------------------------------------------------

template<size_t Round>
constexpr auto fn = functor<Round>();

template<size_t Round>
constexpr auto round(auto a, auto& b, auto c, auto d, auto& e, auto w) NOEXCEPT
{
    BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    constexpr auto f = fn<Round>;
    constexpr auto r = R[Round];
    constexpr auto k = K[Round / columns];
    BC_POP_WARNING()
    BC_POP_WARNING()

    e = /*a =*/ std::rotl(a + f(b, c, d) + w + k, r) + e;
    b = /*c =*/ std::rotl(c, rows);
}

template<size_t Round>
void round(auto& out, const auto& in) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
    round<Round>(
        out[(rounds + 0 - Round) % state_words],
        out[(rounds + 1 - Round) % state_words], // c->b
        out[(rounds + 2 - Round) % state_words],
        out[(rounds + 3 - Round) % state_words],
        out[(rounds + 4 - Round) % state_words], // a->e
        in[W[Round]]);
    BC_POP_WARNING()
    BC_POP_WARNING()
}

template<bool First>
void batch(state_t& out, const words_t& in) NOEXCEPT
{
    constexpr auto offset = First ? zero : to_half(rounds);

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

void sum_state(state_t& out, const state_t& in1, const state_t& in2) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    out[0] += (in1[0] + in2[0]);
    out[1] += (in1[1] + in2[1]);
    out[2] += (in1[2] + in2[2]);
    out[3] += (in1[3] + in2[3]);
    out[4] += (in1[4] + in2[4]);
    BC_POP_WARNING()
}
// The two batches can be vectorized (platform specific) or parallelized.
void rounding(state_t& state, const words_t& buffer) NOEXCEPT
{
    state_t first{ state };
    batch<true>(first, buffer);

    state_t second{ state };
    batch<false>(second, buffer);

    sum_state(state, first, second);
}

#ifndef TESTS
static_assert(W[0] == 0);
static_assert(W[to_half(rounds)] == 5);
static_assert(W[sub1(rounds)] == 11);

static_assert(R[0] == 11);
static_assert(R[to_half(rounds)] == 8);
static_assert(R[sub1(rounds)] == 11);

static_assert(0 / columns == 0);
static_assert(15 / columns == 0);
static_assert(16 / columns == 1);
static_assert(31 / columns == 1);
static_assert(32 / columns == 2);
static_assert(47 / columns == 2);
static_assert(48 / columns == 3);
static_assert(63 / columns == 3);
static_assert(64 / columns == 4);
static_assert(79 / columns == 4);

static_assert(80 / columns == 5);
static_assert(95 / columns == 5);
static_assert(96 / columns == 6);
static_assert(111 / columns == 6);
static_assert(112 / columns == 7);
static_assert(127 / columns == 7);
static_assert(128 / columns == 8);
static_assert(143 / columns == 8);
static_assert(144 / columns == 9);
static_assert(159 / columns == 9);

static_assert(K[0 / columns] == 0x00000000ul);
static_assert(K[15 / columns] == 0x00000000ul);
static_assert(K[16 / columns] == 0x5a827999ul);
static_assert(K[31 / columns] == 0x5a827999ul);
static_assert(K[32 / columns] == 0x6ed9eba1ul);
static_assert(K[47 / columns] == 0x6ed9eba1ul);
static_assert(K[48 / columns] == 0x8f1bbcdcul);
static_assert(K[63 / columns] == 0x8f1bbcdcul);
static_assert(K[64 / columns] == 0xa953fd4eul);
static_assert(K[79 / columns] == 0xa953fd4eul);

static_assert(K[80 / columns] == 0x50a28be6ul);
static_assert(K[95 / columns] == 0x50a28be6ul);
static_assert(K[96 / columns] == 0x5c4dd124ul);
static_assert(K[111 / columns] == 0x5c4dd124ul);
static_assert(K[112 / columns] == 0x6d703ef3ul);
static_assert(K[127 / columns] == 0x6d703ef3ul);
static_assert(K[128 / columns] == 0x7a6d76e9ul);
static_assert(K[143 / columns] == 0x7a6d76e9ul);
static_assert(K[144 / columns] == 0x00000000ul);
static_assert(K[159 / columns] == 0x00000000ul);

static_assert((0 / columns) % columns == 0);
static_assert((15 / columns) % columns == 0);
static_assert((16 / columns) % columns == 1);
static_assert((31 / columns) % columns == 1);
static_assert((32 / columns) % columns == 2);
static_assert((47 / columns) % columns == 2);
static_assert((48 / columns) % columns == 3);
static_assert((63 / columns) % columns == 3);
static_assert((64 / columns) % columns == 4);
static_assert((79 / columns) % columns == 4);
static_assert((80 / columns) % columns == 5);
static_assert((95 / columns) % columns == 5);
static_assert((96 / columns) % columns == 6);
static_assert((111 / columns) % columns == 6);
static_assert((112 / columns) % columns == 7);
static_assert((127 / columns) % columns == 7);
static_assert((128 / columns) % columns == 8);
static_assert((143 / columns) % columns == 8);
static_assert((144 / columns) % columns == 9);
static_assert((159 / columns) % columns == 9);

static_assert(fn<0> == &f0);
static_assert(fn<15> == &f0);
static_assert(fn<16> == &f1);
static_assert(fn<31> == &f1);
static_assert(fn<32> == &f2);
static_assert(fn<47> == &f2);
static_assert(fn<48> == &f3);
static_assert(fn<63> == &f3);
static_assert(fn<64> == &f4);
static_assert(fn<79> == &f4);

static_assert(fn<80> == &f4);
static_assert(fn<95> == &f4);
static_assert(fn<96> == &f3);
static_assert(fn<111> == &f3);
static_assert(fn<112> == &f2);
static_assert(fn<127> == &f2);
static_assert(fn<128> == &f1);
static_assert(fn<143> == &f1);
static_assert(fn<144> == &f0);
static_assert(fn<159> == &f0);
#endif

void accumulate(state_t& state, const block_t& block) NOEXCEPT
{
    // TODO: array_cast not constexpr.
    rounding(state, to_little_endians(array_cast<word_t>(block)));
}

digest_t finalize(const state_t& state) NOEXCEPT
{
    // TODO: array_cast not constexpr.
    return array_cast<byte_t>(to_little_endians(state));
}

digest_t hash(size_t, const byte_t*) NOEXCEPT
{
    const block_t buffer{};
    auto state = H;

    // loop/pad
    accumulate(state, buffer);

    return finalize(state);
}

// Accumulated and padded as SHA1/256 but with LE count.
// Can create a unique algorithm (RMD) and pass to accumlator<RMD>.
// Can split out H/K similiarly to SHA for potential RMD128/MD4/5 support.

BOOST_AUTO_TEST_CASE(rmd_test)
{
}

BOOST_AUTO_TEST_SUITE_END()

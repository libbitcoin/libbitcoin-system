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

template <typename RMD>
constexpr auto algorithm<RMD>::
f0(auto x, auto y, auto z) NOEXCEPT
{
    return x ^ y ^ z;
}

template <typename RMD>
constexpr auto algorithm<RMD>::
f1(auto x, auto y, auto z) NOEXCEPT
{
    return (x & y) | (~x & z);
}

template <typename RMD>
constexpr auto algorithm<RMD>::
f2(auto x, auto y, auto z) NOEXCEPT
{
    return (x | ~y) ^ z;
}

template <typename RMD>
constexpr auto algorithm<RMD>::
f3(auto x, auto y, auto z) NOEXCEPT
{
    return (x & z) | (y & ~z);
}

template <typename RMD>
constexpr auto algorithm<RMD>::
f4(auto x, auto y, auto z) NOEXCEPT
{
    return x ^ (y | ~z);
}

template <typename RMD>
template<size_t Round>
constexpr auto algorithm<RMD>::
functor() NOEXCEPT
{
    using self = algorithm<RMD>;
    constexpr auto fn = (Round / RMD::K::columns) % RMD::K::columns;

    if constexpr      (fn == 0u || fn == 9u)
        return &self::f0<uint32_t, uint32_t, uint32_t>;
    else if constexpr (fn == 1u || fn == 8u)
        return &self::f1<uint32_t, uint32_t, uint32_t>;
    else if constexpr (fn == 2u || fn == 7u)
        return &self::f2<uint32_t, uint32_t, uint32_t>;
    else if constexpr (fn == 3u || fn == 6u)
        return &self::f3<uint32_t, uint32_t, uint32_t>;
    else if constexpr (fn == 4u || fn == 5u)
        return &self::f4<uint32_t, uint32_t, uint32_t>;
}

// Rounds
// ---------------------------------------------------------------------------

template <typename RMD>
template<size_t Round>
constexpr auto algorithm<RMD>::
round(auto a, auto& b, auto c, auto d, auto& e, auto w) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
    constexpr auto f = functor<Round>();
    constexpr auto r = K::rot[Round];
    constexpr auto k = K::get[Round / RMD::K::columns];
    BC_POP_WARNING()
    BC_POP_WARNING()

    e = /*a =*/ std::rotl(a + f(b, c, d) + w + k, r) + e;
    b = /*c =*/ std::rotl(c, RMD::K::rows);
}

template <typename RMD>
template<size_t Round>
constexpr void algorithm<RMD>::
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

template <typename RMD>
template<bool First>
constexpr void algorithm<RMD>::
batch(state_t& out, const words_t& in) NOEXCEPT
{
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

template <typename RMD>
constexpr void algorithm<RMD>::
sum_state(state_t& out, const state_t& in1, const state_t& in2) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    out[0] += (in1[0] + in2[0]);
    out[1] += (in1[1] + in2[1]);
    out[2] += (in1[2] + in2[2]);
    out[3] += (in1[3] + in2[3]);
    out[4] += (in1[4] + in2[4]);
    BC_POP_WARNING()
}

template <typename RMD>
constexpr void algorithm<RMD>::
rounding(state_t& state, const words_t& buffer) NOEXCEPT
{
    // This can be vectorized (platform specific) or parallelized.

    state_t first{ state };
    batch<true>(first, buffer);

    state_t second{ state };
    batch<false>(second, buffer);

    sum_state(state, first, second);
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
////
////template <typename RMD>
////typename algorithm<RMD>::digest_t algorithm<RMD>::
////hash(size_t, const byte_t*) NOEXCEPT
////{
////    const block_t buffer{};
////    auto state = H;
////
////    // loop/pad
////    accumulate(state, buffer);
////
////    return finalize(state);
////}

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

BOOST_AUTO_TEST_CASE(rmd_test)
{
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

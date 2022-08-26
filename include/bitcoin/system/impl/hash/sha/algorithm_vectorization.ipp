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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_VECTORIZATION_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_VECTORIZATION_IPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>

namespace libbitcoin {
namespace system {
namespace sha {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)

// Vector-optimized sigma.
// ----------------------------------------------------------------------------
// This increases native operations through ror expansion and is intended
// only for vectorization (where ror/l are not avaialable).
// This sigma refactoring *increases* native processing time by ~10%.
// Tests shows no material performance change in vectorization.

TEMPLATE
template <unsigned int V, unsigned int W, unsigned int X, unsigned int Y,
    unsigned int Z>
INLINE constexpr auto CLASS::
sigma_(auto x) NOEXCEPT
{
    // Not necesarily preferred for AVX (non-destructive), SSE optimal.
    // On AVX architectures, the VEX-encoded SIMD instructions are nondestructive.
    //
    // intel.com/content/dam/www/public/us/en/documents/white-papers/
    // fast-sha512-implementations-ia-processors-paper.pdf
    //
    // Examples:
    // s0(a) = (a >>>  1) ^ (a >>>  8) ^ (a >> 7)
    // s1(e) = (e >>> 19) ^ (e >>> 61) ^ (e >> 6)
    //
    // s0(a) = ((a >>  1) ^ (a << 63)) ^ ((a >>  8) ^ (a << 56)) ^ (a >> 7)
    // s1(e) = ((e >> 19) ^ (e << 45)) ^ ((e >> 61) ^ (e <<  3)) ^ (e >> 6)
    //
    // s0(a) = (((((a >>  1) ^ a) >>  6) ^ a) >> 1) ^ ((a <<  7) ^ a) << 56
    // s1(e) = (((((e >> 42) ^ e) >> 13) ^ e) >> 6) ^ ((e << 42) ^ e) <<  3
    //
    // Generalization:
    // s (n) = (n >>> x) ^ (n >>> y) ^ (n >> z)
    // s'(n) = (((((n >> V) ^ n) >> W) ^ n) >> X) ^ ((n << Y) ^ n) << Z
    // V =  y - z | y - x
    // W =  z - x | x - z
    // X =      x | z
    // Y =  y - x | y - x
    // Z = 64 - y | 64 - y
    // s0'(n) = (((((n >> (y-z)) ^ n) >> (z-x)) ^ n) >> (x)) ^ ((n << (y-x)) ^ n) << (64-y)
    // s1'(n) = (((((n >> (y-x)) ^ n) >> (x-z)) ^ n) >> (z)) ^ ((n << (y-x)) ^ n) << (64-y)
    //
    // Ambiguous (and mismatched) parenthetic in original, this is explicit and correct:
    // s0'(n) = [((((n >> (y-z)) ^ n) >> (z-x)) ^ n) >> (x)] ^ [((n << (y-x)) ^ n) << (64-y)]
    // s1'(n) = [((((n >> (y-x)) ^ n) >> (x-z)) ^ n) >> (z)] ^ [((n << (y-x)) ^ n) << (64-y)]

    constexpr auto s = SHA::word_bits;
    return f::xor_
    (
        f::shr<V, s>(f::xor_(f::shr<X, s>(f::xor_(f::shr<Y, s>(x), x)), x)),
        f::shl<s - W, s>(f::xor_(f::shl<Z, s>(x), x))
    );
}

TEMPLATE
template <unsigned int A, unsigned int B, unsigned int C, if_equal<C, 7>>
INLINE constexpr auto CLASS::
sigma_(auto x) NOEXCEPT
{
    // Denormalized algorithm requires parameter shift for sigma0 sha512.
    return sigma_<A, B, C - A, B - C, B - A>(x);
}

TEMPLATE
template <unsigned int A, unsigned int B, unsigned int C, if_not_equal<C, 7>>
INLINE constexpr auto CLASS::
sigma_(auto x) NOEXCEPT
{
    return sigma_<C, B, A - C, B - A, B - A>(x);
}

TEMPLATE
template <unsigned int A, unsigned int B, unsigned int C, typename xWord,
    if_extended<xWord>>
INLINE constexpr auto CLASS::
sigma(xWord x) NOEXCEPT
{
    return sigma_<A, B, C>(x);
}

// Common.
// ----------------------------------------------------------------------------

TEMPLATE
template <size_t Word, size_t Lanes>
INLINE auto CLASS::
pack(const wblock_t<Lanes>& wblock, size_t size) NOEXCEPT
{
    BC_ASSERT(size > to_half(Lanes));
    using xword = to_extended<word_t, Lanes>;

    if constexpr (Lanes == 2)
    {
        // size always > 1.
        return byteswap<word_t>(set<xword>(
            wblock[0][Word],
            wblock[1][Word]));
    }
    else if constexpr (Lanes == 4)
    {
        // size always > to_half(lanes).
        return byteswap<word_t>(set<xword>(
            wblock[0][Word],
            wblock[1][Word],
            wblock[2][Word],
            size > 3 ? wblock[3][Word] : 0));
    }
    else if constexpr (Lanes == 8)
    {
        // size always > to_half(lanes).
        return byteswap<word_t>(set<xword>(
            wblock[0][Word],
            wblock[1][Word],
            wblock[2][Word],
            wblock[3][Word],
            wblock[4][Word],
            size > 5 ? wblock[5][Word] : 0,
            size > 6 ? wblock[6][Word] : 0,
            size > 7 ? wblock[7][Word] : 0));
    }
    else if constexpr (Lanes == 16)
    {
        // size always > to_half(lanes).
        return byteswap<word_t>(set<xword>(
            wblock[ 0][Word],
            wblock[ 1][Word],
            wblock[ 2][Word],
            wblock[ 3][Word],
            wblock[ 4][Word],
            wblock[ 5][Word],
            wblock[ 6][Word],
            wblock[ 7][Word],
            wblock[ 8][Word],
            size >  9 ? wblock[ 9][Word] : 0,
            size > 10 ? wblock[10][Word] : 0,
            size > 11 ? wblock[11][Word] : 0,
            size > 12 ? wblock[12][Word] : 0,
            size > 13 ? wblock[13][Word] : 0,
            size > 14 ? wblock[14][Word] : 0,
            size > 15 ? wblock[15][Word] : 0));
    }
}

TEMPLATE
template <typename xWord>
INLINE void CLASS::
input(xbuffer_t<xWord>& xbuffer, iblocks_t& blocks) NOEXCEPT
{
    constexpr auto lanes = capacity<xWord, word_t>;

    // When lanes exceeds iterator blocks.size(), the to_array<lanes>() cast
    // remains safe, but it must not be accessed beyond its actual size.
    const auto& wblock = array_cast<words_t>(blocks.template to_array<lanes>());

    // Limit wblock writes to blocks.size().
    const auto size = blocks.size();

    // TODO: if (size >= lanes)... unconditional/parameterized, else conditional.

    xbuffer[0] = pack<0>(wblock, size);
    xbuffer[1] = pack<1>(wblock, size);
    xbuffer[2] = pack<2>(wblock, size);
    xbuffer[3] = pack<3>(wblock, size);
    xbuffer[4] = pack<4>(wblock, size);
    xbuffer[5] = pack<5>(wblock, size);
    xbuffer[6] = pack<6>(wblock, size);
    xbuffer[7] = pack<7>(wblock, size);
    xbuffer[8] = pack<8>(wblock, size);
    xbuffer[9] = pack<9>(wblock, size);
    xbuffer[10] = pack<10>(wblock, size);
    xbuffer[11] = pack<11>(wblock, size);
    xbuffer[12] = pack<12>(wblock, size);
    xbuffer[13] = pack<13>(wblock, size);
    xbuffer[14] = pack<14>(wblock, size);
    xbuffer[15] = pack<15>(wblock, size);

    // iterator stops at end on advance overflow.
    blocks.template advance<lanes>();
}

// Merkle Hash.
// ----------------------------------------------------------------------------

TEMPLATE
template <typename xWord>
INLINE auto CLASS::
pack_pad_half() NOEXCEPT
{
    constexpr auto pad = chunk_pad();

    return xchunk_t<xWord>
    {
        broadcast<xWord>(pad[0]),
        broadcast<xWord>(pad[1]),
        broadcast<xWord>(pad[2]),
        broadcast<xWord>(pad[3]),
        broadcast<xWord>(pad[4]),
        broadcast<xWord>(pad[5]),
        broadcast<xWord>(pad[6]),
        broadcast<xWord>(pad[7])
    };
}

TEMPLATE
template <typename xWord>
INLINE auto CLASS::
pack_schedule_1() NOEXCEPT
{
    constexpr auto pad = scheduled_pad<one>();

    if constexpr (SHA::rounds == 80)
    {
        return xbuffer_t<xWord>
        {
            broadcast<xWord>(pad[0]),
            broadcast<xWord>(pad[1]),
            broadcast<xWord>(pad[2]),
            broadcast<xWord>(pad[3]),
            broadcast<xWord>(pad[4]),
            broadcast<xWord>(pad[5]),
            broadcast<xWord>(pad[6]),
            broadcast<xWord>(pad[7]),
            broadcast<xWord>(pad[8]),
            broadcast<xWord>(pad[9]),
            broadcast<xWord>(pad[10]),
            broadcast<xWord>(pad[11]),
            broadcast<xWord>(pad[12]),
            broadcast<xWord>(pad[13]),
            broadcast<xWord>(pad[14]),
            broadcast<xWord>(pad[15]),

            broadcast<xWord>(pad[16]),
            broadcast<xWord>(pad[17]),
            broadcast<xWord>(pad[18]),
            broadcast<xWord>(pad[19]),
            broadcast<xWord>(pad[20]),
            broadcast<xWord>(pad[21]),
            broadcast<xWord>(pad[22]),
            broadcast<xWord>(pad[23]),
            broadcast<xWord>(pad[24]),
            broadcast<xWord>(pad[25]),
            broadcast<xWord>(pad[26]),
            broadcast<xWord>(pad[27]),
            broadcast<xWord>(pad[28]),
            broadcast<xWord>(pad[29]),
            broadcast<xWord>(pad[30]),
            broadcast<xWord>(pad[31]),

            broadcast<xWord>(pad[32]),
            broadcast<xWord>(pad[33]),
            broadcast<xWord>(pad[34]),
            broadcast<xWord>(pad[35]),
            broadcast<xWord>(pad[36]),
            broadcast<xWord>(pad[37]),
            broadcast<xWord>(pad[38]),
            broadcast<xWord>(pad[39]),
            broadcast<xWord>(pad[40]),
            broadcast<xWord>(pad[41]),
            broadcast<xWord>(pad[42]),
            broadcast<xWord>(pad[43]),
            broadcast<xWord>(pad[44]),
            broadcast<xWord>(pad[45]),
            broadcast<xWord>(pad[46]),
            broadcast<xWord>(pad[47]),

            broadcast<xWord>(pad[48]),
            broadcast<xWord>(pad[49]),
            broadcast<xWord>(pad[50]),
            broadcast<xWord>(pad[51]),
            broadcast<xWord>(pad[52]),
            broadcast<xWord>(pad[53]),
            broadcast<xWord>(pad[54]),
            broadcast<xWord>(pad[55]),
            broadcast<xWord>(pad[56]),
            broadcast<xWord>(pad[57]),
            broadcast<xWord>(pad[58]),
            broadcast<xWord>(pad[59]),
            broadcast<xWord>(pad[60]),
            broadcast<xWord>(pad[61]),
            broadcast<xWord>(pad[62]),
            broadcast<xWord>(pad[63]),

            broadcast<xWord>(pad[64]),
            broadcast<xWord>(pad[65]),
            broadcast<xWord>(pad[66]),
            broadcast<xWord>(pad[67]),
            broadcast<xWord>(pad[68]),
            broadcast<xWord>(pad[69]),
            broadcast<xWord>(pad[70]),
            broadcast<xWord>(pad[71]),
            broadcast<xWord>(pad[72]),
            broadcast<xWord>(pad[73]),
            broadcast<xWord>(pad[74]),
            broadcast<xWord>(pad[75]),
            broadcast<xWord>(pad[76]),
            broadcast<xWord>(pad[77]),
            broadcast<xWord>(pad[78]),
            broadcast<xWord>(pad[79])
        };
    }
    else
    {
        return xbuffer_t<xWord>
        {
            broadcast<xWord>(pad[0]),
            broadcast<xWord>(pad[1]),
            broadcast<xWord>(pad[2]),
            broadcast<xWord>(pad[3]),
            broadcast<xWord>(pad[4]),
            broadcast<xWord>(pad[5]),
            broadcast<xWord>(pad[6]),
            broadcast<xWord>(pad[7]),
            broadcast<xWord>(pad[8]),
            broadcast<xWord>(pad[9]),
            broadcast<xWord>(pad[10]),
            broadcast<xWord>(pad[11]),
            broadcast<xWord>(pad[12]),
            broadcast<xWord>(pad[13]),
            broadcast<xWord>(pad[14]),
            broadcast<xWord>(pad[15]),

            broadcast<xWord>(pad[16]),
            broadcast<xWord>(pad[17]),
            broadcast<xWord>(pad[18]),
            broadcast<xWord>(pad[19]),
            broadcast<xWord>(pad[20]),
            broadcast<xWord>(pad[21]),
            broadcast<xWord>(pad[22]),
            broadcast<xWord>(pad[23]),
            broadcast<xWord>(pad[24]),
            broadcast<xWord>(pad[25]),
            broadcast<xWord>(pad[26]),
            broadcast<xWord>(pad[27]),
            broadcast<xWord>(pad[28]),
            broadcast<xWord>(pad[29]),
            broadcast<xWord>(pad[30]),
            broadcast<xWord>(pad[31]),

            broadcast<xWord>(pad[32]),
            broadcast<xWord>(pad[33]),
            broadcast<xWord>(pad[34]),
            broadcast<xWord>(pad[35]),
            broadcast<xWord>(pad[36]),
            broadcast<xWord>(pad[37]),
            broadcast<xWord>(pad[38]),
            broadcast<xWord>(pad[39]),
            broadcast<xWord>(pad[40]),
            broadcast<xWord>(pad[41]),
            broadcast<xWord>(pad[42]),
            broadcast<xWord>(pad[43]),
            broadcast<xWord>(pad[44]),
            broadcast<xWord>(pad[45]),
            broadcast<xWord>(pad[46]),
            broadcast<xWord>(pad[47]),

            broadcast<xWord>(pad[48]),
            broadcast<xWord>(pad[49]),
            broadcast<xWord>(pad[50]),
            broadcast<xWord>(pad[51]),
            broadcast<xWord>(pad[52]),
            broadcast<xWord>(pad[53]),
            broadcast<xWord>(pad[54]),
            broadcast<xWord>(pad[55]),
            broadcast<xWord>(pad[56]),
            broadcast<xWord>(pad[57]),
            broadcast<xWord>(pad[58]),
            broadcast<xWord>(pad[59]),
            broadcast<xWord>(pad[60]),
            broadcast<xWord>(pad[61]),
            broadcast<xWord>(pad[62]),
            broadcast<xWord>(pad[63])
        };
    }
}

TEMPLATE
template <typename xWord>
INLINE void CLASS::
pad_half(xbuffer_t<xWord>& xbuffer) NOEXCEPT
{
    static const auto xchunk_pad = pack_pad_half<xWord>();
    array_cast<xWord, SHA::chunk_words, SHA::chunk_words>(xbuffer) = xchunk_pad;
}

TEMPLATE
template <typename xWord>
INLINE void CLASS::
schedule_1(xbuffer_t<xWord>& xbuffer) NOEXCEPT
{
    static const auto xscheduled_pad = pack_schedule_1<xWord>();
    xbuffer = xscheduled_pad;
}

TEMPLATE
template <typename xWord>
INLINE auto CLASS::
pack(const state_t& state) NOEXCEPT
{
    return xstate_t<xWord>
    {
        broadcast<xWord>(state[0]),
        broadcast<xWord>(state[1]),
        broadcast<xWord>(state[2]),
        broadcast<xWord>(state[3]),
        broadcast<xWord>(state[4]),
        broadcast<xWord>(state[5]),
        broadcast<xWord>(state[6]),
        broadcast<xWord>(state[7])
    };
}

TEMPLATE
template <size_t Lane, typename xWord>
INLINE typename CLASS::digest_t CLASS::
unpack(const xstate_t<xWord>& xstate) NOEXCEPT
{
    return array_cast<byte_t>(state_t
    {
        get<word_t, Lane>(byteswap<word_t>(xstate[0])),
        get<word_t, Lane>(byteswap<word_t>(xstate[1])),
        get<word_t, Lane>(byteswap<word_t>(xstate[2])),
        get<word_t, Lane>(byteswap<word_t>(xstate[3])),
        get<word_t, Lane>(byteswap<word_t>(xstate[4])),
        get<word_t, Lane>(byteswap<word_t>(xstate[5])),
        get<word_t, Lane>(byteswap<word_t>(xstate[6])),
        get<word_t, Lane>(byteswap<word_t>(xstate[7]))
    });
}

TEMPLATE
template <typename xWord>
INLINE void CLASS::
output(idigests_t& digests, const xstate_t<xWord>& xstate) NOEXCEPT
{
    constexpr auto lanes = capacity<xWord, word_t>;
    BC_ASSERT(digests.size() > to_half(lanes));

    // When lanes exceeds iterator digests.size(), the to_array<lanes>() cast
    // remains safe, but it must not be accessed beyond its actual size.
    auto& wdigest = array_cast<digest_t>(digests.template to_array<lanes>());

    // Limit wdigest writes to digests.size().
    const auto size = digests.size();

    // TODO: if (size >= lanes)... unconditional, else conditional.

    // size always > 1.
    wdigest[0] = unpack<0>(xstate);
    wdigest[1] = unpack<1>(xstate);

    if constexpr (lanes >= 4)
    {
        // size always > to_half(lanes).
        wdigest[2] = unpack<2>(xstate);
        if (size <= 3) return;
        wdigest[3] = unpack<3>(xstate);
    }

    if constexpr (lanes >= 8)
    {
        // size always > to_half(lanes).
        wdigest[4] = unpack<4>(xstate);
        if (size <= 5) return;
        wdigest[5] = unpack<5>(xstate);
        if (size <= 6) return;
        wdigest[6] = unpack<6>(xstate);
        if (size <= 7) return;
        wdigest[7] = unpack<7>(xstate);
    }

    if constexpr (lanes >= 16)
    {
        // size always > to_half(lanes).
        wdigest[8] = unpack<8>(xstate);
        if (size <= 9) return;
        wdigest[9] = unpack<9>(xstate);
        if (size <= 10) return;
        wdigest[10] = unpack<10>(xstate);
        if (size <= 11) return;
        wdigest[11] = unpack<11>(xstate);
        if (size <= 12) return;
        wdigest[12] = unpack<12>(xstate);
        if (size <= 13) return;
        wdigest[13] = unpack<13>(xstate);
        if (size <= 14) return;
        wdigest[14] = unpack<14>(xstate);
        if (size <= 15) return;
        wdigest[15] = unpack<15>(xstate);
    }

    // iterator stops at end on advance overflow.
    digests.template advance<lanes>();
}

TEMPLATE
template <typename xWord, if_extended<xWord>>
INLINE void CLASS::
vectorize(idigests_t& digests, iblocks_t& blocks) NOEXCEPT
{
    BC_ASSERT(digests.size() == blocks.size());
    constexpr auto lanes = capacity<xWord, word_t>;
    static_assert(is_valid_lanes<lanes>);

    if (blocks.size() > to_half(lanes) && have<xWord>())
    {
        static auto initial = pack<xWord>(H::get);
        xbuffer_t<xWord> xbuffer;

        do
        {
            auto xstate = initial;

            // input() advances block iterator by lanes (or to end).
            input(xbuffer, blocks);
            schedule(xbuffer);
            compress(xstate, xbuffer);
            schedule_1(xbuffer);
            compress(xstate, xbuffer);

            // Second hash
            input(xbuffer, xstate);
            pad_half(xbuffer);
            schedule(xbuffer);
            xstate = initial;
            compress(xstate, xbuffer);

            // output() advances digest iterator by lanes (or to end).
            output(digests, xstate);
        }
        while (blocks.size() > to_half(lanes));
    }
}

TEMPLATE
INLINE void CLASS::
vectorized(digests_t& digests) NOEXCEPT
{
    static_assert(sizeof(digest_t) == to_half(sizeof(block_t)));

    const auto data = digests.front().data();
    const auto size = digests.size() * array_count<digest_t>;
    auto iblocks = iblocks_t{ size, data };
    auto idigests = idigests_t{ to_half(size), data };
    const auto blocks = iblocks.size();

    if constexpr (have_x512)
        vectorize<xint512_t>(idigests, iblocks);
    if constexpr (have_x256)
        vectorize<xint256_t>(idigests, iblocks);
    if constexpr (have_x128)
        vectorize<xint128_t>(idigests, iblocks);

    // iblocks.size() is reduced by vectorization.
    merkle_hash_(digests, blocks - iblocks.size());
}

// Message Schedule (block vectorization).
// ----------------------------------------------------------------------------
// eprint.iacr.org/2012/067.pdf

TEMPLATE
template <typename xWord>
INLINE void CLASS::
compress_(state_t& state, const xbuffer_t<xWord>& xbuffer, size_t size) NOEXCEPT
{
    // Limit compress calls to size.
    constexpr auto lanes = capacity<xWord, word_t>;
    BC_ASSERT(size > to_half(lanes));

    // TODO: if (size >= lanes)... unconditional, else conditional.

    // size always > 1.
    compress<0>(state, xbuffer);
    compress<1>(state, xbuffer);

    if constexpr (lanes >= 4)
    {
        // size always > to_half(lanes).
        compress<2>(state, xbuffer);
        if (size <= 3) return;
        compress<3>(state, xbuffer);
    }

    if constexpr (lanes >= 8)
    {
        // size always > to_half(lanes).
        compress<4>(state, xbuffer);
        if (size <= 5) return;
        compress<5>(state, xbuffer);
        if (size <= 6) return;
        compress<6>(state, xbuffer);
        if (size <= 7) return;
        compress<7>(state, xbuffer);
    }

    if constexpr (lanes >= 16)
    {
        // size always > to_half(lanes).
        compress<8>(state, xbuffer);
        if (size <= 9) return;
        compress<9>(state, xbuffer);
        if (size <= 10) return;
        compress<10>(state, xbuffer);
        if (size <= 11) return;
        compress<11>(state, xbuffer);
        if (size <= 12) return;
        compress<12>(state, xbuffer);
        if (size <= 13) return;
        compress<13>(state, xbuffer);
        if (size <= 14) return;
        compress<14>(state, xbuffer);
        if (size <= 15) return;
        compress<15>(state, xbuffer);
    }
}

TEMPLATE
template <typename xWord, if_extended<xWord>>
INLINE void CLASS::
vectorize(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    constexpr auto lanes = capacity<xWord, word_t>;
    static_assert(is_valid_lanes<lanes>);
    auto size = blocks.size();

    if (size > to_half(lanes) && have<xWord>())
    {
        xbuffer_t<xWord> xbuffer;

        do
        {
            // input() advances block iterator by lanes (or to end).
            input(xbuffer, blocks);
            schedule(xbuffer);
            compress_(state, xbuffer, size);
            size = blocks.size();
        }
        while (size > to_half(lanes));
    }
}

TEMPLATE
INLINE void CLASS::
vectorized(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    if constexpr (have_x512)
        vectorize<xint512_t>(state, blocks);
    if constexpr (have_x256)
        vectorize<xint256_t>(state, blocks);
    if constexpr (have_x128)
        vectorize<xint128_t>(state, blocks);

    // blocks.size() is reduced by vectorization.
    iterate_(state, blocks);
}

TEMPLATE
template <size_t Size>
INLINE void CLASS::
vectorized(state_t& state, const ablocks_t<Size>& blocks) NOEXCEPT
{
    auto iblocks = iblocks_t{ array_cast<byte_t>(blocks) };
    vectorized(state, iblocks);
}

// Message Schedule (sigma vectorization).
// ----------------------------------------------------------------------------

constexpr auto add0(auto a) noexcept { return depromote<decltype(a)>(a + 0); }
constexpr auto add2(auto a) noexcept { return depromote<decltype(a)>(a + 2); }
constexpr auto add3(auto a) noexcept { return depromote<decltype(a)>(a + 3); }
constexpr auto add4(auto a) noexcept { return depromote<decltype(a)>(a + 4); }
constexpr auto add5(auto a) noexcept { return depromote<decltype(a)>(a + 5); }
constexpr auto add6(auto a) noexcept { return depromote<decltype(a)>(a + 6); }
constexpr auto add7(auto a) noexcept { return depromote<decltype(a)>(a + 7); }

TEMPLATE
template <typename xWord, if_extended<xWord>>
INLINE auto CLASS::
sigma1_2(auto x1, auto x2) NOEXCEPT
{
    // partial fill sigma1 may not be worth it.
    return sigma1(set<xWord>(x1, x2, 0, 0));
}

TEMPLATE
template <typename xWord, if_extended<xWord>>
INLINE auto CLASS::
sigma0_8(auto x1, auto x2, auto x3, auto x4, auto x5, auto x6, auto x7,
    auto x8) NOEXCEPT
{
    return sigma0(set<xWord>(x1, x2, x3, x4, x5, x6, x7, x8));
}

TEMPLATE
template<size_t Round>
INLINE void CLASS::
prepare_(buffer_t& buffer) NOEXCEPT
{
    static_assert(SHA::strength != 160);

    constexpr auto r02 = Round - 2;
    constexpr auto r07 = Round - 7;
    constexpr auto r15 = Round - 15;
    constexpr auto r16 = Round - 16;
    constexpr auto s = SHA::word_bits;

    using xword8 = to_extended<word_t, 8>;
    using xword4 = to_extended<word_t, 4>;

    const auto xr15 = sigma0_8<xword8>(
        buffer[add0(r15)], buffer[add1(r15)],
        buffer[add2(r15)], buffer[add3(r15)],
        buffer[add4(r15)], buffer[add5(r15)],
        buffer[add6(r15)], buffer[add7(r15)]);

    auto xr02 = sigma1_2<xword4>(
        buffer[add0(r02)], buffer[add1(r02)]);

    buffer[add0(Round)] = f::add<s>(
        f::add<s>(buffer[add0(r16)], get<word_t, 0>(xr15)),
        f::add<s>(buffer[add0(r07)], get<word_t, 0>(xr02)));
    buffer[add0(r16)] = f::addc<K::get[add0(r16)], s>(buffer[add0(r16)]);

    buffer[add1(Round)] = f::add<s>(
        f::add<s>(buffer[add1(r16)], get<word_t, 1>(xr15)),
        f::add<s>(buffer[add1(r07)], get<word_t, 1>(xr02)));
    buffer[add1(r16)] = f::addc<K::get[add1(r16)], s>(buffer[add1(r16)]);

    // buffer[add2(r02)] is buffer[add0(Round)].
    // buffer[add3(r02)] is buffer[add1(Round)].
    // This is why sigma1 is limited to two lanes.
    xr02 = sigma1_2<xword4>(
        buffer[add2(r02)], buffer[add3(r02)]);

    buffer[add2(Round)] = f::add<s>(
        f::add<s>(buffer[add2(r16)], get<word_t, 2>(xr15)),
        f::add<s>(buffer[add2(r07)], get<word_t, 0>(xr02)));
    buffer[add2(r16)] = f::addc<K::get[add2(r16)], s>(buffer[add2(r16)]);

    buffer[add3(Round)] = f::add<s>(
        f::add<s>(buffer[add3(r16)], get<word_t, 3>(xr15)),
        f::add<s>(buffer[add3(r07)], get<word_t, 1>(xr02)));
    buffer[add3(r16)] = f::addc<K::get[add3(r16)], s>(buffer[add3(r16)]);

    // buffer[add4(r02)] is buffer[add2(Round)].
    // buffer[add5(r02)] is buffer[add3(Round)].
    // This is why sigma1 is limited to two lanes.
    xr02 = sigma1_2<xword4>(
        buffer[add4(r02)], buffer[add5(r02)]);

    buffer[add4(Round)] = f::add<s>(
        f::add<s>(buffer[add4(r16)], get<word_t, 4>(xr15)),
        f::add<s>(buffer[add4(r07)], get<word_t, 0>(xr02)));
    buffer[add4(r16)] = f::addc<K::get[add4(r16)], s>(buffer[add4(r16)]);

    buffer[add5(Round)] = f::add<s>(
        f::add<s>(buffer[add5(r16)], get<word_t, 5>(xr15)),
        f::add<s>(buffer[add5(r07)], get<word_t, 1>(xr02)));
    buffer[add5(r16)] = f::addc<K::get[add5(r16)], s>(buffer[add5(r16)]);

    // buffer[add6(r02)] is buffer[add4(Round)].
    // buffer[add7(r02)] is buffer[add5(Round)].
    // This is why sigma1 is limited to two lanes.
    xr02 = sigma1_2<xword4>(
        buffer[add6(r02)], buffer[add7(r02)]);

    buffer[add6(Round)] = f::add<s>(
        f::add<s>(buffer[add6(r16)], get<word_t, 6>(xr15)),
        f::add<s>(buffer[add6(r07)], get<word_t, 0>(xr02)));
    buffer[add6(r16)] = f::addc<K::get[add6(r16)], s>(buffer[add6(r16)]);

    // buffer[add7(r07)] is buffer[add0(Round)]
    // This is why sigma0 is limited to 8 lanes (vs 16).
    buffer[add7(Round)] = f::add<s>(
        f::add<s>(buffer[add7(r16)], get<word_t, 7>(xr15)),
        f::add<s>(buffer[add7(r07)], get<word_t, 1>(xr02)));
    buffer[add7(r16)] = f::addc<K::get[add7(r16)], s>(buffer[add7(r16)]);
}

TEMPLATE
INLINE void CLASS::
vectorized(auto& buffer) NOEXCEPT
{
    if constexpr (SHA::strength == 160 || is_extended<decltype(buffer.front())>)
    {
        schedule_(buffer);
    }
    else if (have_lanes<word_t, 8> && have_lanes<word_t, 4>)
    {
        // eight round scheduling for fewer blocks than lanes (singles).
        prepare_<16>(buffer);
        prepare_<24>(buffer);
        prepare_<32>(buffer);
        prepare_<40>(buffer);
        prepare_<48>(buffer);
        prepare_<56>(buffer);

        if constexpr (SHA::rounds == 80)
        {
            prepare_<64>(buffer);
            prepare_<72>(buffer);
        }

        add_k(buffer);
    }
    else
    {
        schedule_(buffer);
    }
}

BC_POP_WARNING()

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif
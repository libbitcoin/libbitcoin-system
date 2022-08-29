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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHAX_ALGORITHM_VECTORIZATION_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHAX_ALGORITHM_VECTORIZATION_IPP

#include "../../../test.hpp"

namespace shax {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)

// Common.
// ----------------------------------------------------------------------------

TEMPLATE
template <size_t Word, size_t Lanes>
INLINE auto CLASS::
pack(const wblock_t<Lanes>& wblock) NOEXCEPT
{
    using xword = to_extended<word_t, Lanes>;

    if constexpr (Lanes == 2)
    {
        return byteswap<word_t>(set<xword>(
            wblock[0][Word],
            wblock[1][Word]));
    }
    else if constexpr (Lanes == 4)
    {
        return byteswap<word_t>(set<xword>(
            wblock[0][Word],
            wblock[1][Word],
            wblock[2][Word],
            wblock[3][Word]));
    }
    else if constexpr (Lanes == 8)
    {
        return byteswap<word_t>(set<xword>(
            wblock[0][Word],
            wblock[1][Word],
            wblock[2][Word],
            wblock[3][Word],
            wblock[4][Word],
            wblock[5][Word],
            wblock[6][Word],
            wblock[7][Word]));
    }
    else if constexpr (Lanes == 16)
    {
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
            wblock[ 9][Word],
            wblock[10][Word],
            wblock[11][Word],
            wblock[12][Word],
            wblock[13][Word],
            wblock[14][Word],
            wblock[15][Word]));
    }
}

TEMPLATE
template <typename xWord>
INLINE void CLASS::
input(xbuffer_t<xWord>& xbuffer, iblocks_t& blocks) NOEXCEPT
{
    constexpr auto lanes = capacity<xWord, word_t>;

    const auto& wblock = array_cast<words_t>(blocks.template to_array<lanes>());
    xbuffer[0] = pack<0>(wblock);
    xbuffer[1] = pack<1>(wblock);
    xbuffer[2] = pack<2>(wblock);
    xbuffer[3] = pack<3>(wblock);
    xbuffer[4] = pack<4>(wblock);
    xbuffer[5] = pack<5>(wblock);
    xbuffer[6] = pack<6>(wblock);
    xbuffer[7] = pack<7>(wblock);
    xbuffer[8] = pack<8>(wblock);
    xbuffer[9] = pack<9>(wblock);
    xbuffer[10] = pack<10>(wblock);
    xbuffer[11] = pack<11>(wblock);
    xbuffer[12] = pack<12>(wblock);
    xbuffer[13] = pack<13>(wblock);
    xbuffer[14] = pack<14>(wblock);
    xbuffer[15] = pack<15>(wblock);
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
    // TODO: this is inefficient, swapping all xwords Lanes times.
    // TODO: and getting each lane individually.
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
    BC_ASSERT(digests.size() >= lanes);

    auto& wdigest = array_cast<digest_t>(digests.template to_array<lanes>());

    wdigest[0] = unpack<0>(xstate);
    wdigest[1] = unpack<1>(xstate);

    if constexpr (lanes >= 4)
    {
        wdigest[2] = unpack<2>(xstate);
        wdigest[3] = unpack<3>(xstate);
    }

    if constexpr (lanes >= 8)
    {
        wdigest[4] = unpack<4>(xstate);
        wdigest[5] = unpack<5>(xstate);
        wdigest[6] = unpack<6>(xstate);
        wdigest[7] = unpack<7>(xstate);
    }

    if constexpr (lanes >= 16)
    {
        wdigest[8] = unpack<8>(xstate);
        wdigest[9] = unpack<9>(xstate);
        wdigest[10] = unpack<10>(xstate);
        wdigest[11] = unpack<11>(xstate);
        wdigest[12] = unpack<12>(xstate);
        wdigest[13] = unpack<13>(xstate);
        wdigest[14] = unpack<14>(xstate);
        wdigest[15] = unpack<15>(xstate);
    }

    digests.template advance<lanes>();
}

TEMPLATE
template <typename xWord, if_extended<xWord>>
INLINE void CLASS::
merkle_hash_v_(idigests_t& digests, iblocks_t& blocks) NOEXCEPT
{
    BC_ASSERT(digests.size() == blocks.size());
    constexpr auto lanes = capacity<xWord, word_t>;
    static_assert(is_valid_lanes<lanes>);

    if (blocks.size() >= lanes && have<xWord>())
    {
        static auto initial = pack<xWord>(H::get);
        xbuffer_t<xWord> xbuffer;

        do
        {
            auto xstate = initial;

            // input() advances block iterator by lanes.
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

            // output() advances digest iterator by lanes.
            output(digests, xstate);
        }
        while (blocks.size() >= lanes);
    }
}

TEMPLATE
INLINE void CLASS::
merkle_hash_v(digests_t& digests) NOEXCEPT
{
    // Merkle vector dispatch.
    static_assert(sizeof(digest_t) == to_half(sizeof(block_t)));
    auto offset = zero;

    if (digests.size() >= min_lanes * two)
    {
        const auto data = digests.front().data();
        const auto size = digests.size() * array_count<digest_t>;
        auto iblocks = iblocks_t{ size, data };
        auto idigests = idigests_t{ to_half(size), data };
        const auto blocks = iblocks.size();

        // Merkle hash vector dispatch.
        if constexpr (have_x512)
            merkle_hash_v_<xint512_t>(idigests, iblocks);
        if constexpr (have_x256)
            merkle_hash_v_<xint256_t>(idigests, iblocks);
        if constexpr (have_x128)
            merkle_hash_v_<xint128_t>(idigests, iblocks);

        // iblocks.size() is reduced by vectorization.
        offset = blocks - iblocks.size();
    }

    // Complete rounds using normal form.
    merkle_hash_(digests, offset);
}

// Message Schedule (block vectorization).
// ----------------------------------------------------------------------------
// eprint.iacr.org/2012/067.pdf

TEMPLATE
template <typename xWord>
INLINE void CLASS::
compress_lanes(state_t& state, const xbuffer_t<xWord>& xbuffer) NOEXCEPT
{
    constexpr auto lanes = capacity<xWord, word_t>;

    compress<0>(state, xbuffer);
    compress<1>(state, xbuffer);

    if constexpr (lanes >= 4)
    {
        compress<2>(state, xbuffer);
        compress<3>(state, xbuffer);
    }

    if constexpr (lanes >= 8)
    {
        compress<4>(state, xbuffer);
        compress<5>(state, xbuffer);
        compress<6>(state, xbuffer);
        compress<7>(state, xbuffer);
    }

    if constexpr (lanes >= 16)
    {
        compress<8>(state, xbuffer);
        compress<9>(state, xbuffer);
        compress<10>(state, xbuffer);
        compress<11>(state, xbuffer);
        compress<12>(state, xbuffer);
        compress<13>(state, xbuffer);
        compress<14>(state, xbuffer);
        compress<15>(state, xbuffer);
    }
}

TEMPLATE
template <typename xWord, if_extended<xWord>>
INLINE void CLASS::
iterate_v_(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    constexpr auto lanes = capacity<xWord, word_t>;
    static_assert(is_valid_lanes<lanes>);

    if (blocks.size() >= lanes && have<xWord>())
    {
        xbuffer_t<xWord> xbuffer;

        do
        {
            // input() advances block iterator by lanes.
            input(xbuffer, blocks);
            schedule_(xbuffer);
            compress_lanes(state, xbuffer);
        }
        while (blocks.size() >= lanes);
    }
}

TEMPLATE
INLINE void CLASS::
iterate_v(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    if (blocks.size() >= min_lanes)
{
        // Schedule iteration vector dispatch.
        if constexpr (have_x512)
            iterate_v_<xint512_t>(state, blocks);
        if constexpr (have_x256)
            iterate_v_<xint256_t>(state, blocks);
        if constexpr (have_x128)
            iterate_v_<xint128_t>(state, blocks);
    }

    // Complete rounds using normal form.
    // blocks.size() is reduced by vectorization.
    iterate_(state, blocks);
}

TEMPLATE
template <size_t Size>
INLINE void CLASS::
iterate_v(state_t& state, const ablocks_t<Size>& blocks) NOEXCEPT
{
    if (blocks.size() >= min_lanes)
    {
        auto iblocks = iblocks_t{ array_cast<byte_t>(blocks) };
        iterate_v(state, iblocks);
    }
    else
    {
        iterate_(state, blocks);
    }
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
sigma0_8(auto x1, auto x2, auto x3, auto x4, auto x5, auto x6, auto x7,
    auto x8) NOEXCEPT
{
    return sigma0(set<xWord>(x1, x2, x3, x4, x5, x6, x7, x8));
}

TEMPLATE
template<size_t Round>
INLINE void CLASS::
prepare_8(buffer_t& buffer) NOEXCEPT
{
    // Requires avx512 for sha512 and avx2 for sha256.
    // sigma0x8 message scheduling for single block iteration.
    // Tests of adding sigma1x2 half lanes vectorization show loss ~10%.
    // Tests of adding sigma0x8 full lanes vectorization show a gain of ~5%.
    // The simplicity of sha160 message prepare precludes this optimization.
    static_assert(SHA::strength != 160);

    constexpr auto r02 = Round - 2;
    constexpr auto r07 = Round - 7;
    constexpr auto r15 = Round - 15;
    constexpr auto r16 = Round - 16;
    constexpr auto s = SHA::word_bits;

    const auto xr15 = sigma0_8<to_extended<word_t, 8>>(
        buffer[add0(r15)], buffer[add1(r15)],
        buffer[add2(r15)], buffer[add3(r15)],
        buffer[add4(r15)], buffer[add5(r15)],
        buffer[add6(r15)], buffer[add7(r15)]);

    buffer[add0(Round)] = f::add<s>(
        f::add<s>(buffer[add0(r16)], get<word_t, 0>(xr15)),
        f::add<s>(buffer[add0(r07)], sigma1(buffer[add0(r02)])));
    buffer[add0(r16)] = f::addc<K::get[add0(r16)], s>(buffer[add0(r16)]);

    buffer[add1(Round)] = f::add<s>(
        f::add<s>(buffer[add1(r16)], get<word_t, 1>(xr15)),
        f::add<s>(buffer[add1(r07)], sigma1(buffer[add1(r02)])));
    buffer[add1(r16)] = f::addc<K::get[add1(r16)], s>(buffer[add1(r16)]);

    buffer[add2(Round)] = f::add<s>(
        f::add<s>(buffer[add2(r16)], get<word_t, 2>(xr15)),
        f::add<s>(buffer[add2(r07)], sigma1(buffer[add2(r02)])));
    buffer[add2(r16)] = f::addc<K::get[add2(r16)], s>(buffer[add2(r16)]);

    buffer[add3(Round)] = f::add<s>(
        f::add<s>(buffer[add3(r16)], get<word_t, 3>(xr15)),
        f::add<s>(buffer[add3(r07)], sigma1(buffer[add3(r02)])));
    buffer[add3(r16)] = f::addc<K::get[add3(r16)], s>(buffer[add3(r16)]);

    buffer[add4(Round)] = f::add<s>(
        f::add<s>(buffer[add4(r16)], get<word_t, 4>(xr15)),
        f::add<s>(buffer[add4(r07)], sigma1(buffer[add4(r02)])));
    buffer[add4(r16)] = f::addc<K::get[add4(r16)], s>(buffer[add4(r16)]);

    buffer[add5(Round)] = f::add<s>(
        f::add<s>(buffer[add5(r16)], get<word_t, 5>(xr15)),
        f::add<s>(buffer[add5(r07)], sigma1(buffer[add5(r02)])));
    buffer[add5(r16)] = f::addc<K::get[add5(r16)], s>(buffer[add5(r16)]);

    buffer[add6(Round)] = f::add<s>(
        f::add<s>(buffer[add6(r16)], get<word_t, 6>(xr15)),
        f::add<s>(buffer[add6(r07)], sigma1(buffer[add6(r02)])));
    buffer[add6(r16)] = f::addc<K::get[add6(r16)], s>(buffer[add6(r16)]);

    // buffer[add7(r07)] is buffer[add0(Round)]
    // This is why sigma0 is limited to 8 lanes (vs 16).
    buffer[add7(Round)] = f::add<s>(
        f::add<s>(buffer[add7(r16)], get<word_t, 7>(xr15)),
        f::add<s>(buffer[add7(r07)], sigma1(buffer[add7(r02)])));
    buffer[add7(r16)] = f::addc<K::get[add7(r16)], s>(buffer[add7(r16)]);
}

TEMPLATE
INLINE void CLASS::
schedule_v(auto& buffer) NOEXCEPT
{
    using word = decltype(buffer.front());

    if constexpr (SHA::strength == 160 || !is_same_type<word, word_t>)
    {
        schedule_(buffer);
    }
    else if (!have_lanes<word_t, 8>())
    {
        schedule_(buffer);
    }
    else
    {
        // Schedule prepare vector dispatch.
        prepare_8<16>(buffer);
        prepare_8<24>(buffer);
        prepare_8<32>(buffer);
        prepare_8<40>(buffer);
        prepare_8<48>(buffer);
        prepare_8<56>(buffer);

        if constexpr (SHA::rounds == 80)
        {
            prepare_8<64>(buffer);
            prepare_8<72>(buffer);
        }

        add_k(buffer);
    }
}

// sha intrinsics (compressing)
// ----------------------------------------------------------------------------
//
// Compress.
// SNA-NI/NEON
// buffer[Round] is 128 (w + k).
// buffer is packed on a different axis than full block vectorization.
// State packs 4/5 into xint128 (one state xword plus uint32_t e).
// State aspect is also packed on a different axist than full block,
// though full block (merkle) doesn't support sha160 (half != digest).
//
// SHA-NI/NEON
// buffer[Round] is 128 (w + k).
// buffer is packed on a different axis than full block vectorization.
// State packs 4/8 into xint128 (two state xwords).
// State aspect is also packed on a different axist than full block.
//
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
//
// Each call is 2 rounds, 4 rounds total.
// s, w and k are 128 (4 words each, s1/s2 is 8 word state).
// SHA-NI
//     const auto value = add(w, k);
//     abcd = sha256rnds2(abef, cdgh, value);
//     efgh = sha256rnds2(cdgh, abef, shuffle(value));
// NEON
//     const auto value = vaddq(w, k);
//     abcd = vsha256hq(abef, cdgh, value);
//     efgh = vsha256h2q(cdgh, abef, value);
//
// Schedule.
// Each word is 128, buffer goes from 16 word_t/uint32_t to 4 xint128_t.
// TODO: these use 4 lane xint128 full block vectorization, but instead
// TODO: of each block filling one lane, a single block packs into all 4
// TODO: lanes (x 4 xwords = 16 word_t). This divides round count by 4.
//
// sha160
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
//
// sha256
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

BC_POP_WARNING()

} // namespace shax

#endif

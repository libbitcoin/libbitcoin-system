/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

namespace libbitcoin {
namespace system {
namespace sha {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)

// Common.
// ----------------------------------------------------------------------------

TEMPLATE
template <size_t Word, size_t Lanes>
INLINE auto CLASS::
pack(const xblock_t<Lanes>& xblock) NOEXCEPT
{
    using xword = to_extended<word_t, Lanes>;

    if constexpr (Lanes == 2)
    {
        return byteswap<word_t>(set<xword>(
            xblock[0][Word],
            xblock[1][Word]));
    }
    else if constexpr (Lanes == 4)
    {
        return byteswap<word_t>(set<xword>(
            xblock[0][Word],
            xblock[1][Word],
            xblock[2][Word],
            xblock[3][Word]));
    }
    else if constexpr (Lanes == 8)
    {
        return byteswap<word_t>(set<xword>(
            xblock[0][Word],
            xblock[1][Word],
            xblock[2][Word],
            xblock[3][Word],
            xblock[4][Word],
            xblock[5][Word],
            xblock[6][Word],
            xblock[7][Word]));
    }
    else if constexpr (Lanes == 16)
    {
        return byteswap<word_t>(set<xword>(
            xblock[ 0][Word],
            xblock[ 1][Word],
            xblock[ 2][Word],
            xblock[ 3][Word],
            xblock[ 4][Word],
            xblock[ 5][Word],
            xblock[ 6][Word],
            xblock[ 7][Word],
            xblock[ 8][Word],
            xblock[ 9][Word],
            xblock[10][Word],
            xblock[11][Word],
            xblock[12][Word],
            xblock[13][Word],
            xblock[14][Word],
            xblock[15][Word]));
    }
}

TEMPLATE
template <typename xWord>
INLINE void CLASS::
input(xbuffer_t<xWord>& xbuffer, iblocks_t& blocks) NOEXCEPT
{
    constexpr auto lanes = capacity<xWord, word_t>;

    const auto& xblock = array_cast<words_t>(blocks.template to_array<lanes>());
    xbuffer[0] = pack<0>(xblock);
    xbuffer[1] = pack<1>(xblock);
    xbuffer[2] = pack<2>(xblock);
    xbuffer[3] = pack<3>(xblock);
    xbuffer[4] = pack<4>(xblock);
    xbuffer[5] = pack<5>(xblock);
    xbuffer[6] = pack<6>(xblock);
    xbuffer[7] = pack<7>(xblock);
    xbuffer[8] = pack<8>(xblock);
    xbuffer[9] = pack<9>(xblock);
    xbuffer[10] = pack<10>(xblock);
    xbuffer[11] = pack<11>(xblock);
    xbuffer[12] = pack<12>(xblock);
    xbuffer[13] = pack<13>(xblock);
    xbuffer[14] = pack<14>(xblock);
    xbuffer[15] = pack<15>(xblock);
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
    BC_ASSERT(digests.size() >= lanes);

    auto& xdigest = array_cast<digest_t>(digests.template to_array<lanes>());

    xdigest[0] = unpack<0>(xstate);
    xdigest[1] = unpack<1>(xstate);

    if constexpr (lanes >= 4)
    {
        xdigest[2] = unpack<2>(xstate);
        xdigest[3] = unpack<3>(xstate);
    }

    if constexpr (lanes >= 8)
    {
        xdigest[4] = unpack<4>(xstate);
        xdigest[5] = unpack<5>(xstate);
        xdigest[6] = unpack<6>(xstate);
        xdigest[7] = unpack<7>(xstate);
    }

    if constexpr (lanes >= 16)
    {
        xdigest[8] = unpack<8>(xstate);
        xdigest[9] = unpack<9>(xstate);
        xdigest[10] = unpack<10>(xstate);
        xdigest[11] = unpack<11>(xstate);
        xdigest[12] = unpack<12>(xstate);
        xdigest[13] = unpack<13>(xstate);
        xdigest[14] = unpack<14>(xstate);
        xdigest[15] = unpack<15>(xstate);
    }

    digests.template advance<lanes>();
}

// Merkle Hash.
// ----------------------------------------------------------------------------

TEMPLATE
template <typename xWord, if_extended<xWord>>
INLINE void CLASS::
merkle_hash_invoke(idigests_t& digests, iblocks_t& blocks) NOEXCEPT
{
    BC_ASSERT(digests.size() == blocks.size());
    constexpr auto lanes = capacity<xWord, word_t>;
    static_assert(is_valid_lanes<lanes>);

    if constexpr (have<xWord>())
    {
        if (blocks.size() >= lanes)
        {
            static auto initial = pack<xWord>(H::get);
            BC_PUSH_WARNING(NO_UNINITIALZIED_VARIABLE)
            xbuffer_t<xWord> xbuffer;
            BC_POP_WARNING()

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
                reinput(xbuffer, xstate);
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
}

TEMPLATE
INLINE void CLASS::
merkle_hash_dispatch(digests_t& digests) NOEXCEPT
{
    // Merkle vector dispatch.
    static_assert(sizeof(digest_t) == to_half(sizeof(block_t)));
    auto next = zero;

    if (digests.size() >= min_lanes * two)
    {
        const auto data = digests.front().data();
        const auto size = digests.size() * array_count<digest_t>;
        auto iblocks = iblocks_t{ size, data };
        auto idigests = idigests_t{ to_half(size), data };
        const auto start = iblocks.size();

        // Merkle hash vector dispatch.
        if constexpr (have_x512)
            merkle_hash_invoke<xint512_t>(idigests, iblocks);
        if constexpr (have_x256)
            merkle_hash_invoke<xint256_t>(idigests, iblocks);
        if constexpr (have_x128)
            merkle_hash_invoke<xint128_t>(idigests, iblocks);

        // iblocks.size() is reduced by vectorization.
        next = start - iblocks.size();
    }

    // Complete rounds using normal form.
    merkle_hash_(digests, next);
}

// Message Schedule (block vectorization).
// ----------------------------------------------------------------------------
// eprint.iacr.org/2012/067.pdf

TEMPLATE
template <typename xWord>
INLINE void CLASS::
compress_dispatch(state_t& state, const xbuffer_t<xWord>& xbuffer) NOEXCEPT
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
iterate_invoke(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    constexpr auto lanes = capacity<xWord, word_t>;
    static_assert(is_valid_lanes<lanes>);

    if constexpr (have<xWord>())
    {
        if (blocks.size() >= lanes)
        {
            BC_PUSH_WARNING(NO_UNINITIALZIED_VARIABLE)
            xbuffer_t<xWord> xbuffer;
            BC_POP_WARNING()

            do
            {
                // input() advances block iterator by lanes.
                input(xbuffer, blocks);
                schedule_(xbuffer);
                compress_dispatch(state, xbuffer);
            } while (blocks.size() >= lanes);
        }
    }
}

TEMPLATE
INLINE void CLASS::
iterate_dispatch(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    // Schedule iteration vector dispatch.
    if (blocks.size() >= min_lanes)
    {
        // Schedule iteration vector dispatch.
        if constexpr (have_x512)
            iterate_invoke<xint512_t>(state, blocks);
        if constexpr (have_x256)
            iterate_invoke<xint256_t>(state, blocks);
        if constexpr (have_x128)
            iterate_invoke<xint128_t>(state, blocks);
    }

    // Complete rounds using normal form.
    // blocks.size() is reduced by vectorization.
    iterate_(state, blocks);
}

TEMPLATE
template <size_t Size>
INLINE void CLASS::
iterate_dispatch(state_t& state, const ablocks_t<Size>& blocks) NOEXCEPT
{
    if (blocks.size() >= min_lanes)
    {
        auto iblocks = iblocks_t{ array_cast<byte_t>(blocks) };
        iterate_dispatch(state, iblocks);
    }
    else
    {
        iterate_(state, blocks);
    }
}

// Message Schedule (sigma vectorization).
// ----------------------------------------------------------------------------

TEMPLATE
template <typename xWord, if_extended<xWord>>
INLINE auto CLASS::
sigma0_8(auto x1, auto x2, auto x3, auto x4, auto x5, auto x6, auto x7,
    auto x8) NOEXCEPT
{
    return sigma0(set<xWord>(x1, x2, x3, x4, x5, x6, x7, x8));
}

TEMPLATE
template<size_t Round, size_t Offset>
INLINE void CLASS::
prepare1(buffer_t& buffer, const auto& xsigma0) NOEXCEPT
{
    static_assert(Round >= 16);
    constexpr auto r02 = Round - 2;
    constexpr auto r07 = Round - 7;
    constexpr auto r16 = Round - 16;
    constexpr auto s = SHA::word_bits;

    // buffer[r07 + 7] is buffer[Round + 0]
    // This is why sigma0 is limited to 8 lanes (vs 16).
    buffer[Round + Offset] = f::add<s>(
        f::add<s>(buffer[r16 + Offset], get<word_t, Offset>(xsigma0)),
        f::add<s>(buffer[r07 + Offset], sigma1(buffer[r02 + Offset])));
    buffer[r16 + Offset] = f::addc<K::get[r16 + Offset], s>(buffer[r16 + Offset]);
}

TEMPLATE
template<size_t Round>
INLINE void CLASS::
prepare8(buffer_t& buffer) NOEXCEPT
{
    // Requires avx512 for sha512 and avx2 for sha256.
    // The simplicity of sha160 message prepare precludes this optimization.
    static_assert(SHA::strength != 160);

    // sigma0x8 message scheduling for single block iteration.
    // Does not alter buffer structure, fully private to this method.
    // Tests of adding sigma1x2 half lanes vectorization show loss of ~10%.
    // Tests of adding sigma0x8 full lanes vectorization show gain of ~5%.
    constexpr auto r15 = Round - 15;
    const auto xsigma0 = sigma0_8<to_extended<word_t, 8>>(
        buffer[r15 + 0], buffer[r15 + 1], buffer[r15 + 2], buffer[r15 + 3],
        buffer[r15 + 4], buffer[r15 + 5], buffer[r15 + 6], buffer[r15 + 7]);

    prepare1<Round, 0>(buffer, xsigma0);
    prepare1<Round, 1>(buffer, xsigma0);
    prepare1<Round, 2>(buffer, xsigma0);
    prepare1<Round, 3>(buffer, xsigma0);
    prepare1<Round, 4>(buffer, xsigma0);
    prepare1<Round, 5>(buffer, xsigma0);
    prepare1<Round, 6>(buffer, xsigma0);
    prepare1<Round, 7>(buffer, xsigma0);
}

TEMPLATE
INLINE void CLASS::
schedule_invoke(buffer_t& buffer) NOEXCEPT
{
    if constexpr (have_lanes<word_t, 8>())
    {
        prepare8<16>(buffer);
        prepare8<24>(buffer);
        prepare8<32>(buffer);
        prepare8<40>(buffer);
        prepare8<48>(buffer);
        prepare8<56>(buffer);

        if constexpr (SHA::rounds == 80)
        {
            prepare8<64>(buffer);
            prepare8<72>(buffer);
        }

        add_k(buffer);
    }
    else
    {
        schedule_(buffer);
    }
}

TEMPLATE
INLINE void CLASS::
schedule_dispatch(auto& buffer) NOEXCEPT
{
    // buffer may be vectorized via merkle execution path.
    using word = decltype(buffer.front());

    // Schedule prepare vector dispatch.
    if constexpr ((SHA::strength != 160) && is_same_type<word, word_t> &&
                 ((have_x512 && (capacity<xint512_t, word_t> == 8u)) ||
                  (have_x256 && (capacity<xint256_t, word_t> == 8u)) ||
                  (have_x128 && (capacity<xint128_t, word_t> == 8u))))
    {
        schedule_invoke(buffer);
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

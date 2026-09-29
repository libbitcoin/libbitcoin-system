/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_MERKLE_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_MERKLE_IPP


// Merkle hashing.
// ============================================================================
// No merkle_hash optimizations for sha160 (double_hash requires half_t).

namespace libbitcoin {
namespace system {
namespace sha {

// expanded padding
// ----------------------------------------------------------------------------
// protected

TEMPLATE
template <typename xWord>
INLINE auto CLASS::
pack_pad_half() NOEXCEPT
{
    constexpr auto pad = chunk_pad();

    return xchunk_t<xWord>
    {
        f::broadcast<xWord>(pad[0]),
        f::broadcast<xWord>(pad[1]),
        f::broadcast<xWord>(pad[2]),
        f::broadcast<xWord>(pad[3]),
        f::broadcast<xWord>(pad[4]),
        f::broadcast<xWord>(pad[5]),
        f::broadcast<xWord>(pad[6]),
        f::broadcast<xWord>(pad[7])
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
            f::broadcast<xWord>(pad[0]),
            f::broadcast<xWord>(pad[1]),
            f::broadcast<xWord>(pad[2]),
            f::broadcast<xWord>(pad[3]),
            f::broadcast<xWord>(pad[4]),
            f::broadcast<xWord>(pad[5]),
            f::broadcast<xWord>(pad[6]),
            f::broadcast<xWord>(pad[7]),
            f::broadcast<xWord>(pad[8]),
            f::broadcast<xWord>(pad[9]),
            f::broadcast<xWord>(pad[10]),
            f::broadcast<xWord>(pad[11]),
            f::broadcast<xWord>(pad[12]),
            f::broadcast<xWord>(pad[13]),
            f::broadcast<xWord>(pad[14]),
            f::broadcast<xWord>(pad[15]),

            f::broadcast<xWord>(pad[16]),
            f::broadcast<xWord>(pad[17]),
            f::broadcast<xWord>(pad[18]),
            f::broadcast<xWord>(pad[19]),
            f::broadcast<xWord>(pad[20]),
            f::broadcast<xWord>(pad[21]),
            f::broadcast<xWord>(pad[22]),
            f::broadcast<xWord>(pad[23]),
            f::broadcast<xWord>(pad[24]),
            f::broadcast<xWord>(pad[25]),
            f::broadcast<xWord>(pad[26]),
            f::broadcast<xWord>(pad[27]),
            f::broadcast<xWord>(pad[28]),
            f::broadcast<xWord>(pad[29]),
            f::broadcast<xWord>(pad[30]),
            f::broadcast<xWord>(pad[31]),

            f::broadcast<xWord>(pad[32]),
            f::broadcast<xWord>(pad[33]),
            f::broadcast<xWord>(pad[34]),
            f::broadcast<xWord>(pad[35]),
            f::broadcast<xWord>(pad[36]),
            f::broadcast<xWord>(pad[37]),
            f::broadcast<xWord>(pad[38]),
            f::broadcast<xWord>(pad[39]),
            f::broadcast<xWord>(pad[40]),
            f::broadcast<xWord>(pad[41]),
            f::broadcast<xWord>(pad[42]),
            f::broadcast<xWord>(pad[43]),
            f::broadcast<xWord>(pad[44]),
            f::broadcast<xWord>(pad[45]),
            f::broadcast<xWord>(pad[46]),
            f::broadcast<xWord>(pad[47]),

            f::broadcast<xWord>(pad[48]),
            f::broadcast<xWord>(pad[49]),
            f::broadcast<xWord>(pad[50]),
            f::broadcast<xWord>(pad[51]),
            f::broadcast<xWord>(pad[52]),
            f::broadcast<xWord>(pad[53]),
            f::broadcast<xWord>(pad[54]),
            f::broadcast<xWord>(pad[55]),
            f::broadcast<xWord>(pad[56]),
            f::broadcast<xWord>(pad[57]),
            f::broadcast<xWord>(pad[58]),
            f::broadcast<xWord>(pad[59]),
            f::broadcast<xWord>(pad[60]),
            f::broadcast<xWord>(pad[61]),
            f::broadcast<xWord>(pad[62]),
            f::broadcast<xWord>(pad[63]),

            f::broadcast<xWord>(pad[64]),
            f::broadcast<xWord>(pad[65]),
            f::broadcast<xWord>(pad[66]),
            f::broadcast<xWord>(pad[67]),
            f::broadcast<xWord>(pad[68]),
            f::broadcast<xWord>(pad[69]),
            f::broadcast<xWord>(pad[70]),
            f::broadcast<xWord>(pad[71]),
            f::broadcast<xWord>(pad[72]),
            f::broadcast<xWord>(pad[73]),
            f::broadcast<xWord>(pad[74]),
            f::broadcast<xWord>(pad[75]),
            f::broadcast<xWord>(pad[76]),
            f::broadcast<xWord>(pad[77]),
            f::broadcast<xWord>(pad[78]),
            f::broadcast<xWord>(pad[79])
        };
    }
    else
    {
        return xbuffer_t<xWord>
        {
            f::broadcast<xWord>(pad[0]),
            f::broadcast<xWord>(pad[1]),
            f::broadcast<xWord>(pad[2]),
            f::broadcast<xWord>(pad[3]),
            f::broadcast<xWord>(pad[4]),
            f::broadcast<xWord>(pad[5]),
            f::broadcast<xWord>(pad[6]),
            f::broadcast<xWord>(pad[7]),
            f::broadcast<xWord>(pad[8]),
            f::broadcast<xWord>(pad[9]),
            f::broadcast<xWord>(pad[10]),
            f::broadcast<xWord>(pad[11]),
            f::broadcast<xWord>(pad[12]),
            f::broadcast<xWord>(pad[13]),
            f::broadcast<xWord>(pad[14]),
            f::broadcast<xWord>(pad[15]),

            f::broadcast<xWord>(pad[16]),
            f::broadcast<xWord>(pad[17]),
            f::broadcast<xWord>(pad[18]),
            f::broadcast<xWord>(pad[19]),
            f::broadcast<xWord>(pad[20]),
            f::broadcast<xWord>(pad[21]),
            f::broadcast<xWord>(pad[22]),
            f::broadcast<xWord>(pad[23]),
            f::broadcast<xWord>(pad[24]),
            f::broadcast<xWord>(pad[25]),
            f::broadcast<xWord>(pad[26]),
            f::broadcast<xWord>(pad[27]),
            f::broadcast<xWord>(pad[28]),
            f::broadcast<xWord>(pad[29]),
            f::broadcast<xWord>(pad[30]),
            f::broadcast<xWord>(pad[31]),

            f::broadcast<xWord>(pad[32]),
            f::broadcast<xWord>(pad[33]),
            f::broadcast<xWord>(pad[34]),
            f::broadcast<xWord>(pad[35]),
            f::broadcast<xWord>(pad[36]),
            f::broadcast<xWord>(pad[37]),
            f::broadcast<xWord>(pad[38]),
            f::broadcast<xWord>(pad[39]),
            f::broadcast<xWord>(pad[40]),
            f::broadcast<xWord>(pad[41]),
            f::broadcast<xWord>(pad[42]),
            f::broadcast<xWord>(pad[43]),
            f::broadcast<xWord>(pad[44]),
            f::broadcast<xWord>(pad[45]),
            f::broadcast<xWord>(pad[46]),
            f::broadcast<xWord>(pad[47]),

            f::broadcast<xWord>(pad[48]),
            f::broadcast<xWord>(pad[49]),
            f::broadcast<xWord>(pad[50]),
            f::broadcast<xWord>(pad[51]),
            f::broadcast<xWord>(pad[52]),
            f::broadcast<xWord>(pad[53]),
            f::broadcast<xWord>(pad[54]),
            f::broadcast<xWord>(pad[55]),
            f::broadcast<xWord>(pad[56]),
            f::broadcast<xWord>(pad[57]),
            f::broadcast<xWord>(pad[58]),
            f::broadcast<xWord>(pad[59]),
            f::broadcast<xWord>(pad[60]),
            f::broadcast<xWord>(pad[61]),
            f::broadcast<xWord>(pad[62]),
            f::broadcast<xWord>(pad[63])
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
template <size_t Round, typename xWord>
INLINE void CLASS::
prepare_half(xbuffer_t<xWord>& xbuffer) NOEXCEPT
{
    // Words 8..15 are constant padding, so terms of only those are constant.
    constexpr auto s = SHA::word_bits;
    constexpr auto pad = chunk_pad();

    if constexpr (Round == 23)
    {
        constexpr auto constant = sigma0(pad[0]);
        xbuffer[Round] = f::add<s>(
            f::add<s>(xbuffer[Round - 16], f::broadcast<xWord>(constant)),
            f::add<s>(xbuffer[Round - 7], sigma1(xbuffer[Round - 2])));
    }
    else if constexpr (Round >= 24 && Round <= 30)
    {
        constexpr auto constant = f::add<s>(pad[Round - 24],
            sigma0(pad[Round - 23]));
        xbuffer[Round] = f::add<s>(f::broadcast<xWord>(constant),
            f::add<s>(xbuffer[Round - 7], sigma1(xbuffer[Round - 2])));
    }
    else
    {
        prepare<Round>(xbuffer);
    }
}

TEMPLATE
template <typename xWord>
INLINE void CLASS::
schedule_half(xbuffer_t<xWord>& xbuffer) NOEXCEPT
{
    prepare_half<16>(xbuffer);
    prepare_half<17>(xbuffer);
    prepare_half<18>(xbuffer);
    prepare_half<19>(xbuffer);
    prepare_half<20>(xbuffer);
    prepare_half<21>(xbuffer);
    prepare_half<22>(xbuffer);
    prepare_half<23>(xbuffer);
    prepare_half<24>(xbuffer);
    prepare_half<25>(xbuffer);
    prepare_half<26>(xbuffer);
    prepare_half<27>(xbuffer);
    prepare_half<28>(xbuffer);
    prepare_half<29>(xbuffer);
    prepare_half<30>(xbuffer);
    prepare_half<31>(xbuffer);

    prepare_half<32>(xbuffer);
    prepare_half<33>(xbuffer);
    prepare_half<34>(xbuffer);
    prepare_half<35>(xbuffer);
    prepare_half<36>(xbuffer);
    prepare_half<37>(xbuffer);
    prepare_half<38>(xbuffer);
    prepare_half<39>(xbuffer);
    prepare_half<40>(xbuffer);
    prepare_half<41>(xbuffer);
    prepare_half<42>(xbuffer);
    prepare_half<43>(xbuffer);
    prepare_half<44>(xbuffer);
    prepare_half<45>(xbuffer);
    prepare_half<46>(xbuffer);
    prepare_half<47>(xbuffer);

    prepare_half<48>(xbuffer);
    prepare_half<49>(xbuffer);
    prepare_half<50>(xbuffer);
    prepare_half<51>(xbuffer);
    prepare_half<52>(xbuffer);
    prepare_half<53>(xbuffer);
    prepare_half<54>(xbuffer);
    prepare_half<55>(xbuffer);
    prepare_half<56>(xbuffer);
    prepare_half<57>(xbuffer);
    prepare_half<58>(xbuffer);
    prepare_half<59>(xbuffer);
    prepare_half<60>(xbuffer);
    prepare_half<61>(xbuffer);
    prepare_half<62>(xbuffer);
    prepare_half<63>(xbuffer);

    if constexpr (SHA::rounds == 80)
    {
        prepare_half<64>(xbuffer);
        prepare_half<65>(xbuffer);
        prepare_half<66>(xbuffer);
        prepare_half<67>(xbuffer);
        prepare_half<68>(xbuffer);
        prepare_half<69>(xbuffer);
        prepare_half<70>(xbuffer);
        prepare_half<71>(xbuffer);
        prepare_half<72>(xbuffer);
        prepare_half<73>(xbuffer);
        prepare_half<74>(xbuffer);
        prepare_half<75>(xbuffer);
        prepare_half<76>(xbuffer);
        prepare_half<77>(xbuffer);
        prepare_half<78>(xbuffer);
        prepare_half<79>(xbuffer);
    }
}

TEMPLATE
template <typename xWord>
INLINE const auto& CLASS::
scheduled_1() NOEXCEPT
{
    static const auto xscheduled_pad = pack_schedule_1<xWord>();
    return xscheduled_pad;
}

// expanded state
// ----------------------------------------------------------------------------
// protected

TEMPLATE
template <typename xWord>
INLINE auto CLASS::
pack(const state_t& state) NOEXCEPT
{
    return xstate_t<xWord>
    {
        f::broadcast<xWord>(state[0]),
        f::broadcast<xWord>(state[1]),
        f::broadcast<xWord>(state[2]),
        f::broadcast<xWord>(state[3]),
        f::broadcast<xWord>(state[4]),
        f::broadcast<xWord>(state[5]),
        f::broadcast<xWord>(state[6]),
        f::broadcast<xWord>(state[7])
    };
}

TEMPLATE
template <size_t Lane, typename xWord>
INLINE typename CLASS::digest_t CLASS::
unpack(const xstate_t<xWord>& xstate) NOEXCEPT
{
    // TODO: byteswap state in full one time before unpacking (vs. 8 times).
    return array_cast<byte_t>(state_t
    {
        f::get<word_t, Lane>(f::byteswap<word_t>(xstate[0])),
        f::get<word_t, Lane>(f::byteswap<word_t>(xstate[1])),
        f::get<word_t, Lane>(f::byteswap<word_t>(xstate[2])),
        f::get<word_t, Lane>(f::byteswap<word_t>(xstate[3])),
        f::get<word_t, Lane>(f::byteswap<word_t>(xstate[4])),
        f::get<word_t, Lane>(f::byteswap<word_t>(xstate[5])),
        f::get<word_t, Lane>(f::byteswap<word_t>(xstate[6])),
        f::get<word_t, Lane>(f::byteswap<word_t>(xstate[7]))
    });
}

TEMPLATE
template <typename xWord>
INLINE void CLASS::
xoutput(idigests_t& digests, const xstate_t<xWord>& xstate) NOEXCEPT
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

// vectorizable digest pairs hashing
// ----------------------------------------------------------------------------
// protected

TEMPLATE
constexpr void CLASS::
merkle_hash_(digests_t& digests, size_t offset) NOEXCEPT
{
    const auto blocks = to_half(digests.size());
    for (auto i = offset, j = offset * two; i < blocks; ++i, j += two)
        digests[i] = double_hash(digests[j], digests[add1(j)]);

    digests.resize(blocks);
}

TEMPLATE
template <typename xWord, if_extended<xWord>>
INLINE void CLASS::
merkle_hash_vector(idigests_t& digests, iblocks_t& blocks) NOEXCEPT
{
    BC_ASSERT(digests.size() == blocks.size());
    constexpr auto lanes = capacity<xWord, word_t>;
    static_assert(is_valid_lanes<lanes>);

    if constexpr (have<xWord>)
    {
        if (blocks.size() >= lanes)
        {
            // TODO: expose const structs to avoid local static.
            static const auto initial = pack<xWord>(H::get);

            xbuffer_t<xWord> xbuffer{};

            do
            {
                auto xstate = initial;

                // xinput() advances block iterator by lanes.
                xinput(xbuffer, blocks);
                schedule_<false>(xbuffer);
                compress_<zero, true>(xstate, xbuffer);
                compress_(xstate, scheduled_1<xWord>());

                // Second hash
                inject_left_half(xbuffer, xstate);
                pad_half(xbuffer);
                schedule_half(xbuffer);
                xstate = initial;
                compress_<zero, true>(xstate, xbuffer);

                // xoutput() advances digest iterator by lanes.
                xoutput(digests, xstate);
            }
            while (blocks.size() >= lanes);
        }
    }
}

TEMPLATE
INLINE void CLASS::
merkle_hash_native(idigests_t& digests, iblocks_t& blocks) NOEXCEPT
{
    BC_ASSERT(digests.size() == blocks.size());

    if constexpr (native_double)
    {
        while (blocks.size() >= two)
        {
            const auto& xblock = blocks.template to_array<two>();
            auto& xdigest = digests.template to_array<two>();
            native_double_hash(xdigest[0], xdigest[1], xblock[0], xblock[1]);
            blocks.template advance<two>();
            digests.template advance<two>();
        }
    }
}

TEMPLATE
INLINE void CLASS::
merkle_hash_vector(digests_t& digests) NOEXCEPT
{
    static_assert(sizeof(digest_t) == to_half(sizeof(block_t)));
    constexpr auto lanes = native_double ? two : min_lanes;
    auto next = zero;

    if (digests.size() >= lanes * two)
    {
        const auto data = digests.front().data();
        const auto size = digests.size() * array_count<digest_t>;
        auto iblocks = iblocks_t{ size, data };
        auto idigests = idigests_t{ to_half(size), data };
        const auto start = iblocks.size();

        // Only use if shani is not available.
        if constexpr (use_512 && !native_double)
            merkle_hash_vector<xint512_t>(idigests, iblocks);

        // Only use if shani is not available.
        if constexpr (use_256 && !native_double)
            merkle_hash_vector<xint256_t>(idigests, iblocks);

        // Only use if shani is not available.
        if constexpr (use_128 && !native_double)
            merkle_hash_vector<xint128_t>(idigests, iblocks);

        // Always use if available.
        if constexpr (native_double)
            merkle_hash_native(idigests, iblocks);

        // iblocks.size() is reduced by vectorization.
        next = start - iblocks.size();
    }

    // Complete rounds using normal form.
    merkle_hash_(digests, next);
}

// interface
// ----------------------------------------------------------------------------
// public

// TODO: consider eliminating endianness conversions internal to the root
// computation, instead converting on way in and way out ony, and using non
// converting input/output (nop) functions.

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
merkle_root(digests_t&& digests) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    if (is_zero(digests.size()))
        return {};

    while (!is_one(digests.size()))
    {
        if (is_odd(digests.size()))
            digests.push_back(digests.back());

        merkle_hash(digests);
    }

    return std::move(digests.front());
}

TEMPLATE
constexpr typename CLASS::digests_t& CLASS::
merkle_hash(digests_t& digests) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    if (std::is_constant_evaluated())
    {
        merkle_hash_(digests);
    }
    else if constexpr (vector || native_double)
    {
        // Merkle blocks are hashed at 2 native lanes (as available), otherwise
        // at 16/8/4 vector lanes (as available), and fall back to native or
        // normal (as available) for any remaining block.
        merkle_hash_vector(digests);
    }
    else
    {
        merkle_hash_(digests);
    }

    return digests;
};

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

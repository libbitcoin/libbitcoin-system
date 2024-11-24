/**
 * Copyright (c) 2011-2024 libbitcoin developers (see AUTHORS)
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

#include <type_traits>
#include <utility>

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
    // TODO: byteswap state in full one time before unpacking (vs. 8 times).
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
VCONSTEXPR void CLASS::
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

    if constexpr (have<xWord>())
    {
        if (blocks.size() >= lanes)
        {
            // TODO: expose const structs to avoid local static.
            static auto initial = pack<xWord>(H::get);

            xbuffer_t<xWord> xbuffer{};

            do
            {
                auto xstate = initial;

                // xinput() advances block iterator by lanes.
                xinput(xbuffer, blocks);
                schedule_(xbuffer);
                compress_(xstate, xbuffer);
                schedule_1(xbuffer);
                compress_(xstate, xbuffer);

                // Second hash
                reinput(xbuffer, xstate);
                pad_half(xbuffer);
                schedule_(xbuffer);
                xstate = initial;
                compress_(xstate, xbuffer);

                // xoutput() advances digest iterator by lanes.
                xoutput(digests, xstate);
            }
            while (blocks.size() >= lanes);
        }
    }
}

TEMPLATE
INLINE void CLASS::
merkle_hash_vector(digests_t& digests) NOEXCEPT
{
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
        if constexpr (use_x512)
            merkle_hash_vector<xint512_t>(idigests, iblocks);
        if constexpr (use_x256)
            merkle_hash_vector<xint256_t>(idigests, iblocks);
        if constexpr (use_x128)
            merkle_hash_vector<xint128_t>(idigests, iblocks);

        // iblocks.size() is reduced by vectorization.
        next = start - iblocks.size();
    }

    // Complete rounds using normal form.
    merkle_hash_(digests, next);
}

// interface
// ----------------------------------------------------------------------------
// public

TEMPLATE
VCONSTEXPR typename CLASS::digest_t CLASS::
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
VCONSTEXPR typename CLASS::digests_t& CLASS::
merkle_hash(digests_t& digests) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

// Avoid tautological warning (std::is_constant_evaluated() always false).
#if defined(HAVE_VECTOR_CONSTEXPR)
    if (std::is_constant_evaluated())
    {
        merkle_hash_(digests);
    }
    else
#endif
    if constexpr (vector)
    {
        // TODO: test vector vs. native performance for the 4 lane scenario.
        // Merkle block vectorization is applied at 16/8/4 lanes (as available)
        // and falls back to native/normal (as available) for 3/2/1 lanes.
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

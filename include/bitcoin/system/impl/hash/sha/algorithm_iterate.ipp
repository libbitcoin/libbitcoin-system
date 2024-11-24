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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_ITERATE_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_ITERATE_IPP

#include <type_traits>

// Vectorized message scheduling (across sets of dependent blocks).
// ============================================================================
// eprint.iacr.org/2012/067.pdf

namespace libbitcoin {
namespace system {
namespace sha {

// buffer packing (also used by Merkle for independent block full vectorize)
// ----------------------------------------------------------------------------
// protected

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
xinput(xbuffer_t<xWord>& xbuffer, iblocks_t& blocks) NOEXCEPT
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

// expanded buffer sequential compression
// ----------------------------------------------------------------------------
// protected

TEMPLATE
template <typename Word, size_t Lane, typename xWord,
    if_not_same<Word, xWord>>
    INLINE Word CLASS::
    extract(xWord a) NOEXCEPT
{
    // Extract word from lane of vectorized buffer.
    return get<Word, Lane>(a);
}

TEMPLATE
template <typename xWord>
INLINE void CLASS::
sequential_compress(state_t& state, const xbuffer_t<xWord>& xbuffer) NOEXCEPT
{
    // Each lane is a block, processed sequentially. This is necessary because
    // of the expanded buffer used to obtain vectorized message scheduling.
    // Sequential compression uses non-expanded state (normal form).
    constexpr auto lanes = capacity<xWord, word_t>;

    compress_<0>(state, xbuffer);
    compress_<1>(state, xbuffer);

    if constexpr (lanes >= 4)
    {
        compress_<2>(state, xbuffer);
        compress_<3>(state, xbuffer);
    }

    if constexpr (lanes >= 8)
    {
        compress_<4>(state, xbuffer);
        compress_<5>(state, xbuffer);
        compress_<6>(state, xbuffer);
        compress_<7>(state, xbuffer);
    }

    if constexpr (lanes >= 16)
    {
        compress_<8>(state, xbuffer);
        compress_<9>(state, xbuffer);
        compress_<10>(state, xbuffer);
        compress_<11>(state, xbuffer);
        compress_<12>(state, xbuffer);
        compress_<13>(state, xbuffer);
        compress_<14>(state, xbuffer);
        compress_<15>(state, xbuffer);
    }
}

// iteration
// ----------------------------------------------------------------------------
// protected

TEMPLATE
template <typename xWord, if_extended<xWord>>
INLINE void CLASS::
vector_schedule_sequential_compress(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    constexpr auto lanes = capacity<xWord, word_t>;
    static_assert(is_valid_lanes<lanes>);

    if constexpr (have<xWord>())
    {
        if (blocks.size() >= lanes)
        {
            xbuffer_t<xWord> xbuffer{};

            do
            {
                // input() advances block iterator by lanes.
                xinput(xbuffer, blocks);
                schedule_(xbuffer);

                // message scheduling (above) is vectorized but compression is
                // sequential, though it must operate over the expanded buffer.
                sequential_compress(state, xbuffer);
            } while (blocks.size() >= lanes);
        }
    }
}

TEMPLATE
INLINE void CLASS::
iterate_vector(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    if (blocks.size() >= min_lanes)
    {
        // Schedule iteration vector dispatch.
        if constexpr (use_x512)
            vector_schedule_sequential_compress<xint512_t>(state, blocks);
        if constexpr (use_x256)
            vector_schedule_sequential_compress<xint256_t>(state, blocks);
        if constexpr (use_x128)
            vector_schedule_sequential_compress<xint128_t>(state, blocks);
    }

    // Complete rounds using normal form.
    // blocks.size() is reduced by vectorization.
    iterate_(state, blocks);
}

TEMPLATE
template <size_t Size>
INLINE void CLASS::
iterate_vector(state_t& state, const ablocks_t<Size>& blocks) NOEXCEPT
{
    if (blocks.size() >= min_lanes)
    {
        auto iblocks = iblocks_t{ array_cast<byte_t>(blocks) };
        iterate_vector(state, iblocks);
    }
    else
    {
        iterate_(state, blocks);
    }
}

TEMPLATE
template <size_t Size>
INLINE constexpr void CLASS::
iterate_(state_t& state, const ablocks_t<Size>& blocks) NOEXCEPT
{
    buffer_t buffer{};
    for (auto& block: blocks)
    {
        input(buffer, block);
        schedule(buffer);
        compress(state, buffer);
    }
}

TEMPLATE
INLINE void CLASS::
iterate_(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    buffer_t buffer{};
    for (auto& block: blocks)
    {
        input(buffer, block);
        schedule(buffer);
        compress(state, buffer);
    }
}

TEMPLATE
template <size_t Size>
INLINE constexpr void CLASS::
iterate(state_t& state, const ablocks_t<Size>& blocks) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        iterate_(state, blocks);
    }
    else if constexpr (vector)
    {
        // Multi-block vectorized message scheduling optimization.
        iterate_vector(state, blocks);
    }
    else
    {
        iterate_(state, blocks);
    }
}

TEMPLATE
INLINE void CLASS::
iterate(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    if constexpr (vector)
    {
        // Multi-block vectorized message scheduling optimization.
        iterate_vector(state, blocks);
    }
    else
    {
        iterate_(state, blocks);
    }
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

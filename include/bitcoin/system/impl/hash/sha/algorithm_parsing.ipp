/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_PARSING_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_PARSING_IPP

#include <type_traits>

// 5.2 Parsing the Message
// ============================================================================
// big-endian I/O is conventional for SHA.

namespace libbitcoin {
namespace system {
namespace sha {

// protected
// ----------------------------------------------------------------------------

TEMPLATE
INLINE constexpr void CLASS::
input(buffer_t& buffer, const block_t& block) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        constexpr auto size = SHA::word_bytes;
        from_big<0 * size>(buffer.at(0), block);
        from_big<1 * size>(buffer.at(1), block);
        from_big<2 * size>(buffer.at(2), block);
        from_big<3 * size>(buffer.at(3), block);
        from_big<4 * size>(buffer.at(4), block);
        from_big<5 * size>(buffer.at(5), block);
        from_big<6 * size>(buffer.at(6), block);
        from_big<7 * size>(buffer.at(7), block);
        from_big<8 * size>(buffer.at(8), block);
        from_big<9 * size>(buffer.at(9), block);
        from_big<10 * size>(buffer.at(10), block);
        from_big<11 * size>(buffer.at(11), block);
        from_big<12 * size>(buffer.at(12), block);
        from_big<13 * size>(buffer.at(13), block);
        from_big<14 * size>(buffer.at(14), block);
        from_big<15 * size>(buffer.at(15), block);
    }
    else if constexpr (bc::is_little_endian)
    {
        // This optimization is neutral in 4/8/16 lane sha256 perf.
        ////if constexpr (have_lanes<word_t, 16> && !with_clang)
        ////{
        ////    using xword_t = to_extended<word_t, 16>;
        ////    const auto& in = array_cast<xword_t>(block);
        ////    auto& out = array_cast<xword_t>(buffer);
        ////    out[0] = byteswap<word_t>(in[0]);
        ////}
        ////else if constexpr (have_lanes<word_t, 8> && !with_clang)
        ////{
        ////    using xword_t = to_extended<word_t, 8>;
        ////    const auto& in = array_cast<xword_t>(block);
        ////    auto& out = array_cast<xword_t>(buffer);
        ////    out[0] = byteswap<word_t>(in[0]);
        ////    out[1] = byteswap<word_t>(in[1]);
        ////}
        ////else if constexpr (have_lanes<word_t, 4> && !with_clang)
        ////{
        ////    using xword_t = to_extended<word_t, 4>;
        ////    const auto& in = array_cast<xword_t>(block);
        ////    auto& out = array_cast<xword_t>(buffer);
        ////    out[0] = byteswap<word_t>(in[0]);
        ////    out[1] = byteswap<word_t>(in[1]);
        ////    out[2] = byteswap<word_t>(in[2]);
        ////    out[3] = byteswap<word_t>(in[3]);
        ////}

        const auto& in = array_cast<word_t>(block);
        buffer[0] = native_from_big_end(in[0]);
        buffer[1] = native_from_big_end(in[1]);
        buffer[2] = native_from_big_end(in[2]);
        buffer[3] = native_from_big_end(in[3]);
        buffer[4] = native_from_big_end(in[4]);
        buffer[5] = native_from_big_end(in[5]);
        buffer[6] = native_from_big_end(in[6]);
        buffer[7] = native_from_big_end(in[7]);
        buffer[8] = native_from_big_end(in[8]);
        buffer[9] = native_from_big_end(in[9]);
        buffer[10] = native_from_big_end(in[10]);
        buffer[11] = native_from_big_end(in[11]);
        buffer[12] = native_from_big_end(in[12]);
        buffer[13] = native_from_big_end(in[13]);
        buffer[14] = native_from_big_end(in[14]);
        buffer[15] = native_from_big_end(in[15]);
    }
    else
    {
        array_cast<word_t, SHA::block_words>(buffer) =
            array_cast<word_t>(block);
    }
}

TEMPLATE
INLINE constexpr void CLASS::
input_left(auto& buffer, const half_t& half) NOEXCEPT
{
    using word = array_element<buffer_t>;

    if (std::is_constant_evaluated())
    {
        constexpr auto size = SHA::word_bytes;
        from_big<0 * size>(buffer.at(0), half);
        from_big<1 * size>(buffer.at(1), half);
        from_big<2 * size>(buffer.at(2), half);
        from_big<3 * size>(buffer.at(3), half);
        from_big<4 * size>(buffer.at(4), half);
        from_big<5 * size>(buffer.at(5), half);
        from_big<6 * size>(buffer.at(6), half);
        from_big<7 * size>(buffer.at(7), half);
    }
    else if constexpr (bc::is_little_endian)
    {
        // This optimization is neutral in 4/8 lane sha256 perf.
        ////if constexpr (have_lanes<word_t, 8> && !with_clang)
        ////{
        ////    using xword_t = to_extended<word_t, 8>;
        ////    const auto& in = array_cast<xword_t>(half);
        ////    auto& out = array_cast<xword_t>(buffer);
        ////    out[0] = byteswap<word_t>(in[0]);
        ////}
        ////else if constexpr (have_lanes<word_t, 4> && !with_clang)
        ////{
        ////    using xword_t = to_extended<word_t, 4>;
        ////    const auto& in = array_cast<xword_t>(half);
        ////    auto& out = array_cast<xword_t>(buffer);
        ////    out[0] = byteswap<word_t>(in[0]);
        ////    out[1] = byteswap<word_t>(in[1]);
        ////}

        const auto& in = array_cast<word>(half);
        buffer[0] = native_from_big_end(in[0]);
        buffer[1] = native_from_big_end(in[1]);
        buffer[2] = native_from_big_end(in[2]);
        buffer[3] = native_from_big_end(in[3]);
        buffer[4] = native_from_big_end(in[4]);
        buffer[5] = native_from_big_end(in[5]);
        buffer[6] = native_from_big_end(in[6]);
        buffer[7] = native_from_big_end(in[7]);
    }
    else
    {
        array_cast<word, SHA::chunk_words>(buffer) = array_cast<word>(half);
    }
}

TEMPLATE
INLINE constexpr void CLASS::
input_right(auto& buffer, const half_t& half) NOEXCEPT
{
    using word = array_element<buffer_t>;

    if (std::is_constant_evaluated())
    {
        constexpr auto size = SHA::word_bytes;
        from_big<0 * size>(buffer.at(8), half);
        from_big<1 * size>(buffer.at(9), half);
        from_big<2 * size>(buffer.at(10), half);
        from_big<3 * size>(buffer.at(11), half);
        from_big<4 * size>(buffer.at(12), half);
        from_big<5 * size>(buffer.at(13), half);
        from_big<6 * size>(buffer.at(14), half);
        from_big<7 * size>(buffer.at(15), half);
    }
    else if constexpr (bc::is_little_endian)
    {
        // This optimization is neutral in 4/8 lane sha256 perf.
        ////if constexpr (have_lanes<word_t, 8> && !with_clang)
        ////{
        ////    using xword_t = to_extended<word_t, 8>;
        ////    const auto& in = array_cast<xword_t>(half);
        ////    auto& out = array_cast<xword_t>(buffer);
        ////    out[1] = byteswap<word_t>(in[0]);
        ////}
        ////else if constexpr (have_lanes<word_t, 4> && !with_clang)
        ////{
        ////    using xword_t = to_extended<word_t, 4>;
        ////    const auto& in = array_cast<xword_t>(half);
        ////    auto& out = array_cast<xword_t>(buffer);
        ////    out[2] = byteswap<word_t>(in[0]);
        ////    out[3] = byteswap<word_t>(in[1]);
        ////}

        const auto& in = array_cast<word>(half);
        buffer[8] = native_from_big_end(in[0]);
        buffer[9] = native_from_big_end(in[1]);
        buffer[10] = native_from_big_end(in[2]);
        buffer[11] = native_from_big_end(in[3]);
        buffer[12] = native_from_big_end(in[4]);
        buffer[13] = native_from_big_end(in[5]);
        buffer[14] = native_from_big_end(in[6]);
        buffer[15] = native_from_big_end(in[7]);
    }
    else
    {
        array_cast<word, SHA::chunk_words, SHA::chunk_words>(buffer) =
            array_cast<word>(half);
    }
}

TEMPLATE
INLINE constexpr void CLASS::
input_left(auto& buffer, const quart_t& quarter) NOEXCEPT
{
    using word = array_element<buffer_t>;

    if (std::is_constant_evaluated())
    {
        constexpr auto size = SHA::word_bytes;
        from_big<0 * size>(buffer.at(0), quarter);
        from_big<1 * size>(buffer.at(1), quarter);
        from_big<2 * size>(buffer.at(2), quarter);
        from_big<3 * size>(buffer.at(3), quarter);
    }
    else if constexpr (bc::is_little_endian)
    {
        const auto& in = array_cast<word>(quarter);
        buffer[0] = native_from_big_end(in[0]);
        buffer[1] = native_from_big_end(in[1]);
        buffer[2] = native_from_big_end(in[2]);
        buffer[3] = native_from_big_end(in[3]);
    }
    else
    {
        constexpr auto short_words = to_half(SHA::chunk_words);
        array_cast<word, short_words>(buffer) = array_cast<word>(quarter);
    }
}

TEMPLATE
INLINE constexpr void CLASS::
input_right(auto& buffer, const quart_t& quarter) NOEXCEPT
{
    using word = array_element<buffer_t>;

    if (std::is_constant_evaluated())
    {
        constexpr auto size = SHA::word_bytes;
        from_big<0 * size>(buffer.at(4), quarter);
        from_big<1 * size>(buffer.at(5), quarter);
        from_big<2 * size>(buffer.at(6), quarter);
        from_big<3 * size>(buffer.at(7), quarter);
    }
    else if constexpr (bc::is_little_endian)
    {
        const auto& in = array_cast<word>(quarter);
        buffer[4] = native_from_big_end(in[0]);
        buffer[5] = native_from_big_end(in[1]);
        buffer[6] = native_from_big_end(in[2]);
        buffer[7] = native_from_big_end(in[3]);
    }
    else
    {
        constexpr auto short_words = to_half(SHA::chunk_words);
        array_cast<word, short_words, short_words>(buffer) =
            array_cast<word>(quarter);
    }
}

TEMPLATE
INLINE constexpr typename CLASS::digest_t CLASS::
output(const state_t& state) NOEXCEPT
{
    // Digest is truncated for 256_224, 512_384, 512_256, 512_224.
    constexpr auto digest_bytes = array_count<digest_t>;

    if (std::is_constant_evaluated())
    {
        constexpr auto size = SHA::word_bytes;
        constexpr auto words = floored_divide(digest_bytes, size);

        // digest   word    digest  word    state   digest previous
        // bits     bits    bytes   bytes   words   words  floored
        // --------------------------------------------------------
        // 224      64      28      8       8       3.5     3
        // 256      64      32      8       8       4       4
        // 160      32      20      4       5       5       5
        // 384      64      48      8       8       6       6
        // 224      32      28      4       8       7       7
        // 512      64      64      8       8       8       8
        // 256      32      32      4       8       8       8

        digest_t digest{};

        // all
        if constexpr (words > 2)
        {
            // Convert first three words.
            to_big<0 * size>(digest, state.at(0));
            to_big<1 * size>(digest, state.at(1));
            to_big<2 * size>(digest, state.at(2));
        }

        // sha512-224
        if constexpr (words == 3)
        {
            // Convert fourth word into temporary.
            data_array<size> partial{};
            to_big<0>(partial, state.at(3));

            // Copy first half of fourth word into end of digest.
            digest.at(3 * size + 0) = partial.at(0);
            digest.at(3 * size + 1) = partial.at(1);
            digest.at(3 * size + 2) = partial.at(2);
            digest.at(3 * size + 3) = partial.at(3);
        }

        // sha512-256
        if constexpr (words > 3)
            to_big<3 * size>(digest, state.at(3));

        // sha160
        if constexpr (words > 4)
            to_big<4 * size>(digest, state.at(4));

        // sha256-384
        if constexpr (words > 5)
            to_big<5 * size>(digest, state.at(5));

        // sha256-224
        if constexpr (words > 6)
            to_big<6 * size>(digest, state.at(6));

        // sha256, sha512
        if constexpr (words > 7)
            to_big<7 * size>(digest, state.at(7));

        return digest;
    }
    else if constexpr (bc::is_little_endian)
    {
        // Below could be optimized to remove conversions that truncate, but
        // 224 truncates on a half word boundary, complicating implementation.

        if constexpr (SHA::strength != 160)
        {
            // This optimization is neutral in 4/8 lane sha256 perf.
            ////if constexpr (have_lanes<word_t, 8> && !with_clang)
            ////{
            ////    using xword_t = to_extended<word_t, 8>;
            ////    const auto& in = array_cast<xword_t>(state);
            ////    return array_cast<byte_t, digest_bytes>(wstate_t<xword_t>
            ////    {
            ////        byteswap<word_t>(in[0])
            ////    });
            ////}
            ////else if constexpr (have_lanes<word_t, 4> && !with_clang)
            ////{
            ////    using xword_t = to_extended<word_t, 4>;
            ////    const auto& in = array_cast<xword_t>(state);
            ////    return array_cast<byte_t, digest_bytes>(wstate_t<xword_t>
            ////    {
            ////        byteswap<word_t>(in[0]),
            ////        byteswap<word_t>(in[1])
            ////    });
            ////}

            return array_cast<byte_t, digest_bytes>(state_t
            {
                native_to_big_end(state[0]),
                native_to_big_end(state[1]),
                native_to_big_end(state[2]),
                native_to_big_end(state[3]),
                native_to_big_end(state[4]),
                native_to_big_end(state[5]),
                native_to_big_end(state[6]),
                native_to_big_end(state[7])
            });
        }
        else
        {
            return array_cast<byte_t, digest_bytes>(state_t
            {
                native_to_big_end(state[0]),
                native_to_big_end(state[1]),
                native_to_big_end(state[2]),
                native_to_big_end(state[3]),
                native_to_big_end(state[4])
            });
        }
    }
    else
    {
        return array_cast<byte_t, digest_bytes>(state);
    }
}

// protected
// ----------------------------------------------------------------------------
// These bypass endianness, used for padding and state reintroduction.

TEMPLATE
INLINE constexpr void CLASS::
inject_left(auto& buffer, const auto& left) NOEXCEPT
{
    using words = decltype(buffer);
    static_assert(array_count<words> >= SHA::state_words);

    if (std::is_constant_evaluated())
    {
        buffer.at(0) = left.at(0);
        buffer.at(1) = left.at(1);
        buffer.at(2) = left.at(2);
        buffer.at(3) = left.at(3);
        buffer.at(4) = left.at(4);
        buffer.at(5) = left.at(5);
        buffer.at(6) = left.at(6);
        buffer.at(7) = left.at(7);
    }
    else
    {
        using word = array_element<words>;
        array_cast<word, SHA::state_words>(buffer) = left;
    }
}

TEMPLATE
INLINE constexpr void CLASS::
inject_right(auto& buffer, const auto& right) NOEXCEPT
{
    using words = decltype(buffer);
    static_assert(array_count<words> >= SHA::state_words);

    if (std::is_constant_evaluated())
    {
        buffer.at(8) = right.at(0);
        buffer.at(9) = right.at(1);
        buffer.at(10) = right.at(2);
        buffer.at(11) = right.at(3);
        buffer.at(12) = right.at(4);
        buffer.at(13) = right.at(5);
        buffer.at(14) = right.at(6);
        buffer.at(15) = right.at(7);
    }
    else
    {
        using word = array_element<words>;
        array_cast<word, SHA::state_words, SHA::state_words>(buffer) = right;
    }
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

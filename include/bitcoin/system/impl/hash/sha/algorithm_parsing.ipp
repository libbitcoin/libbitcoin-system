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
        // TODO: evaluate 4/8/16 lane optimization using byteswap.
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
input_left(buffer_t& buffer, const half_t& half) NOEXCEPT
{
    using word = array_element<buffer_t>;

    if (std::is_constant_evaluated())
    {
        // TODO: evaluate 4/8 lane optimization using byteswap.
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
input_right(buffer_t& buffer, const half_t& half) NOEXCEPT
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
        // TODO: evaluate 4/8 lane optimization using byteswap.
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
INLINE constexpr typename CLASS::digest_t CLASS::
output(const state_t& state) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        digest_t digest{};

        constexpr auto size = SHA::word_bytes;
        to_big<0 * size>(digest, state.at(0));
        to_big<1 * size>(digest, state.at(1));
        to_big<2 * size>(digest, state.at(2));
        to_big<3 * size>(digest, state.at(3));
        to_big<4 * size>(digest, state.at(4));

        if constexpr (SHA::strength != 160)
        {
            to_big<5 * size>(digest, state.at(5));
            to_big<6 * size>(digest, state.at(6));
            to_big<7 * size>(digest, state.at(7));
        }

        return digest;
    }
    else if constexpr (bc::is_little_endian)
    {
        if constexpr (SHA::strength == 160)
        {
            return array_cast<byte_t>(state_t
            {
                native_to_big_end(state[0]),
                native_to_big_end(state[1]),
                native_to_big_end(state[2]),
                native_to_big_end(state[3]),
                native_to_big_end(state[4])
            });
        }
        else
        {
            // TODO: evaluate 4/8 lane optimization using byteswap.
            return array_cast<byte_t>(state_t
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
    }
    else
    {
        return array_cast<byte_t>(state);
    }
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

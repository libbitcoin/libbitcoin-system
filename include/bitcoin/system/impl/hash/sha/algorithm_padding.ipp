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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_PADDING_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_PADDING_IPP

#include <type_traits>

// 5.1 Padding the Message
// ============================================================================

namespace libbitcoin {
namespace system {
namespace sha {

// protected
// ----------------------------------------------------------------------------

TEMPLATE
template<size_t Blocks>
CONSTEVAL typename CLASS::buffer_t CLASS::
scheduled_pad() NOEXCEPT
{
    // This precomputed padding is limited to one word of counter.
    static_assert(Blocks <= maximum<word_t> / byte_bits);

    // See comments in accumulator regarding padding endianness.
    constexpr auto index = sub1(array_count<words_t>);
    constexpr auto bytes = safe_multiply(Blocks, array_count<block_t>);

    buffer_t out{};
    out.front() = bit_hi<word_t>;
    out.at(index) = possible_narrow_cast<word_t>(to_bits(bytes));
    schedule(out);
    return out;
}

TEMPLATE
CONSTEVAL typename CLASS::chunk_t CLASS::
chunk_pad() NOEXCEPT
{
    // See comments in accumulator regarding padding endianness.
    constexpr auto bytes = possible_narrow_cast<word_t>(array_count<half_t>);

    chunk_t out{};
    out.front() = bit_hi<word_t>;
    out.back() = to_bits(bytes);
    return out;
}

TEMPLATE
CONSTEVAL typename CLASS::pad_t CLASS::
stream_pad() NOEXCEPT
{
    // See comments in accumulator regarding padding endianness.
    pad_t out{};
    out.front() = bit_hi<word_t>;
    return out;
}

TEMPLATE
template<size_t Blocks>
constexpr void CLASS::
schedule_n(buffer_t& buffer) NOEXCEPT
{
    // Scheduled padding for n whole blocks.
    // This will compile in 4*64 (sha256) or 4*128 (sha512) bytes for each
    // unique size of blocks array hashed by callers (by template expansion).
    // and one for each that is cached for block vectors in schedule_n().
    constexpr auto pad = scheduled_pad<Blocks>();
    buffer = pad;
}

TEMPLATE
constexpr void CLASS::
schedule_n(buffer_t& buffer, size_t blocks) NOEXCEPT
{
    // This optimization saves ~30% in message scheduling for one out of N
    // blocks: (N + 70%)/(N + 100%). So benefit decreases with increasing N.
    // For arbitrary data lengths this will benefit 1/64 hashes on average.
    // All array-sized n-block hashes have precomputed schedules - this
    // benefits only finalized chunk hashing. Testing shows a 5% performance
    // improvement for 128 byte chunk hashes. Accumulator passes all write()
    // of more than one block here.
    if constexpr (caching)
    {
        switch (blocks)
        {
            case 1: schedule_n<1>(buffer); return;
            case 2: schedule_n<2>(buffer); return;
            case 3: schedule_n<3>(buffer); return;
            case 4: schedule_n<4>(buffer); return;
            default:
            {
                pad_n(buffer, blocks);
                schedule(buffer);
                return;
            }
        }
    }
    else
    {
        pad_n(buffer, blocks);
        schedule(buffer);
        return;
    }
}

TEMPLATE
constexpr void CLASS::
schedule_1(buffer_t& buffer) NOEXCEPT
{
    // This ensures single block padding is always prescheduled.
    schedule_n<one>(buffer);
}

TEMPLATE
constexpr void CLASS::
pad_half(buffer_t& buffer) NOEXCEPT
{
    // Pad for any half block, unscheduled buffer.
    constexpr auto pad = chunk_pad();

    if (std::is_constant_evaluated())
    {
        buffer.at(8) = pad.at(0);
        buffer.at(9) = pad.at(1);
        buffer.at(10) = pad.at(2);
        buffer.at(11) = pad.at(3);
        buffer.at(12) = pad.at(4);
        buffer.at(13) = pad.at(5);
        buffer.at(14) = pad.at(6);
        buffer.at(15) = pad.at(7);
    }
    else
    {
        array_cast<word_t, SHA::chunk_words, SHA::chunk_words>(buffer) = pad;
    }
}

TEMPLATE
constexpr void CLASS::
pad_n(buffer_t& buffer, count_t blocks) NOEXCEPT
{
    // Pad any number of whole blocks, unscheduled buffer.
    constexpr auto pad = stream_pad();
    const auto bits = to_bits(blocks * array_count<block_t>);

    if (std::is_constant_evaluated())
    {
        buffer.at(0)  = pad.at(0);
        buffer.at(1)  = pad.at(1);
        buffer.at(2)  = pad.at(2);
        buffer.at(3)  = pad.at(3);
        buffer.at(4)  = pad.at(4);
        buffer.at(5)  = pad.at(5);
        buffer.at(6)  = pad.at(6);
        buffer.at(7)  = pad.at(7);
        buffer.at(8)  = pad.at(8);
        buffer.at(9)  = pad.at(9);
        buffer.at(10) = pad.at(10);
        buffer.at(11) = pad.at(11);
        buffer.at(12) = pad.at(12);
        buffer.at(13) = pad.at(13);
        buffer.at(14) = hi_word<word_t>(bits);
        buffer.at(15) = lo_word<word_t>(bits);
    }
    else
    {
        array_cast<word_t, array_count<pad_t>>(buffer) = pad;

        // Split count into hi/low words and assign end of padded buffer.
        buffer[14] = hi_word<word_t>(bits);
        buffer[15] = lo_word<word_t>(bits);
    }
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

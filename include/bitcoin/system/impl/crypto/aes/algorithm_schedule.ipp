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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_AES_ALGORITHM_SCHEDULE_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_AES_ALGORITHM_SCHEDULE_IPP

#include <algorithm>
#include <iterator>

// Key schedule
// ============================================================================
// Words are little-endian, so RotWord is a right rotation by one byte and
// the round constant is applied to the low order byte.

namespace libbitcoin {
namespace system {
namespace aes {

// public
// ----------------------------------------------------------------------------

TEMPLATE
constexpr typename CLASS::schedule_t CLASS::
expand(const key_t& key) NOEXCEPT
{
    constexpr std_array<uint32_t, 10> rcon
    {
        0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
    };

    constexpr auto nk = K::key_words;
    constexpr auto count = K::round_keys * 4_size;
    std_array<uint32_t, count> words{};

    for (size_t word{}; word < nk; ++word)
    {
        block_t block{};
        for (size_t byte{}; byte < 4_size; ++byte)
            block[byte] = key[word * 4_size + byte];

        words[word] = from_little<uint32_t, 0>(block);
    }

    for (auto word = nk; word < count; ++word)
    {
        auto temp = words[sub1(word)];

        if (is_zero(word % nk))
            temp = bit_xor(sub_word(rotr<8>(temp)), rcon[sub1(word / nk)]);
        else if ((nk > 6_size) && ((word % nk) == 4_size))
            temp = sub_word(temp);

        words[word] = bit_xor(words[word - nk], temp);
    }

    schedule_t schedule{};
    for (size_t round{}; round < K::round_keys; ++round)
    {
        words_t round_key{};
        const auto from = std::next(words.cbegin(), round * round_key.size());
        std::copy_n(from, round_key.size(), round_key.begin());
        schedule[round] = from_words(round_key);
    }

    return schedule;
}

// protected
// ----------------------------------------------------------------------------

// The bytes of the word are substituted in the plane positions of one block.
TEMPLATE
constexpr uint32_t CLASS::
sub_word(uint32_t word) NOEXCEPT
{
    xplanes_t<uint64_t> q{};
    q.front() = wide_cast<uint64_t>(word);
    ortho(q);
    sbox(q);
    ortho(q);
    return narrow_cast<uint32_t>(q.front());
}

// Each round key is sliced as each of the four blocks.
TEMPLATE
constexpr typename CLASS::template xkeys_t<uint64_t> CLASS::
slice(const schedule_t& schedule) NOEXCEPT
{
    xkeys_t<uint64_t> keys{};
    for (size_t round{}; round < K::round_keys; ++round)
    {
        auto& q = keys[round];
        interleave_in(q[0], q[4], to_words(schedule[round]));
        q[1] = q[2] = q[3] = q[0];
        q[5] = q[6] = q[7] = q[4];
        ortho(q);
    }

    return keys;
}

TEMPLATE
template <typename xWord>
INLINE typename CLASS::template xkeys_t<xWord> CLASS::
broadcast(const xkeys_t<uint64_t>& keys) NOEXCEPT
{
    xkeys_t<xWord> wide{};
    for (size_t round{}; round < K::round_keys; ++round)
        for (size_t plane{}; plane < planes; ++plane)
            wide[round][plane] = f::broadcast<xWord>(keys[round][plane]);

    return wide;
}

} // namespace aes
} // namespace system
} // namespace libbitcoin

#endif

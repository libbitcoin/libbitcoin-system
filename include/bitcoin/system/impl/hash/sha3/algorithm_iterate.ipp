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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA3_ALGORITHM_ITERATE_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA3_ALGORITHM_ITERATE_IPP

// 4 SPONGE CONSTRUCTION (absorbing)
// ============================================================================

namespace libbitcoin {
namespace system {
namespace sha3 {

// protected
// ----------------------------------------------------------------------------

TEMPLATE
constexpr void CLASS::
absorb(state_t& state, const block_t& block) NOEXCEPT
{
    words_t words{};
    input(words, block);

    for (size_t word = 0; word < SHA3::block_words; ++word)
        state[word] ^= words[word];

    permute(state);
}

TEMPLATE
template <size_t Size>
constexpr void CLASS::
iterate(state_t& state, const ablocks_t<Size>& blocks) NOEXCEPT
{
    for (const auto& block: blocks)
        absorb(state, block);
}

TEMPLATE
void CLASS::
iterate(state_t& state, iblocks_t& blocks) NOEXCEPT
{
    for (auto& block: blocks)
        absorb(state, block);
}

} // namespace sha3
} // namespace system
} // namespace libbitcoin

#endif

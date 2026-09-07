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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA3_ALGORITHM_PARSING_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA3_ALGORITHM_PARSING_IPP

// B.1 Conversion Functions
// ============================================================================
// little-endian I/O is conventional for SHA3 (lanes are little-endian).

namespace libbitcoin {
namespace system {
namespace sha3 {

// protected
// ----------------------------------------------------------------------------

TEMPLATE
template <size_t ...Words>
INLINE constexpr void CLASS::
input_(words_t& words, const block_t& block,
    std::index_sequence<Words...>) NOEXCEPT
{
    (from_little<Words * SHA3::word_bytes>(words[Words], block), ...);
}

TEMPLATE
INLINE constexpr void CLASS::
input(words_t& words, const block_t& block) NOEXCEPT
{
    input_(words, block, std::make_index_sequence<SHA3::block_words>{});
}

TEMPLATE
template <size_t ...Words>
INLINE constexpr void CLASS::
output_(block_t& bytes, const state_t& state,
    std::index_sequence<Words...>) NOEXCEPT
{
    (to_little<Words * SHA3::word_bytes>(bytes, state[Words]), ...);
}

TEMPLATE
INLINE constexpr typename CLASS::digest_t CLASS::
output(const state_t& state) NOEXCEPT
{
    // The digest is the leading bytes of the rate (single squeeze).
    static_assert(array_count<digest_t> <= block_bytes);
    block_t bytes{};
    output_(bytes, state, std::make_index_sequence<SHA3::block_words>{});

    digest_t digest{};
    std::copy_n(bytes.begin(), array_count<digest_t>, digest.begin());
    return digest;
}

} // namespace sha3
} // namespace system
} // namespace libbitcoin

#endif

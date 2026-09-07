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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA3_ALGORITHM_STREAM_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA3_ALGORITHM_STREAM_IPP

// Streamed hashing (explicitly finalized).
// ============================================================================

namespace libbitcoin {
namespace system {
namespace sha3 {

// public
// ----------------------------------------------------------------------------

TEMPLATE
void CLASS::
accumulate(state_t& state, iblocks_t&& blocks) NOEXCEPT
{
    iterate(state, blocks);
}

TEMPLATE
constexpr void CLASS::
accumulate(state_t& state, const block_t& block) NOEXCEPT
{
    // As an array of 1 arrays is same as the array, this compiles away.
    iterate(state, ablocks_t<one>{ block });
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
finalize(state_t& state) NOEXCEPT
{
    return finalize(state, bytes_t<zero>{});
}

TEMPLATE
template <size_t Size, if_not_greater<Size, CLASSIF::space>>
constexpr typename CLASS::digest_t CLASS::
finalize(state_t& state, const bytes_t<Size>& tail) NOEXCEPT
{
    absorb(state, pad(tail));
    return output(state);
}

// protected
// ----------------------------------------------------------------------------

TEMPLATE
typename CLASS::digest_t CLASS::
finalize(state_t& state, size_t size, const byte_t* tail) NOEXCEPT
{
    absorb(state, pad(size, tail));
    return output(state);
}

} // namespace sha3
} // namespace system
} // namespace libbitcoin

#endif

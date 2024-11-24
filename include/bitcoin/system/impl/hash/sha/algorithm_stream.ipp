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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_STREAM_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_STREAM_IPP

// Streamed hashing (explicitly finalized).
// ============================================================================

namespace libbitcoin {
namespace system {
namespace sha {

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
    buffer_t buffer{};
    input(buffer, block);
    schedule(buffer);
    compress(state, buffer);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
normalize(const state_t& state) NOEXCEPT
{
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
finalize(state_t& state, size_t blocks) NOEXCEPT
{
    buffer_t buffer{};
    schedule_n(buffer, blocks);
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
finalize_second(const state_t& state) NOEXCEPT
{
    // No hash(state_t) optimizations for sha160 (requires chunk_t/half_t).
    static_assert(is_same_type<state_t, chunk_t>);

    buffer_t buffer{};
    auto state2 = H::get;
    reinput(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    compress(state2, buffer);
    return output(state2);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
finalize_double(state_t& state, size_t blocks) NOEXCEPT
{
    // The state out parameter is updated for first hash.
    buffer_t buffer{};
    schedule_n(buffer, blocks);
    compress(state, buffer);

    // Second hash
    reinput(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    auto state2 = H::get;
    compress(state2, buffer);
    return output(state2);
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

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
    // As an array of a 1 arrays is the same as the array, this compiles away.
    iterate(state, ablocks_t<one>{ block });
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
normalize(const state_t& state) NOEXCEPT
{
    return output(state);
}

TEMPLATE
template <size_t Blocks>
constexpr typename CLASS::digest_t CLASS::
finalize(state_t& state) NOEXCEPT
{
    const auto finalizer = [](state_t& state) NOEXCEPT
    {
        buffer_t buffer{};
        schedule_n<Blocks>(buffer);
        compress(state, buffer);
        return output(state);
    };

    if (std::is_constant_evaluated())
    {
        return finalizer(state);
    }
    else if constexpr (native && SHA::strength == 256)
    {
        return native_finalize<Blocks>(state);
    }
    else
    {
        return finalizer(state);
    }
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
finalize(state_t& state, size_t blocks) NOEXCEPT
{
    const auto finalizer = [](state_t& state, size_t blocks) NOEXCEPT
    {
        buffer_t buffer{};
        schedule_n(buffer, blocks);
        compress(state, buffer);
        return output(state);
    };

    if (std::is_constant_evaluated())
    {
        return finalizer(state, blocks);
    }
    else if constexpr (native && SHA::strength == 256)
    {
        return native_finalize(state, blocks);
    }
    else
    {
        return finalizer(state, blocks);
    }
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
finalize_second(const state_t& state) NOEXCEPT
{
    // No hash(state_t) optimizations for sha160 (requires chunk_t/half_t).
    static_assert(is_same_type<state_t, chunk_t>);

    // This hashes a hash result (state) without the endianness conversion.
    const auto finalizer = [](const state_t& state) NOEXCEPT
    {
        auto state2 = H::get;
        buffer_t buffer{};
        inject_left(buffer, state);
        pad_half(buffer);
        schedule(buffer);
        compress(state2, buffer);
        return output(state2);
    };

    if (std::is_constant_evaluated())
    {
        return finalizer(state);
    }
    else if constexpr (native && SHA::strength == 256)
    {
        return native_finalize_second(state);
    }
    else
    {
        return finalizer(state);
    }
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
finalize_double(state_t& state, size_t blocks) NOEXCEPT
{
    // Pad a hash state from a number of blocks.
    const auto finalizer = [](state_t& state, size_t blocks) NOEXCEPT
    {
        buffer_t buffer{};
        schedule_n(buffer, blocks);
        compress(state, buffer);

        // This is finalize_second() but reuses the initial buffer.
        auto state2 = H::get;
        inject_left(buffer, state);
        pad_half(buffer);
        schedule(buffer);
        compress(state2, buffer);

        return output(state2);
    };

    if (std::is_constant_evaluated())
    {
        return finalizer(state, blocks);
    }
    else if constexpr (native && SHA::strength == 256)
    {
        return native_finalize_double(state, blocks);
    }
    else
    {
        return finalizer(state, blocks);
    }
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_SINGLE_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_SINGLE_IPP

// Single hashing.
// ============================================================================

namespace libbitcoin {
namespace system {
namespace sha {

// public
// ----------------------------------------------------------------------------

TEMPLATE
template <size_t Size>
constexpr typename CLASS::digest_t CLASS::
hash(const ablocks_t<Size>& blocks) NOEXCEPT
{
    auto state = H::get;
    iterate(state, blocks);
    return finalize<Size>(state);
}

TEMPLATE
typename CLASS::digest_t CLASS::
hash(iblocks_t&& blocks) NOEXCEPT
{
    // Save block count, as iterable decrements.
    const auto count = blocks.size();
    auto state = H::get;
    iterate(state, blocks);
    return finalize(state, count);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const block_t& block) NOEXCEPT
{
    // As an array of a 1 arrays is the same as the array, this compiles away.
    return hash(ablocks_t<one>{ block });
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const half_t& half) NOEXCEPT
{
    const auto hash1 = [](const half_t& half) NOEXCEPT
    {
        auto state = H::get;
        buffer_t buffer{};
        input_left(buffer, half);
        pad_half(buffer);
        schedule(buffer);
        compress(state, buffer);
        return output(state);
    };

    if (std::is_constant_evaluated())
    {
        return hash1(half);
    }
    else if constexpr (native && SHA::strength == 256)
    {
        return native_hash(half);
    }
    else
    {
        return hash1(half);
    }
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const half_t& left, const half_t& right) NOEXCEPT
{
    const auto hash1 = [](const half_t& left, const half_t& right) NOEXCEPT
    {
        auto state = H::get;
        buffer_t buffer{};
        input_left(buffer, left);
        input_right(buffer, right);
        schedule(buffer);
        compress(state, buffer);
        schedule_1(buffer);
        compress(state, buffer);
        return output(state);
    };

    if (std::is_constant_evaluated())
    {
        return hash1(left, right);
    }
    else if constexpr (native && SHA::strength == 256)
    {
        return native_hash(left, right);
    }
    else
    {
        return hash1(left, right);
    }
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

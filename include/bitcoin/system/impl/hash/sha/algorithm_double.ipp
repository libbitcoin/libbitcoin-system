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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_DOUBLE_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_DOUBLE_IPP

// Double hashing.
// ============================================================================
// No hash(state_t) optimizations for sha160 (requires chunk_t/half_t).
// State input directly to buffer (reinput) eliminates two endianness calls.

namespace libbitcoin {
namespace system {
namespace sha {

// protected
// ----------------------------------------------------------------------------

TEMPLATE
INLINE constexpr void CLASS::
reinput(auto& buffer, const auto& state) NOEXCEPT
{
    static_assert(SHA::strength != 160);

    if (std::is_constant_evaluated())
    {
        buffer[0] = state[0];
        buffer[1] = state[1];
        buffer[2] = state[2];
        buffer[3] = state[3];
        buffer[4] = state[4];
        buffer[5] = state[5];
        buffer[6] = state[6];
        buffer[7] = state[7];
    }
    else
    {
        using word = array_element<decltype(state)>;
        array_cast<word, SHA::state_words>(buffer) = state;
    }
}

// public
// ----------------------------------------------------------------------------

TEMPLATE
template <size_t Size>
constexpr typename CLASS::digest_t CLASS::
double_hash(const ablocks_t<Size>& blocks) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    buffer_t buffer{};
    auto state = H::get;
    iterate(state, blocks);
    schedule_n<Size>(buffer);
    compress(state, buffer);

    // Second hash
    reinput(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    state = H::get;
    compress(state, buffer);
    return output(state);
}

TEMPLATE
typename CLASS::digest_t CLASS::
double_hash(iblocks_t&& blocks) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    // Save block count, as iterable decrements.
    const auto count = blocks.size();

    buffer_t buffer{};
    auto state = H::get;
    iterate(state, blocks);
    schedule_n(buffer, count);
    compress(state, buffer);

    // Second hash
    reinput(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    state = H::get;
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
double_hash(const block_t& block) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    buffer_t buffer{};

    auto state = H::get;
    input(buffer, block);
    schedule(buffer);
    compress(state, buffer);
    schedule_1(buffer);
    compress(state, buffer);

    // Second hash
    reinput(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    state = H::get;
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
double_hash(const half_t& half) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    buffer_t buffer{};
    auto state = H::get;
    input_left(buffer, half);
    pad_half(buffer);
    schedule(buffer);
    compress(state, buffer);

    // Second hash
    reinput(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    state = H::get;
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
double_hash(const half_t& left, const half_t& right) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    buffer_t buffer{};
    auto state = H::get;
    input_left(buffer, left);
    input_right(buffer, right);
    schedule(buffer);
    compress(state, buffer);
    schedule_1(buffer);
    compress(state, buffer);

    // Second hash
    reinput(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    state = H::get;
    compress(state, buffer);
    return output(state);
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

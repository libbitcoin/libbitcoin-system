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
    if (std::is_constant_evaluated())
    {
        auto state = H::get;
        iterate(state, blocks);
        buffer_t buffer{};
        schedule_n<Size>(buffer);
        compress(state, buffer);
        return output(state);
    }
    else if constexpr (native && SHA::strength == 256)
    {
        auto state = H::get;
        iterate(state, blocks);
        native_preswapped(state, pad_blocks(Size));
        return output(state);
    }
    else
    {
        auto state = H::get;
        iterate(state, blocks);
        buffer_t buffer{};
        schedule_n<Size>(buffer);
        compress(state, buffer);
        return output(state);
    }
}

TEMPLATE
typename CLASS::digest_t CLASS::
hash(iblocks_t&& blocks) NOEXCEPT
{
    if constexpr (native && SHA::strength == 256)
    {
        // Save block count, as iterable decrements.
        const auto count = blocks.size();
        auto state = H::get;
        iterate(state, blocks);
        native_preswapped(state, pad_blocks(count));
        return output(state);
    }
    else
    {
        // Save block count, as iterable decrements.
        const auto count = blocks.size();
        auto state = H::get;
        iterate(state, blocks);
        buffer_t buffer{};
        schedule_n(buffer, count);
        compress(state, buffer);
        return output(state);
    }
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const block_t& block) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        auto state = H::get;
        buffer_t buffer{};
        input(buffer, block);
        schedule(buffer);
        compress(state, buffer);
        schedule_1(buffer);
        compress(state, buffer);
        return output(state);
    }
    else if constexpr (native && SHA::strength == 256)
    {
        ////auto state = H::get;
        ////auto& wstate = array_cast<xint128_t>(state);
        ////auto lo = load(wstate[0]);
        ////auto hi = load(wstate[1]);
        ////shuffle(lo, hi);
        ////native_(lo, hi, block);
        ////native_(lo, hi, pad_1());
        ////unshuffle(lo, hi);
        ////byteswap<uint32_t>(lo);
        ////byteswap<uint32_t>(hi);
        ////store(wstate[0], lo);
        ////store(wstate[1], hi);
        ////return array_cast<byte_t>(state);
    
        // Simpler but repeats shuffle/unshuffle, re-loads state, and unloads
        // state before byteswap.
        auto state = H::get;
        native_(state, block);
        native_preswapped(state, pad_block());
        return output(state);
    }
    else
    {
        auto state = H::get;
        buffer_t buffer{};
        input(buffer, block);
        schedule(buffer);
        compress(state, buffer);
        schedule_1(buffer);
        compress(state, buffer);
        return output(state);
    }
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const half_t& half) NOEXCEPT
{
    auto state = H::get;
    buffer_t buffer{};
    input_left(buffer, half);
    pad_half(buffer);
    schedule(buffer);
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const half_t& left, const half_t& right) NOEXCEPT
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
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif

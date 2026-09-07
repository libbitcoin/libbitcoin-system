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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA3_ALGORITHM_SINGLE_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA3_ALGORITHM_SINGLE_IPP

// Single hashing.
// ============================================================================

namespace libbitcoin {
namespace system {
namespace sha3 {

// public
// ----------------------------------------------------------------------------

TEMPLATE
template <size_t Size>
constexpr typename CLASS::digest_t CLASS::
hash(const ablocks_t<Size>& blocks) NOEXCEPT
{
    auto state = H::get;
    iterate(state, blocks);
    return finalize(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const block_t& block) NOEXCEPT
{
    // As an array of 1 arrays is same as the array, this compiles away.
    return hash(ablocks_t<one>{ block });
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const half_t& half) NOEXCEPT
{
    return simple_hash(half);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(uint8_t byte) NOEXCEPT
{
    return simple_hash(bytes_t<one>{ byte });
}

TEMPLATE
typename CLASS::digest_t CLASS::
hash(iblocks_t&& blocks) NOEXCEPT
{
    auto state = H::get;
    iterate(state, blocks);
    return finalize(state);
}

TEMPLATE
template <size_t Size, if_not_greater<Size, CLASSIF::space>>
constexpr typename CLASS::digest_t CLASS::
simple_hash(const bytes_t<Size>& bytes) NOEXCEPT
{
    auto state = H::get;
    return finalize(state, bytes);
}

} // namespace sha3
} // namespace system
} // namespace libbitcoin

#endif

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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_SHORT_ID_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_SHORT_ID_HPP

#include <bitcoin/system/chain/header.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

/// Compact block short transaction ids [bip152].
class BC_API short_id
{
public:
    /// A short id as an integer, the low bits of a siphash word.
    using integer = uint64_t;

    /// Significant bits of a short id integer.
    static constexpr size_t bits = to_bits(mini_hash_size);
    static constexpr integer mask = unmask_right<integer>(bits);

    /// The siphash key of a compact block, from its header and nonce.
    static siphash_key to_key(const header& header, uint64_t nonce) NOEXCEPT;

    /// The short id of a transaction hash under the key of its block.
    static integer to_id(const siphash_key& key,
        const hash_digest& hash) NOEXCEPT;

    /// The short id of its serialization, and the serialization of the id.
    static integer from_mini(const mini_hash& id) NOEXCEPT;
    static mini_hash to_mini(integer id) NOEXCEPT;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif

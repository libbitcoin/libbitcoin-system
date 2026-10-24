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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_SIGNET_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_SIGNET_HPP

#include <bitcoin/system/chain/header.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/transaction.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

/// Signet block solution [bip325].
class BC_API signet
{
public:
    /// True if the coinbase solution satisfies the challenge, txids in order.
    static bool verify(const chain::header& header,
        const transaction& coinbase, hashes&& txids,
        const script& challenge) NOEXCEPT;

protected:
    /// Extract the solution and the commitment cleared of it, false if none.
    static bool get_solution(data_chunk& solution, data_chunk& cleared,
        const script& commitment) NOEXCEPT;

    /// The virtual transaction committing to the block, paying to challenge.
    static transaction to_spend(const chain::header& header,
        const hash_digest& root, const script& challenge) NOEXCEPT;

    /// The solution spend of to_spend, invalid if not parseable.
    static transaction to_sign(const chain::header& header,
        const transaction& coinbase, hashes&& txids,
        const script& challenge) NOEXCEPT;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif

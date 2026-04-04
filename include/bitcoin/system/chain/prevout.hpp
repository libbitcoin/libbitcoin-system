/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_PREVOUT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_PREVOUT_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Defaults are set so non-population issues usually imply invalidity.
class BC_API prevout final
{
public:
    ///************************************************************************
    /// CONSENSUS: 
    /// A height of zero is immature (unspendable) despite unspent state.
    ///************************************************************************
    union
    {
        /// Unused for coinbase.
        /// The confirmed height of the prevout or max_uint32.
        size_t prevout_height{ max_uint32 };

        /// node: populated with database identifier for the parent tx.
        /// Link::terminal must derive from default of max_uint32.
        uint32_t parent_tx;
    };

    ///************************************************************************
    /// CONSENSUS: 
    /// A mtp of max_uint32 fails locktime maturity (until time overflow).
    ///************************************************************************
    union
    {
        /// Unused for coinbase.
        /// The median time past at confirmed prevout block or max_uint32.
        uint32_t median_time_past{ max_uint32 };

        /// node: populated with database identifier for the input/point.
        /// Link::terminal must derive from default of max_uint32.
        uint32_t point_link;
    };

    ///************************************************************************
    /// CONSENSUS: 
    /// An unspent coinbase collision is immature (unspendable) and spent
    /// collision is mature [bip30]. CB collision presumed precluded by bip34.
    /// This is NOT guarded by system::chain confirmation checks.
    ///************************************************************************
    /// The confirmed height of the spender or max_uint32.
    uint32_t spender_height{ max_uint32 };

    /// node: set via block.populate() as internal spends do not require
    /// prevout block association for relative locktime checks. So
    /// median_time_past is not required as locked is determined here.
    bool locked{ true };

    /// The previous output is of a coinbase transaction.
    /// node: populated, does not require prevout block association.
    bool coinbase{ true };
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif

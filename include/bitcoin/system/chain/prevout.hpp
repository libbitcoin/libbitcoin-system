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
        /// The confirmed chain height of the prevout (zero if not found).
        /// Unused if the input owning this prevout is null (coinbase).
        size_t height;

        /// node: populated with a database identifier for the parent tx.
        uint32_t parent{ zero };
    };

    ///************************************************************************
    /// CONSENSUS: 
    /// A mtp of max_uint32 fails locktime maturity (until time overflow).
    ///************************************************************************
    union
    {
        /// The median time past at height (max_uint32 if not found/confirmed).
        /// Unused if the input owning this prevout is null (coinbase).
        uint32_t median_time_past{ max_uint32 };

        /// node: set to the database record of the input association.
        uint32_t link;
    };

    ///************************************************************************
    /// CONSENSUS: 
    /// An unspent coinbase collision is immature (unspendable) and spent
    /// collision is mature (bip30). CB collision presumed precluded by bip34.
    ///************************************************************************
    union
    {
        /// If input owning this prevout is null (coinbase), this implies that
        /// all outputs of any duplicate txs are fully spent at height.
        /// If the input owning this prevout is not null (not coinbase), this
        /// indicates whether the prevout is spent at height (double spend).
        bool spent{ true };

        /// node: indicates that the input spends output inside same block.
        bool inside;
    };

    /// node: set via block.populate() as internal spends do not
    /// require prevout block association for relative locktime checks.
    /// So median_time_past is not required as locked is determined here.
    bool locked{ true };

    /// The previous output is of a coinbase transaction.
    /// node: populated, does not require prevout block association.
    bool coinbase{ true };
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif

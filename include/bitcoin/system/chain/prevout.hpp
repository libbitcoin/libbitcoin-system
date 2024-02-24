/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

class BC_API prevout final
{
public:
    ///************************************************************************
    /// CONSENSUS: 
    /// A height of zero is immature (unspendable) despite unspent state.
    ///************************************************************************
    /// The confirmed chain height of the prevout (zero if not found).
    size_t height{ zero };

    ///************************************************************************
    /// CONSENSUS: 
    /// A mtp of max_uint32 fails locktime maturity (until time overflow).
    ///************************************************************************
    /// The median time past at height (max_uint32 if not found/confirmed).
    uint32_t median_time_past{ max_uint32 };

    ///************************************************************************
    /// CONSENSUS: 
    /// An unspent coinbase collision is immature (unspendable) and spent
    /// collision is mature (bip30). Coinbase collision precluded by bip34.
    ///************************************************************************
    /// For coinbase tx, implies fully spent at height or not found.
    /// Coinbase value stored in first input of coinbase transaction.
    /// For a non-coinbase input this indicates spent at height.
    bool spent{ true };

    /// The output is of a coinbase transaction.
    bool coinbase{ false };
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif

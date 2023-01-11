/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
    //*************************************************************************
    // CONSENSUS: 
    // A height of zero is immature (unspendable) despite unspent state.
    //*************************************************************************
    /// The confirmed chain height of the prevout (zero if not found).
    size_t height;

    //*************************************************************************
    // CONSENSUS: 
    // A mtp of max_uint32 fails locktime maturity (until time overflow).
    //*************************************************************************
    /// The median time past at height (max_uint32 if not found/confirmed).
    uint32_t median_time_past;

    //*************************************************************************
    // CONSENSUS: 
    // For the first coinbase input, this indicates that a transaction of the
    // same hash is either spent by height or does not exist by height (bip30).
    // If the coinbase height is below height then a collision must be assumed.
    // An unspent collision is immature (unspendable) and spent is mature.
    //*************************************************************************
    // For non-coinbase inputs this indicates spent at height.
    bool spent;

    // The output is of a coinbase transaction.
    bool coinbase;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif

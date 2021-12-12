/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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

#include <memory>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/chain/output.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API prevout
  : public output
{
public:
    typedef std::shared_ptr<prevout> ptr;

    /// Use base class constructors, default construction is invalid.
    using chain::output::output;

    //*************************************************************************
    // CONSENSUS: 
    // A height of zero is immature (unspendable) despite unspent state.
    //*************************************************************************
    /// The confirmed chain height of the prevout (zero if not found).
    size_t height = zero;

    //*************************************************************************
    // CONSENSUS: 
    // A mtp of max_uint32 fails locktime maturity (until time overflow).
    //*************************************************************************
    /// The median time past at height (max_uint32 if not found/confirmed).
    uint32_t median_time_past = max_uint32;

    //*************************************************************************
    // CONSENSUS: 
    // For the first coinbase input, this indicates that a transaction of the
    // same hash is either spent by height or does not exist by height (bip30).
    // If the coinbase height is below height then a collision must be assumed.
    // An unspent collision is immature (unspendable) and spent is mature.
    //*************************************************************************
    // For non-coinbase inputs this indicates spent at height.
    bool spent = true;

    // The output is of a coinbase transaction.
    bool coinbase = false;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif

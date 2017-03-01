/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_WALLET_SELECT_OUTPUTS_HPP
#define LIBBITCOIN_WALLET_SELECT_OUTPUTS_HPP

#include <cstdint>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/output_point.hpp>

namespace libbitcoin {
namespace wallet {

struct BC_API select_outputs
{
    enum class algorithm
    {
        /// The first available unspent output that is the minimum greater than
        /// the specified amount if any. If there are none, it returns a set of
        /// the largest outputs (in descending order to minimize the number of
        /// inputs) that are smaller than the specified amount and the amount of
        /// change.
        greedy,

        /// A set of individual unspent outputs that satisfy the specified
        /// amount. For example, setting amount to 0 will return all unspent
        /// outputs since any of them can satisfy that amount. The change
        /// amount will always be 0.
        individual
    };

    /// Select optimal outpoints for a spend from unspent outputs list.
    /// Return includes the amount of change remaining from the spend.
    static void select(chain::points_info& out,
        const chain::output_info::list& unspent, uint64_t minimum_value,
        algorithm option=algorithm::greedy);
};

} // namespace wallet
} // namespace libbitcoin

#endif

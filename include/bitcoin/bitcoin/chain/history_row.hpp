/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_CHAIN_HISTORY_ROW_HPP
#define LIBBITCOIN_CHAIN_HISTORY_ROW_HPP

#include <cstdint>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/point.hpp>

namespace libbitcoin {
namespace chain {

/// Use "kind" for union differentiation.
enum class point_kind : uint32_t
{
    output = 0,
    spend = 1
};

struct BC_API history_row
{
    /// Is this an output or spend.
    point_kind kind;

    /// Input or output point.
    chain::point point;

    /// Block height of the transaction.
    uint64_t height;

    union
    {
        /// If output, then satoshis value of output.
        uint64_t value;

        /// If spend, then checksum hash of previous output point
        /// To match up this row with the output, recompute the
        /// checksum from the output row with spend_checksum(row.point)
        uint64_t previous_checksum;
    };
};

typedef std::vector<history_row> history;

} // namespace chain
} // namespace libbitcoin

#endif

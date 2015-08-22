/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_WALLET_SELECT_OUTPUTS_HPP
#define LIBBITCOIN_WALLET_SELECT_OUTPUTS_HPP

#include <cstddef>
#include <cstdint>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/point.hpp>

namespace libbitcoin {
namespace wallet {

// Used by transaction_indexer and select_outputs()
struct BC_API output_info
{
    chain::output_point point;
    uint64_t value;
};

typedef std::vector<output_info> output_info_list;

struct BC_API select_outputs_result
{
    chain::output_point::list points;
    uint64_t change;
};

enum class select_outputs_algorithm
{
    greedy
};

/**
 * Select optimal outputs for a send from unspent outputs list.
 * Returns output list and remaining change to be sent to
 * a change address.
 */
BC_API select_outputs_result select_outputs(output_info_list unspent,
    uint64_t min_value,
    select_outputs_algorithm algorithm = select_outputs_algorithm::greedy);

} // namspace wallet
} // namspace libbitcoin

#endif


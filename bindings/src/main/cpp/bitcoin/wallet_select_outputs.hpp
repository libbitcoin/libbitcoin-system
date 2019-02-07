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
#ifndef LIBBITCOIN__WALLET_SELECT_OUTPUTS_HPP
#define LIBBITCOIN__WALLET_SELECT_OUTPUTS_HPP

//#include <cstdint>
#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/chain/points_value.hpp>
#include <bitcoin/bitcoin/wallet/select_outputs.hpp>
#include <chain_points_value.hpp>

namespace libbitcoin {
//namespace wallet {
namespace api {

class BC_API wallet_select_outputs
{
public:
    enum class algorithm
    {
        /// The smallest single sufficient unspent output, if one exists, or a
        /// sufficient set of unspent outputs, if such a set exists. The set is
        /// minimal by number of outputs but not necessarily by total value.
        greedy,

        /// A set of individually sufficient unspent outputs. Each individual
        /// member of the set is sufficient. Return ascending order by value.
        individual
    };

    /// Select outpoints for a spend from a list of unspent outputs.
    static void select(chain_points_value& out,
        const chain_points_value& unspent, uint64_t minimum_value,
        algorithm option=algorithm::greedy);

    wallet::select_outputs getValue() {
		return value;
	}

	void setValue(wallet::select_outputs value) {
		this->value = value;
	}
private:
	wallet::select_outputs value;

//private:
//    static void greedy(chain_points_value& out,
//        const chain_points_value& unspent, uint64_t minimum_value);
//
//    static void individual(chain_points_value& out,
//        const chain_points_value& unspent, uint64_t minimum_value);
};

} // namespace wallet
} // namespace libbitcoin

#endif

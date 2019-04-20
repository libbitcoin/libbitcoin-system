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
#ifndef LIBBITCOIN__CHAIN_CHAIN_STATE_DATA_HPP
#define LIBBITCOIN__CHAIN_CHAIN_STATE_DATA_HPP

#include <bitcoin/bitcoin/chain/chain_state.hpp>

namespace libbitcoin {

//namespace chain {

namespace api {

struct chain_chain_state_data
{
public:
	chain::chain_state::data* getValue() {
		return value_;
	}

	void setValue(chain::chain_state::data* value) {
		value_ = value;
	}
private:
	chain::chain_state::data* value_;
};

} // api
//} // namespace chain
} // namespace libbitcoin

#endif

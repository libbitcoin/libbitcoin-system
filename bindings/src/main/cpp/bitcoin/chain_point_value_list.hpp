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
#ifndef LIBBITCOIN__CHAIN_POINT_VALUE_LIST_HPP
#define LIBBITCOIN__CHAIN_POINT_VALUE_LIST_HPP

#include <bitcoin/bitcoin/chain/point_value.hpp>

namespace libbitcoin {
//namespace chain {
namespace api {

class BC_API chain_point_value_list {
public:

	chain_point_value_list() :
			value_(new chain::point_value::list) {
	}

	chain_point_value_list(chain::point_value::list* list) :
			value_(list) {
	}

	chain_point_value_list(const chain_point_value_list& points) :
			value_(points.value_) {
	}

	chain::point_value get(size_t i) {
		return (*value_)[i];
	}

	void set(size_t i, chain::point_value *t) {
		(*value_)[i] = *t;
	}

	chain_point_value_list* getpoint_values() {
		return new chain_point_value_list(value_);
	}

	size_t getSize() {
		return value_->size();
	}

	chain::point_value::list* getValue() {
		return value_;
	}

	void setValue(chain::point_value::list* value) {
		value_ = value;
	}

private:

	chain::point_value::list* value_;

};

} // namespace api
//} // namespace chain
} // namespace libbitcoin

#endif

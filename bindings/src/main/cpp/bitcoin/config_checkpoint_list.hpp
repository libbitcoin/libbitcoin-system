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
#ifndef LIBBITCOIN__CONFIG_CHECKPOINT_LIST_HPP
#define LIBBITCOIN__CONFIG_CHECKPOINT_LIST_HPP

#include <bitcoin/bitcoin/config/checkpoint.hpp>
#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/error.hpp>

namespace libbitcoin {
//namespace config {
namespace api {

class BC_API config_checkpoint_list {
public:

	config_checkpoint_list() :
		value(new config::checkpoint::list()) {
	}

	config::checkpoint get(size_t i) {
		return (*value)[i];
	}

	void set(size_t i, config::checkpoint *t) {
		(*value)[i] = *t;
	}

	size_t getSize() {
		return value->size();
	}

	config::checkpoint::list* getValue() {
		return value;
	}

	void setValue(config::checkpoint::list *value) {
		this->value = value;
	}

private:

	config::checkpoint::list *value;

};

} // namespace api
//} // namespace config
} // namespace libbitcoin

#endif

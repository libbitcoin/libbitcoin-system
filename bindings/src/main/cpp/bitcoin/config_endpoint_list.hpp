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
#ifndef LIBBITCOIN__CONFIG_ENDPOINT_LIST_HPP
#define LIBBITCOIN__CONFIG_ENDPOINT_LIST_HPP

#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/config_endpoint.hpp>

namespace libbitcoin {
//namespace config {
namespace api {

class BC_API config_endpoint_list {
public:

	config_endpoint_list() :
			value(new libbitcoin::config::endpoint::list) {
	}

	libbitcoin::config::endpoint& get(size_t i) {
		return (*value)[i];
	}

	void set(size_t i, config::endpoint *t) {
		(*value)[i] = *t;
	}

	libbitcoin::config::endpoint::list *cast() {
		return value;
	}

	size_t getSize() {
		return value->size();
	}

	config::endpoint::list* getValue() {
		return value;
	}

	void setValue(config::endpoint::list* value) {
		this->value = value;
	}

private:

	libbitcoin::config::endpoint::list* value;

};

} // namespace api
//} // namespace chain
} // namespace libbitcoin

#endif

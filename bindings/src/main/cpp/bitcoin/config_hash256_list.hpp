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
#ifndef LIBBITCOIN__CONFIG_HASH256_LIST_HPP
#define LIBBITCOIN__CONFIG_HASH256_LIST_HPP

#include <bitcoin/bitcoin/config/hash256.hpp>
#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/error.hpp>

namespace libbitcoin {
//namespace config {
namespace api {

class BC_API config_hash256_list {
public:

	config_hash256_list() :
			value_(new config::hash256::list) {
	}

	config::hash256& get(size_t i) {
		return (*value_)[i];
	}

	void set(size_t i, config::hash256 *t) {
		(*value_)[i] = *t;
	}

	config::hash256::list* getValue() {
		return value_;
	}

	size_t getSize() {
		return value_->size();
	}

private:

	config::hash256::list* value_;

};

} // namespace api
//} // namespace config
} // namespace libbitcoin

#endif

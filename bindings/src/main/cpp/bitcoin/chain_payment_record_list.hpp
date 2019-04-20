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
#ifndef LIBBITCOIN__CHAIN_PAYMENT_RECORD_LIST_HPP
#define LIBBITCOIN__CHAIN_PAYMENT_RECORD_LIST_HPP

#include <bitcoin/bitcoin/chain/payment_record.hpp>

namespace libbitcoin {
//namespace chain {
namespace api {

class BC_API chain_payment_record_list {
public:

	chain_payment_record_list() :
			value_(new chain::payment_record::list) {
	}

	chain::payment_record get(size_t i) {
		return (*value_)[i];
	}

	void set(size_t i, chain::payment_record *t) {
		(*value_)[i] = *t;
	}

	size_t getSize() {
		return value_->size();
	}

	chain::payment_record::list* getValue() {
		return value_;
	}

	void setValue(chain::payment_record::list *value) {
		value_ = value;
	}

private:

	chain::payment_record::list* value_;

};

} // namespace api
//} // namespace chain
} // namespace libbitcoin

#endif

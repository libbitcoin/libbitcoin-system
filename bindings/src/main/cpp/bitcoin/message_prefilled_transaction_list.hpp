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
#ifndef LIBBBITCOIN__MESSAGE_PREFILLED_TRANSACTION_LIST_HPP
#define LIBBBITCOIN__MESSAGE_PREFILLED_TRANSACTION_LIST_HPP

#include <bitcoin/message/prefilled_transaction.hpp>

namespace libbitcoin {
namespace api {

class message_prefilled_transaction_list {
public:
	message::prefilled_transaction::list getValue() {
        return value;
    }

    void setValue(message::prefilled_transaction::list value) {
        this->value = value;
    }
private:
    message::prefilled_transaction::list value;

};

} // namespace api
} // namespace libbitcoin

#endif

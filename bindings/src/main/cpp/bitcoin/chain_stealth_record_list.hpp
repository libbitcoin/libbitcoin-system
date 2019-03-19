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
#ifndef LIBBITCOIN__CHAIN_STEALTH_RECORD_LIST_HPP
#define LIBBITCOIN__CHAIN_STEALTH_RECORD_LIST_HPP

#include <bitcoin/bitcoin/chain/stealth_record.hpp>

namespace libbitcoin {
//namespace chain {
namespace api {

class BC_API chain_stealth_record_list
{
public:

  chain_stealth_record_list()
    : value(new chain::stealth_record::list())
  {
  }

  chain::stealth_record get(size_t i) {
    return chain_stealth_record_list::value[i];
  }

  void set(size_t i, chain::stealth_record *t) {
    chain_stealth_record_list::value[i] = *t;
  }

  chain::stealth_record::list cast() {
    return value;
  }

  size_t getSize() {
    return value.size();
  }

  chain::stealth_record::list getValue() {
        return value;
    }

    void setValue(chain::stealth_record::list value) {
        this->value = value;
    }
private:

  chain::stealth_record::list value;

};

} // namespace api
//} // namespace chain
} // namespace libbitcoin

#endif

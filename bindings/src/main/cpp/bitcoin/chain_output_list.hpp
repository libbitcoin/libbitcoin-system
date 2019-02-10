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
#ifndef LIBBITCOIN__CHAIN_OUTPUT_LIST_HPP
#define LIBBITCOIN__CHAIN_OUTPUT_LIST_HPP

#include <bitcoin/bitcoin/chain/output.hpp>
#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/error.hpp>

namespace libbitcoin {
//namespace chain {
namespace api {

class BC_API chain_output_list
{
public:

  chain_output_list()
    : value(new libbitcoin::chain::output::list())
  {
  }

  libbitcoin::chain::output get(size_t i) {
    return chain_output_list::value[i];
  }

  void set(size_t i, libbitcoin::chain::output *t) {
    chain_output_list::value[i] = *t;
  }

  libbitcoin::chain::output::list getvalue() {
    return value;
  }

  void setValue(chain::output::list value) {
	  this->value = value;
  }

  size_t getSize() {
    return value.size();
  }

private:

  libbitcoin::chain::output::list value;

};

} // namespace api
//} // namespace chain
} // namespace libbitcoin

#endif

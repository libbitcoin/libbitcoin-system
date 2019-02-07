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
namespace config {
namespace api {

class BC_API config_hash256_list
{
public:

  config_hash256_list()
    : list(new hash256::list())
  {
  }

  hash256 get(size_t i) {
    return config_hash256_list::list[i];
  }

  void set(size_t i, hash256 *t) {
    config_hash256_list::list[i] = *t;
  }

  hash256::list cast() {
    return list;
  }

  size_t getSize() {
    return list.size();
  }

private:

  hash256::list list;

};

} // namespace api
} // namespace config
} // namespace libbitcoin

#endif

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
#ifndef LIBBITCOIN__CONFIG_AUTHORITY_LIST_HPP
#define LIBBITCOIN__CONFIG_AUTHORITY_LIST_HPP

#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/config_authority.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>

namespace libbitcoin {
namespace config {
namespace api {

class BC_API config_authority_list
{
public:

  config_authority_list()
    : authorities(new libbitcoin::config::authority::list())
  {
  }

  config_authority get(size_t i) {
    return authorities[i];
  }

  void set(size_t i, authority *t) {
    authorities[i] = *t;
  }

  libbitcoin::config::authority::list cast() {
    return authorities;
  }

  size_t getSize() {
    return authorities.size();
  }

private:

  libbitcoin::config::authority::list authorities;

};

} // namespace api
} // namespace chain
} // namespace libbitcoin

#endif

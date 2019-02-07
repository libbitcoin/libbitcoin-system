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
#include <bitcoin/config_endpoint.hpp>
#include <bitcoin/bitcoin/config/endpoint.hpp>

namespace libbitcoin {
namespace config {
namespace api {

class BC_API config_endpoint_list
{
public:

  config_endpoint_list()
    : endpoints(new libbitcoin::config::endpoint::list())
  {
  }

  config_endpoint get(size_t i) {
    return endpoints[i];
  }

  void set(size_t i, endpoint *t) {
    endpoints[i] = *t;
  }

  libbitcoin::config::endpoint::list cast() {
    return endpoints;
  }

  size_t getSize() {
    return endpoints.size();
  }

private:

  libbitcoin::config::endpoint::list endpoints;

};

} // namespace api
} // namespace chain
} // namespace libbitcoin

#endif

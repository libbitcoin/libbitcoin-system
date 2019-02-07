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
#ifndef LIBBITCOIN__CHAIN_POINT_LIST_HPP
#define LIBBITCOIN__CHAIN_POINT_LIST_HPP

#include <bitcoin/bitcoin/chain/point.hpp>
#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/error.hpp>

namespace libbitcoin {
//namespace chain {
namespace api {

class BC_API chain_point_list : public libbitcoin::chain::point::list
{
//public:
//
//  chain_point_list()
//    : points(new chain::point::list())
//  {
//  }
//
//  chain::point get(size_t i) {
//    return chain_point_list::points[i];
//  }
//
//  void set(size_t i, chain::point *t) {
//    chain_point_list::points[i] = *t;
//  }
//
//  chain::point::list getpoints() {
//    return points;
//  }
//
//  size_t getSize() {
//    return points.size();
//  }
//
//private:
//
//  chain::point::list points;

};

} // namespace api
//} // namespace chain
} // namespace libbitcoin

#endif

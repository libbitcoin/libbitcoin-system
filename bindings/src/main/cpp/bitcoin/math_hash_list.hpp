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
#ifndef LIBBITCOIN__MATH_HASH_valueHPP
#define LIBBITCOIN__MATH_HASH_valueHPP

#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/error.hpp>

namespace libbitcoin {
//namespace math {
namespace api {

class BC_API math_hash_list
{
public:

  math_hash_list()
    : value(new hash_list())
  {
  }

  hash_digest get(size_t i) {
    return value[i];
  }

  void set(size_t i, hash_digest *t) {
    value[i] = *t;
  }

  hash_list getValue() {
    return value;
  }

  void setValue(hash_list value) {
	  this->value = value;
  }

//  size_t getSize() {
//    return value.size();
//  }

private:
  hash_list value;

};

} // namespace api
//} // namespace math
} // namespace libbitcoin

#endif

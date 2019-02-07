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
#ifndef LIBBITCOIN__CHAIN_BLOCK_INDEXES_HPP
#define LIBBITCOIN__CHAIN_BLOCK_INDEXES_HPP

#include <bitcoin/bitcoin/chain/block.hpp>
#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/error.hpp>

namespace libbitcoin {
//namespace chain {
namespace api {

class BC_API chain_block_indexes : public libbitcoin::chain::block::indexes
{
//public:
//
//  chain_block_indexes()
//    : indexes(new block::indexes())
//  {
//  }
//
//  size_t get(size_t i) {
//    return chain_block_indexes::indexes[i];
//  }
//
//  void set(size_t i, block *t) {
//    chain_block_indexes::indexes[i] = *t;
//  }
//
//  block::indexes getIndexes() {
//    return indexes;
//  }
//
//  size_t getSize() {
//    return indexes.size();
//  }
//
//private:
//
//  block::indexes indexes;

};

} // namespace api
//} // namespace chain
} // namespace libbitcoin

#endif

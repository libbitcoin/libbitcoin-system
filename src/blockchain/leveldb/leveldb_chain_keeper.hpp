/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) 
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_BLOCKCHAIN_LEVELDB_CHAIN_KEEPER_H
#define LIBBITCOIN_BLOCKCHAIN_LEVELDB_CHAIN_KEEPER_H

#include <bitcoin/blockchain/organizer.hpp>
#include <bitcoin/blockchain/leveldb_blockchain.hpp>

#include "leveldb_common.hpp"

namespace libbitcoin {

class leveldb_chain_keeper
  : public chain_keeper
{
public:
    leveldb_chain_keeper(leveldb_common_ptr common, leveldb_databases db);

    void start();
    void stop();

    void add(block_detail_ptr incoming_block);
    int find_index(const hash_digest& search_block_hash);
    big_number end_slice_difficulty(size_t slice_begin_index);
    bool end_slice(size_t slice_begin_index,
        block_detail_list& sliced_blocks);

private:
    bool clear_transaction_data(leveldb_transaction_batch& batch,
        const transaction_type& remove_tx);

    leveldb_common_ptr common_;
    leveldb_databases db_;
};

typedef std::shared_ptr<leveldb_chain_keeper> leveldb_chain_keeper_ptr;

} // namespace libbitcoin

#endif


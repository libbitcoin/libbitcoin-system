/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_BLOCKCHAIN_LEVELDB_BLOCKCHAIN_HPP
#define LIBBITCOIN_BLOCKCHAIN_LEVELDB_BLOCKCHAIN_HPP

#include <bitcoin/blockchain/blockchain.hpp>

#include <boost/interprocess/sync/file_lock.hpp>
#include <leveldb/db.h>

#include <bitcoin/blockchain/organizer.hpp>
#include <bitcoin/utility/subscriber.hpp>
#include <bitcoin/threadpool.hpp>

namespace libbitcoin {

class leveldb_common;
typedef std::shared_ptr<leveldb_common> leveldb_common_ptr;

class leveldb_blockchain
  : public blockchain
{
public:
    // Used by internal components so need public definition here
    typedef subscriber<
        const std::error_code&, size_t, const block_list&, const block_list&>
            reorganize_subscriber_type;

    typedef std::function<void (const std::error_code)> start_handler;

    leveldb_blockchain(threadpool& pool);
    ~leveldb_blockchain();

    // Non-copyable
    leveldb_blockchain(const leveldb_blockchain&) = delete;
    void operator=(const leveldb_blockchain&) = delete;

    void start(const std::string& prefix, start_handler handle_start);
    void stop();

    void store(const block_type& block,
        store_block_handler handle_store);
    void import(const block_type& block, size_t height,
        import_block_handler handle_import);

    // fetch block header by height
    void fetch_block_header(size_t height,
        fetch_handler_block_header handle_fetch);
    // fetch block header by hash
    void fetch_block_header(const hash_digest& block_hash,
        fetch_handler_block_header handle_fetch);
    // fetch transaction hashes in block by height
    void fetch_block_transaction_hashes(size_t height,
        fetch_handler_block_transaction_hashes handle_fetch);
    // fetch transaction hashes in block by hash
    void fetch_block_transaction_hashes(const hash_digest& block_hash,
        fetch_handler_block_transaction_hashes handle_fetch);
    // fetch height of block by hash
    void fetch_block_height(const hash_digest& block_hash,
        fetch_handler_block_height handle_fetch);
    // fetch height of latest block
    void fetch_last_height(fetch_handler_last_height handle_fetch);
    // fetch transaction by hash
    void fetch_transaction(const hash_digest& transaction_hash,
        fetch_handler_transaction handle_fetch);
    // fetch height and offset within block of transaction by hash
    void fetch_transaction_index(const hash_digest& transaction_hash,
        fetch_handler_transaction_index handle_fetch);
    // fetch spend of an output point
    void fetch_spend(const output_point& outpoint,
        fetch_handler_spend handle_fetch);
    // fetch outputs, values and spends for an address.
    void fetch_history(const payment_address& address,
        fetch_handler_history handle_fetch, size_t from_height=0);

    void subscribe_reorganize(reorganize_handler handle_reorganize);

private:
    typedef std::atomic<size_t> seqlock_type;
    typedef std::unique_ptr<leveldb::DB> database_ptr;
    typedef std::unique_ptr<leveldb::Comparator> comparator_ptr;

    typedef std::function<bool (size_t)> perform_read_functor;

    bool initialize(const std::string& prefix);

    void begin_write();

    template <typename Handler, typename... Args>
    void finish_write(Handler handler, Args&&... args)
    {
        ++seqlock_;
        // seqlock is now even again.
        BITCOIN_ASSERT(seqlock_ % 2 == 0);
        handler(std::forward<Args>(args)...);
    }

    void do_store(const block_type& block,
        store_block_handler handle_store);
    void do_import(const block_type& block, size_t height,
        import_block_handler handle_import);

    // Uses sequence looks to try to read shared data.
    // Try to initiate asynchronous read operation. If it fails then
    // sleep for a small amount of time and then retry read operation.
    void fetch(perform_read_functor perform_read);

    template <typename Handler, typename... Args>
    bool finish_fetch(size_t slock, Handler handler, Args&&... args)
    {
        if (slock != seqlock_)
            return false;
        handler(std::forward<Args>(args)...);
        return true;
    }

    bool fetch_block_header_by_height(size_t height, 
        fetch_handler_block_header handle_fetch, size_t slock);
    bool fetch_block_header_by_hash(const hash_digest& block_hash, 
        fetch_handler_block_header handle_fetch, size_t slock);
    bool do_fetch_block_height(const hash_digest& block_hash,
        fetch_handler_block_height handle_fetch, size_t slock);
    bool do_fetch_last_height(
        fetch_handler_last_height handle_fetch, size_t slock);
    bool do_fetch_transaction(const hash_digest& transaction_hash,
        fetch_handler_transaction handle_fetch, size_t slock);
    bool do_fetch_transaction_index(const hash_digest& transaction_hash,
        fetch_handler_transaction_index handle_fetch, size_t slock);
    bool do_fetch_spend(const output_point& outpoint,
        fetch_handler_spend handle_fetch, size_t slock);
    bool do_fetch_history(const payment_address& address,
        fetch_handler_history handle_fetch, size_t from_height, size_t slock);

    io_service& ios_;
    // Queue for writes to the blockchain.
    async_strand strand_;
    // Queue for serializing reorganization handler calls.
    async_strand reorg_strand_;
    // Lock the database directory with a file lock.
    boost::interprocess::file_lock flock_;
    // seqlock used for writes.
    seqlock_type seqlock_;

    // Comparator to order blocks by height logically.
    // Otherwise the last block in the database
    // might not be the largest height in our blockchain.
    comparator_ptr height_comparator_;
    leveldb::Options open_options_;

    // Blocks indexed by height.
    database_ptr db_block_;
    // Block heights indexed by hash (a secondary lookup table).
    database_ptr db_block_hash_;
    // Transactions indexed by hash.
    database_ptr db_tx_;
    // Lookup whether an output point is spent.
    // Value is the input point spend.
    database_ptr db_spend_;
    // Address to list of output points + values
    database_ptr db_credit_;
    // Address to list of spend input points.
    database_ptr db_debit_;

    leveldb_common_ptr common_;
    // Organize stuff
    orphans_pool_ptr orphans_;
    chain_keeper_ptr chain_;
    organizer_ptr organize_;

    reorganize_subscriber_type::ptr reorganize_subscriber_;
};

} // namespace libbitcoin

#endif


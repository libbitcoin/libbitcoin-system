#ifndef LIBBITCOIN_BLOCKCHAIN_LEVELDB_BLOCKCHAIN_HPP
#define LIBBITCOIN_BLOCKCHAIN_LEVELDB_BLOCKCHAIN_HPP

#include <bitcoin/blockchain/blockchain.hpp>

#include <boost/interprocess/sync/file_lock.hpp>
#include <leveldb/db.h>

#include <bitcoin/blockchain/organizer.hpp>
#include <bitcoin/utility/subscriber.hpp>
#include <bitcoin/async_service.hpp>

namespace libbitcoin {

class leveldb_common;
typedef std::shared_ptr<leveldb_common> leveldb_common_ptr;

class leveldb_blockchain
  : public blockchain, public async_strand
{
public:
    // Used by internal components so need public definition here
    typedef subscriber<
        const std::error_code&, size_t, const block_list&, const block_list&>
            reorganize_subscriber_type;

    typedef std::function<void (const std::error_code)> start_handler;

    leveldb_blockchain(async_service& service);
    ~leveldb_blockchain();

    // Non-copyable
    leveldb_blockchain(const leveldb_blockchain&) = delete;
    void operator=(const leveldb_blockchain&) = delete;

    void start(const std::string& prefix, start_handler handle_start);
    void stop();

    void store(const message::block& stored_block,
        store_block_handler handle_store);
    void import(const message::block& import_block, size_t depth,
        import_block_handler handle_import);

    // fetch block header by depth
    void fetch_block_header(size_t depth,
        fetch_handler_block_header handle_fetch);
    // fetch block header by hash
    void fetch_block_header(const hash_digest& block_hash,
        fetch_handler_block_header handle_fetch);
    // fetch transaction hashes in block by depth
    void fetch_block_transaction_hashes(size_t depth,
        fetch_handler_block_transaction_hashes handle_fetch);
    // fetch transaction hashes in block by hash
    void fetch_block_transaction_hashes(const hash_digest& block_hash,
        fetch_handler_block_transaction_hashes handle_fetch);
    // fetch depth of block by hash
    void fetch_block_depth(const hash_digest& block_hash,
        fetch_handler_block_depth handle_fetch);
    // fetch depth of latest block
    void fetch_last_depth(fetch_handler_last_depth handle_fetch);
    // fetch transaction by hash
    void fetch_transaction(const hash_digest& transaction_hash,
        fetch_handler_transaction handle_fetch);
    // fetch depth and offset within block of transaction by hash
    void fetch_transaction_index(const hash_digest& transaction_hash,
        fetch_handler_transaction_index handle_fetch);
    // fetch spend of an output point
    void fetch_spend(const message::output_point& outpoint,
        fetch_handler_spend handle_fetch);
    // fetch outputs associated with an address
    void fetch_outputs(const payment_address& address,
        fetch_handler_outputs handle_fetch);

    void subscribe_reorganize(reorganize_handler handle_reorganize);

private:
    typedef std::unique_ptr<leveldb::DB> database_ptr;
    typedef std::unique_ptr<leveldb::Comparator> comparator_ptr;

    bool initialize(const std::string& prefix);

    void do_store(const message::block& store_block,
        store_block_handler handle_store);
    void do_import(const message::block& import_block, size_t depth,
        import_block_handler handle_import);

    void fetch_block_header_by_depth(size_t depth, 
        fetch_handler_block_header handle_fetch);
    void fetch_block_header_by_hash(const hash_digest& block_hash, 
        fetch_handler_block_header handle_fetch);
    void do_fetch_block_depth(const hash_digest& block_hash,
        fetch_handler_block_depth handle_fetch);
    void do_fetch_last_depth(fetch_handler_last_depth handle_fetch);
    void do_fetch_transaction(const hash_digest& transaction_hash,
        fetch_handler_transaction handle_fetch);
    void do_fetch_transaction_index(const hash_digest& transaction_hash,
        fetch_handler_transaction_index handle_fetch);
    void do_fetch_spend(const message::output_point& outpoint,
        fetch_handler_spend handle_fetch);
    void do_fetch_outputs(const payment_address& address,
        fetch_handler_outputs handle_fetch);

    boost::interprocess::file_lock flock_;

    // Comparator to order blocks by depth logically.
    // Otherwise the last block in the database
    // might not be the largest depth in our blockchain.
    comparator_ptr depth_comparator_;
    leveldb::Options open_options_;

    // Blocks indexed by depth.
    database_ptr db_blocks_;
    // Block depths indexed by hash (a secondary lookup table).
    database_ptr db_blocks_hash_;
    // Transactions indexed by hash.
    database_ptr db_txs_;
    // Lookup whether an output point is spent.
    // Value is the input point spend.
    database_ptr db_spends_;
    // Address to list of output points.
    database_ptr db_address_;

    leveldb_common_ptr common_;
    // Organize stuff
    orphans_pool_ptr orphans_;
    chain_keeper_ptr chain_;
    organizer_ptr organize_;

    reorganize_subscriber_type::ptr reorganize_subscriber_;
};

} // namespace libbitcoin

#endif


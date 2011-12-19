#include <bitcoin/blockchain/bdb_blockchain.hpp>

#include DB_CXX_HEADER

#include <bitcoin/transaction.hpp>
#include <bitcoin/util/logger.hpp>

#include "bdb_common.hpp"
#include "bdb_chain_keeper.hpp"
#include "bdb_organizer.hpp"
#include "data_type.hpp"
#include "txn_guard.hpp"
#include "protobuf_wrapper.hpp"

namespace libbitcoin {

constexpr uint32_t env_flags =
    DB_CREATE|
    DB_RECOVER|
    DB_INIT_LOCK|
    DB_INIT_LOG|
    DB_INIT_TXN|
    DB_INIT_MPOOL|
    DB_THREAD|
    DB_CXX_NO_EXCEPTIONS;

constexpr uint32_t db_flags = DB_CREATE|DB_THREAD;

bdb_blockchain::bdb_blockchain(const std::string& prefix)
{
    initialize(prefix);
}

bdb_blockchain::bdb_blockchain()
{
    // Private method. Should never be called by user!
    // Only by factory methods
}

template <typename Database>
inline void shutdown_database(Database*& database)
{
    database->close(0);
    delete database;
    database = nullptr;
}
bdb_blockchain::~bdb_blockchain()
{
    // Close secondaries before primaries
    shutdown_database(db_blocks_hash_);
    shutdown_database(db_txs_hash_);
    // Close primaries
    shutdown_database(db_blocks_);
    shutdown_database(db_txs_);
    shutdown_database(env_);
    // delete
    google::protobuf::ShutdownProtobufLibrary();
}

bool bdb_blockchain::setup(const std::string& prefix)
{
    bdb_blockchain handle;
    if (!handle.initialize(prefix))
        return false;
    handle.db_blocks_->truncate(nullptr, 0, 0);
    handle.db_txs_->truncate(nullptr, 0, 0);
    // Save genesis block
    txn_guard_ptr txn = std::make_shared<txn_guard>(handle.env_);
    if (!handle.common_->save_block(txn, 0, genesis_block()))
    {
        txn->abort();
        return false;
    }
    txn->commit();
    return true;
}

// Because BDB is dumb
hash_digest second_hash;

int get_block_hash(Db*, const Dbt*, const Dbt* data, Dbt* second_key)
{
    std::stringstream ss(std::string(
        reinterpret_cast<const char*>(data->get_data()), data->get_size()));
    protobuf::Block proto_block;
    proto_block.ParseFromIstream(&ss);
    message::block serial_block = protobuf_to_block_header(proto_block);
    second_hash = hash_block_header(serial_block);
    second_key->set_data(second_hash.data());
    second_key->set_size(second_hash.size());
    return 0;
}

int get_tx_hash(Db*, const Dbt*, const Dbt* data, Dbt* second_key)
{
    std::stringstream ss(std::string(
        reinterpret_cast<const char*>(data->get_data()), data->get_size()));
    protobuf::Transaction proto_tx;
    proto_tx.ParseFromIstream(&ss);
    message::transaction serial_tx = protobuf_to_transaction(proto_tx);
    second_hash = hash_transaction(serial_tx);
    second_key->set_data(second_hash.data());
    second_key->set_size(second_hash.size());
    return 0;
}

int bt_compare_blocks(DB*, const DBT* dbt1, const DBT* dbt2)
{
    data_chunk key_data1(dbt1->size), key_data2(dbt2->size);
    memcpy(key_data1.data(), dbt1->data, dbt1->size);
    memcpy(key_data2.data(), dbt2->data, dbt2->size);
    uint32_t depth1 = cast_chunk<uint32_t>(key_data1),
        depth2 = cast_chunk<uint32_t>(key_data2);
    if (depth1 < depth2)
        return -1;
    else if (depth1 > depth2)
        return 1;
    return 0;
}

bool bdb_blockchain::initialize(const std::string& prefix)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    env_ = new DbEnv(0);
    env_->set_lk_max_locks(10000);
    env_->set_lk_max_objects(10000);
    env_->open(prefix.c_str(), env_flags, 0);
    // Create database objects
    db_blocks_ = new Db(env_, 0);
    db_blocks_hash_ = new Db(env_, 0);
    db_txs_ = new Db(env_, 0);
    db_txs_hash_ = new Db(env_, 0);
    if (db_blocks_->set_bt_compare(bt_compare_blocks) != 0)
    {
        log_fatal() << "Internal error setting BTREE comparison function";
        return false;
    }
    txn_guard txn(env_);
    db_blocks_->open(txn.get(), "blocks", "block-data", DB_BTREE, db_flags, 0);
    db_blocks_hash_->open(txn.get(), "blocks", "block-hash", 
        DB_BTREE, db_flags, 0);
    db_blocks_->associate(txn.get(), db_blocks_hash_, get_block_hash, 0);
    db_txs_->open(txn.get(), "transactions", "tx-data", DB_BTREE, db_flags, 0);
    db_txs_hash_->open(txn.get(), "transactions", "tx-hash",
        DB_BTREE, db_flags, 0);
    db_txs_->associate(txn.get(), db_txs_hash_, get_tx_hash, 0);
    txn.commit();

    common_ = std::make_shared<bdb_common>(env_,
        db_blocks_, db_blocks_hash_, db_txs_, db_txs_hash_);

    orphans_ = std::make_shared<orphans_pool>(10);
    chain_ = std::make_shared<bdb_chain_keeper>(common_, env_,
        db_blocks_, db_blocks_hash_);
    organize_ = std::make_shared<bdb_organizer>(orphans_, chain_);
    return true;
}

void bdb_blockchain::store(const message::block& stored_block, 
    store_block_handler handle_store)
{
    service()->post(std::bind(&bdb_blockchain::do_store,
        shared_from_this(), stored_block, handle_store));
}
void bdb_blockchain::do_store(const message::block& stored_block,
    store_block_handler handle_store)
{
    block_detail_ptr stored_detail =
        std::make_shared<block_detail>(stored_block);
    int depth = chain_->find_index(hash_block_header(stored_block));
    if (depth != -1)
    {
        handle_store(error::object_already_exists,
            block_info{block_status::confirmed, depth});
        return;
    }
    orphans_->add(stored_detail);
    organize_->start();
    // Every 10 blocks, we flush database
    static size_t flush_counter = 0;
    if (++flush_counter == 10)
    {
        env_->txn_checkpoint(0, 0, 0);
        flush_counter = 0;
    }
    handle_store(std::error_code(),
        block_info{block_status::orphan, stored_detail->depth()});
}

template<typename Index>
bool fetch_block_impl(Db* db_blocks_x, txn_guard_ptr txn, const Index& index,
    bdb_common_ptr common, message::block& serial_block)
{
    protobuf::Block proto_block;
    if (!bdb_common::proto_read(db_blocks_x, txn, index, proto_block))
        return false;
    if (!common->reconstruct_block(txn, proto_block, serial_block))
        return false;
    return true;
} 

void bdb_blockchain::fetch_block(size_t depth,
    fetch_handler_block handle_fetch)
{
    service()->post(std::bind(
        &bdb_blockchain::fetch_block_by_depth, shared_from_this(),
            depth, handle_fetch));
}

void bdb_blockchain::fetch_block_by_depth(size_t depth,
    fetch_handler_block handle_fetch)
{
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    message::block serial_block;
    if (!fetch_block_impl(db_blocks_, txn, depth, common_, serial_block))
    {
        txn->abort();
        handle_fetch(error::missing_object, message::block());
        return;
    }
    txn->commit();
    handle_fetch(std::error_code(), serial_block);
}

void bdb_blockchain::fetch_block(const hash_digest& block_hash,
    fetch_handler_block handle_fetch)
{
    service()->post(std::bind(
        &bdb_blockchain::fetch_block_by_hash, shared_from_this(),
            block_hash, handle_fetch));
}

void bdb_blockchain::fetch_block_by_hash(const hash_digest& block_hash, 
    fetch_handler_block handle_fetch)
{
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    message::block serial_block;
    if (!fetch_block_impl(db_blocks_hash_, txn, block_hash,
        common_, serial_block))
    {
        txn->abort();
        handle_fetch(error::missing_object, message::block());
        return;
    }
    txn->commit();
    handle_fetch(std::error_code(), serial_block);
}

void bdb_blockchain::fetch_block_locator(
    fetch_handler_block_locator handle_fetch)
{
    service()->post(std::bind(
        &bdb_blockchain::do_fetch_block_locator, shared_from_this(), 
            handle_fetch));
}
void bdb_blockchain::do_fetch_block_locator(
    fetch_handler_block_locator handle_fetch)
{
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    uint32_t last_block_depth = common_->find_last_block_depth(txn);
    if (last_block_depth == std::numeric_limits<uint32_t>::max())
    {
        log_error() << "Empty blockchain";
        handle_fetch(error::missing_object, message::block_locator());
        return;
    }

    message::block_locator locator;
    std::vector<size_t> indices = block_locator_indices(last_block_depth);
    for (size_t current_index: indices)
    {
        // bdb provides no way to lookup secondary index AFAIK
        // we instead regenerate block hash from its header
        protobuf::Block proto_block;
        if (!bdb_common::proto_read(db_blocks_, txn,
            current_index, proto_block))
        {
            log_fatal() << "Missing block " << current_index;
            handle_fetch(error::missing_object, message::block_locator());
            return;
        }
        hash_digest current_hash = 
            hash_block_header(protobuf_to_block_header(proto_block));
        locator.push_back(current_hash);
    }
    txn->commit();
    handle_fetch(std::error_code(), locator);
}

void bdb_blockchain::fetch_balance(const short_hash& pubkey_hash,
    fetch_handler_balance handle_fetch)
{
}

} // libbitcoin


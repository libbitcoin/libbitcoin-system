#include <bitcoin/blockchain/bdb_blockchain.hpp>

#include <fstream>

#include <boost/filesystem.hpp>

#include DB_CXX_HEADER

#include <bitcoin/transaction.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/utility/serializer.hpp>
#include <bitcoin/format.hpp>

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
    DB_THREAD;

constexpr uint32_t db_flags = DB_CREATE|DB_THREAD;

bdb_blockchain::bdb_blockchain(async_service& service)
  : async_strand(service)
{
#ifndef CXX_COMPAT
    env_ = nullptr;
    db_blocks_ = nullptr;
    db_blocks_hash_ = nullptr;
    db_txs_ = nullptr;
    db_spends_ = nullptr;
    db_address_ = nullptr;
#endif
    reorganize_subscriber_ =
        std::make_shared<reorganize_subscriber_type>(service);
}
bdb_blockchain::~bdb_blockchain()
{
    BITCOIN_ASSERT(!env_);
}

void bdb_blockchain::start(const std::string& prefix,
    start_handler handle_start)
{
    queue(
        [this, prefix, handle_start]
        {
            if (initialize(prefix))
                handle_start(std::error_code());
            else
                handle_start(error::operation_failed);
        });
}
void bdb_blockchain::stop()
{
    reorganize_subscriber_->relay(error::service_stopped,
        0, block_list(), block_list());
    shutdown();
}

template <typename Database>
inline void shutdown_database(Database*& database)
{
    database->close(0);
    delete database;
    database = nullptr;
}
void bdb_blockchain::shutdown()
{
    // Initialisation never started
    if (!env_)
        return;
    // Close secondaries before primaries
    shutdown_database(db_blocks_hash_);
    // Close primaries
    shutdown_database(db_blocks_);
    shutdown_database(db_txs_);
    shutdown_database(db_spends_);
    shutdown_database(db_address_);
    shutdown_database(env_);
    // delete
    google::protobuf::ShutdownProtobufLibrary();
}

bool bdb_blockchain::setup(const std::string& prefix)
{
    async_service fake_service;
    bdb_blockchain handle(fake_service);
    if (!handle.initialize(prefix))
        return false;
    handle.db_blocks_->truncate(nullptr, 0, 0);
    handle.db_txs_->truncate(nullptr, 0, 0);
    handle.db_spends_->truncate(nullptr, 0, 0);
    handle.db_address_->truncate(nullptr, 0, 0);
    // Save genesis block
    txn_guard_ptr txn = std::make_shared<txn_guard>(handle.env_);
    if (!handle.common_->save_block(txn, 0, genesis_block()))
    {
        txn->abort();
        return false;
    }
    txn->commit();
    handle.shutdown();
    return true;
}

// Because BDB is dumb
hash_digest second_hash;

int get_block_hash(Db*, const Dbt*, const Dbt* data, Dbt* second_key)
{
    std::stringstream ss(std::string(
        reinterpret_cast<const char*>(data->get_data()), data->get_size()));
    proto::Block proto_block;
    proto_block.ParseFromIstream(&ss);
    message::block serial_block = proto_to_block_header(proto_block);
    second_hash = hash_block_header(serial_block);
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
    // Try to lock the directory first
    boost::filesystem::path lock_path = prefix;
    lock_path = lock_path / "db-lock";
    std::ofstream touch_file(lock_path.native(), std::ios::app);
    touch_file.close();
    flock_ = lock_path.c_str();
    if (!flock_.try_lock())
    {
        // Database already opened elsewhere
        return false;
    }
    // Continue on
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    env_ = new DbEnv(DB_CXX_NO_EXCEPTIONS);
    env_->set_lk_max_locks(10000);
    env_->set_lk_max_objects(10000);
    env_->set_cachesize(1, 0, 1);
    if (env_->open(prefix.c_str(), env_flags, 0) != 0)
        return false;
    if (env_->set_flags(DB_TXN_NOSYNC, 1) != 0)
        return false;
    // Create database objects
    db_blocks_ = new Db(env_, 0);
    db_blocks_hash_ = new Db(env_, 0);
    db_txs_ = new Db(env_, 0);
    db_spends_ = new Db(env_, 0);
    db_address_ = new Db(env_, 0);
    if (db_blocks_->set_bt_compare(bt_compare_blocks) != 0)
    {
        log_fatal() << "Internal error setting BTREE comparison function";
        return false;
    }
    txn_guard txn(env_);
    if (db_blocks_->open(txn.get(), "blocks", "block-data",
            DB_BTREE, db_flags, 0) != 0)
        return false;
    if (db_blocks_hash_->open(txn.get(), "blocks", "block-hash", 
            DB_BTREE, db_flags, 0) != 0)
        return false;
    db_blocks_->associate(txn.get(), db_blocks_hash_, get_block_hash, 0);
    if (db_txs_->open(txn.get(), "transactions", "tx",
            DB_BTREE, db_flags, 0) != 0)
        return false;
    if (db_spends_->open(txn.get(), "transactions", "spends",
            DB_BTREE, db_flags, 0) != 0)
        return false;
    if (db_address_->set_flags(DB_DUP) != 0)
        return false;
    if (db_address_->open(txn.get(), "address", "address",
            DB_BTREE, db_flags, 0) != 0)
        return false;
    txn.commit();

    common_ = std::make_shared<bdb_common>(env_,
        db_blocks_, db_blocks_hash_, db_txs_, db_spends_, db_address_);

    orphans_ = std::make_shared<orphans_pool>(20);
    bdb_chain_keeper_ptr chainkeeper = 
        std::make_shared<bdb_chain_keeper>(common_, env_,
            db_blocks_, db_blocks_hash_, db_txs_, db_spends_, db_address_);
    chain_ = chainkeeper;
    organize_ = std::make_shared<bdb_organizer>(
        common_, orphans_, chainkeeper, reorganize_subscriber_);

    return true;
}

void bdb_blockchain::store(const message::block& stored_block, 
    store_block_handler handle_store)
{
    queue(
        std::bind(&bdb_blockchain::do_store,
            this, stored_block, handle_store));
}
void bdb_blockchain::do_store(const message::block& stored_block,
    store_block_handler handle_store)
{
    block_detail_ptr stored_detail =
        std::make_shared<block_detail>(stored_block);
    int depth = chain_->find_index(hash_block_header(stored_block));
    if (depth != -1)
    {
        handle_store(error::duplicate,
            block_info{block_status::confirmed, depth});
        return;
    }
    if (!orphans_->add(stored_detail))
    {
        handle_store(error::duplicate,
            block_info{block_status::orphan, 0});
    }
    organize_->start();
    handle_store(stored_detail->errc(), stored_detail->info());
    // Every N blocks, we flush database
    static size_t flush_counter = 0;
    if (++flush_counter == 2000)
    {
        env_->txn_checkpoint(0, 0, 0);
        flush_counter = 0;
    }
}

void bdb_blockchain::import(const message::block& import_block,
    size_t depth, import_block_handler handle_import)
{
    queue(
        std::bind(&bdb_blockchain::do_import,
            this, import_block, depth, handle_import));
}
void bdb_blockchain::do_import(const message::block& import_block,
    size_t depth, import_block_handler handle_import)
{
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    if (!common_->save_block(txn, depth, import_block))
    {
        txn->abort();
        handle_import(error::operation_failed);
        return;
    }
    txn->commit();
    handle_import(std::error_code());
}

template<typename Index>
bool fetch_block_header_impl(txn_guard_ptr txn, const Index& index,
    bdb_common_ptr common, message::block& serial_block)
{
    proto::Block proto_block = common->fetch_proto_block(txn, index);
    if (!proto_block.IsInitialized())
        return false;
    serial_block = proto_to_block_header(proto_block);
    return true;
} 

void bdb_blockchain::fetch_block_header(size_t depth,
    fetch_handler_block_header handle_fetch)
{
    queue(
        std::bind(&bdb_blockchain::fetch_block_header_by_depth,
            this, depth, handle_fetch));
}

void bdb_blockchain::fetch_block_header_by_depth(size_t depth,
    fetch_handler_block_header handle_fetch)
{
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    message::block serial_block;
    if (!fetch_block_header_impl(txn, depth, common_, serial_block))
    {
        txn->abort();
        handle_fetch(error::not_found, message::block());
        return;
    }
    txn->commit();
    handle_fetch(std::error_code(), serial_block);
}

void bdb_blockchain::fetch_block_header(const hash_digest& block_hash,
    fetch_handler_block_header handle_fetch)
{
    queue(
        std::bind(&bdb_blockchain::fetch_block_header_by_hash,
            this, block_hash, handle_fetch));
}

void bdb_blockchain::fetch_block_header_by_hash(
    const hash_digest& block_hash, fetch_handler_block_header handle_fetch)
{
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    message::block serial_block;
    if (!fetch_block_header_impl(txn, block_hash, common_, serial_block))
    {
        txn->abort();
        handle_fetch(error::not_found, message::block());
        return;
    }
    txn->commit();
    handle_fetch(std::error_code(), serial_block);
}

template<typename Index, typename Handler>
void fetch_blk_tx_hashes_impl(const Index& index, DbEnv* env,
    bdb_common_ptr common, Handler handle_fetch)
{
    txn_guard_ptr txn = std::make_shared<txn_guard>(env);
    proto::Block proto_block = common->fetch_proto_block(txn, index);
    if (!proto_block.IsInitialized())
    {
        txn->abort();
        handle_fetch(error::not_found, message::inventory_list());
        return;
    }
    txn->commit();
    message::inventory_list tx_hashes;
    for (const std::string& raw_tx_hash: proto_block.transactions())
    {
        message::inventory_vector tx_inv;
        tx_inv.type = message::inventory_type_id::transaction;
        BITCOIN_ASSERT(raw_tx_hash.size() == tx_inv.hash.size());
        std::copy(raw_tx_hash.begin(), raw_tx_hash.end(),
            tx_inv.hash.begin());
        tx_hashes.push_back(tx_inv);
    }
    handle_fetch(std::error_code(), tx_hashes);
}

void bdb_blockchain::fetch_block_transaction_hashes(size_t depth,
    fetch_handler_block_transaction_hashes handle_fetch)
{
    queue(
        [this, depth, handle_fetch]
        {
            fetch_blk_tx_hashes_impl(depth, env_, common_, handle_fetch);
        });
}

void bdb_blockchain::fetch_block_transaction_hashes(
    const hash_digest& block_hash,
    fetch_handler_block_transaction_hashes handle_fetch)
{
    queue(
        [this, block_hash, handle_fetch]
        {
            fetch_blk_tx_hashes_impl(block_hash, env_, common_, handle_fetch);
        });
}

void bdb_blockchain::fetch_block_depth(const hash_digest& block_hash,
    fetch_handler_block_depth handle_fetch)
{
    queue(
        std::bind(&bdb_blockchain::do_fetch_block_depth,
            this, block_hash, handle_fetch));
}
void bdb_blockchain::do_fetch_block_depth(const hash_digest& block_hash,
    fetch_handler_block_depth handle_fetch)
{
    readable_data_type key;
    key.set(block_hash);
    writable_data_type primary_key;
    empty_data_type ignore_data;
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    if (db_blocks_hash_->pget(txn->get(), key.get(),
        primary_key.get(), ignore_data.get(), 0) != 0)
    {
        txn->abort();
        handle_fetch(error::not_found, 0);
        return;
    }
    txn->commit();
    size_t depth = cast_chunk<uint32_t>(primary_key.data());
    handle_fetch(std::error_code(), depth);
}

void bdb_blockchain::fetch_last_depth(fetch_handler_last_depth handle_fetch)
{
    queue(
        std::bind(&bdb_blockchain::do_fetch_last_depth,
            this, handle_fetch));
}
void bdb_blockchain::do_fetch_last_depth(fetch_handler_last_depth handle_fetch)
{
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    uint32_t last_depth = common_->find_last_block_depth(txn);
    txn->commit();
    if (last_depth == std::numeric_limits<uint32_t>::max())
    {
        handle_fetch(error::not_found, 0);
        return;
    }
    handle_fetch(std::error_code(), last_depth);
}

void bdb_blockchain::fetch_transaction(const hash_digest& transaction_hash,
    fetch_handler_transaction handle_fetch)
{
    queue(
        std::bind(&bdb_blockchain::do_fetch_transaction,
            this, transaction_hash, handle_fetch));
}
void bdb_blockchain::do_fetch_transaction(const hash_digest& transaction_hash,
    fetch_handler_transaction handle_fetch)
{
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    proto::Transaction proto_tx =
        common_->fetch_proto_transaction(txn, transaction_hash);
    txn->commit();
    if (!proto_tx.IsInitialized())
    {
        handle_fetch(error::not_found, message::transaction());
        return;
    }
    message::transaction tx = proto_to_transaction(proto_tx);
    handle_fetch(std::error_code(), tx);
}

void bdb_blockchain::fetch_transaction_index(
    const hash_digest& transaction_hash,
    fetch_handler_transaction_index handle_fetch)
{
    queue(
        std::bind(&bdb_blockchain::do_fetch_transaction_index,
            this, transaction_hash, handle_fetch));
}
void bdb_blockchain::do_fetch_transaction_index(
    const hash_digest& transaction_hash,
    fetch_handler_transaction_index handle_fetch)
{
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    proto::Transaction proto_tx =
        common_->fetch_proto_transaction(txn, transaction_hash);
    txn->commit();
    if (!proto_tx.IsInitialized())
    {
        handle_fetch(error::not_found, 0, 0);
        return;
    }
    size_t parent_block_depth = 0, index_in_parent = 0;
    for (auto parent: proto_tx.parent())
    {
        if (parent.depth() > parent_block_depth)
        {
            parent_block_depth = parent.depth();
            index_in_parent = parent.index();
        }
    }
    handle_fetch(std::error_code(), parent_block_depth, index_in_parent);
}

void bdb_blockchain::fetch_spend(const message::output_point& outpoint,
    fetch_handler_spend handle_fetch)
{
    queue(
        std::bind(&bdb_blockchain::do_fetch_spend,
            this, outpoint, handle_fetch));
}
void bdb_blockchain::do_fetch_spend(const message::output_point& outpoint,
    fetch_handler_spend handle_fetch)
{
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    message::input_point input_spend;
    if (!common_->fetch_spend(txn, outpoint, input_spend))
    {
        txn->abort();
        handle_fetch(error::unspent_output, message::input_point());
        return;
    }
    txn->commit();
    handle_fetch(std::error_code(), input_spend);
}

void bdb_blockchain::fetch_outputs(const payment_address& address,
    fetch_handler_outputs handle_fetch)
{
    if (address.type() != payment_type::pubkey_hash)
        handle_fetch(error::unsupported_payment_type,
            message::output_point_list());
    else
        queue(
            std::bind(&bdb_blockchain::do_fetch_outputs,
                this, address, handle_fetch));
}
void bdb_blockchain::do_fetch_outputs(const payment_address& address,
    fetch_handler_outputs handle_fetch)
{
    // Associated outputs
    message::output_point_list assoc_outs;
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    Dbc* cursor;
    db_address_->cursor(txn->get(), &cursor, 0);
    BITCOIN_ASSERT(cursor != nullptr);
    readable_data_type key;
    // version byte + hash for key
    serializer serial;
    serial.write_byte(address.version());
    serial.write_short_hash(address.hash());
    key.set(serial.data());
    writable_data_type value;
    int ret = cursor->get(key.get(), value.get(), DB_SET);
    while (ret != DB_NOTFOUND)
    {
        message::output_point outpoint;
        // We need a copy not a temporary
        data_chunk raw_outpoint(value.data());
        // Then read the value off
        deserializer deserial(raw_outpoint);
        outpoint.hash = deserial.read_hash();
        outpoint.index = deserial.read_4_bytes();
        assoc_outs.push_back(outpoint);
        ret = cursor->get(key.get(), value.get(), DB_NEXT_DUP);
    }
    cursor->close();
    txn->commit();
    handle_fetch(std::error_code(), assoc_outs);
}

void bdb_blockchain::subscribe_reorganize(
    reorganize_handler handle_reorganize)
{
    reorganize_subscriber_->subscribe(handle_reorganize);
}

} // namespace libbitcoin


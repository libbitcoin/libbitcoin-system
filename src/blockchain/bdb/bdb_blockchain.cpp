#include <bitcoin/blockchain/bdb_blockchain.hpp>

#include DB_CXX_HEADER

#include <bitcoin/transaction.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/utility/serializer.hpp>
#include <bitcoin/data_helpers.hpp>

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
    DB_TXN_NOSYNC|
    DB_CXX_NO_EXCEPTIONS;

constexpr uint32_t db_flags = DB_CREATE|DB_THREAD;

bdb_blockchain::bdb_blockchain(async_service& service,
    const std::string& prefix)
  : strand_(service.get_service())
{
    initialize(prefix);
}

bdb_blockchain::bdb_blockchain(async_service& fake_service)
  : strand_(fake_service.get_service())
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
    env_->set_cachesize(1, 0, 1);
    env_->open(prefix.c_str(), env_flags, 0);
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
    db_blocks_->open(txn.get(), "blocks", "block-data", DB_BTREE, db_flags, 0);
    db_blocks_hash_->open(txn.get(), "blocks", "block-hash", 
        DB_BTREE, db_flags, 0);
    db_blocks_->associate(txn.get(), db_blocks_hash_, get_block_hash, 0);
    db_txs_->open(txn.get(), "transactions", "tx", DB_BTREE, db_flags, 0);
    db_spends_->open(txn.get(), "transactions", "spends",
        DB_BTREE, db_flags, 0);
    db_address_->set_flags(DB_DUP);
    db_address_->open(txn.get(), "address", "address", DB_BTREE, db_flags, 0);
    txn.commit();

    common_ = std::make_shared<bdb_common>(env_,
        db_blocks_, db_blocks_hash_, db_txs_, db_spends_, db_address_);

    reorganize_subscriber_ =
        std::make_shared<reorganize_subscriber_type>(strand_);

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
    strand_.post(
        std::bind(&bdb_blockchain::do_store, shared_from_this(),
            stored_block, handle_store));
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
    handle_store(std::error_code(), stored_detail->info());
    // Every 10 blocks, we flush database
    static size_t flush_counter = 0;
    if (++flush_counter == 2000)
    {
        env_->txn_checkpoint(0, 0, 0);
        flush_counter = 0;
    }
}

template<typename Index>
bool fetch_block_impl(txn_guard_ptr txn, const Index& index,
    bdb_common_ptr common, message::block& serial_block)
{
    protobuf::Block proto_block = common->fetch_proto_block(txn, index);
    if (!proto_block.IsInitialized())
        return false;
    if (!common->reconstruct_block(txn, proto_block, serial_block))
        return false;
    return true;
} 

void bdb_blockchain::fetch_block(size_t depth,
    fetch_handler_block handle_fetch)
{
    strand_.post(
        std::bind(&bdb_blockchain::fetch_block_by_depth, shared_from_this(),
            depth, handle_fetch));
}

void bdb_blockchain::fetch_block_by_depth(size_t depth,
    fetch_handler_block handle_fetch)
{
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    message::block serial_block;
    if (!fetch_block_impl(txn, depth, common_, serial_block))
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
    strand_.post(
        std::bind(&bdb_blockchain::fetch_block_by_hash, shared_from_this(),
            block_hash, handle_fetch));
}

void bdb_blockchain::fetch_block_by_hash(const hash_digest& block_hash, 
    fetch_handler_block handle_fetch)
{
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    message::block serial_block;
    if (!fetch_block_impl(txn, block_hash, common_, serial_block))
    {
        txn->abort();
        handle_fetch(error::missing_object, message::block());
        return;
    }
    txn->commit();
    handle_fetch(std::error_code(), serial_block);
}

void bdb_blockchain::fetch_block_depth(const hash_digest& block_hash,
    fetch_handler_block_depth handle_fetch)
{
    strand_.post(
        std::bind(&bdb_blockchain::do_fetch_block_depth, shared_from_this(),
            block_hash, handle_fetch));
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
        handle_fetch(error::missing_object, 0);
        return;
    }
    txn->commit();
    size_t depth = cast_chunk<uint32_t>(primary_key.data());
    handle_fetch(std::error_code(), depth);
}

void bdb_blockchain::fetch_last_depth(fetch_handler_last_depth handle_fetch)
{
    strand_.post(
        std::bind(&bdb_blockchain::do_fetch_last_depth, shared_from_this(),
            handle_fetch));
}
void bdb_blockchain::do_fetch_last_depth(fetch_handler_last_depth handle_fetch)
{
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    uint32_t last_depth = common_->find_last_block_depth(txn);
    txn->commit();
    if (last_depth == std::numeric_limits<uint32_t>::max())
    {
        handle_fetch(error::missing_object, 0);
        return;
    }
    handle_fetch(std::error_code(), last_depth);
}

void bdb_blockchain::fetch_block_locator(
    fetch_handler_block_locator handle_fetch)
{
    strand_.post(
        std::bind(&bdb_blockchain::do_fetch_block_locator, shared_from_this(), 
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
        txn->abort();
        handle_fetch(error::missing_object, message::block_locator());
        return;
    }

    message::block_locator locator;
    indices_list indices = block_locator_indices(last_block_depth);
    for (size_t current_index: indices)
    {
        // bdb provides no way to lookup secondary index AFAIK
        // we instead regenerate block hash from its header
        protobuf::Block proto_block =
            common_->fetch_proto_block(txn, current_index);
        if (!proto_block.IsInitialized())
        {
            log_fatal() << "Missing block " << current_index;
            txn->abort();
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

void bdb_blockchain::fetch_transaction(const hash_digest& transaction_hash,
    fetch_handler_transaction handle_fetch)
{
    strand_.post(
        std::bind(&bdb_blockchain::do_fetch_transaction, shared_from_this(),
            transaction_hash, handle_fetch));
}
void bdb_blockchain::do_fetch_transaction(const hash_digest& transaction_hash,
    fetch_handler_transaction handle_fetch)
{
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    protobuf::Transaction proto_tx =
        common_->fetch_proto_transaction(txn, transaction_hash);
    txn->commit();
    if (!proto_tx.IsInitialized())
    {
        handle_fetch(error::missing_object, message::transaction());
        return;
    }
    message::transaction tx = protobuf_to_transaction(proto_tx);
    handle_fetch(std::error_code(), tx);
}

void bdb_blockchain::fetch_transaction_index(
    const hash_digest& transaction_hash,
    fetch_handler_transaction_index handle_fetch)
{
    strand_.post(
        std::bind(&bdb_blockchain::do_fetch_transaction_index,
            shared_from_this(), transaction_hash, handle_fetch));
}
void bdb_blockchain::do_fetch_transaction_index(
    const hash_digest& transaction_hash,
    fetch_handler_transaction_index handle_fetch)
{
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    protobuf::Transaction proto_tx =
        common_->fetch_proto_transaction(txn, transaction_hash);
    txn->commit();
    if (!proto_tx.IsInitialized())
    {
        handle_fetch(error::missing_object, 0, 0);
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
    strand_.post(
        std::bind(&bdb_blockchain::do_fetch_spend, shared_from_this(),
            outpoint, handle_fetch));
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

void bdb_blockchain::fetch_outputs(const short_hash& pubkey_hash,
    fetch_handler_outputs handle_fetch)
{
    strand_.post(
        std::bind(&bdb_blockchain::do_fetch_outputs, shared_from_this(),
            pubkey_hash, handle_fetch));
}
void bdb_blockchain::do_fetch_outputs(const short_hash& pubkey_hash,
    fetch_handler_outputs handle_fetch)
{
    // Associated outputs
    message::output_point_list assoc_outs;
    txn_guard_ptr txn = std::make_shared<txn_guard>(env_);
    Dbc* cursor;
    db_address_->cursor(txn->get(), &cursor, 0);
    BITCOIN_ASSERT(cursor != nullptr);
    readable_data_type key;
    key.set(pubkey_hash);
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

} // libbitcoin


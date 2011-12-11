#include <bitcoin/blockchain/bdb_blockchain.hpp>

#include <bitcoin/transaction.hpp>
#include <bitcoin/util/logger.hpp>

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

bdb_blockchain::bdb_blockchain(const std::string& prefix)
{
    initialize(prefix);
}

bdb_blockchain::bdb_blockchain()
{
    // Private method. Should never be called by user!
    // Only by factory methods
}

bdb_blockchain::~bdb_blockchain()
{
    google::protobuf::ShutdownProtobufLibrary();
}

bool bdb_blockchain::setup(const std::string& prefix)
{
    bdb_blockchain handle;
    handle.initialize(prefix);
    handle.db_blocks_->truncate(nullptr, 0, 0);
    handle.db_txs_->truncate(nullptr, 0, 0);
    return handle.save_block(0, genesis_block());
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

void initialize_database(const bdb_guard<DbEnv>& env, bdb_guard<Db>& database)
{
    database.set(new Db(env.get(), 0));
}

void bdb_blockchain::initialize(const std::string& prefix)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    env_.set(new DbEnv(0));
    env_->set_lk_max_locks(10000);
    env_->set_lk_max_objects(10000);
    env_->open(prefix.c_str(), env_flags, 0);
    initialize_database(env_, db_blocks_);
    initialize_database(env_, db_blocks_hash_);
    initialize_database(env_, db_txs_);
    initialize_database(env_, db_txs_hash_);
    txn_guard txn(env_);
    db_blocks_->open(txn.get(), "blocks", "block-data", DB_BTREE, db_flags, 0);
    db_blocks_hash_->open(txn.get(), "blocks", "block-hash", 
        DB_BTREE, db_flags, 0);
    db_blocks_->associate(txn.get(), db_blocks_hash_.get(), get_block_hash, 0);
    db_txs_->open(txn.get(), "transactions", "tx-data", DB_BTREE, db_flags, 0);
    db_txs_hash_->open(txn.get(), "transactions", "tx-hash",
        DB_BTREE, db_flags, 0);
    db_txs_->associate(txn.get(), db_txs_hash_.get(), get_tx_hash, 0);
    txn.commit();
}

bool bdb_blockchain::save_block(size_t depth, 
    const message::block serial_block)
{
    protobuf::Block proto_block =
        block_header_to_protobuf(depth, serial_block);
    txn_guard txn(env_);
    for (const message::transaction& block_tx: serial_block.transactions)
    {
        uint32_t block_tx_id = save_transaction(block_tx);
        if (block_tx_id == 0)
        {
            txn.abort();
            return false;
        }
        proto_block.add_transactions(block_tx_id);
    }
    std::ostringstream oss;
    if (!proto_block.SerializeToOstream(&oss))
    {
        txn.abort();
        return false;
    }
    data_type key, value;
    key.set(depth);
    value.set(oss.str());
    db_blocks_->put(txn.get(), key.get(), value.get(), DB_NOOVERWRITE);
    txn.commit();
}

uint32_t bdb_blockchain::save_transaction(const message::transaction& block_tx)
{
    uint32_t block_tx_id = rand();
    BITCOIN_ASSERT(block_tx_id != 0);
    protobuf::Transaction proto_tx = transaction_to_protobuf(block_tx);
    std::ostringstream oss;
    if (!proto_tx.SerializeToOstream(&oss))
        return 0;
    data_type key, value;
    key.set(block_tx_id);
    value.set(oss.str());
    txn_guard txn(env_);
    // Should check for duplicates first
    db_txs_->put(txn.get(), key.get(), value.get(), DB_NOOVERWRITE);
    txn.commit();
    return block_tx_id;
}

void bdb_blockchain::store(const message::block& block, 
    store_block_handler handle_store)
{
    service()->post(std::bind(
        &bdb_blockchain::do_store, shared_from_this(), block, handle_store));
}
void bdb_blockchain::do_store(const message::block& block,
    store_block_handler handle_store)
{
    static size_t depth = 0;
    ++depth;
    save_block(depth, block);
    handle_store(std::error_code(), block_status::orphan);
}

bool read(Db* database, DbTxn* txn, Dbt* key, std::stringstream& ss)
{
    Dbt data;
    data.set_flags(DB_DBT_MALLOC);
    int ret = database->get(nullptr, key, &data, 0);
    if (!data.get_data())
        return false;
    ss  << std::string(
        reinterpret_cast<const char*>(data.get_data()), data.get_size());
    free(data.get_data());
    return true;
}

template<typename Index, typename ProtoType>
bool proto_read(bdb_guard<Db>& database, txn_guard& txn,
    const Index& index, ProtoType& proto_object)
{
    data_type key;
    key.set(index);
    std::stringstream ss;
    if (!read(database.get(), txn.get(), key.get(), ss))
        return false;
    proto_object.ParseFromIstream(&ss);
    return true;
}

template<typename Index>
bool fetch_block_impl(bdb_guard<Db>& db_block_x, bdb_guard<Db>& db_txs,
    txn_guard& txn, const Index& index, message::block& serial_block)
{
    protobuf::Block proto_block;
    if (!proto_read(db_block_x, txn, index, proto_block))
        return false;
    serial_block = protobuf_to_block_header(proto_block);
    for (uint32_t tx_index: proto_block.transactions())
    {
        protobuf::Transaction proto_tx;
        if (!proto_read(db_txs, txn, tx_index, proto_tx))
            return false;
        serial_block.transactions.push_back(protobuf_to_transaction(proto_tx));
    }
    return true;
} 

void bdb_blockchain::fetch_block(size_t depth, fetch_handler_block handle_fetch)
{
    service()->post(std::bind(
        &bdb_blockchain::fetch_block_by_depth, shared_from_this(),
            depth, handle_fetch));
}

void bdb_blockchain::fetch_block_by_depth(size_t depth,
    fetch_handler_block handle_fetch)
{
    txn_guard txn(env_);
    message::block serial_block;
    if (!fetch_block_impl(db_blocks_, db_txs_, txn, depth, serial_block))
    {
        txn.abort();
        handle_fetch(error::missing_object, message::block());
        return;
    }
    txn.commit();
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
    txn_guard txn(env_);
    message::block serial_block;
    if (!fetch_block_impl(db_blocks_hash_, db_txs_, txn, 
        block_hash, serial_block))
    {
        txn.abort();
        handle_fetch(error::missing_object, message::block());
        return;
    }
    txn.commit();
    handle_fetch(std::error_code(), serial_block);
}

void bdb_blockchain::fetch_block_locator(fetch_handler_block_locator handle_fetch)
{
    service()->post(std::bind(
        &bdb_blockchain::do_fetch_block_locator, shared_from_this(), 
            handle_fetch));
}
void bdb_blockchain::do_fetch_block_locator(
    fetch_handler_block_locator handle_fetch)
{
    txn_guard txn(env_);
    Dbc* cursor;
    db_blocks_->cursor(txn.get(), &cursor, 0);
    Dbt key, data;
    if (cursor->get(&key, &data, DB_LAST) == DB_NOTFOUND)
    {
        log_error() << "Empty blockchain";
        handle_fetch(error::missing_object, message::block_locator());
        return;
    }
    BITCOIN_ASSERT(key.get_size() == 4);
    data_chunk raw_depth;
    extend_data(raw_depth, std::string(
        reinterpret_cast<const char*>(key.get_data()), key.get_size()));
    uint32_t last_block_depth = cast_chunk<uint32_t>(raw_depth);
    cursor->close();

    message::block_locator locator;
    std::vector<size_t> indices = block_locator_indices(last_block_depth);
    for (size_t current_index: indices)
    {
        // bdb provides no way to lookup secondary index AFAIK
        // we instead regenerate block hash from its header
        protobuf::Block proto_block;
        if (!proto_read(db_blocks_, txn, current_index, proto_block))
        {
            log_fatal() << "Missing block " << current_index;
            handle_fetch(error::missing_object, message::block_locator());
            return;
        }
        hash_digest current_hash = 
            hash_block_header(protobuf_to_block_header(proto_block));
        locator.push_back(current_hash);
    }
    txn.commit();

    handle_fetch(std::error_code(), locator);
}

void bdb_blockchain::fetch_balance(const short_hash& pubkey_hash,
    fetch_handler_balance handle_fetch)
{
}

} // libbitcoin


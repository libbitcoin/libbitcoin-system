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
#ifdef LEVELDB_ENABLED

#include <bitcoin/blockchain/leveldb_blockchain.hpp>

#include <fstream>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include <leveldb/cache.h>
#include <leveldb/comparator.h>
#include <leveldb/filter_policy.h>
#include <bitcoin/constants.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/utility/serializer.hpp>
#include <bitcoin/format.hpp>
#include "leveldb_common.hpp"
#include "leveldb_chain_keeper.hpp"
#include "leveldb_organizer.hpp"

namespace libbitcoin {

using std::placeholders::_1;

class height_comparator
  : public leveldb::Comparator
{
public:
    int Compare(const leveldb::Slice& a, const leveldb::Slice& b) const;

    const char* Name() const;
    void FindShortestSeparator(std::string*, const leveldb::Slice&) const {}
    void FindShortSuccessor(std::string*) const {}
};

int height_comparator::Compare(
    const leveldb::Slice& a, const leveldb::Slice& b) const
{
    uint32_t height_a = recreate_height(a), height_b = recreate_height(b);
    if (height_a < height_b)
        return -1;
    else if (height_a > height_b)
        return +1;
    // a == b
    return 0;
}

const char* height_comparator::Name() const
{
    // Named this way for historical reasons.
    return "depth_comparator";
}

leveldb_blockchain::leveldb_blockchain(threadpool& pool)
  : ios_(pool.service()), strand_(pool), reorg_strand_(pool), seqlock_(0)
{
    reorganize_subscriber_ =
        std::make_shared<reorganize_subscriber_type>(pool);
}
leveldb_blockchain::~leveldb_blockchain()
{
    delete open_options_.block_cache;
    delete open_options_.filter_policy;
}

void leveldb_blockchain::start(const std::string& prefix,
    start_handler handle_start)
{
    strand_.randomly_queue(
        [this, prefix, handle_start]
        {
            if (initialize(prefix))
                handle_start(std::error_code());
            else
                handle_start(error::operation_failed);
        });
}

void close(std::unique_ptr<leveldb::DB>& db)
{
    // delete the database, closing it.
    db.reset();
}
void leveldb_blockchain::stop()
{
    reorganize_subscriber_->relay(error::service_stopped,
        0, block_list(), block_list());
    close(db_block_);
    close(db_block_hash_);
    close(db_tx_);
    close(db_spend_);
    close(db_credit_);
    close(db_debit_);
}

bool open_db(const std::string& prefix, const std::string& db_name,
    std::unique_ptr<leveldb::DB>& db, leveldb::Options open_options)
{
    using boost::filesystem::path;
    path db_path = path(prefix) / db_name;
    leveldb::DB* db_base_ptr = nullptr;

    // LevelDB does not accept UNICODE strings for operating systems that
    // support them, so we must explicity pass an non-UNICODE string for the
    // name and home it works out.
    // Also we must normalize the path name format
    // (e.g. backslashes on Windows).
    // http://www.boost.org/doc/libs/1_46_0/libs/filesystem/v3/doc/reference.html#generic-pathname-format
    // Note: this change should have no effect on a POSIX system.
    leveldb::Status status = leveldb::DB::Open(open_options, 
        db_path.generic_string(), &db_base_ptr);

    if (!status.ok())
    {
        log_fatal(LOG_BLOCKCHAIN) << "Internal error opening '"
            << db_name << "' database: " << status.ToString();
        return false;
    }
    BITCOIN_ASSERT(db_base_ptr);
    // The cointainer ensures db_base_ptr is now managed.
    db.reset(db_base_ptr);
    return true;
}

void open_stealth_db(const std::string& prefix,
    std::unique_ptr<mmfile>& file, std::unique_ptr<stealth_database>& db)
{
    using boost::filesystem::path;
    path db_path = path(prefix) / "stealth.db";
    file.reset(new mmfile(db_path.generic_string().c_str()));
    db.reset(new stealth_database(*file));
}

bool leveldb_blockchain::initialize(const std::string& prefix)
{
    using boost::filesystem::path;
    // Try to lock the directory first
    path lock_path = path(prefix) / "db-lock";

    // See related comments above
    std::ofstream touch_file(
        lock_path.generic_string(), std::ios::app);

    touch_file.close();

    // See related comments above, and
    // http://stackoverflow.com/questions/11352641/boostfilesystempath-and-fopen
    flock_ = lock_path.generic_string().c_str();

    if (!flock_.try_lock())
    {
        // Database already opened elsewhere
        return false;
    }
    // Create comparator for blocks database.
    height_comparator_.reset(new height_comparator);
    // Open LevelDB databases
    const size_t cache_size = 1 << 20;
    // block_cache, filter_policy and comparator must be deleted after use!
    open_options_.block_cache = leveldb::NewLRUCache(cache_size / 2);
    open_options_.write_buffer_size = cache_size / 4;
    open_options_.filter_policy = leveldb::NewBloomFilterPolicy(10);
    open_options_.compression = leveldb::kNoCompression;
    open_options_.max_open_files = 256;
    open_options_.create_if_missing = true;
    // The blocks database options needs its height comparator too.
    leveldb::Options blocks_open_options = open_options_;
    blocks_open_options.comparator = height_comparator_.get();
    if (!open_db(prefix, "block", db_block_, blocks_open_options))
        return false;
    if (!open_db(prefix, "block_hash", db_block_hash_, open_options_))
        return false;
    if (!open_db(prefix, "tx", db_tx_, open_options_))
        return false;
    if (!open_db(prefix, "spend", db_spend_, open_options_))
        return false;
    if (!open_db(prefix, "credit", db_credit_, open_options_))
        return false;
    if (!open_db(prefix, "debit", db_debit_, open_options_))
        return false;
    // Open custom databases.
    open_stealth_db(prefix, stealth_file_, db_stealth_);
    // Initialize other components.
    leveldb_databases databases{
        db_block_.get(), db_block_hash_.get(), db_tx_.get(),
        db_spend_.get(), db_credit_.get(), db_debit_.get()};
    special_databases special_dbs{
        db_stealth_.get()};
    // G++ has an internal compiler error when you use the implicit * cast.
    common_ = std::make_shared<leveldb_common>(databases, special_dbs);
    // Validate and organisation components.
    orphans_ = std::make_shared<orphans_pool>(20);
    leveldb_chain_keeper_ptr chainkeeper =
        std::make_shared<leveldb_chain_keeper>(common_, databases);
    chain_ = chainkeeper;
    auto reorg_handler = [this](
        const std::error_code& ec, size_t fork_point,
        const blockchain::block_list& arrivals,
        const blockchain::block_list& replaced)
    {
        // Post this operation without using the strand. Therefore
        // calling the reorganize callbacks won't prevent store() from
        // continuing.
        reorg_strand_.queue(
            [=]()
            {
                reorganize_subscriber_->relay(
                    ec, fork_point, arrivals, replaced);
            });
    };
    organize_ = std::make_shared<leveldb_organizer>(
        common_, orphans_, chainkeeper, reorg_handler);
    return true;
}

void leveldb_blockchain::begin_write()
{
    ++seqlock_;
    // seqlock is now odd.
    BITCOIN_ASSERT(seqlock_ % 2 == 1);
}

void leveldb_blockchain::store(const block_type& stored_block,
    store_block_handler handle_store)
{
    strand_.randomly_queue(
        std::bind(&leveldb_blockchain::do_store,
            this, stored_block, handle_store));
}
void leveldb_blockchain::do_store(const block_type& stored_block,
    store_block_handler handle_store)
{
    begin_write();
    block_detail_ptr stored_detail =
        std::make_shared<block_detail>(stored_block);
    int height = chain_->find_index(hash_block_header(stored_block.header));
    if (height != -1)
    {
        finish_write(handle_store, error::duplicate,
            block_info{block_status::confirmed, static_cast<size_t>(height)});
        return;
    }
    if (!orphans_->add(stored_detail))
    {
        finish_write(handle_store, error::duplicate,
            block_info{block_status::orphan, 0});
        return;
    }
    organize_->start();
    finish_write(handle_store, stored_detail->errc(), stored_detail->info());
}

void leveldb_blockchain::import(const block_type& import_block,
    size_t height, import_block_handler handle_import)
{
    strand_.randomly_queue(
        &leveldb_blockchain::do_import,
            this, import_block, height, handle_import);
}
void leveldb_blockchain::do_import(const block_type& import_block,
    size_t height, import_block_handler handle_import)
{
    begin_write();
    if (!common_->save_block(height, import_block))
    {
        finish_write(handle_import, error::operation_failed);
        return;
    }
    finish_write(handle_import, std::error_code());
}

void leveldb_blockchain::fetch(perform_read_functor perform_read)
{
    // Implements the seqlock counter logic.
    auto try_read = [this, perform_read]
        {
            size_t slock = seqlock_;
            if (slock % 2 == 1)
                return false;
            if (perform_read(slock))
                return true;
            return false;
        };
    // Initiate async read operation.
    ios_.post([this, try_read]
        {
            // Sleeping inside seqlock loop is fine since we
            // need to finish write op before we can read anyway.
            while (!try_read())
                std::this_thread::sleep_for(std::chrono::microseconds(100000));
        });
}

void leveldb_blockchain::fetch_block_header(size_t height,
    fetch_handler_block_header handle_fetch)
{
    fetch(
        std::bind(&leveldb_blockchain::fetch_block_header_by_height,
            this, height, handle_fetch, _1));
}

bool leveldb_blockchain::fetch_block_header_by_height(size_t height,
    fetch_handler_block_header handle_fetch, size_t slock)
{
    leveldb_block_info blk;
    if (!common_->get_block(blk, height, true, false))
        return finish_fetch(slock, handle_fetch,
            error::not_found, block_header_type());
    return finish_fetch(slock, handle_fetch, std::error_code(), blk.header);
}

void leveldb_blockchain::fetch_block_header(const hash_digest& block_hash,
    fetch_handler_block_header handle_fetch)
{
    fetch(
        std::bind(&leveldb_blockchain::fetch_block_header_by_hash,
            this, block_hash, handle_fetch, _1));
}

bool leveldb_blockchain::fetch_block_header_by_hash(
    const hash_digest& block_hash,
    fetch_handler_block_header handle_fetch, size_t slock)
{
    leveldb_block_info blk;
    uint32_t height = common_->get_block_height(block_hash);
    if (height == std::numeric_limits<uint32_t>::max() ||
        !common_->get_block(blk, height, true, false))
    {
        return finish_fetch(slock, handle_fetch,
            error::not_found, block_header_type());
    }
    return finish_fetch(slock, handle_fetch, std::error_code(), blk.header);
}

template<typename Handler, typename SeqLockPtr>
bool fetch_blk_tx_hashes_impl(size_t height,
    leveldb_common_ptr common, Handler handle_fetch,
    size_t slock, SeqLockPtr seqlock)
{
    leveldb_block_info blk;
    if (!common->get_block(blk, height, false, true))
    {
        if (slock != *seqlock)
            return false;
        handle_fetch(error::not_found, hash_digest_list());
        return true;
    }
    if (slock != *seqlock)
        return false;
    handle_fetch(std::error_code(), blk.tx_hashes);
    return true;
}

void leveldb_blockchain::fetch_block_transaction_hashes(size_t height,
    fetch_handler_block_transaction_hashes handle_fetch)
{
    fetch(
        [this, height, handle_fetch](size_t slock)
        {
            return fetch_blk_tx_hashes_impl(
                height, common_, handle_fetch, slock, &seqlock_);
        });
}

void leveldb_blockchain::fetch_block_transaction_hashes(
    const hash_digest& block_hash,
    fetch_handler_block_transaction_hashes handle_fetch)
{
    fetch(
        [this, block_hash, handle_fetch](size_t slock)
        {
            uint32_t height = common_->get_block_height(block_hash);
            if (height == std::numeric_limits<uint32_t>::max())
                return false;
            return fetch_blk_tx_hashes_impl(
                height, common_, handle_fetch, slock, &seqlock_);
        });
}

void leveldb_blockchain::fetch_block_height(const hash_digest& block_hash,
    fetch_handler_block_height handle_fetch)
{
    fetch(
        std::bind(&leveldb_blockchain::do_fetch_block_height,
            this, block_hash, handle_fetch, _1));
}
bool leveldb_blockchain::do_fetch_block_height(const hash_digest& block_hash,
    fetch_handler_block_height handle_fetch, size_t slock)
{
    uint32_t height = common_->get_block_height(block_hash);
    if (height == std::numeric_limits<uint32_t>::max())
        return finish_fetch(slock, handle_fetch, error::not_found, 0);
    return finish_fetch(slock, handle_fetch, std::error_code(), height);
}

void leveldb_blockchain::fetch_last_height(
    fetch_handler_last_height handle_fetch)
{
    fetch(
        std::bind(&leveldb_blockchain::do_fetch_last_height,
            this, handle_fetch, _1));
}
bool leveldb_blockchain::do_fetch_last_height(
    fetch_handler_last_height handle_fetch, size_t slock)
{
    uint32_t last_height = common_->find_last_block_height();
    if (last_height == std::numeric_limits<uint32_t>::max())
        return finish_fetch(slock, handle_fetch, error::not_found, 0);
    return finish_fetch(slock, handle_fetch, std::error_code(), last_height);
}

void leveldb_blockchain::fetch_transaction(
    const hash_digest& transaction_hash,
    fetch_handler_transaction handle_fetch)
{
    fetch(
        std::bind(&leveldb_blockchain::do_fetch_transaction,
            this, transaction_hash, handle_fetch, _1));
}
bool leveldb_blockchain::do_fetch_transaction(
    const hash_digest& transaction_hash,
    fetch_handler_transaction handle_fetch, size_t slock)
{
    leveldb_tx_info tx;
    if (!common_->get_transaction(tx, transaction_hash, false, true))
        return finish_fetch(slock, handle_fetch,
            error::not_found, transaction_type());
    return finish_fetch(slock, handle_fetch, std::error_code(), tx.tx);
}

void leveldb_blockchain::fetch_transaction_index(
    const hash_digest& transaction_hash,
    fetch_handler_transaction_index handle_fetch)
{
    fetch(
        std::bind(&leveldb_blockchain::do_fetch_transaction_index,
            this, transaction_hash, handle_fetch, _1));
}
bool leveldb_blockchain::do_fetch_transaction_index(
    const hash_digest& transaction_hash,
    fetch_handler_transaction_index handle_fetch, size_t slock)
{
    leveldb_tx_info tx;
    if (!common_->get_transaction(tx, transaction_hash, true, false))
        return finish_fetch(slock, handle_fetch, error::not_found, 0, 0);
    return finish_fetch(slock, handle_fetch, std::error_code(),
        tx.height, tx.index);
}

void leveldb_blockchain::fetch_spend(const output_point& outpoint,
    fetch_handler_spend handle_fetch)
{
    fetch(
        std::bind(&leveldb_blockchain::do_fetch_spend,
            this, outpoint, handle_fetch, _1));
}
bool leveldb_blockchain::do_fetch_spend(const output_point& outpoint,
    fetch_handler_spend handle_fetch, size_t slock)
{
    input_point input_spend;
    if (!common_->fetch_spend(outpoint, input_spend))
        return finish_fetch(slock, handle_fetch,
            error::unspent_output, input_point());
    return finish_fetch(slock, handle_fetch, std::error_code(), input_spend);
}

const uint8_t* slice_begin(const leveldb::Slice& data_slice)
{
    return reinterpret_cast<const uint8_t*>(data_slice.data());
}

class point_iterator
{
public:
    typedef std::unique_ptr<leveldb::DB> database_ptr;

    point_iterator(database_ptr& db, const payment_address& address)
      : raw_address_(1 + short_hash_size)
    {
        set_raw_address(address);
        set_iterator(db, raw_address_);
    }
    ~point_iterator()
    {
        BITCOIN_ASSERT(it_->status().ok());
    }

    bool valid()
    {
        return it_->Valid() && it_->key().starts_with(slice(raw_address_));
    }

    void load()
    {
        // Deserialize
        load_checksum(it_->key());
        load_data(it_->value());
    }

    void next()
    {
        it_->Next();
    }

    uint64_t checksum() const
    {
        return checksum_;
    }

protected:
    virtual void load_data(leveldb::Slice data) = 0;

private:
    void set_raw_address(const payment_address& address)
    {
        BITCOIN_ASSERT(raw_address_.size() == 1 + short_hash_size);
        auto serial = make_serializer(raw_address_.begin());
        serial.write_byte(address.version());
        serial.write_short_hash(address.hash());
        BITCOIN_ASSERT(
            std::distance(raw_address_.begin(), serial.iterator()) ==
            1 + short_hash_size);
    }

    void set_iterator(database_ptr& db, const data_chunk& raw_address)
    {
        it_.reset(db->NewIterator(leveldb::ReadOptions()));
        it_->Seek(slice(raw_address));
    }

    void load_checksum(leveldb::Slice key)
    {
        const uint8_t* begin = slice_begin(key.data());
        const uint8_t* end = begin + key.size();
        BITCOIN_ASSERT(key.size() == 1 + short_hash_size + 8);
        auto deserial = make_deserializer(begin + 1 + short_hash_size, end);
        checksum_ = deserial.read_8_bytes();
        BITCOIN_ASSERT(deserial.iterator() == end);
    }

    leveldb_iterator it_;
    data_chunk raw_address_;

    uint64_t checksum_;
};

class outpoint_iterator
  : public point_iterator
{
public:
    outpoint_iterator(database_ptr& db, const payment_address& address)
      : point_iterator(db, address) {}

    const output_point& outpoint() const
    {
        return outpoint_;
    }
    uint64_t value() const
    {
        return value_;
    }
    uint32_t height() const
    {
        return height_;
    }

protected:
    void load_data(leveldb::Slice data)
    {
        const uint8_t* begin = slice_begin(data.data());
        const uint8_t* end = begin + data.size();
        BITCOIN_ASSERT(data.size() == 36 + 8 + 4);
        auto deserial = make_deserializer(begin, end);
        outpoint_.hash = deserial.read_hash();
        outpoint_.index = deserial.read_4_bytes();
        value_ = deserial.read_8_bytes();
        height_ = deserial.read_4_bytes();
        BITCOIN_ASSERT(deserial.iterator() == end);
    }

private:
    output_point outpoint_;
    uint64_t value_;
    uint32_t height_;
};

class inpoint_iterator
  : public point_iterator
{
public:
    inpoint_iterator(database_ptr& db, const payment_address& address)
      : point_iterator(db, address) {}

    input_point inpoint()
    {
        return inpoint_;
    }
    uint32_t height() const
    {
        return height_;
    }

protected:
    void load_data(leveldb::Slice data)
    {
        const uint8_t* begin = slice_begin(data.data());
        const uint8_t* end = begin + data.size();
        BITCOIN_ASSERT(data.size() == 36 + 4);
        auto deserial = make_deserializer(begin, end);
        inpoint_.hash = deserial.read_hash();
        inpoint_.index = deserial.read_4_bytes();
        height_ = deserial.read_4_bytes();
        BITCOIN_ASSERT(deserial.iterator() == end);
    }

private:
    input_point inpoint_;
    uint32_t height_ = max_index;
};

void leveldb_blockchain::fetch_history(const payment_address& address,
    fetch_handler_history handle_fetch, size_t from_height)
{
    fetch(
        std::bind(&leveldb_blockchain::do_fetch_history,
            this, address, handle_fetch, from_height, _1));
}
bool leveldb_blockchain::do_fetch_history(const payment_address& address,
    fetch_handler_history handle_fetch, size_t from_height, size_t slock)
{
#ifdef _WIN32
    constexpr uint32_t max_height = UINT_LEAST32_MAX;
#else
    constexpr uint32_t max_height = std::numeric_limits<uint32_t>::max();
#endif

    struct spend_data
    {
        input_point point;
        uint32_t height;
    };
    typedef std::unordered_map<uint64_t, spend_data> spend_map;

    // First we create map of spends...
    spend_map spends;
    for (inpoint_iterator debit_it(db_debit_, address);
        debit_it.valid(); debit_it.next())
    {
        debit_it.load();
        uint64_t checksum = debit_it.checksum();
        spends.emplace(checksum,
            spend_data{debit_it.inpoint(), debit_it.height()});
    }
    // ... Then we load outputs.
    history_list history;
    for (outpoint_iterator credit_it(db_credit_, address);
        credit_it.valid(); credit_it.next())
    {
        credit_it.load();
        // Row with no spend (yet)
        history_row row{
            credit_it.outpoint(),
            credit_it.height(),
            credit_it.value(),
            input_point{null_hash, max_index},
            max_height
        };
        // Now search for the spend. If it exists,
        // then load and add it to the row.
        uint64_t checksum = credit_it.checksum();
        auto it = spends.find(checksum);
        const bool spend_exists = it != spends.end();
        if (spend_exists)
        {
            const spend_data& data = it->second;
            row.spend = data.point;
            row.spend_height = data.height;
            BITCOIN_ASSERT(row.spend_height >= row.output_height);
        }
        // Filter entries below the from_height.
        if (row.output_height >= from_height ||
            (spend_exists && row.spend_height >= from_height))
        {
            history.push_back(row);
        }
    }
    // Finish.
    return finish_fetch(slock, handle_fetch, std::error_code(), history);
}

void leveldb_blockchain::fetch_stealth(const stealth_prefix& prefix,
    fetch_handler_stealth handle_fetch, size_t from_height)
{
    fetch(
        std::bind(&leveldb_blockchain::do_fetch_stealth,
            this, prefix, handle_fetch, from_height, _1));
}
bool leveldb_blockchain::do_fetch_stealth(const stealth_prefix& prefix,
    fetch_handler_stealth handle_fetch, size_t from_height, size_t slock)
{
    stealth_list results;
    auto read_func = [&results, prefix](const uint8_t* it)
    {
        if (!stealth_match(prefix, it))
            return;
        constexpr uint32_t row_size = 4 + 33 + 21 + 32;
        // Skip bitfield value since we don't need it.
        auto deserial = make_deserializer(it + 4, it + row_size);
        stealth_row row;
        row.ephemkey = deserial.read_data(33);
        uint8_t address_version = deserial.read_byte();
        const short_hash address_hash = deserial.read_short_hash();
        row.address.set(address_version, address_hash);
        row.transaction_hash = deserial.read_hash();
        BITCOIN_ASSERT(deserial.iterator() == it + row_size);
        results.push_back(row);
    };
    BITCOIN_ASSERT(db_stealth_);
    db_stealth_->scan(read_func, from_height);
    // Finish.
    return finish_fetch(slock, handle_fetch, std::error_code(), results);
}

void leveldb_blockchain::subscribe_reorganize(
    reorganize_handler handle_reorganize)
{
    reorganize_subscriber_->subscribe(handle_reorganize);
}

} // namespace libbitcoin

#endif
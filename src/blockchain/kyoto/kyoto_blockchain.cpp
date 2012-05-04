#include <bitcoin/blockchain/kyoto_blockchain.hpp>

#include <fstream>

#include <boost/filesystem.hpp>

#include <bitcoin/utility/assert.hpp>

#include "kyoto_common.hpp"

namespace libbitcoin {

namespace fs = boost::filesystem;
namespace ky = kyotocabinet;

kyoto_blockchain_options::kyoto_blockchain_options()
  : create_if_missing(true)
{
}

blockchain_ptr kyoto_blockchain::create(async_service& service,
    const std::string& prefix, start_handler handle_start,
    const kyoto_blockchain_options& options)
{
    kyoto_blockchain* kyoto_chain = new kyoto_blockchain(service);
    blockchain_ptr chain(kyoto_chain);
    kyoto_chain->service_.post(
        [=, chain]()
        {
            kyoto_chain->initialize(prefix, handle_start, options);
        });
    return chain;
}

kyoto_blockchain::kyoto_blockchain(async_service& service)
  : service_(service.get_service())
{
}

void kyoto_blockchain::initialize(const std::string& prefix,
    start_handler handle_start, const kyoto_blockchain_options& options)
{
    auto failure_signal =
        std::bind(handle_start, error::start_failed, nullptr, false);
    bool newly_created = false;
    if (!fs::is_regular_file(fs::path(prefix) / "blocks.kct"))
    {
        if (options.create_if_missing)
            newly_created = true;
        else
        {
            failure_signal();
            return;
        }
    }
    // Try to lock the directory first
    fs::path lock_path = prefix;
    lock_path /= "db-lock";
    std::ofstream touch_file(lock_path.native(), std::ios::app);
    touch_file.close();
    flock = lock_path.c_str();
    // Database already opened elsewhere
    if (!flock.try_lock())
    {
        failure_signal();
        return;
    }
    common_ = std::make_shared<kyoto_common>();
    if (!common_->start_databases(prefix, options))
    {
        failure_signal();
        return;
    }
    if (newly_created)
    {
        // Add genesis block to blockchain
        common_->save_block(0, genesis_block());
    }
    BITCOIN_ASSERT(!newly_created || options.create_if_missing);
    handle_start(std::error_code(), shared_from_this(), newly_created);
}

void save_block(const message::block& save_block)
{
}

void kyoto_blockchain::store(
    const message::block& stored_block,
    store_block_handler handle_store)
{
}

// fetch block header by depth
void kyoto_blockchain::fetch_block_header(size_t depth,
    fetch_handler_block_header handle_fetch)
{
    auto this_ptr = shared_from_this();
    service_.post(
        [=, this_ptr]()
        {
            message::block blk;
            if (!common_->fetch_block_header(depth, blk))
                handle_fetch(error::not_found, message::block());
            else
                handle_fetch(std::error_code(), blk);
        });
}

// fetch block header by hash
void kyoto_blockchain::fetch_block_header(
    const hash_digest& block_hash,
    fetch_handler_block_header handle_fetch)
{
}

// fetch transaction hashes in block by depth
void kyoto_blockchain::fetch_block_transaction_hashes(
    size_t depth, fetch_handler_block_transaction_hashes handle_fetch)
{
}

// fetch transaction hashes in block by hash
void kyoto_blockchain::fetch_block_transaction_hashes(
    const hash_digest& block_hash,
    fetch_handler_block_transaction_hashes handle_fetch)
{
}

// fetch depth of block by hash
void kyoto_blockchain::fetch_block_depth(const hash_digest& block_hash,
    fetch_handler_block_depth handle_fetch)
{
}

// fetch depth of latest block
void kyoto_blockchain::fetch_last_depth(
    fetch_handler_last_depth handle_fetch)
{
}

// fetch transaction by hash
void kyoto_blockchain::fetch_transaction(
    const hash_digest& transaction_hash,
    fetch_handler_transaction handle_fetch)
{
}

// fetch depth and offset within block of transaction by hash
void kyoto_blockchain::fetch_transaction_index(
    const hash_digest& transaction_hash,
    fetch_handler_transaction_index handle_fetch)
{
}

// fetch spend of an output point
void kyoto_blockchain::fetch_spend(const message::output_point& outpoint,
    fetch_handler_spend handle_fetch)
{
}

// fetch outputs associated with an address
void kyoto_blockchain::fetch_outputs(const payment_address& address,
    fetch_handler_outputs handle_fetch)
{
}

void kyoto_blockchain::subscribe_reorganize(
    reorganize_handler handle_reorganize)
{
}

} // libbitcoin


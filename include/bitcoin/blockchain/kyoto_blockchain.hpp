#ifndef LIBBITCOIN_BLOCKCHAIN_KYOTO_BLOCKCHAIN_H
#define LIBBITCOIN_BLOCKCHAIN_KYOTO_BLOCKCHAIN_H

#include <bitcoin/blockchain/blockchain.hpp>

#include <boost/interprocess/sync/file_lock.hpp>

#include <kcpolydb.h>

#include <bitcoin/async_service.hpp>

namespace libbitcoin {

struct kyoto_blockchain_options
{
    kyoto_blockchain_options();
    bool create_if_missing;
};

class kyoto_common;
typedef std::shared_ptr<kyoto_common> kyoto_common_ptr;

class kyoto_blockchain
  : public blockchain,
    public std::enable_shared_from_this<kyoto_blockchain>
{
public:
    typedef std::function<
        void (const std::error_code, blockchain_ptr, bool)> start_handler;

    static blockchain_ptr create(async_service& service,
        const std::string& prefix,
        start_handler handle_start =
            [](const std::error_code&, blockchain_ptr, bool) {},
        const kyoto_blockchain_options& options=kyoto_blockchain_options());

    // Non-copyable
    kyoto_blockchain(const kyoto_blockchain&) = delete;
    void operator=(const kyoto_blockchain&) = delete;

    void store(const message::block& stored_block,
        store_block_handler handle_store);

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
    kyoto_blockchain(async_service& service);
    void initialize(const std::string& prefix, start_handler handle_start,
        const kyoto_blockchain_options& options);

    io_service& service_;
    boost::interprocess::file_lock flock;

    kyoto_common_ptr common_;
};

} // libbitcoin

#endif


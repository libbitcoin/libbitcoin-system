#ifndef LIBBITCOIN_BLOCKCHAIN_BLOCKCHAIN_H
#define LIBBITCOIN_BLOCKCHAIN_BLOCKCHAIN_H

#include <boost/utility.hpp>
#include <functional>

#include <bitcoin/block.hpp>
#include <bitcoin/error.hpp>
#include <bitcoin/messages.hpp>

namespace libbitcoin {

class blockchain
{
public:
    typedef std::function<void (const std::error_code&, block_info)>
        store_block_handler;

    // Not supported in g++ yet
    //template <typename Message>
    //using fetch_handler = std::function<void (Message)>;

    typedef std::function<
        void (const std::error_code&, const message::block&)>
            fetch_handler_block_header;

    typedef std::function<
        void (const std::error_code&, const message::inventory_list&)>
            fetch_handler_block_transaction_hashes;

    typedef std::function<void (const std::error_code&, size_t)>
        fetch_handler_block_depth;

    typedef std::function<void (const std::error_code&, size_t)>
        fetch_handler_last_depth;

    typedef std::function<
        void (const std::error_code&, const message::block_locator&)>
            fetch_handler_block_locator;

    typedef std::function<
        void (const std::error_code&, const message::transaction&)>
            fetch_handler_transaction;

    typedef std::function<
        void (const std::error_code&, size_t, size_t)>
            fetch_handler_transaction_index;

    typedef std::function<
        void (const std::error_code&, const message::input_point&)>
            fetch_handler_spend;

    typedef std::function<
        void (const std::error_code&, const message::output_point_list&)>
            fetch_handler_outputs;

    typedef std::vector<std::shared_ptr<message::block>> block_list;
    typedef std::function<
        void (const std::error_code&, size_t, 
            const block_list&, const block_list&)> reorganize_handler;

    virtual void store(const message::block& stored_block, 
        store_block_handler handle_store) = 0;

    // fetch block header by depth
    virtual void fetch_block_header(size_t depth,
        fetch_handler_block_header handle_fetch) = 0;
    // fetch block header by hash
    virtual void fetch_block_header(const hash_digest& block_hash,
        fetch_handler_block_header handle_fetch) = 0;
    // fetch transaction hashes in block by depth
    virtual void fetch_block_transaction_hashes(size_t depth,
        fetch_handler_block_transaction_hashes handle_fetch) = 0;
    // fetch transaction hashes in block by hash
    virtual void fetch_block_transaction_hashes(const hash_digest& block_hash,
        fetch_handler_block_transaction_hashes handle_fetch) = 0;
    // fetch depth of block by hash
    virtual void fetch_block_depth(const hash_digest& block_hash,
        fetch_handler_block_depth handle_fetch) = 0;
    // fetch depth of latest block
    virtual void fetch_last_depth(fetch_handler_last_depth handle_fetch) = 0;
    // fetch block locator
    virtual void fetch_block_locator(
        fetch_handler_block_locator handle_fetch) = 0;
    // fetch transaction by hash
    virtual void fetch_transaction(const hash_digest& transaction_hash,
        fetch_handler_transaction handle_fetch) = 0;
    // fetch depth and offset within block of transaction by hash
    virtual void fetch_transaction_index(
        const hash_digest& transaction_hash,
        fetch_handler_transaction_index handle_fetch) = 0;

    // fetch of inputs and outputs is a future possibility
    // for now use fetch_transaction and lookup input/output

    // fetch spend of an output point
    virtual void fetch_spend(const message::output_point& outpoint,
        fetch_handler_spend handle_fetch) = 0;
    // fetch outputs associated with an address
    virtual void fetch_outputs(const short_hash& pubkey_hash,
        fetch_handler_outputs handle_fetch) = 0;

    virtual void subscribe_reorganize(
        reorganize_handler handle_reorganize) = 0;
};

typedef std::function<void (const std::error_code&, const message::block&)>
    blockchain_fetch_handler_block;

void fetch_block(blockchain_ptr chain, size_t depth,
    blockchain_fetch_handler_block handle_fetch);
void fetch_block(blockchain_ptr chain, const hash_digest& block_hash,
    blockchain_fetch_handler_block handle_fetch);

typedef std::function<
    void (const std::error_code&, const message::block_locator&)>
        blockchain_fetch_handler_block_locator;

void fetch_block_locator(blockchain_ptr chain,
    blockchain_fetch_handler_block_locator handle_fetch);

} // namespace libbitcoin

#endif


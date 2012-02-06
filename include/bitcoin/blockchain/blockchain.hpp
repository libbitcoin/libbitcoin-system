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
            fetch_handler_block;

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

    virtual void store(const message::block& stored_block, 
        store_block_handler handle_store) = 0;

    virtual void fetch_block(size_t depth,
        fetch_handler_block handle_fetch) = 0;
    virtual void fetch_block(const hash_digest& block_hash,
        fetch_handler_block handle_fetch) = 0;
    virtual void fetch_block_depth(const hash_digest& block_hash,
        fetch_handler_block_depth handle_fetch) = 0;
    virtual void fetch_last_depth(fetch_handler_last_depth handle_fetch) = 0;
    virtual void fetch_block_locator(
        fetch_handler_block_locator handle_fetch) = 0;
    virtual void fetch_transaction(const hash_digest& transaction_hash,
        fetch_handler_transaction handle_fetch) = 0;
    virtual void fetch_transaction_index(
        const hash_digest& transaction_hash,
        fetch_handler_transaction_index handle_fetch) = 0;

    // fetch of inputs and outputs is a future possibility
    // for now use fetch_transaction and lookup input/output

    // Returns input_point
    virtual void fetch_spend(const message::output_point& outpoint,
        fetch_handler_spend handle_fetch) = 0;
    // List of output_points
    virtual void fetch_outputs(const short_hash& pubkey_hash,
        fetch_handler_outputs handle_fetch) = 0;
};

} // libbitcoin

#endif


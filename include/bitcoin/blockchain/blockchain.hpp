#ifndef LIBBITCOIN_BLOCKCHAIN_BLOCKCHAIN_H
#define LIBBITCOIN_BLOCKCHAIN_BLOCKCHAIN_H

#include <boost/utility.hpp>
#include <functional>

#include <bitcoin/block.hpp>
#include <bitcoin/error.hpp>
#include <bitcoin/messages.hpp>

namespace libbitcoin {

class blockchain
 : private boost::noncopyable
{
public:
    typedef std::function<void (const std::error_code&)> store_handler;
    typedef std::function<void (const std::error_code&, block_info)>
        store_block_handler;

    // Not supported in g++ yet
    //template <typename Message>
    //using fetch_handler = std::function<void (Message)>;

    typedef std::function<
        void (const std::error_code&, const message::block&)>
            fetch_handler_block;

    typedef std::function<void (const std::error_code&, size_t)>
        fetch_handler_last_depth;

    typedef std::function<
        void (const std::error_code&, const message::block_locator&)>
            fetch_handler_block_locator;

    typedef std::function<void (const std::error_code&, uint64_t)>
        fetch_handler_balance;

    virtual void store(const message::block& stored_block, 
        store_block_handler handle_store) = 0;

    virtual void fetch_block(size_t depth,
        fetch_handler_block handle_fetch) = 0;
    virtual void fetch_block(const hash_digest& block_hash,
        fetch_handler_block handle_fetch) = 0;
    virtual void fetch_last_depth(fetch_handler_last_depth handle_fetch) = 0;
    virtual void fetch_block_locator(
        fetch_handler_block_locator handle_fetch) = 0;
    virtual void fetch_balance(const short_hash& pubkey_hash,
        fetch_handler_balance handle_fetch) = 0;
};

} // libbitcoin

#endif


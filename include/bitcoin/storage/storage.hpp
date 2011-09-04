#ifndef LIBBITCOIN_STORAGE_STORAGE_H
#define LIBBITCOIN_STORAGE_STORAGE_H

#include <boost/utility.hpp>
#include <functional>

#include <bitcoin/error.hpp>
#include <bitcoin/messages.hpp>

namespace libbitcoin {

class storage
  : private boost::noncopyable
{
public:
    typedef std::function<void (const std::error_code&)> store_handler;

    // Not supported in g++ yet
    //template <typename Message>
    //using fetch_handler = std::function<void (Message)>;
    typedef std::function<void (
        const std::error_code&, const message::inv_list&)>
                fetch_handler_inventories;

    typedef std::function<void (
        const std::error_code&, const message::block&)>
            fetch_handler_block;

    typedef std::function<void (
        const std::error_code&, const message::block_locator&)>
            fetch_handler_block_locator;

    typedef std::function<void (
        const std::error_code&, const message::transaction_output&)> 
            fetch_handler_output;

    typedef std::function<void (const std::error_code&, bool)> exists_handler;

    virtual void store(const message::inv& inv,
            store_handler handle_store) = 0;
    virtual void store(const message::transaction& transaction,
            store_handler handle_store) = 0;
    virtual void store(const message::block& block, 
            store_handler handle_store) = 0;

    virtual void fetch_inventories(
            fetch_handler_inventories handle_fetch) = 0;
    virtual void fetch_block_by_depth(size_t block_number,
            fetch_handler_block handle_fetch) = 0;
    virtual void fetch_block_by_hash(hash_digest block_hash,    
            fetch_handler_block handle_fetch) = 0;
    virtual void fetch_block_locator(
            fetch_handler_block_locator handle_fetch) = 0;
    virtual void fetch_output_by_hash(hash_digest transaction_hash, 
            uint32_t index, fetch_handler_output handle_fetch) = 0;

    virtual void block_exists_by_hash(hash_digest block_hash,
            exists_handler handle_exists) = 0;
};

} // libbitcoin

#endif


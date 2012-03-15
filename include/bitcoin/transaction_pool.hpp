#ifndef LIBBITCOIN_TRANSACTION_POOL_H
#define LIBBITCOIN_TRANSACTION_POOL_H

#include <functional>
#include <boost/circular_buffer.hpp>

#include <bitcoin/async_service.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/messages.hpp>
#include <bitcoin/utility/threads.hpp>
#include <bitcoin/blockchain/blockchain.hpp>

namespace libbitcoin {

struct transaction_entry_info
{
    typedef std::function<void (const std::error_code&)> confirm_handler;
    hash_digest hash;
    message::transaction tx;
    confirm_handler handle_confirm;
};

typedef boost::circular_buffer<transaction_entry_info> pool_buffer;

class transaction_pool
  : public std::enable_shared_from_this<transaction_pool>
{
public:
    typedef std::function<void (const std::error_code&)> store_handler;
    typedef std::function<void (bool)> exists_handler;

    typedef transaction_entry_info::confirm_handler confirm_handler;

    static transaction_pool_ptr create(
        async_service& service, blockchain_ptr chain);

    // Non-copyable
    transaction_pool(const transaction_pool&) = delete;
    void operator=(const transaction_pool&) = delete;

    void store(const message::transaction& stored_transaction,
        confirm_handler handle_confirm, store_handler handle_store);
    void exists(const hash_digest& transaction_hash,
        exists_handler handle_exists);

private:
    transaction_pool(async_service& service);
    void initialize(blockchain_ptr chain);

    void do_store(const message::transaction& stored_transaction,
        confirm_handler handle_confirm, store_handler handle_store);
    void handle_delegate(const std::error_code& ec, 
        const message::transaction& tx, store_handler handle_store);

    void do_exists(const hash_digest& transaction_hash,
        exists_handler handle_exists);

    void reorganize(const std::error_code& ec,
        size_t fork_point,
        const blockchain::block_list& new_blocks,
        const blockchain::block_list& replaced_blocks);
    void resubmit_all();
    void takeout_confirmed(const blockchain::block_list& new_blocks);
    void try_delete(const hash_digest& tx_hash);

    io_service::strand strand_;
    blockchain_ptr chain_;
    pool_buffer pool_;
};

} // libbitcoin

#endif


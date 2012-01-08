#ifndef LIBBITCOIN_TRANSACTION_POOL_H
#define LIBBITCOIN_TRANSACTION_POOL_H

#include <functional>
#include <boost/circular_buffer.hpp>

#include <bitcoin/types.hpp>
#include <bitcoin/messages.hpp>
#include <bitcoin/utility/threads.hpp>

namespace libbitcoin {

struct transaction_entry_info
{
    hash_digest hash;
    message::transaction tx;
};

typedef boost::circular_buffer<transaction_entry_info> pool_buffer;

class transaction_pool
  : public std::enable_shared_from_this<transaction_pool>,
    public threaded_service
{
public:
    typedef std::function<void (const std::error_code&)> store_handler;

    static transaction_pool_ptr create(blockchain_ptr chain);

    // Non-copyable
    transaction_pool(const transaction_pool&) = delete;
    void operator=(const transaction_pool&) = delete;

    void store(const message::transaction& stored_transaction,
        store_handler handle_store);

private:
    transaction_pool();
    void initialize(blockchain_ptr chain);

    void do_store(const message::transaction& stored_transaction,
        store_handler handle_store);
    void handle_delegate(const std::error_code& ec, 
        const message::transaction& tx, store_handler handle_store);

    blockchain_ptr chain_;
    pool_buffer pool_;
};

} // libbitcoin

#endif


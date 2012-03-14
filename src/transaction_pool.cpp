#include <bitcoin/transaction_pool.hpp>

#include <bitcoin/error.hpp>
#include <bitcoin/exporter.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/validate.hpp>
#include <bitcoin/utility/assert.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

transaction_pool_ptr transaction_pool::create(
    async_service& service, blockchain_ptr chain)
{
    transaction_pool_ptr tx_pool(new transaction_pool(service));
    tx_pool->initialize(chain);
    return tx_pool;
}

transaction_pool::transaction_pool(async_service& service)
  : strand_(service.get_service()), pool_(100)
{
}
void transaction_pool::initialize(blockchain_ptr chain)
{
    chain_ = chain;
    chain_->subscribe_reorganize(
        strand_.wrap(std::bind(&transaction_pool::reorganize,
            shared_from_this(), _1, _2, _3, _4)));
}

void transaction_pool::store(const message::transaction& stored_transaction,
    store_handler handle_store)
{
    strand_.post(
        std::bind(&transaction_pool::do_store, shared_from_this(),
            stored_transaction, handle_store));
}
void transaction_pool::do_store(
    const message::transaction& stored_transaction,
    store_handler handle_store)
{
    exporter_ptr saver = std::make_shared<satoshi_exporter>();
    validate_transaction_ptr validate =
        std::make_shared<validate_transaction>(chain_, saver,
            stored_transaction, pool_, strand_);
    validate->start(strand_.wrap(std::bind(
        &transaction_pool::handle_delegate,
            shared_from_this(), _1, stored_transaction, handle_store)));
}

void transaction_pool::handle_delegate(const std::error_code& ec,
    const message::transaction& tx, store_handler handle_store)
{
    if (!ec)
        pool_.push_back({hash_transaction(tx), tx});
    handle_store(ec);
}

void transaction_pool::exists(const hash_digest& transaction_hash,
    exists_handler handle_exists)
{
    strand_.post(
        std::bind(&transaction_pool::do_exists, shared_from_this(),
            transaction_hash, handle_exists));
}
void transaction_pool::do_exists(const hash_digest& transaction_hash,
    exists_handler handle_exists)
{
    for (const transaction_entry_info& entry: pool_)
        if (entry.hash == transaction_hash)
        {
            handle_exists(true);
            return;
        }
    handle_exists(false);
}

void transaction_pool::reorganize(const std::error_code& ec,
    size_t fork_point,
    const blockchain::block_list& new_blocks,
    const blockchain::block_list& replaced_blocks)
{
    // new blocks come in - remove txs in new
    // old blocks taken out - resubmit txs in old
    chain_->subscribe_reorganize(
        strand_.wrap(std::bind(&transaction_pool::reorganize,
            shared_from_this(), _1, _2, _3, _4)));
}

} // libbitcoin


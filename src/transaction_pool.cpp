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
  : strand_(service.get_service()), pool_(2000)
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
    confirm_handler handle_confirm, store_handler handle_store)
{
    strand_.post(
        std::bind(&transaction_pool::do_store, shared_from_this(),
            stored_transaction, handle_confirm, handle_store));
}
void transaction_pool::do_store(
    const message::transaction& stored_transaction,
    confirm_handler handle_confirm, store_handler handle_store)
{
    transaction_entry_info new_tx_entry{
        hash_transaction(stored_transaction),
        stored_transaction,
        handle_confirm};

    // Does it already exist in pool?
    for (const transaction_entry_info& entry: pool_)
        if (entry.hash == new_tx_entry.hash)
        {
            handle_store(error::object_already_exists);
            return;
        }

    exporter_ptr saver = std::make_shared<satoshi_exporter>();
    validate_transaction_ptr validate =
        std::make_shared<validate_transaction>(chain_, saver,
            stored_transaction, pool_, strand_);
    validate->start(strand_.wrap(std::bind(
        &transaction_pool::handle_delegate,
            shared_from_this(), _1, new_tx_entry, handle_store)));
}

void transaction_pool::handle_delegate(const std::error_code& ec,
    const transaction_entry_info& tx_entry, store_handler handle_store)
{
    if (!ec)
        pool_.push_back(tx_entry);
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
    if (!replaced_blocks.empty())
        resubmit_all();
    else
        takeout_confirmed(new_blocks);
    // new blocks come in - remove txs in new
    // old blocks taken out - resubmit txs in old
    chain_->subscribe_reorganize(
        strand_.wrap(std::bind(&transaction_pool::reorganize,
            shared_from_this(), _1, _2, _3, _4)));
}

void handle_resubmit(const std::error_code& ec,
    transaction_pool::confirm_handler handle_confirm)
{
    if (ec)
        handle_confirm(ec);
}
void transaction_pool::resubmit_all()
{
    for (const transaction_entry_info& entry: pool_)
        store(entry.tx, entry.handle_confirm,
            std::bind(handle_resubmit, _1, entry.handle_confirm));
    pool_.clear();
}

void transaction_pool::takeout_confirmed(
    const blockchain::block_list& new_blocks)
{
    for (auto new_block: new_blocks)
        for (const message::transaction& new_tx: new_block->transactions)
            try_delete(hash_transaction(new_tx));
}
void transaction_pool::try_delete(const hash_digest& tx_hash)
{
    for (auto it = pool_.begin(); it != pool_.end(); ++it)
        if (it->hash == tx_hash)
        {
            auto handle_confirm = it->handle_confirm;
            pool_.erase(it);
            handle_confirm(std::error_code());
            return;
        }
}

} // namespace libbitcoin


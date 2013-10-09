#include <bitcoin/transaction_pool.hpp>

#include <bitcoin/error.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/validate.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/format.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

transaction_pool::transaction_pool(
    threadpool& pool, blockchain& chain)
  : strand_(pool), chain_(chain), pool_(2000)
{
    log_debug() << "transaction_pool()";
}

void transaction_pool::set_capacity(size_t capacity)
{
    log_debug() << "transaction_pool::set_capacity(" << capacity << ")";
    pool_.set_capacity(capacity);
}

void transaction_pool::start()
{
    log_debug() << "start()";
    chain_.subscribe_reorganize(
        strand_.wrap(&transaction_pool::reorganize,
            this, _1, _2, _3, _4));
}

void transaction_pool::validate(const transaction_type& tx,
    validate_handler handle_validate)
{
    log_debug() << "transaction_pool::validate("
        << hash_transaction(tx) << ")";
    strand_.queue(&transaction_pool::do_validate,
        this, tx, handle_validate);
}
void transaction_pool::do_validate(const transaction_type& tx,
    validate_handler handle_validate)
{
    log_debug() << "transaction_pool::do_validate("
        << hash_transaction(tx) << ")";
    validate_transaction_ptr validate =
        std::make_shared<validate_transaction>(
            chain_, tx, pool_, strand_);
    validate->start(strand_.wrap(
        &transaction_pool::validation_complete,
            this, _1, _2, hash_transaction(tx), handle_validate));
}

void transaction_pool::validation_complete(
    const std::error_code& ec, const index_list& unconfirmed,
    const hash_digest& tx_hash, validate_handler handle_validate)
{
    log_debug() << "transaction_pool::validation_complete("
        << ec.message() << ", unconfirmed(" << unconfirmed.size() << "), "
        << tx_hash << ")";
    if (ec == error::input_not_found || ec == error::validate_inputs_failed)
    {
        log_debug() << "  missing input";
        BITCOIN_ASSERT(unconfirmed.size() == 1);
        //BITCOIN_ASSERT(unconfirmed[0] < tx.inputs.size());
        handle_validate(ec, unconfirmed);
    }
    else if (ec)
    {
        log_debug() << "  other error";
        BITCOIN_ASSERT(unconfirmed.empty());
        handle_validate(ec, index_list());
    }
    // Re-check as another transaction might've been added in the interim
    else if (tx_exists(tx_hash))
    {
        log_debug() << "  duplicate tx";
        handle_validate(error::duplicate, index_list());
    }
    else
    {
        log_debug() << "  tx OK passed";
        handle_validate(std::error_code(), unconfirmed);
    }
}

bool transaction_pool::tx_exists(const hash_digest& tx_hash)
{
    for (const transaction_entry_info& entry: pool_)
        if (entry.hash == tx_hash)
            return true;
    return false;
}

void transaction_pool::store(const transaction_type& tx,
    confirm_handler handle_confirm, validate_handler handle_validate)
{
    auto perform_store = [this, tx, handle_confirm]
    {
        // When new tx are added to the circular buffer,
        // any tx at the front will be droppped.
        // We notify the API user of this through the handler.
        if (pool_.size() == pool_.capacity())
        {
            auto handle_confirm = pool_.front().handle_confirm;
            handle_confirm(error::pool_filled);
        }
        // We store a precomputed tx hash to make lookups faster.
        pool_.push_back(
            {hash_transaction(tx), tx, handle_confirm});
    };
    auto wrap_handle_validate = [perform_store, handle_validate](
        const std::error_code& ec, const index_list& unconfirmed)
    {
        log_debug() << "wrap_handle_validate ec = " << ec.message();
        if (!ec)
            perform_store();
        handle_validate(ec, unconfirmed);
    };
    log_debug() << "transaction_pool::store(" << hash_transaction(tx) << ")";
    validate(tx, wrap_handle_validate);
}

void transaction_pool::fetch(const hash_digest& transaction_hash,
    fetch_handler handle_fetch)
{
    log_debug() << "transaction_pool::fetch(" << transaction_hash << ")";
    strand_.queue(
        [this, transaction_hash, handle_fetch]()
        {
            log_debug() << "transaction_pool::do_fetch("
                << transaction_hash << ")";
            for (const transaction_entry_info& entry: pool_)
                if (entry.hash == transaction_hash)
                {
                    log_debug() << "  found!";
                    handle_fetch(std::error_code(), entry.tx);
                    return;
                }
            log_debug() << "  not found...";
            handle_fetch(error::not_found, transaction_type());
        });
}

void transaction_pool::exists(const hash_digest& transaction_hash,
    exists_handler handle_exists)
{
    log_debug() << "transaction_pool::exists("
        << transaction_hash << ")";
    strand_.queue(
        [this, transaction_hash, handle_exists]()
        {
            log_debug() << "transaction_pool::do_exists("
                << transaction_hash << ")";
            handle_exists(tx_exists(transaction_hash));
        });
}

void transaction_pool::reorganize(const std::error_code& ec,
    size_t fork_point,
    const blockchain::block_list& new_blocks,
    const blockchain::block_list& replaced_blocks)
{
    log_debug() << "transaction_pool::reorganize(" << ec.message()
        << ", " << fork_point << ", new_blocks(" << new_blocks.size()
        << "), replaced_blocks(" << replaced_blocks.size() << "))";
    if (!new_blocks.empty())
        log_debug() << "  new_blocks from "
            << hash_block_header(new_blocks.front()->header) << " to "
            << hash_block_header(new_blocks.back()->header);
    if (!replaced_blocks.empty())
        log_debug() << "  replaced_blocks from "
            << hash_block_header(replaced_blocks.front()->header) << " to "
            << hash_block_header(replaced_blocks.back()->header);
    if (!replaced_blocks.empty())
        invalidate_pool();
    else
        takeout_confirmed(new_blocks);
    // new blocks come in - remove txs in new
    // old blocks taken out - resubmit txs in old
    chain_.subscribe_reorganize(
        strand_.wrap(&transaction_pool::reorganize,
            this, _1, _2, _3, _4));
}

void transaction_pool::invalidate_pool()
{
    log_debug() << "transaction_pool::invalidate_pool()";
    // See http://www.jwz.org/doc/worse-is-better.html
    // for why we take this approach.
    // We return with an error_code and don't handle this case.
    for (const transaction_entry_info& entry: pool_)
    {
        log_debug() << "  clearing " << hash_transaction(entry.tx);
        entry.handle_confirm(error::blockchain_reorganized);
    }
    pool_.clear();
}

void transaction_pool::takeout_confirmed(
    const blockchain::block_list& new_blocks)
{
    log_debug() << "transaction_pool::takeout_confirmed()";
    for (auto new_block: new_blocks)
        for (const transaction_type& new_tx: new_block->transactions)
            try_delete(hash_transaction(new_tx));
}
void transaction_pool::try_delete(const hash_digest& tx_hash)
{
    log_debug () << "transaction_pool::try_delete(" << tx_hash << ")";
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


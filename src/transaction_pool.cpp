/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/transaction.hpp>
#include <bitcoin/bitcoin/transaction_pool.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/validate.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

transaction_pool::transaction_pool(
    threadpool& pool, blockchain& chain)
  : strand_(pool), chain_(chain), pool_(2000)
{
}

void transaction_pool::set_capacity(size_t capacity)
{
    pool_.set_capacity(capacity);
}

void transaction_pool::start()
{
    chain_.subscribe_reorganize(
        std::bind(&transaction_pool::reorganize, this, _1, _2, _3, _4));
}

void transaction_pool::validate(const transaction_type& tx,
    validate_handler handle_validate)
{
    strand_.queue(&transaction_pool::do_validate,
        this, tx, handle_validate);
}
void transaction_pool::do_validate(const transaction_type& tx,
    validate_handler handle_validate)
{
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
    if (ec == error::input_not_found || ec == error::validate_inputs_failed)
    {
        BITCOIN_ASSERT(unconfirmed.size() == 1);
        //BITCOIN_ASSERT(unconfirmed[0] < tx.inputs.size());
        handle_validate(ec, unconfirmed);
    }
    else if (ec)
    {
        BITCOIN_ASSERT(unconfirmed.empty());
        handle_validate(ec, index_list());
    }
    // Re-check as another transaction might've been added in the interim
    else if (tx_exists(tx_hash))
    {
        handle_validate(error::duplicate, index_list());
    }
    else
    {
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
        if (!ec)
            perform_store();
        handle_validate(ec, unconfirmed);
    };
    validate(tx, wrap_handle_validate);
}

void transaction_pool::fetch(const hash_digest& transaction_hash,
    fetch_handler handle_fetch)
{
    strand_.queue(
        [this, transaction_hash, handle_fetch]()
        {
            for (const transaction_entry_info& entry: pool_)
                if (entry.hash == transaction_hash)
                {
                    handle_fetch(std::error_code(), entry.tx);
                    return;
                }
            handle_fetch(error::not_found, transaction_type());
        });
}

void transaction_pool::exists(const hash_digest& transaction_hash,
    exists_handler handle_exists)
{
    strand_.queue(
        [this, transaction_hash, handle_exists]()
        {
            handle_exists(tx_exists(transaction_hash));
        });
}

void transaction_pool::reorganize(const std::error_code& ec,
    size_t /* fork_point */,
    const blockchain::block_list& new_blocks,
    const blockchain::block_list& replaced_blocks)
{
    if (ec)
    {
        BITCOIN_ASSERT(ec == error::service_stopped);
        return;
    }
    if (!replaced_blocks.empty())
        strand_.queue(&transaction_pool::invalidate_pool, this);
    else
        strand_.queue(&transaction_pool::takeout_confirmed, this, new_blocks);
    // new blocks come in - remove txs in new
    // old blocks taken out - resubmit txs in old
    chain_.subscribe_reorganize(
        std::bind(&transaction_pool::reorganize, this, _1, _2, _3, _4));
}

void transaction_pool::invalidate_pool()
{
    // See http://www.jwz.org/doc/worse-is-better.html
    // for why we take this approach.
    // We return with an error_code and don't handle this case.
    for (const transaction_entry_info& entry: pool_)
        entry.handle_confirm(error::blockchain_reorganized);
    pool_.clear();
}

void transaction_pool::takeout_confirmed(
    const blockchain::block_list& new_blocks)
{
    for (auto new_block: new_blocks)
        for (const transaction_type& new_tx: new_block->transactions)
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


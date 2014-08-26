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
#include <bitcoin/blockchain/blockchain.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/async_parallel.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

// fetch_block
typedef blockchain_fetch_handler_block handler_block;

class fetch_block_t
  : public std::enable_shared_from_this<fetch_block_t>
{
public:
    fetch_block_t(blockchain& chain)
      : chain_(chain), stopped_(false) {}

    template <typename BlockIndex>
    void start(const BlockIndex& index, handler_block handle)
    {
        handle_ = handle;
        auto this_ptr = shared_from_this();
        chain_.fetch_block_header(index,
            [this, this_ptr](const std::error_code& ec,
                const block_header_type& block_header)
            {
                if (stop_on_error(ec))
                    return;
                block_.header = block_header;
                fetch_hashes();
            });
    }

private:
    bool stop_on_error(const std::error_code& ec)
    {
        if (stopped_)
            return true;
        else if (ec)
        {
            stopped_ = true;
            handle_(ec, block_type());
            return true;
        }
        return false;
    }

    void fetch_hashes()
    {
        //chain_.fetch_block_transaction_hashes(
        //    hash_block_header(block_.header),
        //    std::bind(&fetch_block_t::fetch_transactions,
        //        shared_from_this(), _1, _2));
    }

    void fetch_transactions(const std::error_code& ec,
        const hash_digest_list& tx_hashes)
    {
        if (stop_on_error(ec))
            return;
        block_.transactions.resize(tx_hashes.size());
        count_ = 0;
        for (size_t tx_index = 0;
            tx_index < tx_hashes.size(); ++tx_index)
        {
            fetch_tx(tx_hashes[tx_index], tx_index);
        }
    }

    void fetch_tx(const hash_digest& tx_hash, size_t tx_index)
    {
        auto this_ptr = shared_from_this();
        chain_.fetch_transaction(tx_hash,
            [this, this_ptr, tx_index](
                const std::error_code& ec,
                const transaction_type& tx)
            {
                if (stop_on_error(ec))
                    return;
                BITCOIN_ASSERT(tx_index < block_.transactions.size());
                block_.transactions[tx_index] = tx;
                if (++count_ == block_.transactions.size())
                {
                    handle_(std::error_code(), block_);
                }
            });
    }

    blockchain& chain_;
    handler_block handle_;

    block_type block_;
    atomic_counter count_;
    bool stopped_;
};

void fetch_block(blockchain& chain, size_t height,
    handler_block handle_fetch)
{
    auto fetcher = std::make_shared<fetch_block_t>(chain);
    fetcher->start(height, handle_fetch);
}
void fetch_block(blockchain& chain, const hash_digest& block_hash,
    handler_block handle_fetch)
{
    auto fetcher = std::make_shared<fetch_block_t>(chain);
    fetcher->start(block_hash, handle_fetch);
}

// fetch_block_locator
typedef blockchain_fetch_handler_block_locator handler_locator;

class fetch_locator
  : public std::enable_shared_from_this<fetch_locator>
{
public:
    fetch_locator(blockchain& chain)
      : chain_(chain) {}

    void start(handler_locator handle)
    {
        handle_ = handle;
        auto this_ptr = shared_from_this();
        chain_.fetch_last_height(
            std::bind(&fetch_locator::populate,
                this_ptr, _1, _2));
    }

private:
    bool stop_on_error(const std::error_code& ec)
    {
        if (ec)
        {
            handle_(ec, block_locator_type());
            return true;
        }
        return false;
    }

    void populate(const std::error_code& ec, size_t last_height)
    {
        if (stop_on_error(ec))
            return;
        indexes_ = block_locator_indexes(last_height);
        // We reverse our list so we can pop items off the top
        // as we need to get them, and push items to our locator.
        // The order of items in the locator should match
        // the order of items in our index.
        std::reverse(indexes_.begin(), indexes_.end());
        loop();
    }

    void loop()
    {
        // Stop looping and finish.
        if (indexes_.empty())
        {
            handle_(std::error_code(), locator_);
            return;
        }
        auto this_ptr = shared_from_this();
        size_t height = indexes_.back();
        indexes_.pop_back();
        chain_.fetch_block_header(height,
            std::bind(&fetch_locator::append, this_ptr, _1, _2, height));
    }

    void append(const std::error_code& ec,
        const block_header_type& blk_header, size_t /* height */)
    {
        if (stop_on_error(ec))
            return;
        hash_digest block_hash = hash_block_header(blk_header);
        locator_.push_back(block_hash);
        // Continue the loop.
        loop();
    }

    blockchain& chain_;
    handler_locator handle_;
    index_list indexes_;
    block_locator_type locator_;
};

void fetch_block_locator(blockchain& chain, handler_locator handle_fetch)
{
    auto fetcher = std::make_shared<fetch_locator>(chain);
    fetcher->start(handle_fetch);
}

} // namespace libbitcoin


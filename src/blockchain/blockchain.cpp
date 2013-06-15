#include <bitcoin/blockchain/blockchain.hpp>

#include <bitcoin/constants.hpp>
#include <bitcoin/utility/assert.hpp>

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
                const block_type& block_header)
            {
                if (stop_on_error(ec))
                    return;
                block_ = block_header;
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
        chain_.fetch_block_transaction_hashes(
            hash_block_header(block_),
            std::bind(&fetch_block_t::fetch_transactions,
                shared_from_this(), _1, _2));
    }

    void fetch_transactions(const std::error_code& ec,
        const inventory_list& tx_hashes)
    {
        if (stop_on_error(ec))
            return;
        block_.transactions.resize(tx_hashes.size());
        count_ = 0;
        for (size_t tx_index = 0;
            tx_index < tx_hashes.size(); ++tx_index)
        {
            fetch_tx(tx_hashes, tx_index);
        }
    }

    void fetch_tx(const inventory_list& tx_hashes, size_t tx_index)
    {
        auto this_ptr = shared_from_this();
        const inventory_vector_type& inv = tx_hashes[tx_index];
        BITCOIN_ASSERT(inv.type ==
            inventory_type_id::transaction);
        BITCOIN_ASSERT(block_.transactions.size() == tx_hashes.size());
        chain_.fetch_transaction(inv.hash,
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

void fetch_block(blockchain& chain, size_t depth,
    handler_block handle_fetch)
{
    auto fetcher = std::make_shared<fetch_block_t>(chain);
    fetcher->start(depth, handle_fetch);
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
        chain_.fetch_last_depth(
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

    void populate(const std::error_code& ec, size_t last_depth)
    {
        if (stop_on_error(ec))
            return;
        indexes_ = block_locator_indexes(last_depth);
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
        size_t depth = indexes_.back();
        indexes_.pop_back();
        chain_.fetch_block_header(depth,
            std::bind(&fetch_locator::append, this_ptr, _1, _2, depth));
    }

    void append(const std::error_code& ec, const block_type& block_header,
        size_t depth)
    {
        if (stop_on_error(ec))
            return;
        hash_digest block_hash = hash_block_header(block_header);
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

// fetch_history
typedef blockchain_fetch_handler_history handler_history;

class fetch_history_t
  : public std::enable_shared_from_this<fetch_history_t>
{
public:
    fetch_history_t(blockchain& chain)
      : chain_(chain), stopped_(false) {}

    void start(const payment_address& address, handler_history handle)
    {
        handle_ = handle;
        auto this_ptr = shared_from_this();
        chain_.fetch_outputs(address,
            [this, this_ptr](const std::error_code& ec,
                const output_point_list& outpoints)
            {
                if (stop_on_error(ec))
                    return;
                outpoints_ = outpoints;
                fetch_spends();
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
            handle_(ec, output_point_list(), input_point_list());
            return true;
        }
        return false;
    }

    void fetch_spends()
    {
        inpoints_.resize(outpoints_.size());
        count_ = 0;
        for (size_t i = 0; i < inpoints_.size(); ++i)
            chain_.fetch_spend(outpoints_[i],
                std::bind(&fetch_history_t::spend_fetched,
                    shared_from_this(), _1, _2, i));
    }

    void spend_fetched(const std::error_code& ec,
        const input_point& inpoint, size_t i)
    {
        if (ec == error::unspent_output)
        {
            inpoints_[i].hash = null_hash;
            inpoints_[i].index = std::numeric_limits<uint32_t>::max();
        }
        else if (stop_on_error(ec))
            return;
        else
            inpoints_[i] = inpoint;
        if (++count_ == inpoints_.size())
        {
            BITCOIN_ASSERT(inpoints_.size() == outpoints_.size());
            handle_(std::error_code(), outpoints_, inpoints_);
        }
    }

    blockchain& chain_;
    handler_history handle_;

    output_point_list outpoints_;
    input_point_list inpoints_;
    bool stopped_;
    atomic_counter count_;
};

void fetch_history(blockchain& chain, const payment_address& address,
    handler_history handle_fetch)
{
    auto fetcher = std::make_shared<fetch_history_t>(chain);
    fetcher->start(address, handle_fetch);
}

// fetch_output_values
typedef blockchain_fetch_handler_output_values handler_output_values;

class fetch_output_values_t
  : public std::enable_shared_from_this<fetch_output_values_t>
{
public:
    fetch_output_values_t(blockchain& chain)
      : chain_(chain), stopped_(false) {}

    void start(const output_point_list& outpoints, handler_output_values handle)
    {
        // Just finish. No outpoints to fetch.
        if (outpoints.empty())
        {
            handle(std::error_code(), output_value_list());
            return;
        }
        handle_ = handle;
        values_.resize(outpoints.size());
        count_ = 0;
        for (size_t i = 0; i < outpoints.size(); ++i)
        {
            const output_point& outpoint = outpoints[i];
            chain_.fetch_transaction(outpoint.hash,
                std::bind(&fetch_output_values_t::tx_fetched,
                    shared_from_this(), _1, _2, outpoint.index, i));
        }
    }

private:
    bool stop_on_error(const std::error_code& ec)
    {
        if (stopped_)
            return true;
        else if (ec)
        {
            stopped_ = true;
            handle_(ec, output_value_list());
            return true;
        }
        return false;
    }

    void tx_fetched(const std::error_code& ec, const transaction_type& tx,
        size_t output_index, size_t index)
    {
        if (stop_on_error(ec))
            return;
        BITCOIN_ASSERT(output_index < tx.outputs.size());
        values_[index] = tx.outputs[output_index].value;
        if (++count_ == values_.size())
            handle_(std::error_code(), values_);
    }

    blockchain& chain_;
    handler_output_values handle_;

    output_value_list values_;
    atomic_counter count_;
    bool stopped_;
};

void fetch_output_values(blockchain& chain, const output_point_list& outpoints,
    handler_output_values handle_fetch)
{
    auto fetcher = std::make_shared<fetch_output_values_t>(chain);
    fetcher->start(outpoints, handle_fetch);
}

} // namespace libbitcoin


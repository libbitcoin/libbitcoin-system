#include <bitcoin/blockchain/blockchain.hpp>

#include <bitcoin/utility/assert.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

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
                const message::block& block_header)
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
            handle_(ec, message::block());
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
        const message::inventory_list& tx_hashes)
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

    void fetch_tx(const message::inventory_list& tx_hashes, size_t tx_index)
    {
        auto this_ptr = shared_from_this();
        const message::inventory_vector& inv = tx_hashes[tx_index];
        BITCOIN_ASSERT(inv.type ==
            message::inventory_type::transaction);
        size_t tx_hashes_size = tx_hashes.size();
        chain_.fetch_transaction(inv.hash,
            [this, this_ptr, tx_index, tx_hashes_size](
                const std::error_code& ec,
                const message::transaction& tx)
            {
                if (stop_on_error(ec))
                    return;
                BITCOIN_ASSERT(tx_index < block_.transactions.size());
                block_.transactions[tx_index] = tx;
                ++count_;
                BITCOIN_ASSERT(block_.transactions.size() == tx_hashes_size);
                if (count_ == tx_hashes_size)
                    handle_(std::error_code(), block_);
            });
    }

    blockchain& chain_;
    handler_block handle_;

    message::block block_;
    size_t count_;
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
    // An intermediate type used to keep metadata about the locator
    // so we can sort the locator before returning.
    typedef std::pair<size_t, hash_digest> meta_entry;
    typedef std::vector<meta_entry> meta_locator;

    bool stop_on_error(const std::error_code& ec)
    {
        if (stopped_)
            return true;
        else if (ec)
        {
            stopped_ = true;
            handle_(ec, message::block_locator());
            return true;
        }
        return false;
    }

    void populate(const std::error_code& ec, size_t last_depth)
    {
        if (stop_on_error(ec))
            return;
        index_list indexes = block_locator_indexes(last_depth);
        auto this_ptr = shared_from_this();
        for (size_t depth: indexes)
            chain_.fetch_block_header(depth,
                std::bind(&fetch_locator::append,
                    this_ptr, _1, _2, depth, indexes.size()));
    }

    void append(const std::error_code& ec,
        const message::block& block_header, size_t depth, size_t entries)
    {
        if (stop_on_error(ec))
            return;
        meta_.push_back(std::make_pair(depth, hash_block_header(block_header)));
        if (meta_.size() == entries)
            final();
    }

    void final()
    {
        std::sort(meta_.begin(), meta_.end(),
            [](const meta_entry& entry_a, const meta_entry& entry_b)
            {
                return entry_a.first > entry_b.first;
            });
        message::block_locator final_locator;
        for (const meta_entry& entry: meta_)
            final_locator.push_back(entry.second);
        handle_(std::error_code(), final_locator);
    }

    blockchain& chain_;
    handler_locator handle_;
    bool stopped_;
    meta_locator meta_;
};

void fetch_block_locator(blockchain& chain, handler_locator handle_fetch)
{
    auto fetcher = std::make_shared<fetch_locator>(chain);
    fetcher->start(handle_fetch);
}

} // namespace libbitcoin


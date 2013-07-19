#include "leveldb_validate_block.hpp"

#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/format.hpp>
#include <bitcoin/transaction.hpp>

namespace libbitcoin {

leveldb_validate_block::leveldb_validate_block(leveldb_common_ptr common,
    int fork_index, const block_detail_list& orphan_chain,
    int orphan_index, size_t height, const block_type& current_block)
  : validate_block(height, current_block), common_(common),
    height_(height), fork_index_(fork_index),
    orphan_index_(orphan_index), orphan_chain_(orphan_chain)
{
}

block_header_type leveldb_validate_block::fetch_block(size_t fetch_height)
{
    if (fetch_height > fork_index_)
    {
        size_t fetch_index = fetch_height - fork_index_ - 1;
        BITCOIN_ASSERT(fetch_index <= orphan_index_);
        BITCOIN_ASSERT(orphan_index_ < orphan_chain_.size());
        return orphan_chain_[fetch_index]->actual().header;
    }
    leveldb_block_info blk;
    // We only really need the bits and timestamp fields.
    bool get_status = common_->get_block(blk, fetch_height, true, false);
    BITCOIN_ASSERT(get_status);
    return blk.header;
}

uint32_t leveldb_validate_block::previous_block_bits()
{
    // Read block d - 1 and return bits
    return fetch_block(height_ - 1).bits;
}

uint64_t leveldb_validate_block::actual_timespan(const uint64_t interval)
{
    // height - interval and height - 1, return time difference
    return fetch_block(height_ - 1).timestamp - 
        fetch_block(height_ - interval).timestamp;
}

uint64_t leveldb_validate_block::median_time_past()
{
    // read last 11 block times into array and select median value
    std::vector<uint64_t> times;
    for (int i = height_ - 1; i >= 0 && i >= (int)height_ - 11; --i)
        times.push_back(fetch_block(i).timestamp);
    BITCOIN_ASSERT(
        (height_ < 11 && times.size() == height_) || times.size() == 11);
    std::sort(times.begin(), times.end());
    return times[times.size() / 2];
}

bool tx_after_fork(const leveldb_tx_info& tx, size_t fork_index)
{
    if (tx.height <= fork_index)
        return false;
    return true;
}

bool leveldb_validate_block::transaction_exists(const hash_digest& tx_hash)
{
    leveldb_tx_info tx;
    if (!common_->get_transaction(tx, tx_hash, true, false))
        return false;
    return !tx_after_fork(tx, fork_index_);
}

bool leveldb_validate_block::is_output_spent(
    const output_point& outpoint)
{
    input_point input_spend;
    if (!common_->fetch_spend(outpoint, input_spend))
        return false;
    // Lookup block height. Is the spend after the fork point?
    return transaction_exists(input_spend.hash);
}

bool leveldb_validate_block::fetch_transaction(transaction_type& tx,
    size_t& tx_height, const hash_digest& tx_hash)
{
    leveldb_tx_info tx_info;
    bool tx_exists = common_->get_transaction(tx_info, tx_hash, true, true);
    if (!tx_exists || tx_after_fork(tx_info, fork_index_))
    {
        if (!fetch_orphan_transaction(tx, tx_height, tx_hash))
            return false;
        return true;
    }
    tx = tx_info.tx;
    tx_height = tx_info.height;
    return true;
}

bool leveldb_validate_block::fetch_orphan_transaction(
    transaction_type& tx, size_t& tx_height, const hash_digest& tx_hash)
{
    for (size_t orphan_iter = 0; orphan_iter <= orphan_index_; ++orphan_iter)
    {
        const block_type& orphan_block =
            orphan_chain_[orphan_iter]->actual();
        for (const transaction_type& orphan_tx: orphan_block.transactions)
        {
            if (hash_transaction(orphan_tx) == tx_hash)
            {
                tx = orphan_tx;
                tx_height = fork_index_ + orphan_iter + 1;
                return true;
            }
        }
    }
    return false;
}

bool leveldb_validate_block::is_output_spent(
    const output_point& previous_output,
    size_t index_in_parent, size_t input_index)
{
    // Search for double spends
    //   This must be done in both chain AND orphan
    // Searching chain when this tx is an orphan is redundant but
    // it does not happen enough to care
    if (is_output_spent(previous_output))
        return true;
    else if (orphan_is_spent(previous_output, index_in_parent, input_index))
        return true;
    return false;
}

bool leveldb_validate_block::orphan_is_spent(
    const output_point& previous_output,
    size_t skip_tx, size_t skip_input)
{
    // TODO factor this to look nicer
    for (size_t orphan_iter = 0; orphan_iter <= orphan_index_; ++orphan_iter)
    {
        const block_type& orphan_block =
            orphan_chain_[orphan_iter]->actual();
        // Skip coinbase
        BITCOIN_ASSERT(orphan_block.transactions.size() >= 1);
        BITCOIN_ASSERT(is_coinbase(orphan_block.transactions[0]));
        for (size_t tx_index = 0; tx_index < orphan_block.transactions.size();
            ++tx_index)
        {
            const transaction_type& orphan_tx =
                orphan_block.transactions[tx_index];
            for (size_t input_index = 0; input_index < orphan_tx.inputs.size();
                ++input_index)
            {
                const transaction_input_type& orphan_input =
                    orphan_tx.inputs[input_index];
                if (orphan_iter == orphan_index_ && tx_index == skip_tx &&
                    input_index == skip_input)
                {
                    continue;
                }
                else if (orphan_input.previous_output == previous_output)
                    return true;
            }
        }
    }
    return false;
}

} // namespace libbitcoin


#include "bdb_validate_block.hpp"

#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/format.hpp>
#include <bitcoin/transaction.hpp>

namespace libbitcoin {

bdb_validate_block::bdb_validate_block(bdb_common_ptr common, int fork_index,
    const block_detail_list& orphan_chain, int orphan_index, size_t depth,
    txn_guard_ptr txn, const message::block& current_block)
  : validate_block(depth, current_block), common_(common),
    txn_(txn), depth_(depth), fork_index_(fork_index),
    orphan_index_(orphan_index), orphan_chain_(orphan_chain)
{
}

message::block bdb_validate_block::fetch_block(size_t fetch_depth)
{
    if (fetch_depth > fork_index_)
    {
        size_t fetch_index = fetch_depth - fork_index_ - 1;
        BITCOIN_ASSERT(fetch_index <= orphan_index_);
        BITCOIN_ASSERT(orphan_index_ < orphan_chain_.size());
        return orphan_chain_[fetch_index]->actual();
    }
    proto::Block proto_block =
        common_->fetch_proto_block(txn_, fetch_depth);
    BITCOIN_ASSERT(proto_block.IsInitialized());
    // We only convert the fields we actually need
    message::block result_block;
    result_block.bits = proto_block.bits();
    result_block.timestamp = proto_block.timestamp();
    return result_block;
}

uint32_t bdb_validate_block::previous_block_bits()
{
    // Read block d - 1 and return bits
    return fetch_block(depth_ - 1).bits;
}

uint64_t bdb_validate_block::actual_timespan(const uint64_t interval)
{
    // depth - interval and depth - 1, return time difference
    return fetch_block(depth_ - 1).timestamp - 
        fetch_block(depth_ - interval).timestamp;
}

uint64_t bdb_validate_block::median_time_past()
{
    // read last 11 block times into array and select median value
    std::vector<uint64_t> times;
    for (int i = depth_ - 1; i >= 0 && i >= (int)depth_ - 11; --i)
        times.push_back(fetch_block(i).timestamp);
    BITCOIN_ASSERT(
        (depth_ < 11 && times.size() == depth_) || times.size() == 11);
    std::sort(times.begin(), times.end());
    return times[times.size() / 2];
}

bool tx_after_fork(const proto::Transaction& proto_tx, size_t fork_index)
{
    for (auto parent: proto_tx.parent())
        if (parent.depth() > fork_index)
            return true;
    return false;
}

bool bdb_validate_block::transaction_exists(const hash_digest& tx_hash)
{
    proto::Transaction proto_tx =
        common_->fetch_proto_transaction(txn_, tx_hash);
    if (!proto_tx.IsInitialized())
        return false;
    return !tx_after_fork(proto_tx, fork_index_);
}

bool bdb_validate_block::is_output_spent(const message::output_point& outpoint)
{
    message::input_point input_spend;
    if (!common_->fetch_spend(txn_, outpoint, input_spend))
        return false;
    // Lookup block depth
    proto::Transaction proto_tx =
        common_->fetch_proto_transaction(txn_, input_spend.hash);
    return !tx_after_fork(proto_tx, fork_index_);
}

bool bdb_validate_block::fetch_transaction(message::transaction& tx, 
    size_t& tx_depth, const hash_digest& tx_hash)
{
    proto::Transaction proto_tx =
        common_->fetch_proto_transaction(txn_, tx_hash);
    if (!proto_tx.IsInitialized() || tx_after_fork(proto_tx, fork_index_))
    {
        if (!fetch_orphan_transaction(tx, tx_depth, tx_hash))
            return false;
        return true;
    }
    BITCOIN_ASSERT(proto_tx.parent_size() > 0);
    tx = proto_to_transaction(proto_tx);
    tx_depth = proto_tx.parent(0).depth();
    return true;
}

bool bdb_validate_block::fetch_orphan_transaction(message::transaction& tx, 
    size_t& tx_depth, const hash_digest& tx_hash)
{
    for (size_t orphan_iter = 0; orphan_iter <= orphan_index_; ++orphan_iter)
    {
        const message::block& orphan_block =
            orphan_chain_[orphan_iter]->actual();
        for (const message::transaction& orphan_tx: orphan_block.transactions)
        {
            if (hash_transaction(orphan_tx) == tx_hash)
            {
                tx = orphan_tx;
                tx_depth = fork_index_ + orphan_iter + 1;
                return true;
            }
        }
    }
    return false;
}

bool bdb_validate_block::is_output_spent(
    const message::output_point& previous_output,
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

bool bdb_validate_block::orphan_is_spent(
    const message::output_point& previous_output,
    size_t skip_tx, size_t skip_input)
{
    // TODO factor this to look nicer
    for (size_t orphan_iter = 0; orphan_iter <= orphan_index_; ++orphan_iter)
    {
        const message::block& orphan_block =
            orphan_chain_[orphan_iter]->actual();
        // Skip coinbase
        BITCOIN_ASSERT(orphan_block.transactions.size() >= 1);
        BITCOIN_ASSERT(is_coinbase(orphan_block.transactions[0]));
        for (size_t tx_index = 0; tx_index < orphan_block.transactions.size();
            ++tx_index)
        {
            const message::transaction& orphan_tx =
                orphan_block.transactions[tx_index];
            for (size_t input_index = 0; input_index < orphan_tx.inputs.size();
                ++input_index)
            {
                const message::transaction_input& orphan_input =
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


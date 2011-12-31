#include "bdb_validate_block.hpp"

#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/data_helpers.hpp>
#include <bitcoin/transaction.hpp>

namespace libbitcoin {

bdb_validate_block::bdb_validate_block(bdb_common_ptr common, int fork_index,
    const blocks_list& orphan_chain, int orphan_index, size_t depth,
    txn_guard_ptr txn, exporter_ptr saver,
    const message::block& current_block)
  : validate_block(saver, depth, current_block), common_(common),
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
    protobuf::Block proto_block =
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

bool bdb_validate_block::validate_transaction(const message::transaction& tx, 
    size_t index_in_parent, uint64_t& value_in)
{
    BITCOIN_ASSERT(!is_coinbase(tx));
    for (size_t input_index = 0; input_index < tx.inputs.size(); ++input_index)
    {
        if (!connect_input(index_in_parent, tx, input_index, value_in))
            return false;
    }
    return true;
}

bool bdb_validate_block::connect_input(size_t index_in_parent,
    const message::transaction& current_tx,
    size_t input_index, uint64_t& value_in)
{
    // Lookup previous output
    BITCOIN_ASSERT(input_index < current_tx.inputs.size());
    const message::transaction_input& input = current_tx.inputs[input_index];
    const message::output_point& previous_output = input.previous_output;
    protobuf::Transaction previous_tx =
        common_->fetch_proto_transaction(txn_, previous_output.hash);
    if (!previous_tx.IsInitialized())
    {
        return connect_orphan_input(index_in_parent,
            current_tx, input_index, value_in);
    }
    BITCOIN_ASSERT(previous_tx.parent_size() > 0);
    const protobuf::Transaction_Output& previous_tx_out =
        previous_tx.outputs(previous_output.index);
    // Get output amount
    uint64_t output_value = previous_tx_out.value();
    if (output_value > max_money())
        return false;
    // Check coinbase maturity has been reached
    if (previous_tx.is_coinbase())
    {
        uint32_t depth_difference = depth_ - previous_tx.parent(0).depth();
        if (depth_difference < coinbase_maturity)
            return false;
    }
    // Validate script
    const std::string& raw_script = previous_tx_out.script();
    script output_script =
        parse_script(data_chunk(raw_script.begin(), raw_script.end()));
    if (!output_script.run(input.input_script, current_tx, input_index))
    {
        log_error(log_domain::validation) << "Script failed evaluation";
        return false;
    }
    // Search for double spends
    //   This must be done in both chain AND orphan
    if (common_->is_output_spent(txn_, previous_output))
        return false;
    else if (orphan_is_spent(previous_output, index_in_parent, input_index))
        return false;
    // Increase value_in by this output's value
    value_in += output_value;
    if (value_in > max_money())
        return false;
    return true;
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

bool bdb_validate_block::connect_orphan_input(size_t index_in_parent,
    const message::transaction& current_tx,
    size_t input_index, uint64_t& value_in)
{
    const message::transaction_input& input = current_tx.inputs[input_index];
    const message::output_point previous_output = input.previous_output;
    // Lookup previous_tx
    message::transaction previous_tx;
    size_t previous_depth;
    if (!lookup_transaction(previous_output, previous_tx, previous_depth))
        return false;
    BITCOIN_ASSERT(previous_output.index < previous_tx.outputs.size());
    const message::transaction_output& previous_tx_out =
        previous_tx.outputs[previous_output.index];
    uint64_t output_value = previous_tx_out.value;
    if (output_value > max_money())
        return false;
    // Check coinbase maturity has been reached
    if (is_coinbase(previous_tx))
    {
        size_t depth_difference = depth_ - previous_depth;
        if (depth_difference < coinbase_maturity)
            return false;
    }
    // Validate script
    script output_script = previous_tx_out.output_script;
    if (!output_script.run(input.input_script, current_tx, input_index))
    {
        log_error(log_domain::validation) << "Script failed evaluation";
        return false;
    }
    // Search for double spends
    if (orphan_is_spent(previous_output, index_in_parent, input_index))
        return false;
    // Increase value_in by this output's value
    value_in += output_value;
    if (value_in > max_money())
        return false;
    return true;
}

bool bdb_validate_block::lookup_transaction(
    const message::output_point& previous_output,
    message::transaction& previous_tx, size_t& previous_depth)
{
    for (size_t orphan_iter = 0; orphan_iter <= orphan_index_; ++orphan_iter)
    {
        const message::block& orphan_block =
            orphan_chain_[orphan_iter]->actual();
        for (const message::transaction& orphan_tx: orphan_block.transactions)
        {
            if (hash_transaction(orphan_tx) == previous_output.hash)
            {
                previous_tx = orphan_tx;
                previous_depth = fork_index_ + orphan_iter + 1;
                return true;
            }
        }
    }
    return false;
}

} // libbitcoin


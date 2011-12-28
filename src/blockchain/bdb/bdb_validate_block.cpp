#include "bdb_validate_block.hpp"

#include <bitcoin/util/assert.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/transaction.hpp>

namespace libbitcoin {

bdb_validate_block::bdb_validate_block(bdb_common_ptr common, int fork_index,
    const blocks_list& orphan_chain, int orphan_index, size_t depth,
    txn_guard_ptr txn, dialect_ptr dialect,
    const message::block& current_block)
 : validate_block(dialect, depth, current_block), common_(common),
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
        if (!connect_input(tx, input_index, value_in))
            return false;
    }
    return true;
}

bool bdb_validate_block::connect_input(const message::transaction& current_tx,
    size_t input_index, uint64_t& value_in)
{
    // Lookup previous output
    BITCOIN_ASSERT(input_index < current_tx.inputs.size());
    const message::transaction_input& input = current_tx.inputs[input_index];
    protobuf::Transaction previous_tx =
        common_->fetch_proto_transaction(txn_, input.previous_output.hash);
    if (!previous_tx.IsInitialized())
    {
        log_fatal() << "Unimplemented lookup for tx in bdb orphan chain";
        return false;
    }
    BITCOIN_ASSERT(previous_tx.parent_size() > 0);
    const protobuf::Transaction_Output& previous_tx_out =
        previous_tx.outputs(input.previous_output.index);
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
    // Increase value_in by this output's value
    value_in += output_value;
    if (value_in > max_money())
        return false;
    return true;
}

} // libbitcoin


#include "bdb_validate_block.hpp"

#include <bitcoin/util/assert.hpp>
#include <bitcoin/util/logger.hpp>

namespace libbitcoin {

bdb_validate_block::bdb_validate_block(bdb_common_ptr common,
    txn_guard_ptr txn, dialect_ptr dialect, size_t depth,
    const message::block& current_block)
 : validate_block(dialect, depth, current_block), common_(common),
    txn_(txn), depth_(depth)
{
}

uint32_t bdb_validate_block::previous_block_bits()
{
    log_debug() << __PRETTY_FUNCTION__;
    // Read block d - 1 and return bits
    protobuf::Block proto_block =
        common_->fetch_proto_block(txn_, depth_ - 1);
    return proto_block.bits();
}

uint64_t bdb_validate_block::actual_timespan(const uint64_t interval)
{
    log_debug() << __PRETTY_FUNCTION__;
    // depth - interval and depth - 1, return time difference
    protobuf::Block begin_block =
        common_->fetch_proto_block(txn_, depth_ - interval);
    protobuf::Block end_block =
        common_->fetch_proto_block(txn_, depth_ - 1);
    return end_block.timestamp() - begin_block.timestamp();
}

uint64_t bdb_validate_block::median_time_past()
{
    log_debug() << __PRETTY_FUNCTION__;
    // read last 11 block times into array and select median value
    std::vector<uint64_t> times;
    log_debug() << "ret " << times.size() << " " << depth_ - 11;
    for (int i = depth_ - 1; i >= 0 && i >= (int)depth_ - 11; --i)
    {
        log_debug() << "i = " << i;
        protobuf::Block proto_block = common_->fetch_proto_block(txn_, i);
        times.push_back(proto_block.timestamp());
    }
    log_debug() << "depth = " << depth_ << " size = " << times.size();
    BITCOIN_ASSERT(
        (depth_ < 11 && times.size() == depth_) || times.size() == 11);
    std::sort(times.begin(), times.end());
    return times[times.size() / 2];
}

bool bdb_validate_block::validate_transaction(const message::transaction& tx, 
    size_t index_in_parent, uint64_t& value_in)
{
    log_debug() << __PRETTY_FUNCTION__;
    value_in = 0;
    return true;
}

} // libbitcoin


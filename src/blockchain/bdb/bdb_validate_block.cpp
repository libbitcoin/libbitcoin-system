#include "bdb_validate_block.hpp"

namespace libbitcoin {

bdb_validate_block::bdb_validate_block(dialect_ptr dialect, 
    size_t depth, const message::block& current_block)
 : validate_block(dialect, depth, current_block)
{
}

uint32_t bdb_validate_block::previous_block_bits()
{
}

uint64_t bdb_validate_block::actual_timespan(const uint64_t interval)
{
}

uint64_t bdb_validate_block::median_time_past()
{
}

bool bdb_validate_block::validate_transaction(const message::transaction& tx, 
    size_t index_in_parent, uint64_t& value_in)
{
}

} // libbitcoin


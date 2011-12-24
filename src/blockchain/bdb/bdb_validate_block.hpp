#ifndef LIBBITCOIN_BLOCKCHAIN_BERKELEYDB_VALIDATE_BLOCK_H
#define LIBBITCOIN_BLOCKCHAIN_BERKELEYDB_VALIDATE_BLOCK_H

#include <bitcoin/validate.hpp>

#include "bdb_common.hpp"

namespace libbitcoin {

class bdb_validate_block
 : public validate_block
{
public:
    bdb_validate_block(bdb_common_ptr common, txn_guard_ptr txn,
        dialect_ptr dialect, size_t depth,
        const message::block& current_block);

protected:
    uint32_t previous_block_bits();
    uint64_t actual_timespan(const uint64_t interval);
    uint64_t median_time_past();
    bool validate_transaction(const message::transaction& tx, 
        size_t index_in_parent, uint64_t& value_in);

private:
    bdb_common_ptr common_;
    txn_guard_ptr txn_;
    size_t depth_;
};

} // libbitcoin

#endif


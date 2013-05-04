#ifndef LIBBITCOIN_BLOCKCHAIN_BERKELEYDB_VALIDATE_BLOCK_H
#define LIBBITCOIN_BLOCKCHAIN_BERKELEYDB_VALIDATE_BLOCK_H

#include <bitcoin/validate.hpp>

#include "leveldb_common.hpp"
#include "leveldb_organizer.hpp"

namespace libbitcoin {

class leveldb_validate_block
  : public validate_block
{
public:
    leveldb_validate_block(leveldb_common_ptr common, int fork_index,
        const block_detail_list& orphan_chain, int orphan_index,
        size_t depth, const message::block& current_block);

protected:
    uint32_t previous_block_bits();
    uint64_t actual_timespan(const uint64_t interval);
    uint64_t median_time_past();
    bool transaction_exists(const hash_digest& tx_hash);
    bool is_output_spent(const message::output_point& outpoint);
    bool fetch_transaction(message::transaction& tx, 
        size_t& previous_depth, const hash_digest& tx_hash);
    bool is_output_spent(const message::output_point& previous_output,
        size_t index_in_parent, size_t input_index);

private:
    message::block fetch_block(size_t fetch_depth);

    bool fetch_orphan_transaction(message::transaction& tx, 
        size_t& previous_depth, const hash_digest& tx_hash);
    bool orphan_is_spent(const message::output_point& previous_output,
        size_t skip_tx, size_t skip_input);

    leveldb_common_ptr common_;
    size_t depth_;

    size_t fork_index_, orphan_index_;
    const block_detail_list& orphan_chain_;
};

} // namespace libbitcoin

#endif


#include "bdb_organizer.hpp"

#include <bitcoin/dialect.hpp>
#include <bitcoin/util/assert.hpp>

#include "bdb_validate_block.hpp"

namespace libbitcoin {

bdb_organizer::bdb_organizer(bdb_common_ptr common,
    orphans_pool_ptr orphans, bdb_chain_keeper_ptr chain)
 : organizer(orphans, chain), common_(common), chain_(chain)
{
}

bool bdb_organizer::verify(int fork_index, const blocks_list& orphan_chain,
    int orphan_index)
{
    dialect_ptr default_dialect = std::make_shared<original_dialect>();
    BITCOIN_ASSERT(orphan_index < orphan_chain.size());
    const message::block& current_block = orphan_chain[orphan_index]->actual();
    size_t depth = fork_index + orphan_index + 1;
    BITCOIN_ASSERT(depth != 0);
    //bdb_validate_block validate(common_, chain_->txn(),
    //    default_dialect, depth, current_block);
    bdb_validate_block validate(common_, fork_index, orphan_chain,
        orphan_index, depth, chain_->txn(), default_dialect, current_block);
    return validate.validates();
}

} // libbitcoin


#include "bdb_organizer.hpp"

#include <bitcoin/dialect.hpp>
#include <bitcoin/util/assert.hpp>

#include "bdb_validate_block.hpp"

namespace libbitcoin {

bdb_organizer::bdb_organizer(orphans_pool_ptr orphans, chain_keeper_ptr chain)
 : organizer(orphans, chain)
{
}

bool bdb_organizer::verify(int fork_index, const blocks_list& orphan_chain,
    int orphan_index)
{
    dialect_ptr default_dialect = std::make_shared<original_dialect>();
    BITCOIN_ASSERT(orphan_index < orphan_chain.size());
    const message::block& current_block = orphan_chain[orphan_index]->actual();
    size_t depth = fork_index + orphan_index;
    bdb_validate_block validate(default_dialect, depth, current_block);
    return true;
}

} // libbitcoin


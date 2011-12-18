#include "bdb_organizer.hpp"

namespace libbitcoin {

bdb_organizer::bdb_organizer(orphans_pool_ptr orphans, chain_keeper_ptr chain)
 : organizer(orphans, chain)
{
}

bool bdb_organizer::verify(int fork_index, blocks_list& orphan_chain,
    int orphan_index)
{
    return true;
}

} // libbitcoin


#ifndef LIBBITCOIN_BLOCKCHAIN_BERKELEYDB_ORGANIZER_H
#define LIBBITCOIN_BLOCKCHAIN_BERKELEYDB_ORGANIZER_H

#include <bitcoin/blockchain/organizer.hpp>

namespace libbitcoin {

class bdb_organizer
 : public organizer
{
public:
    bdb_organizer(orphans_pool_ptr orphans, chain_keeper_ptr chain);

protected:
    bool verify(int fork_index, blocks_list& orphan_chain,
        int orphan_index);
};

} // libbitcoin

#endif


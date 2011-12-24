#ifndef LIBBITCOIN_BLOCKCHAIN_BERKELEYDB_ORGANIZER_H
#define LIBBITCOIN_BLOCKCHAIN_BERKELEYDB_ORGANIZER_H

#include <bitcoin/blockchain/organizer.hpp>

#include "bdb_common.hpp"
#include "bdb_chain_keeper.hpp"

namespace libbitcoin {

class bdb_organizer
 : public organizer
{
public:
    bdb_organizer(bdb_common_ptr common,
        orphans_pool_ptr orphans, bdb_chain_keeper_ptr chain);

protected:
    bool verify(int fork_index, const blocks_list& orphan_chain,
        int orphan_index);

private:
    bdb_common_ptr common_;
    bdb_chain_keeper_ptr chain_;
};

} // libbitcoin

#endif


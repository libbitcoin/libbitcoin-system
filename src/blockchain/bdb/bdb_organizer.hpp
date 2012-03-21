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
    typedef bdb_blockchain::reorganize_subscriber_type::ptr
        subscriber_ptr;

    bdb_organizer(bdb_common_ptr common, orphans_pool_ptr orphans,
        bdb_chain_keeper_ptr chain, subscriber_ptr reorganize_subscriber);

protected:
    std::error_code verify(int fork_index,
        const block_detail_list& orphan_chain, int orphan_index);
    void reorganize_occured(
        size_t fork_point,
        const blockchain::block_list& arrivals,
        const blockchain::block_list& replaced);

private:
    bdb_common_ptr common_;
    bdb_chain_keeper_ptr chain_;

    subscriber_ptr reorganize_subscriber_;
};

} // namespace libbitcoin

#endif


#ifndef LIBBITCOIN_BLOCKCHAIN_LEVELDB_ORGANIZER_H
#define LIBBITCOIN_BLOCKCHAIN_LEVELDB_ORGANIZER_H

#include <bitcoin/blockchain/organizer.hpp>

#include "leveldb_common.hpp"
#include "leveldb_chain_keeper.hpp"

namespace libbitcoin {

class leveldb_organizer
  : public organizer
{
public:
    typedef leveldb_blockchain::reorganize_subscriber_type::ptr
        subscriber_ptr;

    leveldb_organizer(leveldb_common_ptr common, orphans_pool_ptr orphans,
        leveldb_chain_keeper_ptr chain, subscriber_ptr reorganize_subscriber);

protected:
    std::error_code verify(int fork_index,
        const block_detail_list& orphan_chain, int orphan_index);
    void reorganize_occured(
        size_t fork_point,
        const blockchain::block_list& arrivals,
        const blockchain::block_list& replaced);

private:
    leveldb_common_ptr common_;
    leveldb_chain_keeper_ptr chain_;

    subscriber_ptr reorganize_subscriber_;
};

} // namespace libbitcoin

#endif


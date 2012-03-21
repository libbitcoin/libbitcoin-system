#include "bdb_organizer.hpp"

#include <bitcoin/exporter.hpp>
#include <bitcoin/utility/assert.hpp>

#include "bdb_validate_block.hpp"

namespace libbitcoin {

bdb_organizer::bdb_organizer(bdb_common_ptr common,
    orphans_pool_ptr orphans, bdb_chain_keeper_ptr chain,
    subscriber_ptr reorganize_subscriber)
  : organizer(orphans, chain), common_(common), chain_(chain),
    reorganize_subscriber_(reorganize_subscriber)
{
}

std::error_code bdb_organizer::verify(int fork_index,
    const block_detail_list& orphan_chain, int orphan_index)
{
    exporter_ptr saver = std::make_shared<satoshi_exporter>();
    BITCOIN_ASSERT(orphan_index < orphan_chain.size());
    const message::block& current_block = orphan_chain[orphan_index]->actual();
    size_t depth = fork_index + orphan_index + 1;
    BITCOIN_ASSERT(depth != 0);
    bdb_validate_block validate(common_, fork_index, orphan_chain,
        orphan_index, depth, chain_->txn(), saver, current_block);
    return validate.start();
}
void bdb_organizer::reorganize_occured(
    size_t fork_point,
    const blockchain::block_list& arrivals,
    const blockchain::block_list& replaced)
{
    reorganize_subscriber_->relay(std::error_code(),
        fork_point, arrivals, replaced);
}

} // namespace libbitcoin


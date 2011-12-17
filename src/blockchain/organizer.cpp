#include <bitcoin/blockchain/organizer.hpp>

namespace libbitcoin {

block_detail::block_detail(const message::block& actual_block,
    const std::string& label)
 : actual_block_(actual_block), block_hash_(hash_block_header(actual_block_)),
    processed_(false), label_(label)
{
}

const message::block& block_detail::actual() const
{
    return actual_block_;
}

void block_detail::mark_processed()
{
    processed_ = true;
}
bool block_detail::is_processed()
{
    return processed_;
}

const hash_digest& block_detail::get_hash() const
{
    return block_hash_;
}

const std::string& block_detail::get_label() const
{
    return label_;
}

orphans_pool::orphans_pool(size_t pool_size)
 : pool_(pool_size)
{
}

void orphans_pool::add(block_detail_ptr incoming_block)
{
    // No duplicates
    remove(incoming_block);
    pool_.push_back(incoming_block);
}

blocks_list orphans_pool::trace(block_detail_ptr end_block)
{
    blocks_list traced_chain;
    traced_chain.push_back(end_block);
    while (true)
    {
    resume_loop:
        const hash_digest& previous_hash =
            traced_chain.back()->actual().previous_block_hash;
        for (const block_detail_ptr current_block: pool_)
            if (current_block->get_hash() == previous_hash)
            {
                traced_chain.push_back(current_block);
                goto resume_loop;
            }
        break;
    }
    std::reverse(traced_chain.begin(), traced_chain.end());
    return traced_chain;
}

blocks_list orphans_pool::get_unprocessed()
{
    blocks_list unprocessed_blocks;
    for (const block_detail_ptr current_block: pool_)
    {
        if (!current_block->is_processed())
            unprocessed_blocks.push_back(current_block);
    }
    // Earlier blocks come into pool first. Lets match that
    // Helps avoid fragmentation, but isn't neccessary
    std::reverse(unprocessed_blocks.begin(), unprocessed_blocks.end());
    return unprocessed_blocks;
}

void orphans_pool::remove(block_detail_ptr remove_block)
{
    auto it = std::find(pool_.begin(), pool_.end(), remove_block);
    if (it != pool_.end())
        pool_.erase(it);
}

organizer::organizer(orphans_pool_ptr orphans, chain_keeper_ptr chain)
 : orphans_(orphans), chain_(chain)
{
}

void organizer::start()
{
    // Load unprocessed blocks
    process_queue_ = orphans_->get_unprocessed();
    // As we loop, we pop blocks off and process them
    while (!process_queue_.empty())
    {
        block_detail_ptr process_block = process_queue_.back();
        process_queue_.pop_back();

        // process() can remove blocks from the queue too
        chain_->start();
        process(process_block);
        chain_->stop();
    }
}

void organizer::process(block_detail_ptr process_block)
{
    // Trace the chain in the orphan pool
    blocks_list orphan_chain = orphans_->trace(process_block);
    int fork_index =
        chain_->find_index(orphan_chain[0]->actual().previous_block_hash);
    if (fork_index != -1)
        replace_chain(fork_index, orphan_chain);
    // Don't mark all orphan_chain as processed here because there might be
    // a winning fork from an earlier block
}

void organizer::replace_chain(int fork_index, blocks_list& orphan_chain)
{
    big_number orphan_work = 0;
    // Starting from beginning of the chain, validate blocks
    for (int orphan_index = 0; orphan_index < orphan_chain.size();
        ++orphan_index)
    {
        // Invalid block found
        if (!verify(fork_index, orphan_chain, orphan_index))
            clip_orphans(orphan_chain, orphan_index);
        const message::block& orphan_block =
            orphan_chain[orphan_index]->actual();
        orphan_work += block_work(orphan_block.bits);
    }
    // All remaining blocks in orphan_chain should all be valid now
    // Compare the difficulty of the 2 forks (original and orphan)
    big_number main_work = chain_->end_slice_difficulty(fork_index + 1);
    if (orphan_work <= main_work)
        return;
    // Replace! Switch!
    blocks_list replaced_slice = chain_->end_slice(fork_index + 1);
    for (block_detail_ptr replaced_block: replaced_slice)
    {
        replaced_block->mark_processed();
        orphans_->add(replaced_block);
    }
    for (block_detail_ptr arrival_block: orphan_chain)
    {
        orphans_->remove(arrival_block);
        chain_->add(arrival_block);
    }
}

void organizer::clip_orphans(blocks_list& orphan_chain, int orphan_index)
{
    for (auto it = orphan_chain.begin() + orphan_index;
        it != orphan_chain.end(); ++it)
    {
        orphan_chain.erase(it);
        // Remove from orphans pool
        orphans_->remove(*it);
    }
}

bool organizer::verify(int fork_index,
    blocks_list& orphan_chain, int orphan_index)
{
    return true;
}

} // libbitcoin


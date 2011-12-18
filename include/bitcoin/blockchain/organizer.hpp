#ifndef LIBBITCOIN_BLOCKCHAIN_ORGANIZER_H
#define LIBBITCOIN_BLOCKCHAIN_ORGANIZER_H

#include <boost/circular_buffer.hpp>

#include <bitcoin/block.hpp>
#include <bitcoin/messages.hpp>
#include <bitcoin/util/big_number.hpp>

namespace libbitcoin {

// Metadata + block
class block_detail
{
public:
    block_detail(const message::block& actual_block,
        const std::string& label = "");
    const message::block& actual() const;
    void mark_processed();
    bool is_processed();
    const hash_digest& hash() const;
    const std::string& label() const;
private:
    const message::block actual_block_;
    const hash_digest block_hash_;
    bool processed_;
    // For debugging
    std::string label_;
};

typedef std::shared_ptr<block_detail> block_detail_ptr;
typedef std::vector<block_detail_ptr> blocks_list;

// An unordered memory pool for orphan blocks
class orphans_pool
{
public:
    orphans_pool(size_t pool_size);
    void add(block_detail_ptr incoming_block);
    blocks_list trace(block_detail_ptr end_block);
    blocks_list unprocessed();
    void remove(block_detail_ptr remove_block);
private:
    boost::circular_buffer<block_detail_ptr> pool_;
};

typedef std::shared_ptr<orphans_pool> orphans_pool_ptr;

// The actual blockchain is encapsulated by this
class chain_keeper
{
public:
    virtual void start() = 0;
    virtual void stop() = 0;

    virtual void add(block_detail_ptr incoming_block) = 0;
    virtual int find_index(const hash_digest& search_block_hash) = 0;
    virtual big_number end_slice_difficulty(size_t slice_begin_index) = 0;
    virtual blocks_list end_slice(size_t slice_begin_index) = 0;
};

typedef std::shared_ptr<chain_keeper> chain_keeper_ptr;

// Structure which organises the blocks from the orphan pool to the blockchain
class organizer
{
public:
    organizer(orphans_pool_ptr orphans, chain_keeper_ptr chain);

    void start();
private:
    void process(block_detail_ptr process_block);
    void replace_chain(int fork_index, blocks_list& orphan_chain);
    bool verify(int fork_index, blocks_list& orphan_chain, int orphan_index);
    void clip_orphans(blocks_list& orphan_chain, int orphan_index);

    orphans_pool_ptr orphans_;
    chain_keeper_ptr chain_;

    blocks_list process_queue_;
};

typedef std::shared_ptr<organizer> organizer_ptr;

} // libbitcoin

#endif


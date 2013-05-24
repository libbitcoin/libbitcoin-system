#include <bitcoin/bitcoin.hpp>
using namespace bc;

#include <future>

#include "chains.hpp"

blockchain::block_list load_chain(const string_list& raw_chain)
{
    blockchain::block_list blks;
    for (const std::string& raw_repr: raw_chain)
    {
        auto block = std::make_shared<block_type>();
        data_chunk raw_block = bytes_from_pretty(raw_repr);
        satoshi_load(raw_block.begin(), raw_block.end(), *block);
        blks.push_back(block);
    }
    return blks;
}

void display_chain(const blockchain::block_list& chain)
{
    hash_digest previous = null_hash;
    for (const auto& blk: chain)
    {
        if (previous != null_hash)
        {
            BITCOIN_ASSERT(blk->previous_block_hash == previous);
        }
        previous = hash_block_header(*blk);
        log_info() << previous;
    }
}

std::string block_status_str(block_status status)
{
    switch (status)
    {
        case block_status::orphan:
            return "orphan";
        case block_status::confirmed:
            return "confirmed";
        case block_status::rejected:
            return "rejected";
    }
}

void store(blockchain& chain, const block_type& blk)
{
    std::promise<std::error_code> ec_promise;
    std::promise<block_info> info_promise;
    auto block_stored =
        [&ec_promise, &info_promise](
            const std::error_code& ec, block_info info)
        {
            ec_promise.set_value(ec);
            info_promise.set_value(info);
        };
    chain.store(blk, block_stored);
    std::error_code ec = ec_promise.get_future().get();
    block_info info = info_promise.get_future().get();
    log_info() << "Block " << hash_block_header(blk)
        << " [" << block_status_str(info.status) << "]";
    if (ec)
    {
        log_info() << "  NOT stored.";
    }
    else
    {
        log_info() << "  Stored at " << info.depth << ".";
    }
}

size_t last_depth(blockchain& chain)
{
    std::promise<std::error_code> ec_promise;
    std::promise<size_t> depth_promise;
    auto fetch_depth =
        [&ec_promise, &depth_promise](
            const std::error_code& ec, size_t block_depth)
        {
            ec_promise.set_value(ec);
            depth_promise.set_value(block_depth);
        };
    std::error_code ec = ec_promise.get_future().get();
    size_t block_depth = depth_promise.get_future().get();
    if (ec)
        log_error() << "last_depth: " << ec.message();
    return block_depth;
}

block_type get_block(blockchain& chain, size_t depth)
{
    std::promise<std::error_code> ec_promise;
    std::promise<block_type> block_promise;
    auto fetch_block =
        [&ec_promise, &block_promise](
            const std::error_code& ec, const block_type& blk)
        {
            ec_promise.set_value(ec);
            block_promise.set_value(blk);
        };
    std::error_code ec = ec_promise.get_future().get();
    const block_type& blk = block_promise.get_future().get();
    if (ec)
        log_error() << "last_depth: " << ec.message();
    return blk;
}

void show_chain(blockchain& chain)
{
    size_t depth = last_depth(chain);
    for (size_t i = 0; i < depth; ++i)
    {
        block_type blk = get_block(chain, i);
        log_info() << hash_block_header(blk);
    }
}

int main()
{
    system("rm -fr database/*");
    blockchain::block_list chain[3] = {
        load_chain(raw_chain_0),
        load_chain(raw_chain_1),
        load_chain(raw_chain_2)
    };
    log_info() << "Chain 0 -----";
    display_chain(chain[0]);
    log_info() << "Chain 1 -----";
    display_chain(chain[1]);
    log_info() << "Chain 2 -----";
    display_chain(chain[2]);

    threadpool pool(1);
    leveldb_blockchain blkchain(pool);
    std::promise<std::error_code> ec_promise;
    auto blockchain_started =
        [&ec_promise](const std::error_code& ec)
        {
            ec_promise.set_value(ec);
        };
    blkchain.start("database", blockchain_started);
    std::error_code ec = ec_promise.get_future().get();
    if (ec)
        return 1;

    blkchain.import(genesis_block(), 0, [](const std::error_code& ec) {});

    for (size_t i = 0; i < 6; ++i)
    {
        log_info() << "i = " << i;
        for (size_t cidx = 0; cidx < 3; ++cidx)
        {
            log_info() << cidx;
            BITCOIN_ASSERT(cidx < sizeof(chain));
            BITCOIN_ASSERT(i < chain[cidx].size());
            store(blkchain, *chain[cidx][i]);
        }
    }
    store(blkchain, *chain[1][6]);
    show_chain(blkchain);
    pool.stop();
    pool.join();
    blkchain.stop();
    return 0;
}


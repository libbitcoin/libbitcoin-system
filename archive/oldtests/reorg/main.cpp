/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin.hpp>
using namespace bc;

#include <future>

#include "chains.hpp"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

blockchain::block_list load_chain(const string_list& raw_chain)
{
    blockchain::block_list blks;
    for (const std::string& raw_repr: raw_chain)
    {
        auto block = std::make_shared<block_type>();
        data_chunk raw_block = decode_hex(raw_repr);
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
            BITCOIN_ASSERT(blk->header.previous_block_hash == previous);
        }
        previous = hash_block_header(blk->header);
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
    std::error_code ec;
    block_info info;
    std::promise<bool> promise;
    auto block_stored =
        [&ec, &info, &promise](
            const std::error_code& cec, block_info cinfo)
        {
            ec = cec;
            info = cinfo;
            promise.set_value(true);
        };
    chain.store(blk, block_stored);
    bool success = promise.get_future().get();
    BITCOIN_ASSERT(success);
    log_info() << "Block " << hash_block_header(blk.header)
        << " [" << block_status_str(info.status) << "]";
    if (ec)
    {
        log_info() << "  NOT stored (reason=" << ec.message() << ")";
    }
    else if (info.status == block_status::confirmed)
    {
        log_info() << "  Stored at " << info.height << ".";
    }
}

size_t last_height(blockchain& chain)
{
    std::error_code ec;
    size_t height;
    std::promise<bool> promise;
    auto fetch_height =
        [&ec, &height, &promise](
            const std::error_code& cec, size_t cheight)
        {
            ec = cec;
            height = cheight;
            promise.set_value(true);
        };
    chain.fetch_last_height(fetch_height);
    bool success = promise.get_future().get();
    BITCOIN_ASSERT(success);
    if (ec)
        log_error() << "last_height: " << ec.message();
    return height;
}

block_header_type get_block(blockchain& chain, size_t height)
{
    std::error_code ec;
    block_header_type block;
    std::promise<bool> promise;
    auto fetch_block =
        [&ec, &block, &promise](
            const std::error_code& cec, const block_header_type& cblk)
        {
            ec = cec;
            block = cblk;
            promise.set_value(true);
        };
    chain.fetch_block_header(height, fetch_block);
    bool success = promise.get_future().get();
    BITCOIN_ASSERT(success);
    if (ec)
        log_error() << "last_height: " << ec.message();
    return block;
}

void show_chain(blockchain& chain)
{
    size_t height = last_height(chain);
    for (size_t i = 0; i < height; ++i)
    {
        block_header_type blk_header = get_block(chain, i);
        log_info() << hash_block_header(blk_header);
    }
}

void reorganize(blockchain& blkchain,
    const std::error_code& ec, size_t fork_point,
    const bc::blockchain::block_list& new_blocks,
    const bc::blockchain::block_list& replaced_blocks)
{
    if (ec)
    {
        log_error() << "Reorganize failed: " << ec.message();
        return;
    }
    std::ostringstream oss;
    for (const auto& blk: new_blocks)
        oss << "New block: " << hash_block_header(blk->header) << "\n";
    for (const auto& blk: replaced_blocks)
        oss << "Removed block: " << hash_block_header(blk->header) << "\n";
    log_info() << oss.str();
    blkchain.subscribe_reorganize(
        std::bind(reorganize, std::ref(blkchain), _1, _2, _3, _4));
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
    blkchain.subscribe_reorganize(
        std::bind(reorganize, std::ref(blkchain), _1, _2, _3, _4));
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

    big_number running_work[3] = {0, 0, 0};
    for (size_t i = 0; i < 6; ++i)
    {
        log_info() << "i = " << i;
        for (size_t cidx = 0; cidx < 3; ++cidx)
        {
            running_work[cidx] += block_work(chain[cidx][i]->header.bits);
            BITCOIN_ASSERT(cidx < sizeof(chain));
            BITCOIN_ASSERT(i < chain[cidx].size());
            store(blkchain, *chain[cidx][i]);
        }
    }
    running_work[1] += block_work(chain[1][6]->header.bits);
    store(blkchain, *chain[1][6]);
    show_chain(blkchain);
    size_t biggest_work_idx = 0;
    for (size_t i = 1; i < 3; ++i)
        if (running_work[i] > running_work[biggest_work_idx])
            biggest_work_idx = i;
    log_info() << "Chain " << biggest_work_idx << " has the biggest work.";
    pool.stop();
    pool.join();
    blkchain.stop();
    return 0;
}


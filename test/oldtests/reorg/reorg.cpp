/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <future>
#include <bitcoin/bitcoin.hpp>

#include "chains.hpp"

using namespace bc;
using std::placeholders::_1;
using std::placeholders::_2;

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

blockchain::block_list chain[3] = {
    load_chain(raw_chain_0),
    load_chain(raw_chain_1),
    load_chain(raw_chain_2)
};

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

blockchain* c = nullptr;

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

void handle_final_store(const std::error_code& ec, block_info info,
    const block_type& blk)
{
    log_info() << "Block " << hash_block_header(blk)
        << " [" << block_status_str(info.status) << "]";
    if (ec)
    {
        log_info() << "  NOT stored.";
    }
    else if (info.status == block_status::confirmed)
    {
        log_info() << "  Stored at " << info.height << ".";
    }
}

void next_store(const std::error_code& ec, block_info info,
    const block_type& blk, size_t i, size_t cidx)
{
    log_info() << "Block " << hash_block_header(blk)
        << " [" << block_status_str(info.status) << "]";
    if (ec)
    {
        log_info() << "  NOT stored.";
    }
    else if (info.status == block_status::confirmed)
    {
        log_info() << "  Stored at " << info.height << ".";
    }
    ++cidx;
    if (cidx >= 3)
    {
        BITCOIN_ASSERT(cidx == 3);
        cidx = 0;
        ++i;
        if (i >= 6)
        {
            BITCOIN_ASSERT(i == 6);
            c->store(*chain[1][6],
                std::bind(handle_final_store, _1, _2, *chain[1][6]));
            log_info() << "STOP!";
            return;
        }
    }
    BITCOIN_ASSERT(cidx < sizeof(chain));
    BITCOIN_ASSERT(i < chain[cidx].size());
    c->store(*chain[cidx][i],
        std::bind(next_store, _1, _2, *chain[cidx][i], i, cidx));
}

void handle_reorganize(const std::error_code& ec, size_t fork_point,
    const blockchain::block_list& arrivals,
    const blockchain::block_list& replaced)
{
    log_info() << "Reorganize! " << fork_point << " " << ec.message();
    for (auto blk: arrivals)
        log_info() << "Arrive: " << hash_block_header(*blk);
    for (auto blk: replaced)
        log_info() << "Remove: " << hash_block_header(*blk);
    c->subscribe_reorganize(handle_reorganize);
}

int main()
{
    system("rm -fr database/*");
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
    c = &blkchain;

    size_t i = 0, cidx = 0;
    c->store(*chain[cidx][i],
        std::bind(next_store, _1, _2, *chain[cidx][i], i, cidx));

    c->subscribe_reorganize(handle_reorganize);

    std::cin.get();

    pool.stop();
    pool.join();
    blkchain.stop();
    return 0;
}


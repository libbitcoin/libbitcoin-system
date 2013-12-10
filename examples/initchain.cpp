/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/*
  Create a new leveldb blockchain database.
*/
#include <future>
#include <bitcoin/bitcoin.hpp>
using namespace bc;

int main(int argc, char** argv)
{
    if (argc != 2)
        return 1;
    const std::string dbpath = argv[1];
    // Threadpool context containing 1 thread.
    threadpool pool(1);
    // leveldb_blockchain operations execute in pool's thread.
    leveldb_blockchain chain(pool);
    // Completion handler for starting the leveldb_blockchain.
    // Does nothing.
    auto blockchain_start = [](const std::error_code& ec) {};
    // Start blockchain with a database path.
    chain.start(dbpath, blockchain_start);
    // First block is the genesis block.
    block_type first_block = genesis_block();
    std::promise<std::error_code> ec_promise;
    // Completion handler for import method.
    auto import_finished = 
        [&ec_promise](const std::error_code& ec)
        {
            ec_promise.set_value(ec);
        };
    // Import the genesis block at height 0.
    // Doesn't validate or perform checks on the block.
    chain.import(first_block, 0, import_finished);
    // Wait until std::error_code is set by
    // import_finished completion handler.
    std::error_code ec = ec_promise.get_future().get();
    if (ec)
    {
        log_error() << "Importing genesis block failed: " << ec.message();
        return -1;
    }
    log_info() << "Imported genesis block "
        << hash_block_header(first_block.header);
    // All threadpools stopping in parallel...
    pool.stop();
    // ... Make them all join main thread and wait until they finish.
    pool.join();
    // Now safely close leveldb_blockchain.
    chain.stop();
    return 0;
}


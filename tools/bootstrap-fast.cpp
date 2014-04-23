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
/*
  Imports blocks from bootstrap.dat
  bootstrap.dat is the raw blocks in the format
  [magic bytes] [block length] [serialized block]

  No validation is performed on the blocks, ensure the data is correct.
*/
#include <future>
#include <bitcoin/bitcoin.hpp>
#include <time.h>

#define LOG_BOOTSTRAP "bootstrap"

using namespace bc;
using std::placeholders::_1;
using std::placeholders::_2;

bool stopped = false;

void signal_handler(int signal)
{
    log_info(LOG_BOOTSTRAP) << "Caught signal: " << signal;
    stopped = true;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: "
            "bootstrap-fast BLOCKCHAIN_DIR BOOTSTRAP_DATFILE" << std::endl <<
            "Ensure the BLOCKCHAIN_DIR is empty and the " << std::endl <<
            "BOOTSTRAP_DATFILE is correct, no validation is done" << std::endl;
        return 1;
    }

    const std::string dbpath = argv[1];
    const std::string bootstrapfile = argv[2];

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
    const hash_digest genesis_hash = hash_block_header(first_block.header);

    // Total number of blocks parsed from the bootstrap.dat
    int blocks_parsed = 0;

    // position of the pointer in the buffer
    int buffer_position = 0;

    std::ifstream fin(bootstrapfile, std::ios::in | std::ios::binary);

    if (!fin)
    {
        log_error(LOG_BOOTSTRAP) << "Could not open bootstrap.dat file";
        fin.close();

        pool.stop();
        pool.join();
        chain.stop();
        return -1;
    }

    constexpr size_t max_block_size = 1000000;
    // Allocate a 2 * max_block_size buffer, so we are sure to read at least
    // one full block into the buffer, or many smaller ones.
    constexpr size_t buffer_size = 10 * max_block_size;

    int block_size;
    data_chunk buffer(buffer_size);

    // The total size of the block is endian reversed
    // This lambda function will read the chunk from its
    // first byte at position pos in the buffer and return
    // the integer representation of the block size in bytes.
    auto get_block_size =
        [](const data_chunk& buffer, const int pos)
        {
            return (int) (buffer[pos + 3] << 24) +
                   (int) (buffer[pos + 2] << 16) +
                   (int) (buffer[pos + 1] << 8) +
                   (int) (buffer[pos]);
        };

    // This will return false when there are not enough bytes left
    // in the file to fill the buffer, meaning we may miss the last block
    while(!stopped)
    {
        fin.read((char*) &buffer[0], buffer_size);
        buffer_position = 0;

        while(buffer_position < buffer_size - 8 && !stopped)
        {
            // Check we have the magic bytes. This will automatically exit
            // if reach an incorrect part or the end of the file. 
            // Serialized format has its bytes reversed.
            uint32_t bootstrap_magic_value = buffer[buffer_position] + 
                                             (buffer[buffer_position + 1] << 8) +
                                             (buffer[buffer_position + 2] << 16) + 
                                             (buffer[buffer_position + 3] << 24);
                                        
            if (bootstrap_magic_value != magic_value())
            {
                stopped = true;
                break;
            }
            // Get the block size from the reversed endain field.
            block_size = get_block_size(buffer, buffer_position + 4);

            // If the rest of the block isn't inside the buffer break out
            if (buffer_position + block_size + 8 > buffer_size)
                break;

            auto begin_iter = buffer.begin() + buffer_position + 8;
            auto end_iter = begin_iter + block_size;
            block_type blk;
            satoshi_load(begin_iter, end_iter, blk);

            // Assert we match the genesis block hash
            BITCOIN_ASSERT(blocks_parsed != 0 ||
                hash_block_header(blk.header) == genesis_hash);

            std::error_code ec;
            block_info info;
            bool finished = false;

            auto block_imported = [&ec, &blocks_parsed, &finished](
                const std::error_code& cec)
            {
                log_debug(LOG_BOOTSTRAP) << blocks_parsed << " Block imported.";
                // info = blk_info;
                ec = cec;
                finished = true;
            };

            chain.import(blk, blocks_parsed ,block_imported);

            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 10;

            while (!finished)
                nanosleep(&ts, NULL);

            hash_digest block_hash = hash_block_header(blk.header);

            buffer_position += block_size + 8;
            blocks_parsed++;
        }
        log_info(LOG_BOOTSTRAP)
                            << "Rewinding file buffer";
        // Rewind to the last good magic bytes, which didn't have enough space
        // left in the buffer for the block
        fin.seekg(buffer_position - buffer_size, std::ios::cur);
    }

    fin.close();
    // All threadpools stopping in parallel...
    pool.stop();
    // ... Make them all join main thread and wait until they finish.
    pool.join();
    // Now safely close leveldb_blockchain.
    chain.stop();
    return 0;
}

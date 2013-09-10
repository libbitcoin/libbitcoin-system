/*
  Imports blocks from bootstrap.dat
  bootstrap.dat is the raw blocks in the format
  [magic bytes] [block length] [serialized block]
*/
#include <future>
#include <bitcoin/bitcoin.hpp>
using namespace bc;
 
using std::placeholders::_1;
using std::placeholders::_2;

bool stopped = false;
void signal_handler(int sig)
{
    log_info("bootstrap") << "Caught signal: " << sig;
    stopped = true;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "usage: bootstrap <blockchain directory> <bootstrap.dat file>" << std::endl;
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

    // Total number of blocks parsed from the bootstrap.dat
    int blocks_parsed = 0;

    // position of the pointer in the buffer
    int buffer_position = 0;

    std::promise<std::error_code> ec_promise;
    std::promise<block_header_type> blk_header_promise;

    auto genesis_block_fetched_handler = 
        [&ec_promise, &blk_header_promise](const std::error_code& ec,
                                             const block_header_type& header)
        {
            ec_promise.set_value(ec);
            blk_header_promise.set_value(header);
        };

    // See if we can fetch the genesis block from the leveldb database
    chain.fetch_block_header(0, genesis_block_fetched_handler);

    std::error_code ec = ec_promise.get_future().get();
    block_header_type blk_header = blk_header_promise.get_future().get();


    if (ec)
    {
        //Import the genesis_block() if it does not yet exist in the leveldb
        std::promise<std::error_code> ec_promise;

        auto genesis_block_imported_handler = 
            [&ec_promise](const std::error_code& ec)
            {
                ec_promise.set_value(ec);
            };

        chain.import(first_block, 0, genesis_block_imported_handler);
        ec = ec_promise.get_future().get();

        if (ec)
        {
            log_error() << "Importing genesis block failed: " << ec.message();
            return -1;
        }
        else 
            log_info() << "Imported genesis block";

    }
    // If we already have a block stored in the leveldb, check its hash 
    // matches the one coded into genesis_block()
    else 
        BITCOIN_ASSERT(hash_block_header(blk_header) ==
                         hash_block_header(first_block.header));

    std::ifstream fin(bootstrapfile, std::ios::in | std::ios::binary);

    if (!fin)
    {
        log_error() << "Could not open bootstrap.dat file";
        fin.close();
 
        pool.stop();
        pool.join();
        chain.stop();
        return -1;
    }

    constexpr size_t max_block_size = 1000000;
    // Allocate a 2 * max_block_size buffer, so we are sure to read at least 
    // one full block into the buffer, or many smaller ones.
    constexpr size_t buffer_size = 2 * max_block_size; 

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
    std::cout << "entering loop" << std::endl;

    while(!stopped)
    {
        std::cout << "started1" << std::endl;

        fin.read((char*) &buffer[0], buffer_size);
        buffer_position = 0;

        std::cout << "started" << std::endl;

        while(buffer_position < buffer_size - 8 && !stopped)
        {
            // Assert we have the magic bytes.
            BITCOIN_ASSERT( buffer[buffer_position] == 0xf9 && 
                        buffer[buffer_position + 1] == 0xbe && 
                        buffer[buffer_position + 2] == 0xb4 && 
                        buffer[buffer_position + 3] == 0xd9 );
 
            // Get the block size from the reversed endain field.
            block_size = get_block_size(buffer, buffer_position + 4);
 
            // If the rest of the block isn't inside the buffer break out
            if (buffer_position + block_size + 8 > buffer_size)
                break;
            
            std::promise<std::error_code> ec_promise;
            std::promise<block_info> info_promise;
            block_type blk;

            satoshi_load(buffer.begin() + buffer_position + 8,
                         buffer.begin() + buffer_position + 8 + block_size,
                         blk);

            // Assert we match the genesis block hash
            if (blocks_parsed == 0)
                BITCOIN_ASSERT(hash_block_header(blk.header) ==
                                 hash_block_header(first_block.header));

            auto block_imported = 
            [&ec_promise, &info_promise](const std::error_code& ec,
                                         const block_info info)
            {  
                // handle_store(ec, info, block_hash);
                ec_promise.set_value(ec);
                info_promise.set_value(info);
            };

            chain.store(blk, std::bind(block_imported, _1, _2));
            hash_digest block_hash = hash_block_header(blk.header);

            std::error_code ec = ec_promise.get_future().get();
            block_info info = info_promise.get_future().get();

            // We need orphan blocks so we can do the next getblocks round
            if (ec && info.status != block_status::orphan)
            {
                log_error("bootstrap")
                    << "Storing block " << encode_hex(block_hash)
                    << ": " << ec.message();
            }
            else
            switch (info.status)
            {
                // There should be no orphans
                case block_status::orphan:
                    log_error("bootstrap")
                        << "Orphan block " << encode_hex(block_hash); 
                break;
 
                case block_status::rejected:
                    log_error("bootstrap")
                        << "Rejected block " << encode_hex(block_hash);
                break;
 
                case block_status::confirmed:
                    log_info("bootstrap")
                        << "Block #" << info.height << " " << encode_hex(block_hash);
                break;
            }

            // Fixes segmentation fault, maybe this is an issue with ec_promise and .get_future() 
            usleep(1000);

            buffer_position += block_size + 8;
            blocks_parsed++;
        }
 
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

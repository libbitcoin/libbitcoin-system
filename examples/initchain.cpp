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


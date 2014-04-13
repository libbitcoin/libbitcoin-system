/*
Create a new leveldb blockchain database.
*/
#include <iostream>

#ifndef LEVELDB_ENABLED

int main()
{
    std::cerr << "initchain: The feature is not supported in this build." << std::endl;
    return 1;
}

#else

#include <future>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

void create_file(const std::string& filename, size_t filesize)
{
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    constexpr size_t chunk_size = 100000;
    std::vector<char> random_buffer(chunk_size);
    for (size_t i = 0; i < filesize; i += chunk_size)
        file.write(random_buffer.data(), chunk_size);
}

void initialize_new_stealth_database(const std::string& filename)
{
    create_file(filename, 100000000);
    mmfile file(filename);
    auto serial = make_serializer(file.data());
    serial.write_4_bytes(1);
    // should last us a decade
    size_t max_header_rows = 10000;
    serial.write_4_bytes(max_header_rows);
    serial.write_4_bytes(0);
    for (size_t i = 0; i < max_header_rows; ++i)
        serial.write_4_bytes(0);
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "initchain: No directory specified." << std::endl;
        return 1;
    }
    const std::string dbpath = argv[1];
    // Create custom databases first.
    initialize_new_stealth_database(dbpath + "/stealth.db");
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

#endif

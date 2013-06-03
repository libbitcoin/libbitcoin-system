/*
  Display info from the last block in our blockchain at "./database/"
*/
#include <bitcoin/bitcoin.hpp>
using namespace bc;

blockchain* chain = nullptr;

// Completion handler for when the blockchain has finished initializing.
void blockchain_started(const std::error_code& ec);
// Fetch tbe last block now that we have the depth.
void depth_fetched(const std::error_code& ec, size_t last_depth);
// Result: print the block header.
void display_block_header(const std::error_code& ec, const block_type& blk);

void blockchain_started(const std::error_code& ec)
{
    // std::error_code's can be tested like bools, and
    // compared against specific error enums.
    // See <bitcoin/error.hpp> for a full list of them.
    if (ec)
    {
        log_error() << "Blockchain failed to start: " << ec.message();
        return;
    }
    // Blockchain has safely started.
    log_info() << "Blockchain started.";
    // chain should've been set inside main().
    assert(chain);
    // Begin fetching the last depth number.
    chain->fetch_last_depth(depth_fetched);
}

void depth_fetched(const std::error_code& ec, size_t last_depth)
{
    if (ec)
    {
        log_error() << "Failed to fetch last depth: " << ec.message();
        return;
    }
    // Display the block number.
    log_info() << "depth: " << last_depth;
    assert(chain);
    // Begin fetching the block header.
    chain->fetch_block_header(last_depth, display_block_header);
}

void display_block_header(const std::error_code& ec, const block_type& blk)
{
    if (ec)
    {
        log_error() << "Failure fetching block header: " << ec.message();
        return;
    }
    // 32 byte std::array of uint8_t
    const hash_digest& blk_hash = hash_block_header(blk);
    // Encode block hash into a pretty hex string.
    log_info() << "hash: " << pretty_hex(blk_hash);
    // Display a few fields from the block header.
    // See <bitcoin/primitives.hpp> for the definition of block_type.
    log_info() << "version: " << blk.version;
    // hash_digest can be used directly in log_info(),
    // implicity calling pretty_hex() on the hash_digest.
    log_info() << "previous_block_hash: " << blk.previous_block_hash;
    log_info() << "merkle: " << blk.merkle;
    log_info() << "timestamp: " << blk.timestamp;
    log_info() << "bits: " << blk.bits;
    log_info() << "nonce: " << blk.nonce;
    // This is not the full block, only the header.
    // For the full block use fetch_block() instead.
    assert(blk.transactions.size() == 0);
    // A goodbye message.
    log_info() << "Finished.";
}

int main()
{
    // Define a threadpool with 1 thread.
    threadpool pool(1);
    // Create a LevelDB blockchain.
    leveldb_blockchain ldb_chain(pool);
    // Initialize our global 'chain' pointer from above.
    chain = &ldb_chain;
    // Start the database using its implementation specific method.
    ldb_chain.start("database", blockchain_started);
    // Keep running until the user presses enter.
    // Since libbitcoin is asynchronous, you need to synchronise with
    // them to know when to exit safely.
    // For these examples we just pause until enter for simplicity sake.
    std::cin.get();
    // Begin stopping the threadpools in parallel (only 1 here).
    pool.stop();
    // Join them one by one.
    pool.join();
    // Finally stop the blockchain safely now everything has stopped.
    ldb_chain.stop();
    return 0;
}


.. _examples_display-last:

examples/display-last.cpp
#########################

Fetches and displays info about the last block in a blockchain.

You will need to call initchain on the database first before using this.
Maybe even download a few blocks into the database.

::

    #include <bitcoin/bitcoin.hpp>
    using namespace bc;
    
    blockchain* chain = nullptr;
    
    // Completion handler for when the blockchain has finished initializing.
    void blockchain_started(const std::error_code& ec);
    // Fetch the last block now that we have the height.
    void height_fetched(const std::error_code& ec, size_t last_height);
    // Result: print the block header.
    void display_block_header(const std::error_code& ec,
        const block_header_type& header);
    
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
        // Begin fetching the last height number.
        chain->fetch_last_height(height_fetched);
    }
    
    void height_fetched(const std::error_code& ec, size_t last_height)
    {
        if (ec)
        {
            log_error() << "Failed to fetch last height: " << ec.message();
            return;
        }
        // Display the block number.
        log_info() << "height: " << last_height;
        assert(chain);
        // Begin fetching the block header.
        chain->fetch_block_header(last_height, display_block_header);
    }
    
    // This is not the full block, only the header.
    // For the full block use fetch_block() instead.
    void display_block_header(const std::error_code& ec,
        const block_header_type& header)
    {
        if (ec)
        {
            log_error() << "Failure fetching block header: " << ec.message();
            return;
        }
        // 32 byte std::array of uint8_t
        const hash_digest& blk_hash = hash_block_header(header);
        // Encode block hash into a pretty hex string.
        log_info() << "hash: " << encode_hex(blk_hash);
        // Display a few fields from the block header.
        // See <bitcoin/primitives.hpp> for the definition of block_type.
        log_info() << "version: " << header.version;
        // hash_digest can be used directly in log_info(),
        // implicity calling encode_hex() on the hash_digest.
        log_info() << "previous_block_hash: " << header.previous_block_hash;
        log_info() << "merkle: " << header.merkle;
        log_info() << "timestamp: " << header.timestamp;
        log_info() << "bits: " << header.bits;
        log_info() << "nonce: " << header.nonce;
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


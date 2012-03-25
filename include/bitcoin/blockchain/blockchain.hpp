#ifndef LIBBITCOIN_BLOCKCHAIN_BLOCKCHAIN_H
#define LIBBITCOIN_BLOCKCHAIN_BLOCKCHAIN_H

/**
 * Represents an interface to a blockchain backend.
 */

#include <boost/utility.hpp>
#include <functional>

#include <bitcoin/block.hpp>
#include <bitcoin/error.hpp>
#include <bitcoin/messages.hpp>

namespace libbitcoin {

class blockchain
{
public:
    typedef std::function<void (const std::error_code&, block_info)>
        store_block_handler;

    // Not supported in g++ yet
    //template <typename Message>
    //using fetch_handler = std::function<void (Message)>;

    typedef std::function<
        void (const std::error_code&, const message::block&)>
            fetch_handler_block_header;

    typedef std::function<
        void (const std::error_code&, const message::inventory_list&)>
            fetch_handler_block_transaction_hashes;

    typedef std::function<void (const std::error_code&, size_t)>
        fetch_handler_block_depth;

    typedef std::function<void (const std::error_code&, size_t)>
        fetch_handler_last_depth;

    typedef std::function<
        void (const std::error_code&, const message::block_locator&)>
            fetch_handler_block_locator;

    typedef std::function<
        void (const std::error_code&, const message::transaction&)>
            fetch_handler_transaction;

    typedef std::function<
        void (const std::error_code&, size_t, size_t)>
            fetch_handler_transaction_index;

    typedef std::function<
        void (const std::error_code&, const message::input_point&)>
            fetch_handler_spend;

    typedef std::function<
        void (const std::error_code&, const message::output_point_list&)>
            fetch_handler_outputs;

    typedef std::vector<std::shared_ptr<message::block>> block_list;
    typedef std::function<
        void (const std::error_code&, size_t, 
            const block_list&, const block_list&)> reorganize_handler;

    /**
     * Store a new block.
     *
     * Subscriber is notified exactly once of changes to the blockchain
     * and needs to re-subscribe to continue being notified.
     *
     * @param[in]   stored_block    Block to store
     * @param[in]   handle_store    Completion handler for store operation.
     * @code
     *  void handle_reorganize(
     *      const std::error_code& ec,   // Status of operation
     *      block_info info              // Status and depth of block
     *  );
     * @endcode
     */
    virtual void store(const message::block& stored_block, 
        store_block_handler handle_store) = 0;

    /**
     * Fetches the block header by depth.
     *
     * @param[in]   depth           Depth of block to fetch
     * @param[in]   handle_fetch    Completion handler for fetch operation.
     * @code
     *  void handle_fetch(
     *      const std::error_code& ec, // Status of operation
     *      const message::block& blk  // Block header
     *  );
     * @endcode
     */
    virtual void fetch_block_header(size_t depth,
        fetch_handler_block_header handle_fetch) = 0;

    /**
     * Fetches the block header by hash.
     *
     * @param[in]   block_hash      Hash of block
     * @param[in]   handle_fetch    Completion handler for fetch operation.
     * @code
     *  void handle_fetch(
     *      const std::error_code& ec, // Status of operation
     *      const message::block& blk  // Block header
     *  );
     * @endcode
     */
    virtual void fetch_block_header(const hash_digest& block_hash,
        fetch_handler_block_header handle_fetch) = 0;

    /**
     * Fetches list of transaction hashes in a block by depth.
     *
     * @param[in]   depth           Depth of block containing transactions.
     * @param[in]   handle_fetch    Completion handler for fetch operation.
     * @code
     *  void handle_fetch(
     *      const std::error_code& ec,             // Status of operation
     *      const message::inventory_list& hashes  // List of hashes
     *  );
     * @endcode
     */
    virtual void fetch_block_transaction_hashes(size_t depth,
        fetch_handler_block_transaction_hashes handle_fetch) = 0;

    /**
     * Fetches list of transaction hashes in a block by block hash.
     *
     * @param[in]   depth           Depth of block to fetch
     * @param[in]   handle_fetch    Completion handler for fetch operation.
     * @code
     *  void handle_fetch(
     *      const std::error_code& ec,             // Status of operation
     *      const message::inventory_list& hashes  // List of hashes
     *  );
     * @endcode
     */
    virtual void fetch_block_transaction_hashes(const hash_digest& block_hash,
        fetch_handler_block_transaction_hashes handle_fetch) = 0;

    /**
     * Fetches the depth of a block given its hash.
     *
     * @param[in]   block_hash      Hash of block
     * @param[in]   handle_fetch    Completion handler for fetch operation.
     * @code
     *  void handle_fetch(
     *      const std::error_code& ec, // Status of operation
     *      size_t block_depth         // Depth of block
     *  );
     * @endcode
     */
    virtual void fetch_block_depth(const hash_digest& block_hash,
        fetch_handler_block_depth handle_fetch) = 0;

    /**
     * Fetches the depth of the last block in our blockchain.
     *
     * @param[in]   handle_fetch    Completion handler for fetch operation.
     * @code
     *  void handle_fetch(
     *      const std::error_code& ec, // Status of operation
     *      size_t block_depth         // Depth of last block
     *  );
     * @endcode
     */
    virtual void fetch_last_depth(fetch_handler_last_depth handle_fetch) = 0;

    /**
     * Creates a block_locator object used to download the blockchain.
     * @deprecated Soon to be replaced by fetch_block_locator free function.
     *
     * @param[in]   handle_fetch    Completion handler for fetch operation.
     * @code
     *  void handle_fetch(
     *      const std::error_code& ec,         // Status of operation
     *      const message::block_locator& loc  // Block locator object
     *  );
     * @endcode
     */
    virtual void fetch_block_locator(
        fetch_handler_block_locator handle_fetch) = 0;

    /**
     * Fetches a transaction by hash
     *
     * @param[in]   transaction_hash  Transaction's hash
     * @param[in]   handle_fetch      Completion handler for fetch operation.
     * @code
     *  void handle_fetch(
     *      const std::error_code& ec,      // Status of operation
     *      const message::transaction& tx  // Transaction
     *  );
     * @endcode
     */
    virtual void fetch_transaction(const hash_digest& transaction_hash,
        fetch_handler_transaction handle_fetch) = 0;

    /**
     * Fetch the block depth that contains a transaction and its offset
     * within a block.
     *
     * @param[in]   transaction_hash  Transaction's hash
     * @param[in]   handle_fetch      Completion handler for fetch operation.
     * @code
     *  void handle_fetch(
     *      const std::error_code& ec, // Status of operation
     *      size_t block_depth,        // Depth of block containing
     *                                 // the transaction.
     *      size_t offset              // Offset of transaction within
     *                                 // the block.
     *  );
     * @endcode
     */
    virtual void fetch_transaction_index(
        const hash_digest& transaction_hash,
        fetch_handler_transaction_index handle_fetch) = 0;

    // fetch of inputs and outputs is a future possibility
    // for now use fetch_transaction and lookup input/output

    /**
     * Fetches a corresponding spend of an output.
     *
     * @param[in]   outpoint        Representation of an output.
     * @param[in]   handle_fetch    Completion handler for fetch operation.
     * @code
     *  void handle_fetch(
     *      const std::error_code& ec,           // Status of operation
     *      const message::input_point& inpoint  // Spend of output
     *  );
     * @endcode
     */
    virtual void fetch_spend(const message::output_point& outpoint,
        fetch_handler_spend handle_fetch) = 0;

    /**
     * Fetches outputs associated with a unencoded bitcoin address.
     *
     * @param[in]   pubkey_hash     Public key hash
     * @param[in]   handle_fetch    Completion handler for fetch operation.
     * @code
     *  void handle_fetch(
     *      const std::error_code& ec, // Status of operation
     *      // Outputs
     *      const message::output_point_list& outpoints
     *  );
     * @endcode
     */
    virtual void fetch_outputs(const short_hash& pubkey_hash,
        fetch_handler_outputs handle_fetch) = 0;

    /**
     * Be notified of the next blockchain change.
     *
     * Subscriber is notified exactly once of changes to the blockchain
     * and needs to re-subscribe to continue being notified.
     *
     * @param[in]   handle_reorganize   Notification handler for changes
     * @code
     *  void handle_reorganize(
     *      const std::error_code& ec,   // Status of operation
     *      size_t fork_point,           // Index where blockchain forks
     *      const block_list& arrivals,  // New blocks added to blockchain
     *      const block_list& replaced   // Blocks removed (empty if none)
     *  );
     * @endcode
     */
    virtual void subscribe_reorganize(
        reorganize_handler handle_reorganize) = 0;
};

typedef std::function<void (const std::error_code&, const message::block&)>
    blockchain_fetch_handler_block;

void fetch_block(blockchain_ptr chain, size_t depth,
    blockchain_fetch_handler_block handle_fetch);
void fetch_block(blockchain_ptr chain, const hash_digest& block_hash,
    blockchain_fetch_handler_block handle_fetch);

typedef std::function<
    void (const std::error_code&, const message::block_locator&)>
        blockchain_fetch_handler_block_locator;

void fetch_block_locator(blockchain_ptr chain,
    blockchain_fetch_handler_block_locator handle_fetch);

} // namespace libbitcoin

#endif


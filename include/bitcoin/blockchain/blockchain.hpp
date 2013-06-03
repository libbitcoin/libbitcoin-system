#ifndef LIBBITCOIN_BLOCKCHAIN_BLOCKCHAIN_HPP
#define LIBBITCOIN_BLOCKCHAIN_BLOCKCHAIN_HPP

/**
 * Represents an interface to a blockchain backend.
 */

#include <boost/utility.hpp>
#include <functional>

#include <bitcoin/block.hpp>
#include <bitcoin/error.hpp>
#include <bitcoin/primitives.hpp>
#include <bitcoin/address.hpp>

namespace libbitcoin {

class blockchain
{
public:
    typedef std::function<void (const std::error_code&, block_info)>
        store_block_handler;

    typedef std::function<void (const std::error_code&)> import_block_handler;

    template <typename Message>
    using fetch_handler = std::function<
        void (const std::error_code&, const Message&)>;

    typedef fetch_handler<block_type> fetch_handler_block_header;

    typedef fetch_handler<inventory_list>
        fetch_handler_block_transaction_hashes;

    typedef fetch_handler<size_t> fetch_handler_block_depth;

    typedef fetch_handler<size_t> fetch_handler_last_depth;

    typedef fetch_handler<block_locator_type>
        fetch_handler_block_locator;

    typedef fetch_handler<transaction_type> fetch_handler_transaction;

    typedef std::function<
        void (const std::error_code&, size_t, size_t)>
            fetch_handler_transaction_index;

    typedef fetch_handler<input_point> fetch_handler_spend;

    typedef fetch_handler<output_point_list> fetch_handler_outputs;

    typedef std::vector<std::shared_ptr<block_type>> block_list;
    typedef std::function<
        void (const std::error_code&, size_t, 
            const block_list&, const block_list&)> reorganize_handler;

    virtual ~blockchain() {};

    /**
     * Store a new block.
     *
     * Subscriber is notified exactly once of changes to the blockchain
     * and needs to re-subscribe to continue being notified.
     *
     * @param[in]   block           Block to store
     * @param[in]   handle_store    Completion handler for store operation.
     * @code
     *  void handle_store(
     *      const std::error_code& ec,   // Status of operation
     *      block_info info              // Status and depth of block
     *  );
     * @endcode
     */
    virtual void store(const block_type& block,
        store_block_handler handle_store) = 0;

    /**
     * Store a new block directly without validating it.
     * No checks are done. Importing an already stored block
     * is undefined.
     *
     * @param[in]   import_block    Block to store
     * @param[in]   depth           Depth of block
     * @param[in]   handle_import   Completion handler for import operation.
     * @code
     *  void handle_import(
     *      const std::error_code& ec   // Status of operation
     *  );
     * @encode
     */
    virtual void import(const block_type& import_block, size_t depth,
        import_block_handler handle_import) = 0;

    /**
     * Fetches the block header by depth.
     *
     * @param[in]   depth           Depth of block to fetch
     * @param[in]   handle_fetch    Completion handler for fetch operation.
     * @code
     *  void handle_fetch(
     *      const std::error_code& ec,  // Status of operation
     *      const block_type& blk       // Block header
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
     *      const std::error_code& ec,  // Status of operation
     *      const block_type& blk       // Block header
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
     *      const std::error_code& ec,      // Status of operation
     *      const inventory_list& hashes    // List of hashes
     *  );
     * @endcode
     */
    virtual void fetch_block_transaction_hashes(size_t depth,
        fetch_handler_block_transaction_hashes handle_fetch) = 0;

    /**
     * Fetches list of transaction hashes in a block by block hash.
     *
     * @param[in]   block_hash      Hash of block
     * @param[in]   handle_fetch    Completion handler for fetch operation.
     * @code
     *  void handle_fetch(
     *      const std::error_code& ec,      // Status of operation
     *      const inventory_list& hashes    // List of hashes
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
     * Fetches a transaction by hash
     *
     * @param[in]   transaction_hash  Transaction's hash
     * @param[in]   handle_fetch      Completion handler for fetch operation.
     * @code
     *  void handle_fetch(
     *      const std::error_code& ec,  // Status of operation
     *      const transaction_type& tx  // Transaction
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
     *      const std::error_code& ec,      // Status of operation
     *      const input_point& inpoint      // Spend of output
     *  );
     * @endcode
     */
    virtual void fetch_spend(const output_point& outpoint,
        fetch_handler_spend handle_fetch) = 0;

    /**
     * Fetches outputs associated with a bitcoin address.
     *
     * @param[in]   address         Bitcoin address
     * @param[in]   handle_fetch    Completion handler for fetch operation.
     * @code
     *  void handle_fetch(
     *      const std::error_code& ec,          // Status of operation
     *      const output_point_list& outpoints  // Outputs
     *  );
     * @endcode
     */
    virtual void fetch_outputs(const payment_address& address,
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
     *      const block_list& added,     // New blocks added to blockchain
     *      const block_list& removed    // Blocks removed (empty if none)
     *  );
     * @endcode
     */
    virtual void subscribe_reorganize(
        reorganize_handler handle_reorganize) = 0;
};

typedef std::function<void (const std::error_code&, const block_type&)>
    blockchain_fetch_handler_block;

/**
 * Fetch a block by depth.
 *
 * If the blockchain reorganises, operation may fail halfway.
 *
 * @param[in]   chain           Blockchain service
 * @param[in]   depth           Depth of block to fetch.
 * @param[in]   handle_fetch    Completion handler for fetch operation.
 * @code
 *  void handle_fetch(
 *      const std::error_code& ec,  // Status of operation
 *      const block_type& blk       // Block header
 *  );
 * @endcode
 */
void fetch_block(blockchain& chain, size_t depth,
    blockchain_fetch_handler_block handle_fetch);

/**
 * Fetch a block by hash.
 *
 * If the blockchain reorganises, operation may fail halfway.
 *
 * @param[in]   chain           Blockchain service
 * @param[in]   block_hash      Hash of block to fetch.
 * @param[in]   handle_fetch    Completion handler for fetch operation.
 * @code
 *  void handle_fetch(
 *      const std::error_code& ec,  // Status of operation
 *      const block_type& blk       // Block header
 *  );
 * @endcode
 */
void fetch_block(blockchain& chain, const hash_digest& block_hash,
    blockchain_fetch_handler_block handle_fetch);

typedef std::function<
    void (const std::error_code&, const block_locator_type&)>
        blockchain_fetch_handler_block_locator;

/**
 * Creates a block_locator object used to download the blockchain.
 *
 * @param[in]   handle_fetch    Completion handler for fetch operation.
 * @code
 *  void handle_fetch(
 *      const std::error_code& ec,      // Status of operation
 *      const block_locator_type& loc   // Block locator object
 *  );
 * @endcode
 */
void fetch_block_locator(blockchain& chain,
    blockchain_fetch_handler_block_locator handle_fetch);

} // namespace libbitcoin

#endif


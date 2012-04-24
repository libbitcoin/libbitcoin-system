#ifndef LIBBITCOIN_TRANSACTION_POOL_H
#define LIBBITCOIN_TRANSACTION_POOL_H

#include <functional>
#include <boost/circular_buffer.hpp>

#include <bitcoin/async_service.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/messages.hpp>
#include <bitcoin/utility/threads.hpp>
#include <bitcoin/blockchain/blockchain.hpp>

namespace libbitcoin {

struct transaction_entry_info
{
    typedef std::function<void (const std::error_code&)> confirm_handler;
    hash_digest hash;
    message::transaction tx;
    confirm_handler handle_confirm;
};

typedef boost::circular_buffer<transaction_entry_info> pool_buffer;

/**
 * Before bitcoin transactions make it into a block, they go into
 * a transaction memory pool. This class encapsulates that functionality
 * performing the neccessary validation of a transaction before accepting
 * it into its internal buffer.
 *
 * The interface has been deliberately kept simple to minimise overhead.
 * This class attempts no tracking of inputs or spends and only provides
 * a store/fetch paradigm. Tracking must be performed externally and make
 * use of store's handle_store and handle_confirm to manage changes in the
 * state of memory pool transactions.
 *
 * @code
 *  async_service service(1);
 *  // transaction_pool needs access to the blockchain
 *  blockchain_ptr chain = load_our_backend();
 *  // create and initialize the transaction memory pool
 *  transaction_pool_ptr txpool = transaction_pool::create(service, chain);
 * @endcode
 */
class transaction_pool
  : public std::enable_shared_from_this<transaction_pool>
{
public:
    typedef std::function<
        void (const std::error_code&, const index_list&)> store_handler;

    typedef std::function<
        void (const std::error_code&, const message::transaction&)>
            fetch_handler;

    typedef std::function<void (bool)> exists_handler;

    typedef transaction_entry_info::confirm_handler confirm_handler;

    static transaction_pool_ptr create(
        async_service& service, blockchain_ptr chain);

    /// Non-copyable class
    transaction_pool(const transaction_pool&) = delete;
    /// Non-copyable class
    void operator=(const transaction_pool&) = delete;

    /**
     * Attempt to store a transaction.
     *
     * handle_store is called on completion. The second argument is a list
     * of unconfirmed input indexes. These inputs refer to a transaction
     * that is not in the blockchain and is currently in the memory pool.
     *
     * @param[in]   stored_transaction  Transaction to store
     * @param[in]   handle_confirm      Handler for when transaction
     *                                  becomes confirmed.
     * @code
     *  void handle_confirm(
     *      const std::error_code& ec    // Status of operation
     *  );
     * @endcode
     * @param[in]   handle_store        Completion handler for
     *                                  store operation.
     * @code
     *  void handle_store(
     *      const std::error_code& ec,      // Status of operation
     *      const index_list& unconfirmed   // Unconfirmed input indexes
     *  );
     * @endcode
     */
    void store(const message::transaction& stored_transaction,
        confirm_handler handle_confirm, store_handler handle_store);

    /**
     * Fetch transaction by hash.
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
    void fetch(const hash_digest& transaction_hash,
        fetch_handler handle_fetch);

    /**
     * Is this transaction in the pool?
     *
     * @param[in]   transaction_hash  Transaction's hash
     * @param[in]   handle_exists     Completion handler for exists operation.
     * @code
     *  void handle_exists(bool);
     * @endcode
     */
    void exists(const hash_digest& transaction_hash,
        exists_handler handle_exists);

private:
    transaction_pool(async_service& service);
    void initialize(blockchain_ptr chain);

    void do_store(const message::transaction& stored_transaction,
        confirm_handler handle_confirm, store_handler handle_store);
    void handle_delegate(
        const std::error_code& ec, const index_list& unconfirmed,
        const transaction_entry_info& tx_entry, store_handler handle_store);

    bool tx_exists(const hash_digest& tx_hash);

    void reorganize(const std::error_code& ec,
        size_t fork_point,
        const blockchain::block_list& new_blocks,
        const blockchain::block_list& replaced_blocks);
    void resubmit_all();
    void takeout_confirmed(const blockchain::block_list& new_blocks);
    void try_delete(const hash_digest& tx_hash);

    io_service::strand strand_;
    blockchain_ptr chain_;
    pool_buffer pool_;
};

} // namespace libbitcoin

#endif


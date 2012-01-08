#ifndef LIBBITCOIN_BLOCKCHAIN_POSTGRESQL_BLOCKCHAIN_H
#define LIBBITCOIN_BLOCKCHAIN_POSTGRESQL_BLOCKCHAIN_H

#include <bitcoin/blockchain/blockchain.hpp>

#include <cppdb/frontend.h>
#include <string>
#include <mutex>

#include <bitcoin/utility/threads.hpp>

namespace libbitcoin {

class pq_blockchain;
typedef shared_ptr<pq_blockchain> pq_blockchain_ptr;

class postgresql_blockchain
  : public blockchain, public threaded_service,
    public std::enable_shared_from_this<postgresql_blockchain>
{
public:
    postgresql_blockchain(
        std::string database, std::string user, std::string password);

    // Non-copyable
    postgresql_blockchain(const postgresql_blockchain&) = delete;
    void operator=(const postgresql_blockchain&) = delete;

    void store(const message::block& block, store_block_handler handle_store);

    void fetch_block(size_t depth, fetch_handler_block handle_fetch);
    void fetch_block(const hash_digest& block_hash,
        fetch_handler_block handle_fetch);
    void fetch_last_depth(fetch_handler_last_depth handle_fetch);
    void fetch_block_locator(fetch_handler_block_locator handle_fetch);
    void fetch_transaction(const hash_digest& transaction_hash,
        fetch_handler_transaction handle_fetch);
    void fetch_output(const message::output_point& outpoint,
        fetch_handler_output handle_fetch);
    void fetch_spend(const message::output_point& outpoint,
        fetch_handler_spend handle_fetch);
    void fetch_balance(const short_hash& pubkey_hash,
        fetch_handler_balance handle_fetch);

private:
    void do_store_block(const message::block& block,
            store_block_handler handle_store);

    void do_fetch_block_locator(fetch_handler_block_locator handle_fetch);
    void do_fetch_balance(const short_hash& pubkey_hash,
        fetch_handler_balance handle_fetch);

    // ------------

    size_t insert(const message::transaction_input& input,
            size_t transaction_id, size_t index_in_parent);
    size_t insert(const message::transaction_output& output,
            size_t transaction_id, size_t index_in_parent);
    size_t insert(const message::transaction& transaction,
        std::vector<size_t>& input_ids, std::vector<size_t>& output_ids);

    pq_blockchain_ptr blockchain_;
    cppdb::session sql_;
};

} // libbitcoin

#endif


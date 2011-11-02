#ifndef LIBBITCOIN_STORAGE_POSTGRESQL_STORAGE_H
#define LIBBITCOIN_STORAGE_POSTGRESQL_STORAGE_H

#include <bitcoin/storage/storage.hpp>

#include <cppdb/frontend.h>
#include <string>
#include <mutex>

#include <bitcoin/util/threaded_service.hpp>

namespace libbitcoin {

class pq_blockchain;
typedef shared_ptr<pq_blockchain> pq_blockchain_ptr;

class postgresql_storage
  : public storage,
    public threaded_service,
    public std::enable_shared_from_this<postgresql_storage>
{
public:
    postgresql_storage(kernel_ptr kernel,
        std::string database, std::string user, std::string password);

    void store(const message::block& block, store_block_handler handle_store);

    void fetch_block_locator(fetch_handler_block_locator handle_fetch);
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


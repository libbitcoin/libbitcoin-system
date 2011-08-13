#ifndef LIBBITCOIN_STORAGE_POSTGRESQL_STORAGE_H
#define LIBBITCOIN_STORAGE_POSTGRESQL_STORAGE_H

#include <bitcoin/storage/storage.hpp>

#include <cppdb/frontend.h>
#include <string>
#include <mutex>

namespace libbitcoin {

class postgresql_storage
  : public storage
{
public:
    postgresql_storage(std::string database, 
            std::string user, std::string password);

    void store(message::inv inv, store_handler handle_store);
    void store(message::transaction transaction, store_handler handle_store);
    void store(message::block block, store_handler handle_store);

    void fetch_inventories(fetch_handler_inventories handle_fetch);
    void fetch_block_by_depth(size_t block_number, 
            fetch_handler_block handle_fetch);
    void fetch_block_by_hash(hash_digest block_hash, 
            fetch_handler_block handle_fetch);
    void fetch_block_locator(fetch_handler_block_locator handle_fetch);
    void fetch_output_by_hash(hash_digest transaction_hash, uint32_t index,
            fetch_handler_output handle_fetch);

    void organize_block_chain();

private:
    void insert(operation oper, size_t script_id);
    size_t insert_script(operation_stack operations);
    void insert(message::transaction_input input,
            size_t transaction_id, size_t index_in_parent);
    void insert(message::transaction_output output,
            size_t transaction_id, size_t index_in_parent);
    size_t insert(message::transaction transaction);

    message::transaction_input_list select_inputs(size_t transaction_id);
    message::transaction_output_list select_outputs(size_t transaction_id);
    script select_script(size_t script_id);

    message::transaction_list read_transactions(cppdb::result result);
    message::block read_block(cppdb::result block_result);

    void matchup_inputs();

    cppdb::session sql_;
    std::mutex mutex_;
};

} // libbitcoin

#endif


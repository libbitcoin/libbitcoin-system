#ifndef LIBBITCOIN_STORAGE_POSTGRESQL_STORAGE_H
#define LIBBITCOIN_STORAGE_POSTGRESQL_STORAGE_H

#include <bitcoin/storage/storage.hpp>

#include <cppdb/frontend.h>
#include <string>

namespace libbitcoin {
namespace storage {

class postgresql_storage : public storage
{
public:
    postgresql_storage(std::string database, std::string user);

    void store(net::message::inv inv, operation_handler handle_store);
    void store(net::message::transaction transaction, 
            operation_handler handle_store);
    void store(net::message::block block, operation_handler handle_store);

    void fetch_inventories(fetch_handler_inventories handle_fetch);
    void fetch_block_number(size_t block_number, 
            fetch_handler_block handle_fetch);

    void organize_blockchain();

private:
    void insert(operation oper, size_t script_id);
    size_t insert_script(operation_stack operations);
    void insert(net::message::transaction_input input,
            size_t transaction_id, size_t index_in_parent);
    void insert(net::message::transaction_output output,
            size_t transaction_id, size_t index_in_parent);
    size_t insert(net::message::transaction transaction);

    net::message::transaction_input_list select_inputs(
            size_t transaction_id);
    net::message::transaction_output_list select_outputs(
            size_t transaction_id);
    script select_script(size_t script_id);

    net::message::transaction_list read_transactions(cppdb::result result);

    cppdb::session sql_;
};

} // storage
} // libbitcoin

#endif


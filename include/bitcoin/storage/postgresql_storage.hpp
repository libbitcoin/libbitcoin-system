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

    void push(net::message::inv inv);
    void push(net::message::transaction transaction);
    void push(net::message::block block);
    void request_inventories(accept_inventories_handler handler);
private:
    void insert(operation oper, size_t script_id);
    size_t insert_script(operation_stack operations);
    void insert(net::message::transaction_input input,
            size_t transaction_id, size_t index_in_parent);
    void insert(net::message::transaction_output output,
            size_t transaction_id, size_t index_in_parent);
    size_t insert(net::message::transaction transaction);

    cppdb::session sql_;
};

} // storage
} // libbitcoin

#endif


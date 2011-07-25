#ifndef LIBBITCOIN_STORAGE_MEMORY_STORAGE_H
#define LIBBITCOIN_STORAGE_MEMORY_STORAGE_H

#include <boost/asio.hpp>
#include <memory>
#include <thread>

#include <bitcoin/storage/storage.hpp>

namespace libbitcoin {
namespace storage {

using boost::asio::io_service;
using std::shared_ptr;

class memory_storage : public storage
{
public:
    memory_storage();

    void store(message::inv inv, operation_handler handle_store);
    void store(message::transaction transaction, 
            operation_handler handle_store);
    void store(message::block block, operation_handler handle_store);

    void fetch_inventories(fetch_handler_inventories handle_fetch);
    void fetch_block_number(size_t block_number, 
            fetch_handler_block handle_fetch);

private:
    service_ptr service_;
    std::thread runner_;
    work_ptr work_;
    strand_ptr strand_;

    // The data
    void do_push_inv(message::inv item);
    void do_push_block(message::block item);
    void do_request_inventories(fetch_handler_inventories handle_fetch);
    message::inv_list inventories_;
};

} // storage
} // libbitcoin

#endif


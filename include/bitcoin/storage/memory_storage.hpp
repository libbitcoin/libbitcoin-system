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

    void push(net::message::inv item);
    void push(net::message::transaction item);
    void push(net::message::block item);
    void request_inventories(accept_inventories_handler handler);

private:
    service_ptr service_;
    std::thread runner_;
    work_ptr work_;
    strand_ptr strand_;

    // The data
    void do_push_inv(net::message::inv item);
    void do_push_block(net::message::block item);
    void do_request_inventories(accept_inventories_handler handler);
    net::message::inv_list inventories_;
};

} // storage
} // libbitcoin

#endif


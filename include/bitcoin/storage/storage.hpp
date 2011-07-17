#ifndef LIBBITCOIN_STORAGE_STORAGE_H
#define LIBBITCOIN_STORAGE_STORAGE_H

#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <memory>
#include <thread>

#include <bitcoin/kernel.hpp>
#include <bitcoin/net/messages.hpp>

namespace libbitcoin {
namespace storage {

using boost::asio::io_service;
using std::shared_ptr;

class storage : boost::noncopyable
{
public:
    typedef std::function<void (net::message::inv_list)>
            accept_inventories_handler;
    virtual void push(net::message::inv item) = 0;
    virtual void push(net::message::block item) = 0;
    virtual void request_inventories(accept_inventories_handler handler) = 0;
};

class memory_storage : public storage
{
public:
    memory_storage(kernel_ptr kern);
    kernel_ptr kernel() const;

    void push(net::message::inv item);
    void push(net::message::block item);
    void request_inventories(accept_inventories_handler handler);

private:
    kernel_ptr kernel_;
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


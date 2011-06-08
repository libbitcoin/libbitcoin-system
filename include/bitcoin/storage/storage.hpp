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
    virtual void push(net::message::inv item) = 0;
};

class memory_storage : public storage
{
public:
    memory_storage(kernel_ptr kern);
    kernel_ptr kernel() const;

    void push(net::message::inv item);

private:
    typedef shared_ptr<io_service> service_ptr;
    typedef shared_ptr<io_service::work> work_ptr;
    typedef shared_ptr<io_service::strand> strand_ptr;

    kernel_ptr kernel_;
    service_ptr service_;
    std::thread runner_;
    work_ptr work_;
    strand_ptr strand_;

    // The data
    std::vector<net::message::inv_vect> inv_list;
};

} // storage
} // libbitcoin

#endif


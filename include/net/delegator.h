#ifndef LIBBITCOIN_NET_DELEGATOR_H
#define LIBBITCOIN_NET_DELEGATOR_H

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <thread>

#include "net/peer.h"

namespace libbitcoin {
namespace net {

using boost::asio::io_service;
using std::shared_ptr;

class delegator : boost::noncopyable
{
public:
    virtual void init() = 0;
    virtual bool connect(std::string ip_addr, unsigned short port=8333) = 0;

    static void run_service(shared_ptr<io_service> io_service);
};

class delegator_default : public delegator
{
public:
    ~delegator_default();
    void init();
    bool connect(std::string ip_addr, unsigned short port=8333);
private:
    shared_ptr<io_service> service;
    std::thread runner;
    shared_ptr<io_service::work> work;

    shared_ptr<dialect> original_dialect;
    boost::ptr_vector<peer> peers;
};

} // net
} // libbitcoin

#endif


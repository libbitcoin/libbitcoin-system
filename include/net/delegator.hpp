#ifndef LIBBITCOIN_NET_DELEGATOR_H
#define LIBBITCOIN_NET_DELEGATOR_H

#include <boost/asio.hpp>
#include <memory>
#include <thread>

#include "net/peer.hpp"

namespace libbitcoin {
namespace net {

using boost::asio::io_service;
using std::shared_ptr;

class delegator : boost::noncopyable
{
public:
    virtual void init() = 0;
    virtual shared_ptr<peer> connect(std::string ip_addr, 
            unsigned short port=8333) = 0;
};

class delegator_default : public delegator
{
public:
    ~delegator_default();
    void init();
    shared_ptr<peer> connect(std::string ip_addr, 
            unsigned short port=8333);
private:
    shared_ptr<io_service> service_;
    std::thread runner_;
    shared_ptr<io_service::work> work_;

    shared_ptr<dialect> default_dialect_;
    std::vector<shared_ptr<peer>> peers_;
};

} // net
} // libbitcoin

#endif


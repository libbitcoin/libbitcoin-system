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

class delegator;
typedef shared_ptr<delegator> delegator_ptr;

class delegator : boost::noncopyable
{
public:
    virtual void init() = 0;
    virtual peer_ptr connect(std::string ip_addr, 
            unsigned short port=8333) = 0;
    virtual void disconnect(peer_ptr peer_obj) = 0;
};

class delegator_default : public delegator, 
    public std::enable_shared_from_this<delegator_default>
{
public:
    ~delegator_default();
    void init();
    peer_ptr connect(std::string ip_addr, 
            unsigned short port=8333);
    void disconnect(peer_ptr peer_obj);  

private:
    void add_peer(peer_ptr peer_obj);
    void perform_disconnect(peer_ptr peer_obj);

    shared_ptr<io_service> service_;
    std::thread runner_;
    shared_ptr<io_service::work> work_;
    shared_ptr<io_service::strand> strand_;

    shared_ptr<dialect> default_dialect_;
    std::vector<peer_ptr> peers_;
};

} // net
} // libbitcoin

#endif


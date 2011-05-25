#ifndef LIBBITCOIN_NET_DELEGATOR_H
#define LIBBITCOIN_NET_DELEGATOR_H

#include <boost/asio.hpp>
#include <memory>
#include <thread>

#include "net/dialect.hpp"
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
    virtual peer_ptr connect(std::string ip_addr, unsigned short port=8333) = 0;
    virtual void disconnect(peer_ptr peer_obj) = 0;
};

class delegator_default : public delegator, 
    public std::enable_shared_from_this<delegator_default>
{
public:
    delegator_default();
    ~delegator_default();
    void init();
    peer_ptr connect(std::string ip_addr, unsigned short port=8333);
    void disconnect(peer_ptr peer_obj);  

private:
    typedef shared_ptr<tcp::socket> socket_ptr;
    typedef shared_ptr<io_service> service_ptr;
    typedef shared_ptr<tcp::acceptor> acceptor_ptr;

    bool start_accept();
    void handle_accept(socket_ptr socket);
    
    peer_ptr create_peer(socket_ptr socket);
    void add_peer(peer_ptr peer_obj);
    void perform_disconnect(peer_ptr peer_obj);

    service_ptr service_;
    std::thread runner_;
    shared_ptr<io_service::work> work_;
    shared_ptr<io_service::strand> strand_;
    acceptor_ptr acceptor_;

    dialect_ptr default_dialect_;
    std::vector<peer_ptr> peers_;
};

} // net
} // libbitcoin

#endif


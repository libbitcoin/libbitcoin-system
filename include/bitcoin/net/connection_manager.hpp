#ifndef LIBBITCOIN_NET_DELEGATOR_H
#define LIBBITCOIN_NET_DELEGATOR_H

#include <boost/asio.hpp>
#include <memory>
#include <thread>

#include "bitcoin/net/dialect.hpp"
#include "bitcoin/net/channel.hpp"

namespace libbitcoin {
namespace net {

using boost::asio::io_service;
using std::shared_ptr;

class connection_manager;
typedef shared_ptr<connection_manager> connection_manager_ptr;

enum connection_flags : uint32_t
{
    none = 0,
    accept_incoming = 1
};

class connection_manager : boost::noncopyable
{
public:
    virtual channel_ptr connect(std::string ip_addr, 
            unsigned short port=8333) = 0;
    virtual void disconnect(channel_ptr channel_obj) = 0;
};

class default_connection_manager : public connection_manager, 
    public std::enable_shared_from_this<default_connection_manager>
{
public:
    default_connection_manager(uint32_t flags);
    ~default_connection_manager();
    channel_ptr connect(std::string ip_addr, unsigned short port=8333);
    void disconnect(channel_ptr channel_obj);  

private:
    typedef shared_ptr<tcp::socket> socket_ptr;
    typedef shared_ptr<io_service> service_ptr;
    typedef shared_ptr<tcp::acceptor> acceptor_ptr;

    bool start_accept();
    void handle_accept(socket_ptr socket);
    
    channel_ptr create_channel(socket_ptr socket);

    service_ptr service_;
    std::thread runner_;
    shared_ptr<io_service::work> work_;
    shared_ptr<io_service::strand> strand_;
    acceptor_ptr acceptor_;

    dialect_ptr default_dialect_;
    std::vector<channel_ptr> channels_;
};

} // net
} // libbitcoin

#endif


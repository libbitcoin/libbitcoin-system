#ifndef LIBBITCOIN_NET_DELEGATOR_H
#define LIBBITCOIN_NET_DELEGATOR_H

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <thread>

#include "bitcoin/net/messages.hpp"

namespace libbitcoin {
namespace net {

using boost::asio::ip::tcp;
using boost::asio::io_service;
using std::shared_ptr;

class delegator;
class channel;
class dialect;

typedef shared_ptr<delegator> delegator_ptr;
typedef std::shared_ptr<dialect> dialect_ptr;
typedef unsigned int channel_handle;
typedef boost::ptr_vector<channel> channel_list;

enum network_flags : uint32_t
{
    none = 0,
    accept_incoming = 1
};

class delegator : boost::noncopyable
{
public:
    virtual channel_handle connect(std::string ip_addr, 
            unsigned short port=8333) = 0;
    virtual void disconnect(channel_handle handle) = 0;
    virtual void send(channel_handle chandle, message::version version) = 0;
    virtual size_t connection_count() const = 0;
};

class delegator_impl : public delegator, 
    public std::enable_shared_from_this<delegator_impl>
{
public:
    delegator_impl(uint32_t flags);
    ~delegator_impl();
    channel_handle connect(std::string ip_addr, unsigned short port=8333);
    void disconnect(channel_handle chandle);  
    void send(channel_handle chandle, message::version version);
    size_t connection_count() const;

private:
    typedef shared_ptr<tcp::socket> socket_ptr;
    typedef shared_ptr<io_service> service_ptr;
    typedef shared_ptr<tcp::acceptor> acceptor_ptr;

    bool start_accept();
    void handle_accept(socket_ptr socket);
    
    channel_handle create_channel(socket_ptr socket);

    service_ptr service_;
    std::thread runner_;
    shared_ptr<io_service::work> work_;
    shared_ptr<io_service::strand> strand_;
    acceptor_ptr acceptor_;

    dialect_ptr default_dialect_;
    channel_list channels_;
};

} // net
} // libbitcoin

#endif


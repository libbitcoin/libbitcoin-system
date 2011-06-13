#ifndef LIBBITCOIN_NET_NETWORK_H
#define LIBBITCOIN_NET_NETWORK_H

#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <memory>
#include <thread>

#include <bitcoin/kernel.hpp>
#include <bitcoin/net/messages.hpp>
#include <bitcoin/net/types.hpp>

namespace libbitcoin {
namespace net {

using boost::asio::ip::tcp;
using boost::asio::io_service;

enum network_flags : uint32_t
{
    none = 0,
    accept_incoming = 1
};

class network : boost::noncopyable
{
public:
    virtual kernel_ptr kernel() const = 0;
    virtual bool start_accept() = 0;
    virtual channel_handle connect(bool& ec, std::string ip_addr, 
            unsigned short port=8333) = 0;
    virtual size_t connection_count() const = 0;
    virtual void disconnect(channel_handle handle) = 0;
    virtual void send(channel_handle chandle, message::version version) = 0;
    virtual void send(channel_handle chandle, message::verack verack) = 0;
    virtual void send(channel_handle chandle, message::getaddr getaddr) = 0;
};

class network_impl : public network, 
    public std::enable_shared_from_this<network_impl>
{
public:
    network_impl(kernel_ptr kern);
    ~network_impl();
    kernel_ptr kernel() const;
    bool start_accept();
    channel_handle connect(bool& ec, std::string ip_addr, 
            unsigned short port=8333);
    size_t connection_count() const;
    void disconnect(channel_handle chandle);  
    void send(channel_handle chandle, message::version version);
    void send(channel_handle chandle, message::verack verack);
    void send(channel_handle chandle, message::getaddr getaddr);

private:
    typedef shared_ptr<tcp::socket> socket_ptr;
    typedef shared_ptr<tcp::acceptor> acceptor_ptr;

    void handle_accept(socket_ptr socket);
    
    channel_handle create_channel(socket_ptr socket);

    kernel_ptr kernel_;
    service_ptr service_;
    std::thread runner_;
    work_ptr work_;
    strand_ptr strand_;
    acceptor_ptr acceptor_;

    dialect_ptr default_dialect_;
    channel_list channels_;
};

} // net
} // libbitcoin

#endif


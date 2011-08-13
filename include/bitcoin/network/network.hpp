#ifndef LIBBITCOIN_NET_NETWORK_H
#define LIBBITCOIN_NET_NETWORK_H

#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <memory>
#include <thread>

#include <bitcoin/kernel.hpp>
#include <bitcoin/messages.hpp>
#include <bitcoin/network/types.hpp>
#include <bitcoin/error.hpp>

namespace libbitcoin {

using boost::asio::ip::tcp;
using boost::asio::io_service;

class network
  : private boost::noncopyable
{
public:
    typedef std::function<
            void (std::error_code, channel_handle chandle)> connect_handler;
    typedef std::function<void (channel_handle)> accept_random_handle;

    virtual kernel_ptr kernel() const = 0;
    virtual bool start_accept() = 0;
    virtual void connect(std::string ip_addr, unsigned short port,
            connect_handler handle_connect) = 0;
    virtual size_t connection_count() const = 0;
    virtual void get_random_handle(accept_random_handle accept_handler) = 0;
    virtual void disconnect(channel_handle handle) = 0;
    virtual void send(channel_handle chandle, message::version version) = 0;
    virtual void send(channel_handle chandle, message::verack verack) = 0;
    virtual void send(channel_handle chandle, message::getaddr getaddr) = 0;
    virtual void send(channel_handle chandle, message::getdata getdata) = 0;
    virtual void send(channel_handle chandle, message::getblocks getblocks) = 0;
};

class network_impl
  : public network, 
    public threaded_service,
    public std::enable_shared_from_this<network_impl>
{
public:
    network_impl(kernel_ptr kern);
    ~network_impl();
    kernel_ptr kernel() const;
    bool start_accept();
    void connect(std::string ip_addr, unsigned short port, 
            connect_handler handle_connect);
    size_t connection_count() const;
    void get_random_handle(accept_random_handle accept_handler);
    void disconnect(channel_handle chandle);  
    void send(channel_handle chandle, message::version version);
    void send(channel_handle chandle, message::verack verack);
    void send(channel_handle chandle, message::getaddr getaddr);
    void send(channel_handle chandle, message::getdata getdata);
    void send(channel_handle chandle, message::getblocks getblocks);

private:
    typedef shared_ptr<tcp::socket> socket_ptr;
    typedef shared_ptr<tcp::acceptor> acceptor_ptr;

    void do_get_random_handle(accept_random_handle accept_handler);
    void handle_connect(const boost::system::error_code& ec, 
            socket_ptr socket, std::string ip_addr, 
            connect_handler handle_connect);
    void handle_accept(socket_ptr socket);
    
    channel_handle create_channel(socket_ptr socket);

    kernel_ptr kernel_;
    acceptor_ptr acceptor_;

    dialect_ptr default_dialect_;
    channel_list channels_;
};

} // libbitcoin

#endif


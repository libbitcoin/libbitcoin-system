#ifndef LIBBITCOIN_NETWORK_NETWORK_H
#define LIBBITCOIN_NETWORK_NETWORK_H

#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <memory>
#include <thread>

#include <bitcoin/messages.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/error.hpp>
#include <bitcoin/utility/threads.hpp>

#include <bitcoin/network/channel.hpp>

namespace libbitcoin {

class acceptor
  : public std::enable_shared_from_this<acceptor>
{
public:
    typedef std::shared_ptr<tcp::acceptor> tcp_acceptor_ptr;

    typedef std::function<
        void (const std::error_code&, channel_ptr)> accept_handler;

    acceptor(thread_core_ptr threaded,
        tcp_acceptor_ptr tcp_accept, exporter_ptr saver);
    void accept(accept_handler handle_accept);

private:
    void call_handle_accept(const boost::system::error_code& ec,
        socket_ptr socket, accept_handler handle_accept);

    thread_core_ptr threaded_;
    tcp_acceptor_ptr tcp_accept_;
    exporter_ptr export_;
};

class network
  : public std::enable_shared_from_this<network>
{
public:
    typedef std::function<
        void (const std::error_code&, channel_ptr)> connect_handler;
    typedef std::function<
        void (const std::error_code&, acceptor_ptr)> listen_handler;

    network();

    network(const network&) = delete;
    void operator=(const network&) = delete;

    void listen(uint16_t port, listen_handler handle_listen);
    void connect(const std::string& hostname, uint16_t port, 
        connect_handler handle_connect);

private:
    void resolve_handler(const boost::system::error_code& ec,
        tcp::resolver::iterator endpoint_iterator,
        connect_handler handle_connect, resolver_ptr, query_ptr);
    void call_connect_handler(const boost::system::error_code& ec, 
        tcp::resolver::iterator, socket_ptr socket,
        connect_handler handle_connect);

    thread_core_ptr threaded_;
    strand_ptr strand_;
    exporter_ptr export_;
};

} // libbitcoin

#endif


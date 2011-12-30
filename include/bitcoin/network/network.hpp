#ifndef LIBBITCOIN_NET_NETWORK_H
#define LIBBITCOIN_NET_NETWORK_H

#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <memory>
#include <thread>

#include <bitcoin/messages.hpp>
#include <bitcoin/network/types.hpp>
#include <bitcoin/error.hpp>
#include <bitcoin/util/threads.hpp>

#include <bitcoin/network/channel.hpp>

namespace libbitcoin {

class network
 : public std::enable_shared_from_this<network>
{
public:
    typedef std::function<void (
        const std::error_code&, channel_ptr)> connect_handler;

    network();
    ~network();

    network(const network&) = delete;
    void operator=(const network&) = delete;

    bool accept();
    void connect(const std::string& hostname, uint16_t port, 
        connect_handler handle_connect);
    void listen(connect_handler handle_connect);

    // List of bitcoin messages
    // version
    // verack
    // addr
    // inv
    // getdata
    // getblocks
    // getheaders
    // tx
    // block
    // headers
    // getaddr
    // checkorder [deprecated]
    // submitorder [deprecated]
    // reply [deprecated]
    // ping
    // alert

private:
    typedef std::shared_ptr<tcp::resolver> resolver_ptr;
    typedef std::shared_ptr<tcp::resolver::query> query_ptr;

    void resolve_handler(const boost::system::error_code& ec,
        tcp::resolver::iterator endpoint_iterator,
        connect_handler handle_connect, resolver_ptr, query_ptr);
    void handle_connect(const boost::system::error_code& ec, 
        tcp::resolver::iterator endpoint_iterator,
        socket_ptr socket, connect_handler handle_connect);
    void handle_accept(socket_ptr socket);

    acceptor_ptr acceptor_;
    std::vector<connect_handler> listeners_;

    thread_core_ptr threaded_;
    strand_ptr strand_;
    dialect_ptr default_dialect_;
};

typedef std::function<void (const std::error_code&)> handshake_handler;

void handshake(channel_ptr node, handshake_handler handle_handshake);

void handshake_connect(network_ptr net, const std::string& hostname,
    uint16_t port, network::connect_handler handle_connect);

} // libbitcoin

#endif


#include <bitcoin/network/network.hpp>

#include <boost/lexical_cast.hpp>
#include <functional>
#include <algorithm>
#include <iostream>

#include <bitcoin/utility/logger.hpp>
#include <bitcoin/exporter.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

acceptor::acceptor(thread_core_ptr threaded, uint16_t port)
  : threaded_(threaded), tcp_acceptor_(*threaded->service())
{
    tcp::endpoint endpoint(tcp::v4(), port);
    tcp_acceptor_.open(endpoint.protocol());
    tcp_acceptor_.set_option(tcp::acceptor::reuse_address(true));
    tcp_acceptor_.bind(endpoint);
    tcp_acceptor_.listen(boost::asio::socket_base::max_connections);
    export_ = std::make_shared<satoshi_exporter>();
}
void acceptor::accept(accept_handler handle_accept)
{
    socket_ptr socket =
        std::make_shared<tcp::socket>(*threaded_->service());
    tcp_acceptor_.async_accept(*socket,
        std::bind(&acceptor::call_handle_accept, shared_from_this(), 
            _1, socket, handle_accept));
}
void acceptor::call_handle_accept(const boost::system::error_code& ec,
    socket_ptr socket, accept_handler handle_accept)
{
    if (ec)
    {
        handle_accept(error::accept_failed, nullptr);
        return;
    }
    channel_ptr channel_object =
        std::make_shared<channel>(socket, threaded_, export_);
    channel_object->start();
    handle_accept(std::error_code(), channel_object);
}

network::network()
{
    threaded_ = std::make_shared<thread_core>();
    strand_ = threaded_->create_strand();
    export_ = std::make_shared<satoshi_exporter>();
}

void network::resolve_handler(const boost::system::error_code& ec,
    tcp::resolver::iterator endpoint_iterator,
    connect_handler handle_connect, resolver_ptr, query_ptr)
{
    if (ec)
    {
        handle_connect(error::resolve_failed, nullptr);
        return;
    }
    socket_ptr socket = std::make_shared<tcp::socket>(*threaded_->service());
    boost::asio::async_connect(*socket, endpoint_iterator,
        std::bind(&network::call_connect_handler, shared_from_this(), 
            _1, _2, socket, handle_connect));
}

void network::call_connect_handler(const boost::system::error_code& ec, 
    tcp::resolver::iterator, socket_ptr socket,
    connect_handler handle_connect)
{
    if (ec)
    {
        handle_connect(error::network_unreachable, nullptr);
        return;
    }
    channel_ptr channel_object =
        std::make_shared<channel>(socket, threaded_, export_);
    channel_object->start();
    handle_connect(std::error_code(), channel_object);
}

void network::connect(const std::string& hostname, uint16_t port,
    connect_handler handle_connect)
{
    resolver_ptr resolver =
        std::make_shared<tcp::resolver>(*threaded_->service());
    query_ptr query =
        std::make_shared<tcp::resolver::query>(hostname,
            boost::lexical_cast<std::string>(port));
    resolver->async_resolve(*query,
        std::bind(&network::resolve_handler, shared_from_this(),
            _1, _2, handle_connect, resolver, query));
}

void network::listen(uint16_t port, listen_handler handle_listen)
{
    acceptor_ptr accept = std::make_shared<acceptor>(threaded_, port);
    handle_listen(std::error_code(), accept);
}

//void network::handle_accept(const boost::system::error_code& ec,
//    socket_ptr socket, acceptor_ptr acceptor,
//    accept_handler handle_accept)
//{
//    tcp::endpoint remote_endpoint = socket->remote_endpoint();
//    log_debug() << "New incoming connection from "
//            << remote_endpoint.address().to_string();
//    //channel_handle chanid = create_channel(socket);
//    //strand()->post(
//    //    [&listeners_, chanid]
//    //    {
//    //        for (connect_handler handle_connect: listeners_)
//    //            handle_connect(std::error_code(), chanid);
//    //        listeners_.clear();
//    //    });
//    //socket.reset(new tcp::socket(*service()));
//    //acceptor_->async_accept(*socket,
//    //        std::bind(&network::handle_accept, shared_from_this(), 
//    //            socket));
//}

} // libbitcoin


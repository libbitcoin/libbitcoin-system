#include <bitcoin/network/network.hpp>

#include <boost/lexical_cast.hpp>
#include <functional>
#include <algorithm>
#include <iostream>

#include <bitcoin/utility/logger.hpp>
#include <bitcoin/dialect.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

network::network()
{
    threaded_ = std::make_shared<thread_core>();
    strand_ = threaded_->create_strand();
    default_dialect_ = std::make_shared<original_dialect>();
}

network::~network()
{
    if (acceptor_)
        acceptor_->close();
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
        std::bind(&network::handle_connect, shared_from_this(), 
            _1, _2, socket, handle_connect));
}

void network::handle_connect(const boost::system::error_code& ec, 
    tcp::resolver::iterator endpoint_iterator,
    socket_ptr socket, connect_handler handle_connect)
{
    if (ec)
    {
        handle_connect(error::network_unreachable, nullptr);
        return;
    }
    channel_ptr channel_object =
        std::make_shared<channel>(socket, threaded_, default_dialect_);
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

void network::listen(connect_handler handle_connect)
{
    strand_->post(
        [&listeners_, handle_connect]
        {
            listeners_.push_back(handle_connect);
        });
}

bool network::accept()
{
    // TODO this should be 1) using std::make_shared 2) be asynchronous
    acceptor_.reset(new tcp::acceptor(*threaded_->service()));
    socket_ptr socket(new tcp::socket(*threaded_->service()));
    try
    {
        tcp::endpoint endpoint(tcp::v4(), 8333);
        acceptor_->open(endpoint.protocol());
        acceptor_->set_option(tcp::acceptor::reuse_address(true));
        acceptor_->bind(endpoint);
        acceptor_->listen(boost::asio::socket_base::max_connections);
        acceptor_->async_accept(*socket,
            std::bind(&network::handle_accept, shared_from_this(), 
                socket));
    }
    catch (std::exception& ex)
    {
        log_error() << "Accepting connections: " << ex.what();
        return false;
    }
    return true;
}

void network::handle_accept(socket_ptr socket)
{
    tcp::endpoint remote_endpoint = socket->remote_endpoint();
    log_debug() << "New incoming connection from "
            << remote_endpoint.address().to_string();
    //channel_handle chanid = create_channel(socket);
    //strand()->post(
    //    [&listeners_, chanid]
    //    {
    //        for (connect_handler handle_connect: listeners_)
    //            handle_connect(std::error_code(), chanid);
    //        listeners_.clear();
    //    });
    //socket.reset(new tcp::socket(*service()));
    //acceptor_->async_accept(*socket,
    //        std::bind(&network::handle_accept, shared_from_this(), 
    //            socket));
}

} // libbitcoin


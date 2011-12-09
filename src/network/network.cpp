#include <bitcoin/network/network.hpp>

#include <boost/lexical_cast.hpp>
#include <functional>
#include <algorithm>
#include <iostream>

#include <bitcoin/util/logger.hpp>
#include <bitcoin/dialect.hpp>

using std::placeholders::_1;
using std::placeholders::_2;

namespace libbitcoin {

using boost::asio::socket_base;

network::network()
{
    default_dialect_.reset(new original_dialect);
}

network::~network()
{
    if (acceptor_)
        acceptor_->close();
}

void network::handle_connect(const boost::system::error_code& ec, 
    socket_ptr socket, std::string hostname, connect_handler handle_connect)
{
    if (ec)
    {
        log_error() << "Connecting to peer " << hostname
            << ": " << ec.message();
        handle_connect(error::system_network_error, nullptr);
        return;
    }
    channel_ptr channel_obj(new channel(socket, service(), default_dialect_));
    channel_obj->start();
    handle_connect(std::error_code(), channel_obj);
}

void network::connect(const std::string& hostname, uint16_t port,
    connect_handler handle_connect)
{
    socket_ptr socket(new tcp::socket(*service()));
    tcp::resolver resolver(*service());
    tcp::resolver::query query(hostname,
        boost::lexical_cast<std::string>(port));
    tcp::endpoint endpoint = *resolver.resolve(query);
    socket->async_connect(endpoint, std::bind(
        &network::handle_connect, shared_from_this(), 
            _1, socket, hostname, handle_connect));
}

void network::listen(connect_handler handle_connect)
{
    strand()->post(
        [&listeners_, handle_connect]
        {
            listeners_.push_back(handle_connect);
        });
}

bool network::start_accept()
{
    acceptor_.reset(new tcp::acceptor(*service()));
    socket_ptr socket(new tcp::socket(*service()));
    try
    {
        tcp::endpoint endpoint(tcp::v4(), 8333);
        acceptor_->open(endpoint.protocol());
        acceptor_->set_option(tcp::acceptor::reuse_address(true));
        acceptor_->bind(endpoint);
        acceptor_->listen(socket_base::max_connections);
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


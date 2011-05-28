#include "bitcoin/net/connection_manager.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <iostream>

#include "bitcoin/util/logger.hpp"
#include "bitcoin/net/dialect.hpp"

namespace libbitcoin {
namespace net {

using boost::asio::socket_base;

static void run_service(shared_ptr<io_service> service)
{
    service->run();
}

default_connection_manager::default_connection_manager()
{
    service_.reset(new io_service);
    work_.reset(new io_service::work(*service_));
    strand_.reset(new io_service::strand(*service_));
    runner_ = std::thread(run_service, service_);
    default_dialect_.reset(new original_dialect);
    //start_accept();
}

default_connection_manager::~default_connection_manager()
{
    if (acceptor_)
        acceptor_->close();
    service_->stop();
    runner_.join();
}

void default_connection_manager::add_channel(channel_ptr channel_obj)
{
    channels_.push_back(channel_obj);
    logger(LOG_DEBUG) << channels_.size() << " peers connected.";
}

void default_connection_manager::perform_disconnect(channel_ptr channel_obj)
{
    auto matches = std::remove(channels_.begin(), channels_.end(), channel_obj);
    channels_.erase(matches, channels_.end());
}

channel_ptr default_connection_manager::create_channel(socket_ptr socket)
{
    channel::init_data init_data = { 
            shared_from_this(), default_dialect_, service_, socket };

    channel_ptr channel_obj(new channel(init_data));
    strand_->post(boost::bind(&default_connection_manager::add_channel, 
                this, channel_obj));
    return channel_obj;
}

channel_ptr default_connection_manager::connect(std::string ip_addr, 
        unsigned short port)
{
    socket_ptr socket(new tcp::socket(*service_));
    try 
    {
        tcp::resolver resolver(*service_);
        tcp::resolver::query query(ip_addr, 
                boost::lexical_cast<std::string>(port));
        tcp::endpoint endpoint = *resolver.resolve(query);
        socket->connect(endpoint);
    }
    catch (std::exception& ex) 
    {
        logger(LOG_ERROR) << "Connecting to peer " << ip_addr 
                << ": " << ex.what();
        return channel_ptr();
    }
    return create_channel(socket);
}

void default_connection_manager::disconnect(channel_ptr channel_obj)
{
    strand_->dispatch(boost::bind(
                &default_connection_manager::perform_disconnect, 
                    this, channel_obj));
    logger(LOG_DEBUG) << channels_.size() << " peer remaining.";
}

bool default_connection_manager::start_accept()
{
    acceptor_.reset(new tcp::acceptor(*service_));
    socket_ptr socket(new tcp::socket(*service_));
    try
    {
        tcp::endpoint endpoint(tcp::v4(), 8333);
        acceptor_->open(endpoint.protocol());
        acceptor_->set_option(tcp::acceptor::reuse_address(true));
        acceptor_->bind(endpoint);
        acceptor_->listen(socket_base::max_connections);
        acceptor_->async_accept(*socket, 
                boost::bind(&default_connection_manager::handle_accept, 
                    this, socket));
    }
    catch (std::exception& ex)
    {
        logger(LOG_ERROR) << "Accepting connections: " << ex.what();
        return false;
    }
    return true;
}

void default_connection_manager::handle_accept(socket_ptr socket)
{
    tcp::endpoint remote_endpoint = socket->remote_endpoint();
    logger(LOG_DEBUG) << "New incoming connection from " 
            << remote_endpoint.address().to_string();
    create_channel(socket);
    socket.reset(new tcp::socket(*service_));
    acceptor_->async_accept(*socket, 
            boost::bind(&default_connection_manager::handle_accept, 
                this, socket));
}

} // net
} // libbitcoin


#include "bitcoin/net/delegator.hpp"

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

delegator_default::delegator_default()
{
    service_.reset(new io_service);
    work_.reset(new io_service::work(*service_));
    strand_.reset(new io_service::strand(*service_));
    runner_ = std::thread(run_service, service_);
    default_dialect_.reset(new original_dialect);
    start_accept();
}

delegator_default::~delegator_default()
{
    if (acceptor_)
        acceptor_->close();
    service_->stop();
    runner_.join();
}

void delegator_default::add_peer(peer_ptr peer_obj)
{
    peers_.push_back(peer_obj);
    logger(LOG_DEBUG) << peers_.size() << " peers connected.";
}

void delegator_default::perform_disconnect(peer_ptr peer_obj)
{
    auto matches = std::remove(peers_.begin(), peers_.end(), peer_obj);
    peers_.erase(matches, peers_.end());
}

peer_ptr delegator_default::create_peer(socket_ptr socket)
{
    peer::init_data init_data = { 
            shared_from_this(), default_dialect_, service_, socket };

    peer_ptr peer_obj(new peer(init_data));
    strand_->post(boost::bind(&delegator_default::add_peer, this, peer_obj));
    return peer_obj;
}

peer_ptr delegator_default::connect(std::string ip_addr, unsigned short port)
{
    socket_ptr socket(new tcp::socket(*service_));
    try 
    {
        tcp::resolver resolver(*service_);
        tcp::resolver::query query(ip_addr, boost::lexical_cast<std::string>(port));
        tcp::endpoint endpoint = *resolver.resolve(query);
        socket->connect(endpoint);
    }
    catch (std::exception& ex) 
    {
        logger(LOG_ERROR) << "Connecting to peer " << ip_addr << ": " << ex.what();
        return peer_ptr();
    }
    return create_peer(socket);
}

void delegator_default::disconnect(peer_ptr peer_obj)
{
    strand_->dispatch(boost::bind(
                &delegator_default::perform_disconnect, this, peer_obj));
    logger(LOG_DEBUG) << peers_.size() << " peers remaining.";
}

bool delegator_default::start_accept()
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
                boost::bind(&delegator_default::handle_accept, this, socket));
    }
    catch (std::exception& ex)
    {
        logger(LOG_ERROR) << "Accepting connections: " << ex.what();
        return false;
    }
    return true;
}

void delegator_default::handle_accept(socket_ptr socket)
{
    tcp::endpoint remote_endpoint = socket->remote_endpoint();
    logger(LOG_DEBUG) << "New incoming connection from " 
            << remote_endpoint.address().to_string();
    create_peer(socket);
    socket.reset(new tcp::socket(*service_));
    acceptor_->async_accept(*socket, 
            boost::bind(&delegator_default::handle_accept, this, socket));
}

} // net
} // libbitcoin


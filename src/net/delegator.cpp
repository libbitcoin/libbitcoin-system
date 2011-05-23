#include "net/delegator.hpp"

#include <boost/bind.hpp>
#include <iostream>

#include "net/dialect.hpp"

namespace libbitcoin {
namespace net {

static void run_service(shared_ptr<io_service> service)
{
    service->run();
}

delegator_default::~delegator_default()
{
    service_->stop();
    runner_.join();
}

void delegator_default::init()
{
    service_.reset(new io_service);
    work_.reset(new io_service::work(*service_));
    strand_.reset(new io_service::strand(*service_));
    runner_ = std::thread(run_service, service_);
    default_dialect_.reset(new original_dialect);
}

void delegator_default::add_peer(shared_ptr<peer> peer_obj)
{
    peers_.push_back(peer_obj);
}

void delegator_default::perform_disconnect(shared_ptr<peer> peer_obj)
{
    auto matches = std::remove(peers_.begin(), peers_.end(), peer_obj);
    peers_.erase(matches, peers_.end());
}

shared_ptr<peer> delegator_default::connect(std::string ip_addr, 
        unsigned short port)
{
    shared_ptr<peer> peer_obj(new peer(shared_from_this(), default_dialect_));
    if (!peer_obj->connect(service_, ip_addr, port)) 
    {
        return shared_ptr<peer>();
    }
    strand_->post(boost::bind(&delegator_default::add_peer, this, peer_obj));
    return peer_obj;
}

void delegator_default::disconnect(shared_ptr<peer> peer_obj)
{
    strand_->post(boost::bind(
                &delegator_default::perform_disconnect, this, peer_obj));
}

} // net
} // libbitcoin


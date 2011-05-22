#include "net/delegator.hpp"

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
    runner_ = std::thread(run_service, service_);
}

shared_ptr<peer> delegator_default::connect(std::string ip_addr, 
        unsigned short port)
{
    if (!default_dialect_)
        default_dialect_.reset(new original_dialect);
    shared_ptr<peer> peer_obj(new peer(default_dialect_));
    if (!peer_obj->connect(service_, ip_addr, port)) 
    {
        return shared_ptr<peer>();
    }
    peers_.push_back(peer_obj);
    return peer_obj;
}

} // net
} // libbitcoin


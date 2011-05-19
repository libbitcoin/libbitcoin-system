#include "net/delegator.h"

namespace libbitcoin {
namespace net {

void delegator::run_service(shared_ptr<io_service> service)
{
    service->run();
}

delegator_default::~delegator_default()
{
    service->stop();
    runner.join();
}

void delegator_default::init()
{
    service.reset(new io_service);
    work.reset(new io_service::work(*service));
    runner = std::thread(run_service, service);
}

bool delegator_default::connect(std::string ip_addr, unsigned short port)
{
    peer *peer_obj = new peer;
    if (!peer_obj->connect(service, ip_addr, port)) {
        delete peer_obj;
        return false;
    }
    peers.push_back(peer_obj);
    return true;
}

} // net
} // libbitcoin


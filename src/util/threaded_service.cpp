#include <bitcoin/util/threaded_service.hpp>

namespace libbitcoin {

void run_service(service_ptr service)
{
    service->run();
}

threaded_service::threaded_service()
{
    service_.reset(new io_service);
    strand_.reset(new io_service::strand(*service_));
    work_.reset(new io_service::work(*service_));
    runner_ = std::thread(run_service, service_);
}

threaded_service::~threaded_service()
{
    service_->stop();
    runner_.join();
}

service_ptr threaded_service::service()
{
    return service_;
}

strand_ptr threaded_service::strand()
{
    return strand_;
}

} // libbitcoin


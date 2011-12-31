#include <bitcoin/utility/threads.hpp>

namespace libbitcoin {

void run_service(service_ptr service)
{
    service->run();
}

thread_core::thread_core()
{
    service_.reset(new io_service);
    // use std::ref here
    work_.reset(new io_service::work(*service_));
    runner_ = std::thread(run_service, service_);
}

thread_core::~thread_core()
{
    service_->stop();
    runner_.join();
}

service_ptr thread_core::service()
{
    return service_;
}

strand_ptr thread_core::create_strand()
{
    return std::make_shared<io_service::strand>(*service_);
}

threaded_service::threaded_service()
  : strand_(core_.create_strand())
{
}

service_ptr threaded_service::service()
{
    return core_.service();
}

strand_ptr threaded_service::strand()
{
    return strand_;
}

} // libbitcoin


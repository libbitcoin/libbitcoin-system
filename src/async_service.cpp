#include <bitcoin/async_service.hpp>

namespace libbitcoin {

async_service::async_service()
  : work_(nullptr)
{
}

async_service::async_service(size_t number_threads)
  : work_(nullptr)
{
    for (size_t i = 0; i < number_threads; ++i)
        spawn();
}

async_service::~async_service()
{
    service_.stop();
    for (std::thread& t: threads_)
        t.join();
}

void async_service::run_service(io_service* service)
{
    service->run();
}

void async_service::spawn()
{
    if (!work_)
        work_ = new io_service::work(service_);
    threads_.push_back(std::thread(&async_service::run_service, &service_));
}
void async_service::shutdown()
{
    delete work_;
    work_ = nullptr;
}

io_service& async_service::get_service()
{
    return service_;
}
const io_service& async_service::get_service() const
{
    return service_;
}

} // namespace libbitcoin


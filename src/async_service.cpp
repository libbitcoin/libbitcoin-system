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

void async_service::spawn()
{
    if (!work_)
        work_ = new io_service::work(ios_);
    threads_.push_back(std::thread([this] { ios_.run(); }));
}

void async_service::stop()
{
    ios_.stop();
}
void async_service::shutdown()
{
    delete work_;
    work_ = nullptr;
}
void async_service::join()
{
    for (std::thread& t: threads_)
        t.join();
}

io_service& async_service::get_service()
{
    return ios_;
}
const io_service& async_service::get_service() const
{
    return ios_;
}

async_strand::async_strand(async_service& service)
  : ios_(service.get_service()), strand_(ios_)
{
}

} // namespace libbitcoin


#include <bitcoin/storage/storage.hpp>

#include <bitcoin/kernel.hpp>

namespace libbitcoin {
namespace storage {

static void run_service(shared_ptr<io_service> service)
{
    service->run();
}

memory_storage::memory_storage(kernel_ptr kern)
 : kernel_(kern)
{
    service_.reset(new io_service);
    work_.reset(new io_service::work(*service_));
    strand_.reset(new io_service::strand(*service_));
    runner_ = std::thread(run_service, service_);
}

kernel_ptr memory_storage::kernel() const
{
    return kernel_;
}

void push_inv_message(net::message::inv item)
{
}
void memory_storage::push(net::message::inv item)
{
}

} // storage
} // libbitcoin


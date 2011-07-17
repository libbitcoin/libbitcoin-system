#include <bitcoin/storage/storage.hpp>

#include <functional>

#include <bitcoin/kernel.hpp>
#include <bitcoin/util/logger.hpp>

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

void memory_storage::do_push_inv(net::message::inv item)
{
    logger(LOG_DEBUG) << "storing " << item.invs.size() << " invs";
    const net::message::inv_list& invs = item.invs;
    inventories_.insert(inventories_.end(), invs.begin(), invs.end());
    logger(LOG_DEBUG) << "total of " << inventories_.size() << " invs";
}
void memory_storage::push(net::message::inv item)
{
    strand_->post(std::bind(&memory_storage::do_push_inv, this, item));
}

void memory_storage::do_push_block(net::message::block item)
{
    logger(LOG_DEBUG) << "storing block.";
}
void memory_storage::push(net::message::block item)
{
    strand_->post(std::bind(&memory_storage::do_push_block, this, item));
}

void memory_storage::do_request_inventories(accept_inventories_handler handler)
{
    // Remove old inventories...
    if (!inventories_.empty())
        handler(inventories_);
}
void memory_storage::request_inventories(accept_inventories_handler handler)
{
    strand_->post(std::bind(
            &memory_storage::do_request_inventories, this, handler));
}

} // storage
} // libbitcoin


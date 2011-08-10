#include <bitcoin/storage/memory_storage.hpp>

#include <functional>

#include <bitcoin/util/logger.hpp>

namespace libbitcoin {

static void run_service(shared_ptr<io_service> service)
{
    service->run();
}

memory_storage::memory_storage()
{
    service_.reset(new io_service);
    work_.reset(new io_service::work(*service_));
    strand_.reset(new io_service::strand(*service_));
    runner_ = std::thread(run_service, service_);
}

void memory_storage::do_push_inv(message::inv item)
{
    log_debug() << "storing " << item.invs.size() << " invs";
    const message::inv_list& invs = item.invs;
    inventories_.insert(inventories_.end(), invs.begin(), invs.end());
    log_debug() << "total of " << inventories_.size() << " invs";
}
void memory_storage::store(message::inv inv, store_handler handle_store)
{
    strand_->post(std::bind(&memory_storage::do_push_inv, this, inv));
    handle_store(std::error_code());
}

void memory_storage::store(message::transaction transaction,
        store_handler handle_store)
{
    handle_store(std::error_code());
}

void memory_storage::do_push_block(message::block block)
{
    log_debug() << "storing block.";
}
void memory_storage::store(message::block block, store_handler handle_store)
{
    strand_->post(std::bind(&memory_storage::do_push_block, this, block));
    handle_store(std::error_code());
}

void memory_storage::do_request_inventories(
        fetch_handler_inventories handle_fetch)
{
    // Remove old inventories...
    if (!inventories_.empty())
        handle_fetch(std::error_code(), inventories_);
}
void memory_storage::fetch_inventories(fetch_handler_inventories handle_fetch)
{
    strand_->post(std::bind(
            &memory_storage::do_request_inventories, this, handle_fetch));
}

void memory_storage::fetch_block_by_depth(size_t block_number, 
        fetch_handler_block handle_fetch)
{
}

void memory_storage::fetch_block_by_hash(hash_digest block_hash, 
        fetch_handler_block handle_fetch)
{
}

void memory_storage::fetch_output_by_hash(hash_digest transaction_hash, 
        uint32_t index, fetch_handler_output handle_fetch)
{
}

} // libbitcoin


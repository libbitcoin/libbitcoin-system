#include <bitcoin/kernel.hpp>

#include <bitcoin/constants.hpp>
#include <bitcoin/network/network.hpp>
#include <bitcoin/storage/storage.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/util/postbind.hpp>

namespace libbitcoin {

void null(std::error_code)
{
}

kernel::kernel()
  : initial_getblocks_(false)
{
}

void kernel::register_network(network_ptr net_comp)
{
    network_component_ = net_comp;
}

network_ptr kernel::get_network()
{
    return network_component_;
}

void kernel::send_failed(channel_handle, const message::version&)
{
}

void kernel::send_failed(channel_handle, const message::verack&)
{
}

void kernel::send_failed(channel_handle, const message::getaddr&)
{
}

void kernel::send_failed(channel_handle, const message::inv&)
{
}

void kernel::send_failed(channel_handle, const message::getdata&)
{
}

void kernel::send_failed(channel_handle, const message::getblocks&)
{
}

bool kernel::recv_message(channel_handle chandle,
        const message::version& message)
{
    log_debug() << "nonce is " << message.nonce;
    log_debug() << "last block is " << message.start_height;
    log_debug() << pretty_hex(message.addr_you.ip_addr);
    network_component_->send(chandle, message::verack());
    if (!initial_getblocks_)
    {
        initial_getblocks_ = true;
        start_initial_getblocks(chandle);
    }
    return true;
}

void kernel::start_initial_getblocks(channel_handle chandle)
{
    storage_component_->fetch_block_locator(
        postbind<std::error_code, message::block_locator>(strand(), std::bind(
            &kernel::request_initial_blocks, shared_from_this(), 
                std::placeholders::_1, std::placeholders::_2, chandle)));
}

void kernel::request_initial_blocks(const std::error_code& ec,
    const message::block_locator& locator, channel_handle chandle)
{
    if (ec)
    {
        log_error() << "Initial block locator: " << ec.message();
        return;
    }
    message::getblocks getblocks;
    getblocks.locator_start_hashes = locator;
    getblocks.hash_stop = null_hash;
    network_component_->send(chandle, getblocks);
}

bool kernel::recv_message(channel_handle, const message::verack&)
{
    // When you receive this, then you know other side is accepting your sends
    return true;
}

bool kernel::recv_message(channel_handle, const message::addr& message)
{
    //for (const message::net_addr addr: message.addr_list)
    //    log_debug() << pretty_hex(addr.ip_addr) << ' ' << addr.port;
    return true;
}

// TODO: Finish this.
template<typename Function>
void ask_block(bool block_exists, Function request_block)
{
    if (!block_exists)
        request_block();
    //else
    //    tween_blocks...
}

bool kernel::recv_message(channel_handle chandle, const message::inv& message)
{
    message::getdata request_message;
    for (const message::inv_vect curr_inv: message.invs)
    {
        if (curr_inv.type == message::inv_type::none)
            return false;

        // Push only block invs to the request queue
        if (curr_inv.type == message::inv_type::block)
        {
            request_message.invs.push_back(curr_inv);
            inventory_tracker_.insert(make_pair(curr_inv.hash, chandle));
        }
    }
    // TODO: Should check if block exists or not first before
    // wasting bandwidth
    if (request_message.invs.size() > 0)
        network_component_->send(chandle, request_message);
    return true;
}

bool kernel::recv_message(channel_handle, const message::block& message)
{
    storage_component_->store(message, null);
    return true;
}

void kernel::handle_connect(channel_handle)
{
}

void kernel::register_storage(storage_ptr stor_comp)
{
    storage_component_ = stor_comp;
}

storage_ptr kernel::get_storage()
{
    return storage_component_;
}

void kernel::tween_blocks(const hash_pair_list& block_hashes)
{
    storage_component_->fetch_block_locator(
        postbind<std::error_code, message::block_locator>(strand(), std::bind(
            &kernel::request_next_blocks, shared_from_this(), 
                std::placeholders::_1, std::placeholders::_2, block_hashes)));
}

void kernel::request_next_blocks(const std::error_code& ec,
    const message::block_locator& locator, const hash_pair_list& block_hashes)
{
    if (ec)
    {
        log_error() << "block locator: " << ec.message();
        return;
    }
    message::getblocks getblocks;
    getblocks.locator_start_hashes = locator;
    for (const hash_pair& continue_hashes: block_hashes)
    {
        const hash_digest& orphan_root = continue_hashes.first;
        const hash_digest& block_hash = continue_hashes.second;

        auto range = inventory_tracker_.equal_range(block_hash);
        for (auto it = range.first; it != range.second; ++it)
        {
            getblocks.hash_stop = orphan_root;
            network_component_->send(it->second, getblocks);
        }
    }
}

} // libbitcoin


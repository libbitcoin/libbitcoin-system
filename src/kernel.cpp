#include <bitcoin/kernel.hpp>

#include <bitcoin/constants.hpp>
#include <bitcoin/network/network.hpp>
#include <bitcoin/storage/storage.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/util/postbind.hpp>

using std::placeholders::_1;
using std::placeholders::_2;

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

void kernel::connect(std::string hostname, unsigned int port)
{
    handshake_connect(network_component_, hostname, port,
        postbind<std::error_code, channel_handle>(strand(), std::bind(
            &kernel::handle_connect, shared_from_this(), _1, _2)));
}

void kernel::handle_connect(const std::error_code& ec, channel_handle chandle)
{
    if (ec)
    {
        log_error() << "Problem connecting";
        return;
    }
    if (!initial_getblocks_)
    {
        initial_getblocks_ = true;
        start_initial_getblocks(chandle);
    }

    network_component_->subscribe_inventory(chandle,
        std::bind(&kernel::receive_inventory, shared_from_this(),
            chandle, _1));
    network_component_->subscribe_block(chandle,
        std::bind(&kernel::receive_block, shared_from_this(), chandle, _1));
}

void kernel::start_initial_getblocks(channel_handle chandle)
{
    storage_component_->fetch_block_locator(
        postbind<std::error_code, message::block_locator>(strand(), std::bind(
            &kernel::request_initial_blocks, shared_from_this(), 
                _1, _2, chandle)));
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
    network_component_->send(chandle, getblocks, null);
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

void kernel::receive_inventory(channel_handle chandle,
    const message::inventory& packet)
{
    message::getdata request_message;
    for (const message::inventory_vector curr_inv: packet.inventories)
    {
        if (curr_inv.type == message::inventory_type::none)
            return;

        // Push only block invs to the request queue
        if (curr_inv.type == message::inventory_type::block)
        {
            request_message.inventories.push_back(curr_inv);
            inventory_tracker_.insert(make_pair(curr_inv.hash, chandle));
        }
    }
    // TODO: Should check if block exists or not first before
    // wasting bandwidth
    if (request_message.inventories.size() > 0)
        network_component_->send(chandle, request_message, null);
    network_component_->subscribe_inventory(chandle,
        std::bind(&kernel::receive_inventory, shared_from_this(),
            chandle, _1));
}

void kernel::handle_block_stored(const std::error_code& ec,
    block_status status, hash_digest block_hash)
{
    if (ec && ec != error::object_already_exists)
    {
        log_error() << "Problem storing block: " << ec.message();
        return;
    }
    else if (status == block_status::orphan)
    {
        hash_pair_list hashes;
        hashes.push_back(std::make_pair(block_hash, block_hash));
        tween_blocks(hashes);
    }
}

void kernel::receive_block(channel_handle chandle, 
    const message::block& packet)
{
    storage_component_->store(packet,
        std::bind(&kernel::handle_block_stored, shared_from_this(),
            _1, _2, hash_block_header(packet)));
    network_component_->subscribe_block(chandle,
        std::bind(&kernel::receive_block, shared_from_this(), chandle, _1));
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
                _1, _2, block_hashes)));
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
            network_component_->send(it->second, getblocks, null);
        }
    }
}

} // libbitcoin


#include <bitcoin/kernel.hpp>

#include <bitcoin/constants.hpp>
#include <bitcoin/network/network.hpp>
#include <bitcoin/blockchain/blockchain.hpp>
#include <bitcoin/utility/logger.hpp>

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
    network_service_ = net_comp;
}

network_ptr kernel::network_service()
{
    return network_service_;
}

void kernel::connect(std::string hostname, uint16_t port)
{
    //handshake_connect(network_service_, hostname, port,
    //    strand()->wrap(std::bind(
    //        &kernel::handle_connect, shared_from_this(), _1, _2)));
}

void kernel::handle_connect(const std::error_code& ec, channel_ptr node)
{
    if (ec)
    {
        log_error() << "Problem connecting";
        return;
    }
    if (!initial_getblocks_)
    {
        initial_getblocks_ = true;
        start_initial_getblocks(node);
    }

    node->subscribe_inventory(std::bind(
        &kernel::receive_inventory, shared_from_this(), _1, _2, node));
    node->subscribe_block(std::bind(
        &kernel::receive_block, shared_from_this(), _1, _2, node));
}

void kernel::start_initial_getblocks(channel_ptr node)
{
    blockchain_service_->fetch_block_locator(
        strand()->wrap(std::bind(&kernel::request_initial_blocks,
            shared_from_this(), _1, _2, node)));
}

void kernel::request_initial_blocks(const std::error_code& ec,
    const message::block_locator& locator, channel_ptr node)
{
    if (ec)
    {
        log_error() << "Initial block locator: " << ec.message();
        return;
    }
    message::get_blocks getblocks;
    getblocks.locator_start_hashes = locator;
    getblocks.hash_stop = null_hash;
    node->send(getblocks, null);
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

void kernel::receive_inventory(const std::error_code& ec,
    const message::inventory& packet, channel_ptr node)
{
    message::get_data request_message;
    for (const message::inventory_vector curr_inv: packet.inventories)
    {
        if (curr_inv.type == message::inventory_type::none)
            return;

        // Push only block invs to the request queue
        if (curr_inv.type == message::inventory_type::block)
        {
            request_message.inventories.push_back(curr_inv);
            inventory_tracker_.insert(make_pair(curr_inv.hash, node));
        }
    }
    // TODO: Should check if block exists or not first before
    // wasting bandwidth
    if (request_message.inventories.size() > 0)
        node->send(request_message, null);
    node->subscribe_inventory(std::bind(
        &kernel::receive_inventory, shared_from_this(), _1, _2, node));
}

void kernel::handle_block_stored(const std::error_code& ec,
    block_info info, hash_digest block_hash)
{
    if (ec && ec != error::object_already_exists)
    {
        log_error() << "Problem storing block: " << ec.message();
        return;
    }
    else if (info.status == block_status::orphan)
    {
        hash_pair_list hashes;
        hashes.push_back(std::make_pair(block_hash, block_hash));
        tween_blocks(hashes);
    }
}

void kernel::receive_block(const std::error_code& ec,
    const message::block& packet, channel_ptr node)
{
    blockchain_service_->store(packet,
        std::bind(&kernel::handle_block_stored, shared_from_this(),
            _1, _2, hash_block_header(packet)));
    node->subscribe_block(
        std::bind(&kernel::receive_block, shared_from_this(), _1, _2, node));
}

void kernel::register_blockchain(blockchain_ptr stor_comp)
{
    blockchain_service_ = stor_comp;
}

blockchain_ptr kernel::blockchain_service()
{
    return blockchain_service_;
}

void kernel::tween_blocks(const hash_pair_list& block_hashes)
{
    blockchain_service_->fetch_block_locator(
        strand()->wrap(std::bind(&kernel::request_next_blocks,
            shared_from_this(), _1, _2, block_hashes)));
}

void kernel::request_next_blocks(const std::error_code& ec,
    const message::block_locator& locator, const hash_pair_list& block_hashes)
{
    if (ec)
    {
        log_error() << "block locator: " << ec.message();
        return;
    }
    message::get_blocks getblocks;
    getblocks.locator_start_hashes = locator;
    for (const hash_pair& continue_hashes: block_hashes)
    {
        const hash_digest& orphan_root = continue_hashes.first;
        const hash_digest& block_hash = continue_hashes.second;

        auto range = inventory_tracker_.equal_range(block_hash);
        for (auto it = range.first; it != range.second; ++it)
        {
            getblocks.hash_stop = orphan_root;
            it->second->send(getblocks, null);
        }
    }
}

} // libbitcoin


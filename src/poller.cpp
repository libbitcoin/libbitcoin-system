#include <bitcoin/poller.hpp>

#include <bitcoin/utility/logger.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

poller::poller(blockchain_ptr chain)
  : chain_(chain)
{
}

void poller::query(channel_ptr node)
{
    chain_->fetch_block_locator(
        std::bind(&poller::initial_ask_blocks,
            shared_from_this(), _1, _2, node));
}

void poller::initial_ask_blocks(const std::error_code& ec,
    const message::block_locator& locator, channel_ptr node)
{
    if (ec)
    {
        log_error(log_domain::poller)
            << "Fetching initial block locator: " << ec.message();
        return;
    }
    node->subscribe_inventory(
        std::bind(&poller::receive_inv,
            shared_from_this(), _1, _2, node));
    node->subscribe_block(
        std::bind(&poller::receive_block,
            shared_from_this(), _1, _2, node));
    ask_blocks(ec, locator, null_hash, node);
}

void handle_send_packet(const std::error_code& ec)
{
    if (ec)
        log_error(log_domain::poller)
            << "Send problem: " << ec.message();
}

void poller::receive_inv(const std::error_code& ec,
    const message::inventory& packet, channel_ptr node)
{
    if (ec)
    {
        log_error(log_domain::poller)
            << "Received bad inventory: " << ec.message();
        return;
    }
    // Filter out only block inventories
    message::get_data getdata;
    for (const message::inventory_vector& ivv: packet.inventories)
    {
        if (ivv.type != message::inventory_type::block)
            continue;
        getdata.inventories.push_back(ivv);
    }
    node->send(getdata, handle_send_packet);
    node->subscribe_inventory(
        std::bind(&poller::receive_inv,
            shared_from_this(), _1, _2, node));
}

void poller::receive_block(const std::error_code& ec,
    const message::block& blk, channel_ptr node)
{
    if (ec)
    {
        log_error(log_domain::poller)
            << "Received bad block: " << ec.message();
        return;
    }
    chain_->store(blk,
        std::bind(&poller::handle_store,
            shared_from_this(), _1, _2,
            hash_block_header(blk), node));
    node->subscribe_block(
        std::bind(&poller::receive_block,
            shared_from_this(), _1, _2, node));
}

void poller::handle_store(const std::error_code& ec, block_info info,
    const hash_digest& block_hash, channel_ptr node)
{
    if (ec)
    {
        log_error(log_domain::poller)
            << "Storing block: " << ec.message();
        return;
    }
    switch (info.status)
    {
        case block_status::orphan:
            // TODO: Make more efficient by storing block hash
            // and next time do not download orphan block again.
            // Remember to remove from list once block is no longer orphan
            chain_->fetch_block_locator(
                std::bind(&poller::ask_blocks, shared_from_this(),
                    _1, _2, block_hash, node));
            break;

        case block_status::rejected:
            log_error(log_domain::poller)
                << "Rejected block " << pretty_hex(block_hash);
            break;

        case block_status::confirmed:
            log_info(log_domain::poller)
                << "Block #" << info.depth << " " << pretty_hex(block_hash);
            break;
    }
}

void poller::ask_blocks(const std::error_code& ec,
    const message::block_locator& locator,
    const hash_digest& hash_stop, channel_ptr node)
{
    if (ec)
    {
        log_error(log_domain::poller)
            << "Ask for blocks: " << ec.message();
        return;
    }
    message::get_blocks packet;
    packet.start_hashes = locator;
    packet.hash_stop = hash_stop;
    node->send(packet, std::bind(&handle_send_packet, _1));
}

} // libbitcoin


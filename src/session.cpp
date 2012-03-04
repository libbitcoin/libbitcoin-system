#include <bitcoin/session.hpp>

#include <bitcoin/blockchain/bdb_blockchain.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

session::session()
  : network_service_(1), disk_service_(1), mempool_service_(1)
{
    hosts_ = std::make_shared<hosts>(network_service_);
    handshake_ = std::make_shared<handshake>(network_service_);
    network_ = std::make_shared<network>(network_service_);
    protocol_ = std::make_shared<protocol>(
        network_service_, hosts_, handshake_, network_);

    chain_ = std::make_shared<bdb_blockchain>(disk_service_, "database");
    poll_ = std::make_shared<poller>(chain_);

    tx_pool_ = transaction_pool::create(mempool_service_, chain_);
}

void session::start(completion_handler handle_complete)
{
    protocol_->start(handle_complete);
    protocol_->subscribe_channel(
        std::bind(&session::download_blockchain, this, _1));
    protocol_->subscribe_channel(
        std::bind(&session::new_channel, this, _1));
}

void session::download_blockchain(channel_ptr node)
{
    poll_->query(node);
}

void session::new_channel(channel_ptr node)
{
    node->subscribe_inventory(
        std::bind(&session::inventory, this, _1, _2, node));
    node->subscribe_get_data(
        std::bind(&session::get_data, this, _1, _2, node));
    node->subscribe_get_blocks(
        std::bind(&session::get_blocks, this, _1, _2, node));
    // tx
    // block
}

void session::inventory(const std::error_code& ec,
    const message::inventory& packet, channel_ptr node)
{
    for (const message::inventory_vector& ivec: packet.inventories)
    {
        if (ivec.type == message::inventory_type::transaction)
        {
            // does it exist already
            // if not then issue getdata
        }
        else if (ivec.type == message::inventory_type::block)
        {
            // does it exist already
            // if not then issue getdata
            // check if already exists
            // if block already downloaded, and if block is orphan
            //   then do getblocks
        }
        else
            log_warning() << "Ignoring unknown inventory type";
    }
    node->subscribe_inventory(
        std::bind(&session::inventory, this, _1, _2, node));
}

void session::get_data(const std::error_code& ec,
    const message::get_data& packet, channel_ptr node)
{
    // simple stuff
    node->subscribe_get_data(
        std::bind(&session::get_data, this, _1, _2, node));
}

void session::get_blocks(const std::error_code& ec,
    const message::get_blocks& packet, channel_ptr node)
{
    // send 500 invs from last fork point
    // have memory of last inv, ready to trigger send next 500 once
    // getdata done for it.
    node->subscribe_get_blocks(
        std::bind(&session::get_blocks, this, _1, _2, node));
}

} // libbitcoin


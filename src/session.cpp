#include <bitcoin/session.hpp>

#include <bitcoin/blockchain/bdb_blockchain.hpp>
#include <bitcoin/transaction.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

session::session(const session_params& params)
  : hosts_(params.hosts), handshake_(params.handshake),
    network_(params.network), protocol_(params.protocol),
    chain_(params.blockchain), poll_(params.poller),
    tx_pool_(params.transaction_pool)
{
}

void session::start(completion_handler handle_complete)
{
    protocol_->start(handle_complete);
    protocol_->subscribe_channel(
        std::bind(&session::download_blockchain, this, _1));
    protocol_->subscribe_channel(
        std::bind(&session::new_channel, this, _1));
}

void session::stop(completion_handler handle_complete)
{
    protocol_->stop(handle_complete);
}

void session::download_blockchain(channel_ptr node)
{
    poll_->query(node);
}

void session::new_channel(channel_ptr node)
{
    BITCOIN_ASSERT(node);
    node->subscribe_inventory(
        std::bind(&session::inventory, this, _1, _2, node));
    node->subscribe_get_data(
        std::bind(&session::get_data, this, _1, _2, node));
    node->subscribe_get_blocks(
        std::bind(&session::get_blocks, this, _1, _2, node));
    // tx
    // block
    protocol_->subscribe_channel(
        std::bind(&session::new_channel, this, _1));
}

void session::inventory(const std::error_code& ec,
    const message::inventory& packet, channel_ptr node)
{
    if (ec)
    {
        log_error(log_domain::session) << "inventory: " << ec.message();
        return;
    }
    for (const message::inventory_vector& ivec: packet.inventories)
    {
        if (ivec.type == message::inventory_type::transaction)
        {
            log_debug(log_domain::session)
                << "Transaction inventory: " << pretty_hex(ivec.hash);
            // does it exist already
            // if not then issue getdata
            tx_pool_->exists(ivec.hash,
                std::bind(&session::request_tx_data, this,
                    _1, ivec.hash, node));
        }
        else if (ivec.type == message::inventory_type::block)
        {
            log_debug(log_domain::session)
                << "Block inventory: " << pretty_hex(ivec.hash);
            // does it exist already
            // if not then issue getdata
            // check if already exists
            // if block already downloaded, and if block is orphan
            //   then do getblocks
        }
        else
            log_warning(log_domain::session)
                << "Ignoring unknown inventory type";
    }
    node->subscribe_inventory(
        std::bind(&session::inventory, this, _1, _2, node));
}

void session::get_data(const std::error_code& ec,
    const message::get_data& packet, channel_ptr node)
{
    if (ec)
    {
        log_error(log_domain::session) << "get_data: " << ec.message();
        return;
    }
    // simple stuff
    node->subscribe_get_data(
        std::bind(&session::get_data, this, _1, _2, node));
}

void session::get_blocks(const std::error_code& ec,
    const message::get_blocks& packet, channel_ptr node)
{
    if (ec)
    {
        log_error(log_domain::session) << "get_blocks: " << ec.message();
        return;
    }
    // send 500 invs from last fork point
    // have memory of last inv, ready to trigger send next 500 once
    // getdata done for it.
    node->subscribe_get_blocks(
        std::bind(&session::get_blocks, this, _1, _2, node));
}

void handle_send_get_data(const std::error_code& ec)
{
    if (ec)
        log_error(log_domain::session) << "Requesting data: " << ec.message();
}
void session::request_tx_data(bool tx_exists,
    const hash_digest& tx_hash, channel_ptr node)
{
    if (tx_exists)
        return;
    message::get_data request_tx;
    request_tx.inventories.push_back(
        {message::inventory_type::transaction, tx_hash});
    node->send(request_tx, handle_send_get_data);
}

} // libbitcoin


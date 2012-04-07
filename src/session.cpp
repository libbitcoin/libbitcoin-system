#include <bitcoin/session.hpp>

#include <bitcoin/blockchain/bdb_blockchain.hpp>
#include <bitcoin/transaction.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

session::session(async_service& service, const session_params& params)
  : hosts_(params.hosts_), handshake_(params.handshake_),
    network_(params.network_), protocol_(params.protocol_),
    chain_(params.blockchain_), poll_(params.poller_),
    tx_pool_(params.transaction_pool_),
    strand_(service.get_service()), grabbed_invs_(20)
{
}

void handle_set_start_depth(const std::error_code&)
{
    // Set start depth in handshake
    // Do nothing
}
void session::start(completion_handler handle_complete)
{
    auto this_ptr = shared_from_this();
    protocol_->start(handle_complete);
    protocol_->subscribe_channel(
        [&, this_ptr](channel_ptr node)
        {
            poll_->query(node);
        });
    protocol_->subscribe_channel(
        std::bind(&session::new_channel, this_ptr, _1));
    chain_->fetch_last_depth(
        std::bind(&handshake::set_start_depth,
            handshake_, _2, handle_set_start_depth));
    chain_->subscribe_reorganize(
        std::bind(&session::set_start_depth, this_ptr, _1, _2, _3, _4));
}

void session::stop(completion_handler handle_complete)
{
    protocol_->stop(handle_complete);
}

void session::new_channel(channel_ptr node)
{
    auto this_ptr = shared_from_this();
    BITCOIN_ASSERT(node);
    node->subscribe_inventory(
        std::bind(&session::inventory, this_ptr, _1, _2, node));
    node->subscribe_get_data(
        std::bind(&session::get_data, this_ptr, _1, _2, node));
    node->subscribe_get_blocks(
        std::bind(&session::get_blocks, this_ptr, _1, _2, node));
    // tx
    // block
    protocol_->subscribe_channel(
        std::bind(&session::new_channel, this_ptr, _1));
    poll_->monitor(node);
}

void session::set_start_depth(const std::error_code& ec, size_t fork_point,
    const blockchain::block_list& new_blocks,
    const blockchain::block_list& replaced_blocks)
{
    size_t last_depth = fork_point + new_blocks.size();
    handshake_->set_start_depth(last_depth, handle_set_start_depth);
    chain_->subscribe_reorganize(
        std::bind(&session::set_start_depth,
            shared_from_this(), _1, _2, _3, _4));
}

void session::inventory(const std::error_code& ec,
    const message::inventory& packet, channel_ptr node)
{
    auto this_ptr = shared_from_this();
    if (ec)
    {
        log_error(log_domain::session) << "inventory: " << ec.message();
        return;
    }
    for (const message::inventory_vector& ivec: packet.inventories)
    {
        if (ivec.type == message::inventory_type::transaction)
            strand_.post(
                std::bind(&session::new_tx_inventory,
                    this_ptr, ivec.hash, node));
        else if (ivec.type == message::inventory_type::block);
            // Do nothing. Handled by poller.
        else
            log_warning(log_domain::session)
                << "Ignoring unknown inventory type";
    }
    node->subscribe_inventory(
        std::bind(&session::inventory, this_ptr, _1, _2, node));
}

void session::new_tx_inventory(const hash_digest& tx_hash, channel_ptr node)
{
    if (grabbed_invs_.exists(tx_hash))
        return;
    log_debug(log_domain::session)
        << "Transaction inventory: " << pretty_hex(tx_hash);
    // does it exist already
    // if not then issue getdata
    tx_pool_->exists(tx_hash,
        std::bind(&session::request_tx_data,
            shared_from_this(), _1, tx_hash, node));
    grabbed_invs_.store(tx_hash);
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
        std::bind(&session::get_data, shared_from_this(), _1, _2, node));
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
        std::bind(&session::get_blocks, shared_from_this(), _1, _2, node));
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

} // namespace libbitcoin


#include <bitcoin/getx_responder.hpp>

#include <bitcoin/network/channel.hpp>
#include <bitcoin/transaction_pool.hpp>
#include <bitcoin/blockchain/blockchain.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

getx_responder::getx_responder(async_service& service,
    blockchain_ptr chain, transaction_pool_ptr txpool)
  : service_(service.get_service()), chain_(chain), txpool_(txpool)
{
}

void getx_responder::monitor(channel_ptr node)
{
    node->subscribe_get_data(
        service_.wrap(std::bind(
            &getx_responder::receive_get_data,
                shared_from_this(), _1, _2, node)));
}

void getx_responder::receive_get_data(const std::error_code& ec,
    const message::get_data packet, channel_ptr node)
{
    if (ec)
        return;
    for (const message::inventory_vector& inv: packet.inventories)
    {
        switch (inv.type)
        {
            case message::inventory_type::transaction:
                // First attempt lookup in faster pool, then do slow
                // lookup in blockchain after.
                txpool_->fetch(inv.hash,
                    service_.wrap(std::bind(
                        &getx_responder::pool_tx,
                            shared_from_this(), _1, _2, inv.hash, node)));
                break;

            case message::inventory_type::block:
                break;

            // Ignore everything else
            case message::inventory_type::error:
            case message::inventory_type::none:
            default:
                break;
        }
    }
    node->subscribe_get_data(
        service_.wrap(std::bind(
            &getx_responder::receive_get_data,
                shared_from_this(), _1, _2, node)));
}

void getx_responder::pool_tx(const std::error_code& ec,
    const message::transaction& tx, const hash_digest& tx_hash,
    channel_ptr node)
{
    if (ec)
    {
        chain_->fetch_transaction(tx_hash,
            service_.wrap(std::bind(
                &getx_responder::chain_tx,
                    shared_from_this(), _1, _2, node)));
    }
    else
        node->send(tx, [](const std::error_code& ec) {});
}

void getx_responder::chain_tx(const std::error_code& ec,
    const message::transaction& tx, channel_ptr node)
{
    if (ec)
        return;
    node->send(tx, [](const std::error_code& ec) {});
}

} // libbitcoin


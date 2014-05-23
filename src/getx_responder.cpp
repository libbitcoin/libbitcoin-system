/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/blockchain/blockchain.hpp>
#include <bitcoin/getx_responder.hpp>
#include <bitcoin/network/channel.hpp>
#include <bitcoin/transaction_pool.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

getx_responder::getx_responder(blockchain& chain, transaction_pool& txpool)
  : chain_(chain), txpool_(txpool)
{
}

void getx_responder::monitor(channel_ptr node)
{
    // Use this wrapper to add shared state to our node for inv
    // requests to trigger getblocks requests so a node can continue
    // downloading blocks.
    channel_with_state special;
    special.node = node;
    // Serve tx and blocks to nodes.
    node->subscribe_get_data(
        std::bind(&getx_responder::receive_get_data,
            this, _1, _2, special));
}

void getx_responder::receive_get_data(const std::error_code& ec,
    const get_data_type& packet, channel_with_state special)
{
    if (ec)
        return;
    for (const inventory_vector_type& inv: packet.inventories)
    {
        switch (inv.type)
        {
            case inventory_type_id::transaction:
                // First attempt lookup in faster pool, then do slow
                // lookup in blockchain after.
                txpool_.fetch(inv.hash,
                    std::bind(&getx_responder::pool_tx,
                        this, _1, _2, inv.hash, special.node));
                break;

            case inventory_type_id::block:
                fetch_block(chain_, inv.hash,
                    std::bind(&getx_responder::send_block,
                        this, _1, _2, special.node));
                break;

            // Ignore everything else
            case inventory_type_id::error:
            case inventory_type_id::none:
            default:
                break;
        }
    }
    special.node->subscribe_get_data(
        std::bind(&getx_responder::receive_get_data,
            this, _1, _2, special));
}

void getx_responder::pool_tx(const std::error_code& ec,
    const transaction_type& tx, const hash_digest& tx_hash,
    channel_ptr node)
{
    if (ec)
    {
        chain_.fetch_transaction(tx_hash,
            std::bind(&getx_responder::chain_tx,
                this, _1, _2, node));
    }
    else
        node->send(tx, [](const std::error_code&) {});
}

void getx_responder::chain_tx(const std::error_code& ec,
    const transaction_type& tx, channel_ptr node)
{
    if (ec)
        return;
    node->send(tx, [](const std::error_code&) {});
}

void getx_responder::send_block(const std::error_code& ec,
    const block_type& blk, channel_ptr node)
{
    if (ec)
        return;
    node->send(blk, [](const std::error_code&) {});
}

} // libbitcoin


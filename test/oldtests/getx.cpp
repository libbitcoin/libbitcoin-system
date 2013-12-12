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
#include <bitcoin/bitcoin.hpp>
using namespace bc;

threadpool pool(4);
network_ptr net;
acceptor_ptr acc;
channel_ptr send_node, recv_node;
handshake_ptr hs;
blockchain_ptr chain;
transaction_pool_ptr txpool;
getx_responder_ptr getx;

void handle_accept(const std::error_code& ec, channel_ptr node)
{
    send_node = node;
    hs->ready(node, [node](const std::error_code&) { getx->monitor(node); });
}

void handle_listen(const std::error_code& ec, acceptor_ptr accpt)
{
    acc = accpt;
    acc->accept(handle_accept);
}

void handle_send(const std::error_code& ec)
{
}

void handle_blk(const std::error_code& ec, const message::block blk)
{
    log_info() << "blk: " << ec.message() << " " << hash_block_header(blk);
}

void handle_connect(const std::error_code& ec, channel_ptr node)
{
    recv_node = node;
    message::get_data getdat;
    getdat.inventories.push_back(
        {message::inventory_type::transaction,
            decode_hex_digest<hash_digest>("e72e4f025695446cfd5c5349d1720beb38801f329a00281f350cb7e847153397")});
    getdat.inventories.push_back(
        {message::inventory_type::block,
            decode_hex_digest<hash_digest>("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f")});
    node->subscribe_block(handle_blk);
    sleep(1);
    node->send(getdat, handle_send);
}

void blkloc(const std::error_code& ec, const message::block_locator& loc)
{
    if (ec)
        return;
}

void blockchain_started(const std::error_code& ec, blockchain_ptr blkchain)
{
    if (ec)
    {
        log_error() << ec.message();
        return;
    }
    chain = blkchain;
    net = std::make_shared<network>(pool);
    net->listen(8045, handle_listen);
    hs = std::make_shared<handshake>(pool);
    hs->connect(net, "localhost", 8045, handle_connect);
    txpool = transaction_pool::create(pool, chain);
    getx = std::make_shared<getx_responder>(pool, chain, txpool);
    chain->fetch_block_locator(blkloc);
}

int main()
{
    chain = bdb_blockchain::create(pool, "database", blockchain_started);
    std::cin.get();
    return 0;
}


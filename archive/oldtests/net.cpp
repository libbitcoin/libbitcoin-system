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
#include <bitcoin/network/network.hpp>
#include <bitcoin/network/handshake.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/threadpool.hpp>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>

using std::placeholders::_1;
using std::placeholders::_2;
using namespace libbitcoin;

void error_exit(const std::string& message, int status=1)
{
    log_error() << "net: " << message;
    exit(status);
}

std::mutex mutex;
std::condition_variable condition;
size_t inv_count = 0;

void receive_inv(const std::error_code &ec, const inventory_type& packet,
    channel_ptr node)
{
    log_info() << "Received:";
    for (const inventory_vector_type& ivv: packet.inventories)
    {
        if (ivv.type != inventory_type_id::block)
            log_info() << "  --";
        else
            log_info() << "  " << encode_hex(ivv.hash);
    }
    //node->subscribe_inventory(std::bind(&receive_inv, _1, _2, node));
    std::unique_lock<std::mutex> lock(mutex);
    inv_count += packet.inventories.size();
    condition.notify_one();
}

void handle_send_getblock(const std::error_code& ec)
{
    if (ec)
        error_exit(ec.message());
}

get_blocks_type create_getblocks_message()
{
    get_blocks_type packet;
    hash_digest genesis{0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0xd6, 0x68, 
                        0x9c, 0x08, 0x5a, 0xe1, 0x65, 0x83, 0x1e, 0x93, 
                        0x4f, 0xf7, 0x63, 0xae, 0x46, 0xa2, 0xa6, 0xc1, 
                        0x72, 0xb3, 0xf1, 0xb6, 0x0a, 0x8c, 0xe2, 0x6f};
    packet.start_hashes.push_back(genesis);
    packet.hash_stop.fill(0);
    return packet;
}

void show_ip(const std::error_code& ec, const network_address_type& addr)
{
    if (ec)
        error_exit(ec.message());
    log_debug() << encode_hex(addr.ip);
}
void handle_handshake(const std::error_code& ec, channel_ptr node,
    handshake& hs)
{
    if (ec)
        error_exit(ec.message());
    node->subscribe_inventory(std::bind(&receive_inv, _1, _2, node));
    node->send(create_getblocks_message(), 
        std::bind(&handle_send_getblock, _1));
    hs.fetch_network_address(show_ip);
}

void handle_init(const std::error_code& ec, handshake& hs, network& net)
{
    if (ec)
        error_exit(ec.message());
    connect(hs, net, "localhost", 8333,
        std::bind(&handle_handshake, _1, _2, std::ref(hs)));
}

int main()
{
    threadpool pool(1);
    network net(pool);
    handshake hs(pool);
    hs.start(std::bind(handle_init, _1, std::ref(hs), std::ref(net)));

    std::unique_lock<std::mutex> lock(mutex);
    condition.wait(lock, []{ return inv_count >= 500; });
    pool.stop();
    pool.join();
    return 0;
}


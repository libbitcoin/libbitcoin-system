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
/*
  Connect to a Bitcoin node on localhost, port 8333.
*/
#include <iostream>
#include <bitcoin/bitcoin.hpp>
using namespace bc;

// Connection is established and version messages are exchanged.
void connect_established(const std::error_code& ec, channel_ptr node);

void connect_established(const std::error_code& ec, channel_ptr node)
{
    if (ec)
    {
        log_error() << "Connection stopped: " << ec.message();
        return;
    }
    // ...
    // You could start downloading blocks here:
    //
    // get_blocks_type getblocks;
    // getblocks.start_hashes.push_back(decode_hash("..."));
    // getblocks.hash_stop = decode_hash("...");
    // node->send(getblocks, std::bind(getblocks_sent, _1, node));
}

int main()
{
    threadpool pool(1);
    network net(pool);
    handshake hs(pool);
    connect(hs, net, "localhost", 8333, connect_established);
    std::cin.get();
    pool.stop();
    pool.join();
    return 0;
}



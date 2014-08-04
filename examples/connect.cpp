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
using std::placeholders::_1;

// Connection is established.
// Send version message to remote host.
void connect_started(const std::error_code& ec, channel_ptr node);
// Verson message finished sending.
// Program completed.
void version_sent(const std::error_code& ec, channel_ptr node);
// Display message when connection is stopped.
void connect_stopped(const std::error_code& ec);

void connect_started(const std::error_code& ec, channel_ptr node)
{
    if (ec)
    {
        log_error() << "Connect: " << ec.message();
        return;
    }
    // Create our version message we want to send.
    // Fill in a bunch of fields.
    version_type version;
    version.version = 60000;
    version.services = 1;
    version.address_me.services = version.services;
    version.address_me.ip =
        ip_address_type{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                        0x00, 0x00, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x01};
    version.address_me.port = 8333;
    version.address_you.services = version.services;
    version.address_you.ip = 
        ip_address_type{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                        0x00, 0x00, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x01};
    version.address_you.port = 8333;
    // Set the user agent.
    version.user_agent = "/libbitcoin/connect-test/";
    version.start_height = 0;
    version.nonce = rand();
    // Begin the send.
    // Calls version_sent callback when complete.
    node->send(version, std::bind(version_sent, _1, node));
    // Calls connect_stopped() when connection closes.
    node->subscribe_stop(connect_stopped);
}

void version_sent(const std::error_code& ec, channel_ptr node)
{
    if (ec)
        log_error() << "Sending version: " << ec.message();
    else
        log_info() << "Version sent.";
}

void connect_stopped(const std::error_code& ec)
{
    log_error() << "Connection stopped: " << ec.message();
}

int main()
{
    threadpool pool(1);
    network net(pool);
    net.connect("localhost", 8333, connect_started);
    std::cin.get();
    pool.stop();
    pool.join();
    return 0;
}


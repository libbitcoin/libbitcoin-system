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
  Accept connections from Bitcoin nodes on port 8333.
*/
#include <iostream>
#include <bitcoin/bitcoin.hpp>
using namespace bc;
using std::placeholders::_1;
using std::placeholders::_2;

// Listening has started.
// Wait to accept a connection.
void listening_started(const std::error_code& ec, acceptor_ptr accept);
// Connection to another Bitcoin node has been established.
// Wait for version message.
void accepted_connection(const std::error_code& ec, channel_ptr node,
    acceptor_ptr accept);
// Version message received.
// Display the user agent.
void version_received(const std::error_code& ec, const version_type& version,
    channel_ptr node);
void node_stopped(const std::error_code& ec);

void listening_started(const std::error_code& ec, acceptor_ptr accept)
{
    if (ec)
    {
        log_error() << "Listen failed: " << ec.message();
        return;
    }
    // Accept first connection.
    accept->accept(
        std::bind(accepted_connection, _1, _2, accept));
}

void accepted_connection(const std::error_code& ec, channel_ptr node,
    acceptor_ptr accept)
{
    if (ec)
    {
        log_error() << "Accept: " << ec.message();
        return;
    }
    log_info() << "Accepted connection!";
    node->subscribe_stop(node_stopped);
    // Now we need to keep it alive otherwise the connection is closed.
    // So we bind node to function call. Once all references expire,
    // the connection is closed.
    node->subscribe_version(
        std::bind(version_received, _1, _2, node));
    // Keep accepting more connections.
    accept->accept(
        std::bind(accepted_connection, _1, _2, accept));
}

void version_received(const std::error_code& ec, const version_type& version,
    channel_ptr node)
{
    // error::service_stopped means the connection was closed.
    if (ec == error::service_stopped)
        return;
    else if (ec)
    {
        log_error() << "Version message: " << ec.message();
        return;
    }
    // Display user agent field.
    log_info() << "User agent: " << version.user_agent;
}

void node_stopped(const std::error_code& ec)
{
    if (ec == error::service_stopped)
        log_info() << "Connection closed.";
    else if (ec)
        log_error() << "Connection closed: " << ec.message();
}

int main()
{
    // Start a threadpool with 4 threads.
    threadpool pool(4);
    // Networking component.
    network net(pool);
    // Start a listener on port 8333
    net.listen(8333, listening_started);
    // Wait...
    std::cin.get();
    // Stop threadpool and join threads.
    pool.stop();
    pool.join();
    return 0;
}


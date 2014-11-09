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
using namespace libbitcoin;

using std::placeholders::_1;
using std::placeholders::_2;

channel_ptr channode;

void error_exit(const std::string& message, int status=1)
{
    log_error() << "net: " << message;
    exit(status);
}

void handle_send(const std::error_code& ec)
{
    if (ec)
        error_exit(ec.message());
    log_info() << "handle_send";
}

void handle_stop(const std::error_code& ec)
{
    if (ec)
        log_error() << "Stopped: " << ec.message();
    log_info() << "Channel stopped.";
    channode->send(message::verack(), handle_send);
}

void handle_handshake(const std::error_code& ec, channel_ptr node,
    handshake_ptr hs)
{
    if (ec)
        error_exit(ec.message());
    log_info() << "Connected";
    channode = node;
    node->subscribe_stop(handle_stop);
}
void handle_init(const std::error_code& ec, handshake_ptr hs, network_ptr net)
{
    if (ec)
        error_exit(ec.message());
    hs->connect(net, "localhost", 8333,
        std::bind(&handle_handshake, _1, _2, hs));
}

int main()
{
    network_ptr net = std::make_shared<network>();
    handshake_ptr hs = std::make_shared<handshake>();
    hs->start(std::bind(handle_init, _1, hs, net));
    std::cin.get();
    return 0;
}


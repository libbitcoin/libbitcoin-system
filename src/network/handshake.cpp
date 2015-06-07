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
#include <bitcoin/bitcoin/network/handshake.hpp>

#include <functional>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/network.hpp>
#include <bitcoin/bitcoin/utility/async_parallel.hpp>
#include <bitcoin/bitcoin/version.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;

handshake::handshake(threadpool& pool)
  : strand_(pool.service())
{
    // Setup template version packet with defaults
    template_version_.version = protocol_version;
    template_version_.services = 1;
    // non-constant field
    //template_version_.timestamp = time(NULL);
    template_version_.address_me.services = template_version_.services;
    template_version_.address_me.ip = localhost_ip();
    template_version_.address_me.port = protocol_port;
    template_version_.address_you.services = template_version_.services;
    template_version_.address_you.ip = message::ip_address{
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x01
        }
    };
    template_version_.address_you.port = protocol_port;
    template_version_.user_agent = "/libbitcoin:" LIBBITCOIN_VERSION "/";
    template_version_.start_height = 0;
    template_version_.nonce = rand();
}

void handshake::start(start_handler handle_start)
{
    discover_external_ip(std::bind(handle_start, _1));
}

void handshake::ready(channel::pointer node,
    handshake::handshake_handler handle_handshake)
{
    auto completion_callback = async_parallel(handle_handshake, 3);

    message::announce_version session_version = template_version_;
    session_version.timestamp = time(NULL);
    node->send(session_version,
        strand_.wrap(std::bind(&handshake::handle_message_sent,
            this, _1, completion_callback)));

    node->subscribe_version(
        strand_.wrap(std::bind(&handshake::receive_version,
            this, _1, _2, node, completion_callback)));
    node->subscribe_verack(
        strand_.wrap(std::bind(&handshake::receive_verack,
            this, _1, _2, completion_callback)));
}

void handshake::handle_message_sent(const std::error_code& ec,
    handshake::handshake_handler completion_callback)
{
    completion_callback(ec);
}

void handshake::receive_version(
    const std::error_code& ec, const message::announce_version&,
    channel::pointer node, handshake::handshake_handler completion_callback)
{
    if (ec)
        completion_callback(ec);
    else
        node->send(message::verack(),
            strand_.wrap(std::bind(&handshake::handle_message_sent,
                this, _1, completion_callback)));
}

void handshake::receive_verack(
    const std::error_code& ec, const message::verack&,
    handshake::handshake_handler completion_callback)
{
    completion_callback(ec);
}

void handshake::discover_external_ip(discover_ip_handler handle_discover)
{
    strand_.post(
        std::bind(&handshake::do_discover_external_ip,
            this, handle_discover));
}

message::ip_address handshake::localhost_ip()
{
    return message::ip_address{
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x01
        }
    };
}

void handshake::do_discover_external_ip(discover_ip_handler handle_discover)
{
    template_version_.address_me.ip = localhost_ip();
    handle_discover(std::error_code(), template_version_.address_me.ip);
}

void handshake::fetch_network_address(
    fetch_network_address_handler handle_fetch)
{
    strand_.post(
        std::bind(&handshake::do_fetch_network_address,
            this, handle_fetch));
}
void handshake::do_fetch_network_address(
    fetch_network_address_handler handle_fetch)
{
    handle_fetch(std::error_code(), template_version_.address_me);
}

void handshake::set_port(uint16_t port, setter_handler handle_set)
{
    strand_.post(
        std::bind(&handshake::do_set_port,
            this, port, handle_set));
}
void handshake::do_set_port(uint16_t port, setter_handler handle_set)
{
    template_version_.address_me.port = port;
    handle_set(std::error_code());
}

void handshake::set_user_agent(const std::string& user_agent,
    setter_handler handle_set)
{
    strand_.post(
        std::bind(&handshake::do_set_user_agent,
            this, user_agent, handle_set));
}
void handshake::do_set_user_agent(const std::string& user_agent,
    setter_handler handle_set)
{
    template_version_.user_agent = user_agent;
    handle_set(std::error_code());
}

void handshake::set_start_height(uint32_t height, setter_handler handle_set)
{
    strand_.post(
        std::bind(&handshake::do_set_start_height,
            this, height, handle_set));
}
void handshake::do_set_start_height(uint32_t height, setter_handler handle_set)
{
    template_version_.start_height = height;
    handle_set(std::error_code());
}

void finish_connect(const std::error_code& ec,
    channel::pointer node, handshake& shake,
    network::connect_handler handle_connect)
{
    if (ec)
        handle_connect(ec, node);
    else
        shake.ready(node, std::bind(handle_connect, _1, node));
}

void connect(handshake& shake, network& net,
    const std::string& hostname, uint16_t port,
    network::connect_handler handle_connect)
{
    net.connect(hostname, port,
        std::bind(finish_connect, _1, _2, std::ref(shake), handle_connect));
}

} // namespace network
} // namespace libbitcoin

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

#include <cstdint>
#include <functional>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/network.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/utility/async_parallel.hpp>
#include <bitcoin/bitcoin/version.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;

#define BC_USER_AGENT "/libbitcoin:" LIBBITCOIN_VERSION "/"

handshake::handshake(threadpool& pool, uint16_t port, uint32_t start_height)
  : strand_(pool.service())
{
    // TODO: shouldn't the nonce change on every handshake (like timestamp)?
    template_version_.nonce = rand();
    // template_version_.timestamp = time(nullptr);

    // Set fixed values inversion template.
    template_version_.version = bc::protocol_version;
    template_version_.user_agent = BC_USER_AGENT;
    template_version_.services = 1;

    // Set default values inversion template.
    template_version_.start_height = start_height;
    template_version_.address_me.services = template_version_.services;
    template_version_.address_me.ip = bc::localhost_ip_address;
    template_version_.address_me.port = port;
    template_version_.address_you.services = template_version_.services;
    template_version_.address_you.ip = bc::localhost_ip_address;
    template_version_.address_you.port = port;
}

void handshake::start(start_handler handle_start)
{
    discover_external_ip(std::bind(handle_start, _1));
}

void handshake::ready(channel_ptr node,
    handshake::handshake_handler handle_handshake)
{
    constexpr size_t sync = 3;

    // synchrnize three code paths (or error) before calling handle_handshake.
    const auto completion_callback = async_parallel(handle_handshake, sync);

    // Copy the version template and set its timestamp.
    auto session_version = template_version_;
    session_version.timestamp = time(nullptr);

    // TODO: where does session_version get customized?
    // Since we removed cURL discover_external_ip always returns localhost.
    // The port value was formerly hardwired to bc::protocol_port.

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

void handshake::receive_version(const std::error_code& ec, const version_type&,
    channel_ptr node, handshake::handshake_handler completion_callback)
{
    if (ec)
        completion_callback(ec);
    else
        node->send(verack_type(),
            strand_.wrap(std::bind(&handshake::handle_message_sent,
                this, _1, completion_callback)));
}

void handshake::receive_verack(const std::error_code& ec, const verack_type&,
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

void handshake::do_discover_external_ip(discover_ip_handler handle_discover)
{
    template_version_.address_me.ip = localhost_ip_address;
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

// TODO: deprecate (any reason to set this dynamically)?
void handshake::set_user_agent(const std::string& user_agent,
    setter_handler handle_set)
{
    strand_.post(
        std::bind(&handshake::do_set_user_agent,
            this, user_agent, handle_set));
}

// TODO: deprecate (any reason to set this dynamically)?
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

void finish_connect(const std::error_code& ec, channel_ptr node,
    handshake& shake, network::connect_handler handle_connect)
{
    if (ec)
        handle_connect(ec, node);
    else
        shake.ready(node, std::bind(handle_connect, _1, node));
}

void connect(handshake& shake, network& net, const std::string& hostname,
    uint16_t port, network::connect_handler handle_connect)
{
    net.connect(hostname, port,
        std::bind(finish_connect,
            _1, _2, std::ref(shake), handle_connect));
}

} // namespace network
} // namespace libbitcoin


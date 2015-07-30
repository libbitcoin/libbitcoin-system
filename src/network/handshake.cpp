/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
#include <cstdlib>
#include <functional>
#include <system_error>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/network.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/utility/async_parallel.hpp>
#include <bitcoin/bitcoin/utility/random.hpp>
#include <bitcoin/bitcoin/version.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;

// unpublished for now
enum services : uint64_t
{
    node_network = 1
};

// unpublished for now
#define BC_USER_AGENT "/libbitcoin:" LIBBITCOIN_VERSION "/"

// This is a non-routable ipv6 mapping of an ipv4 local only address.
// This should be replaced with our own ip address detection.
BC_CONSTEXPR ip_address_type mapped_local_ip_address =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x01
};

handshake::handshake(threadpool& pool, uint16_t port, uint32_t start_height)
  : strand_(pool)
{
    // Set fixed values inversion template.
    template_version_.version = bc::protocol_version;
    template_version_.user_agent = BC_USER_AGENT;
    template_version_.services = services::node_network;

    // Set default values inversion template.
    template_version_.start_height = start_height;
    template_version_.address_me.services = template_version_.services;
    template_version_.address_me.ip = mapped_local_ip_address;
    template_version_.address_me.port = port;
    template_version_.address_you.services = template_version_.services;
    template_version_.address_you.ip = mapped_local_ip_address;
    template_version_.address_you.port = port;
}

void handshake::start(start_handler handle_start)
{
    discover_external_ip(std::bind(handle_start, _1));
}

// This will not fire the handshake completion until all three
// subscriptions fire, which will never happen if the peer doesn't complete
// the handshare or disconnect. So caller cannot depend on the handler.
void handshake::ready(channel_ptr node,
    handshake::handshake_handler handle_handshake)
{
    constexpr size_t sync = 3;

    // synchrnize three code paths (or error) before calling handle_handshake.
    const auto completion_callback = async_parallel(handle_handshake, sync);

    // Copy the version template and set its timestamp.
    auto session_version = template_version_;
    session_version.nonce = pseudo_random();
    session_version.timestamp = std::time(nullptr);

    // Since we removed cURL discover_external_ip always returns localhost.
    // The port value was formerly hardwired to bc::protocol_port.

    node->subscribe_version(
        strand_.wrap(&handshake::receive_version,
            this, _1, _2, node, completion_callback));

    node->subscribe_verack(
        strand_.wrap(&handshake::receive_verack,
            this, _1, _2, completion_callback));

    node->send(session_version,
        strand_.wrap(&handshake::handle_message_sent,
            this, _1, completion_callback));
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
            strand_.wrap(&handshake::handle_message_sent,
                this, _1, completion_callback));
}

void handshake::receive_verack(const std::error_code& ec, const verack_type&,
    handshake::handshake_handler completion_callback)
{
    completion_callback(ec);
}

void handshake::discover_external_ip(discover_ip_handler handle_discover)
{
    strand_.queue(
        std::bind(&handshake::do_discover_external_ip,
            this, handle_discover));
}

void handshake::do_discover_external_ip(discover_ip_handler handle_discover)
{
    template_version_.address_me.ip = mapped_local_ip_address;
    handle_discover(error::success, template_version_.address_me.ip);
}

void handshake::fetch_network_address(
    fetch_network_address_handler handle_fetch)
{
    strand_.queue(
        std::bind(&handshake::do_fetch_network_address,
            this, handle_fetch));
}

void handshake::do_fetch_network_address(
    fetch_network_address_handler handle_fetch)
{
    handle_fetch(error::success, template_version_.address_me);
}

void handshake::set_port(uint16_t port, setter_handler handle_set)
{
    strand_.queue(
        std::bind(&handshake::do_set_port,
            this, port, handle_set));
}

void handshake::do_set_port(uint16_t port, setter_handler handle_set)
{
    template_version_.address_me.port = port;
    handle_set(error::success);
}

// TODO: deprecate (any reason to set this dynamically)?
void handshake::set_user_agent(const std::string& user_agent,
    setter_handler handle_set)
{
    strand_.queue(
        std::bind(&handshake::do_set_user_agent,
            this, user_agent, handle_set));
}

// TODO: deprecate (any reason to set this dynamically)?
void handshake::do_set_user_agent(const std::string& user_agent,
    setter_handler handle_set)
{
    template_version_.user_agent = user_agent;
    handle_set(error::success);
}

void handshake::set_start_height(uint64_t height, setter_handler handle_set)
{
    strand_.queue(
        std::bind(&handshake::do_set_start_height,
            this, height, handle_set));
}

void handshake::do_set_start_height(uint64_t height, setter_handler handle_set)
{
    // We type this method as uint64_t because that is what is returned by
    // fetch_last_height, which feeds directly into this method. But start_height
    // is uint32_t in the satoshi network protocol.
    BITCOIN_ASSERT(height <= bc::max_uint32);
    template_version_.start_height = static_cast<uint32_t>(height);
    handle_set(error::success);
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


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
#ifndef LIBBITCOIN_NETWORK_HANDSHAKE_HPP
#define LIBBITCOIN_NETWORK_HANDSHAKE_HPP

#include <cstdint>
#include <string>
#include <system_error>
#include <boost/asio.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/announce_version.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/message/verack.hpp>
#include <bitcoin/bitcoin/network/network.hpp>
#include <bitcoin/bitcoin/utility/async_parallel.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class BC_API handshake
{
public:
    typedef std::function<void (const std::error_code&)> start_handler;

    typedef std::function<void (const std::error_code&)> handshake_handler;

    typedef std::function<void (
        const std::error_code&, const message::ip_address&)>
            discover_ip_handler;

    typedef std::function<void (
        const std::error_code&, const message::network_address&)>
            fetch_network_address_handler;

    typedef std::function<void (const std::error_code&)> setter_handler;

    handshake(threadpool& pool);

    handshake(const handshake&) = delete;
    void operator=(const handshake&) = delete;

    void start(start_handler handle_start);
    void ready(channel::pointer node, handshake_handler handle_handshake);
    void discover_external_ip(discover_ip_handler handle_discover);
    void fetch_network_address(fetch_network_address_handler handle_fetch);
    void set_port(uint16_t port, setter_handler handle_set);
    void set_user_agent(const std::string& user_agent,
        setter_handler handle_set);
    void set_start_height(uint32_t height, setter_handler handle_set);

private:
    void handle_connect(const std::error_code& ec,
        channel::pointer node, network::connect_handler handle_connect);

    void handle_message_sent(const std::error_code& ec,
        handshake::handshake_handler completion_callback);

    void receive_version(const std::error_code& ec,
        const message::announce_version&, channel::pointer node,
        handshake::handshake_handler completion_callback);

    void receive_verack(const std::error_code& ec, const message::verack&,
        handshake::handshake_handler completion_callback);

    message::ip_address localhost_ip();
    void do_discover_external_ip(discover_ip_handler handler_discover);
    void do_fetch_network_address(fetch_network_address_handler handle_fetch);
    void do_set_port(uint16_t port, setter_handler handle_set);
    void do_set_user_agent(const std::string& user_agent,
        setter_handler handle_set);
    void do_set_start_height(uint32_t height, setter_handler handle_set);

    boost::asio::io_service::strand strand_;
    message::announce_version template_version_;
};

BC_API void connect(handshake& shake, network& net,
    const std::string& hostname, uint16_t port,
    network::connect_handler handle_connect);

} // namespace network
} // namespace libbitcoin

#endif


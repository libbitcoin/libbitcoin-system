/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
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

#include <atomic>

#include <bitcoin/primitives.hpp>
#include <bitcoin/network/network.hpp>
#include <bitcoin/threadpool.hpp>
#include <bitcoin/utility/async_parallel.hpp>

namespace libbitcoin {

class handshake
{
public:
    typedef std::function<void (const std::error_code&)> start_handler;

    typedef std::function<void (const std::error_code&)> handshake_handler;

    typedef std::function<void (
        const std::error_code&, const ip_address_type&)>
            discover_ip_handler;

    typedef std::function<void (
        const std::error_code&, const network_address_type&)>
            fetch_network_address_handler;

    typedef std::function<void (const std::error_code&)> setter_handler;

    handshake(threadpool& pool);

    handshake(const handshake&) = delete;
    void operator=(const handshake&) = delete;

    void start(start_handler handle_start);

    void ready(channel_ptr node, handshake_handler handle_handshake);

    void discover_external_ip(discover_ip_handler handle_discover);
    void fetch_network_address(fetch_network_address_handler handle_fetch);
    void set_port(uint16_t port, setter_handler handle_set);
    void set_user_agent(const std::string& user_agent,
        setter_handler handle_set);
    void set_start_height(uint32_t height, setter_handler handle_set);

private:
    void handle_connect(const std::error_code& ec,
        channel_ptr node, network::connect_handler handle_connect);

    void handle_message_sent(const std::error_code& ec,
        handshake::handshake_handler completion_callback);

    void receive_version(const std::error_code& ec, const version_type&,
        channel_ptr node, handshake::handshake_handler completion_callback);

    void receive_verack(const std::error_code& ec, const verack_type&,
        handshake::handshake_handler completion_callback);

    bool lookup_external(const std::string& website,
        ip_address_type& ip);
    ip_address_type localhost_ip();
    void do_discover_external_ip(discover_ip_handler handler_discover);
    void do_fetch_network_address(fetch_network_address_handler handle_fetch);
    void do_set_port(uint16_t port, setter_handler handle_set);
    void do_set_user_agent(const std::string& user_agent,
        setter_handler handle_set);
    void do_set_start_height(uint32_t height, setter_handler handle_set);

    io_service::strand strand_;
    version_type template_version_;
};

void connect(handshake& shake, network& net,
    const std::string& hostname, uint16_t port,
    network::connect_handler handle_connect);

} // namespace libbitcoin

#endif


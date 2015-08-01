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
#ifndef LIBBITCOIN_HANDSHAKE_HPP
#define LIBBITCOIN_HANDSHAKE_HPP

#include <cstdint>
#include <string>
#include <system_error>
#include <boost/asio.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/network/network.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/utility/async_strand.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class BC_API handshake
{
public:
    typedef std::function<void(const std::error_code&)> handshake_handler;
    typedef std::function<void (const std::error_code&)> setter_handler;

    static const network_address_type unspecified;

    handshake(threadpool& pool, const config::authority& self=unspecified);

    /// This class is not copyable.
    handshake(const handshake&) = delete;
    void operator=(const handshake&) = delete;

    void ready(channel_ptr node, handshake_handler handle_handshake,
        bool relay=true);
    void set_start_height(uint64_t height, setter_handler handle_set);

private:
    void handle_connect(const std::error_code& ec, channel_ptr node,
        network::connect_handler handle_connect);
    void handle_version_sent(const std::error_code& ec,
        channel_ptr node, handshake_handler handle_handshake);
    void handle_verack_sent(const std::error_code& ec,
        handshake_handler handle_handshake);
    void receive_version(const std::error_code& ec, 
        const version_type& version, channel_ptr node,
        handshake_handler handle_handshake);
    void receive_verack(const std::error_code& ec, const verack_type&,
        channel_ptr node, handshake_handler handle_handshake);

    void do_set_start_height(uint64_t height, setter_handler handle_set);

    async_strand strand_;
    version_type template_version_;
};

BC_API void connect(handshake& shake, network& net,
    const std::string& hostname, uint16_t port,
    network::connect_handler handle_connect, bool relay=true);

} // namespace network
} // namespace libbitcoin

#endif


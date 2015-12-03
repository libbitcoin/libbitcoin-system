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
#ifndef LIBBITCOIN_NETWORK_PROTOCOL_ADDRESS_HPP
#define LIBBITCOIN_NETWORK_PROTOCOL_ADDRESS_HPP

#include <memory>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/message/address.hpp>
#include <bitcoin/bitcoin/message/get_address.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/protocol_events.hpp>
#include <bitcoin/bitcoin/network/p2p.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

/**
 * Address protocol.
 * Attach this to a channel immediately following handshake completion.
 */
class BC_API protocol_address
  : public protocol_events, track<protocol_address>
{
public:
    typedef std::shared_ptr<protocol_address> ptr;

    /**
     * Construct an address protocol instance.
     * @param[in]  pool      The thread pool used by the protocol.
     * @param[in]  network   The network interface.
     * @param[in]  channel   The channel on which to start the protocol.
     */
    protocol_address(threadpool& pool, p2p& network, channel::ptr channel);

    /**
     * Start the protocol.
     * @param[in]  settings  Configuration settings.
     */
    void start(const settings& settings);

private:
    void handle_send_address(const code& ec);
    void handle_send_get_address(const code& ec);
    void handle_store_addresses(const code& ec);

    bool handle_receive_address(const code& ec,
        const message::address& address);
    bool handle_receive_get_address(const code& ec,
        const message::get_address& message);

    p2p& network_;
    message::address self_;
};

} // namespace network
} // namespace libbitcoin

#endif

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
#ifndef LIBBITCOIN_NETWORK_PROTOCOL_PING_HPP
#define LIBBITCOIN_NETWORK_PROTOCOL_PING_HPP

#include <memory>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/message/ping.hpp>
#include <bitcoin/bitcoin/message/pong.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/p2p.hpp>
#include <bitcoin/bitcoin/network/protocol_timer.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {
        
/**
 * Ping-pong protocol.
 * Attach this to a channel immediately following handshake completion.
 */
class BC_API protocol_ping
  : public protocol_timer, track<protocol_ping>
{
public:
    typedef std::shared_ptr<protocol_ping> ptr;

    /**
     * Construct a ping protocol instance.
     * @param[in]  pool      The thread pool used by the protocol.
     * @param[in]  channel   The channel on which to start the protocol.
     */
    protocol_ping(threadpool& pool, p2p&, channel::ptr channel);

    /**
     * Start the protocol.
     * @param[in]  settings  Configuration settings.
     */
    void start(const settings& settings);

private:
    void send_ping(const code& ec);

    void handle_send_ping(const code& ec);
    void handle_send_pong(const code& ec);

    bool handle_receive_ping(const code& ec, const message::ping& message);
    bool handle_receive_pong(const code& ec, const message::pong& message,
        uint64_t nonce);
};

} // namespace network
} // namespace libbitcoin

#endif

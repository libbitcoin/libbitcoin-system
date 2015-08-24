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
#include <boost/date_time.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/message/ping_pong.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/timeout.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {
        
/**
 * Ping protocol.
 * Attach this to a node immediately following handshake completion.
 */
class BC_API protocol_ping
  : public std::enable_shared_from_this<protocol_ping>
{
public:
    typedef std::shared_ptr<protocol_ping> ptr;

    /**
     * Construct a ping protocol instance.
     * @param[in]  peer    The channel on which to start the protocol.
     * @param[in]  pool    The thread pool used by the protocol.
     * @param[in]  period  The time period of outgoing ping messages.
     */
    protocol_ping(channel::ptr peer, threadpool& pool, 
        const boost::posix_time::time_duration& period);

    /// This class is not copyable.
    protocol_ping(const protocol_ping&) = delete;
    void operator=(const protocol_ping&) = delete;
    
    /**
     * Start the protocol on the configured channel.
     */
    void start();

private:
    bool stopped() const;
    void handle_stop(const code& ec);
    void handle_timer(const code& ec);

    void handle_send_ping(const code& ec) const;
    void handle_send_pong(const code& ec) const;
    void handle_receive_ping(const code& ec, const message::ping& ping);
    void handle_receive_pong(const code& ec, const message::pong& ping,
        uint64_t nonce);

    channel::ptr peer_;
    deadline::ptr deadline_;
    dispatcher dispatch_;
    bool stopped_;
};

} // namespace network
} // namespace libbitcoin

#endif

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
#include <system_error>
#include <boost/date_time.hpp>
#include <boost/system/error_code.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

/**
 * Address protocol.
 * Attach this to a node immediately following handshake completion.
 */
class BC_API protocol_address
  : public std::enable_shared_from_this<protocol_address>
{
public:
    typedef std::shared_ptr<protocol_address> ptr;
    typedef std::function<void(const std::error_code&)> handler;

    /**
     * Construct an address protocol instance.
     * @param[in]  peer   The channel on which to start the protocol.
     * @param[in]  pool   The thread pool used by the protocol.
     * @param[in]  hosts  The address pool that this class populates.
     * @param[in]  self   The address that represents us to peers.
     */
    protocol_address(channel_ptr peer, threadpool& pool, hosts& hosts,
        const message::network_address& self);

    /// This class is not copyable.
    protocol_address(const protocol_address&) = delete;
    void operator=(const protocol_address&) = delete;
    
    /**
     * Start the protocol on the configured channel.
     */
    void start();

    /**
     * Start the protocol on the configured channel.
     * This override will invoke the completion handler after handling the
     * first address message from the peer or timeout.
     * @param[in]  handle_seeded  Will be invoked upon expire or completion.
     * @param[in]  timeout        The time limit on get address completion.
     */
    void start(handler handle_seeded,
        const boost::posix_time::time_duration& timeout);

private:
    bool stopped() const;
    void handle_stop(const std::error_code& ec, handler complete);
    void handle_timer(const std::error_code& ec, handler complete) const;

    message::network_address address() const;
    bool publish_self_address() const;

    void handle_receive_address(const std::error_code& ec,
        const message::address& address, handler complete);
    void handle_send_address(const std::error_code& ec, handler complete) const;
    void handle_send_get_address(const std::error_code& ec, handler complete) const;
    void handle_store_addresses(const std::error_code& ec, handler complete) const;

    channel_ptr peer_;
    threadpool& pool_;
    dispatcher dispatch_;
    deadline::ptr deadline_;
    bool stopped_;

    hosts& hosts_;
    const message::address self_;
};

} // namespace network
} // namespace libbitcoin

#endif

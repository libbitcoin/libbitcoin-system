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
#ifndef LIBBITCOIN_NETWORK_PROTOCOL_HANDSHAKE_HPP
#define LIBBITCOIN_NETWORK_PROTOCOL_HANDSHAKE_HPP

#include <cstdint>
#include <memory>
#include <system_error>
#include <boost/date_time.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/message/verack.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {
    
/**
 * Version protocol (handshake).
 * Attach this to a node immediately following socket creation.
 */
class BC_API protocol_version
  : public std::enable_shared_from_this<protocol_version>
{
public:
    typedef std::shared_ptr<protocol_version> ptr;
    typedef std::function<void(const std::error_code&)> handler;
    typedef std::function<void(const std::error_code&)> set_height_handler;

    /**
     * Construct a version protocol instance.
     * @param[in]  peer     The channel on which to start the protocol.
     * @param[in]  pool     The thread pool used by the protocol.
     * @param[in]  timeout  The time limit on handshake completion.
     * @param[in]  self     The address that represents us to peers.
     * @param[in]  relay    Set relay in version message to peer.
     */
    protocol_version(channel_ptr peer, threadpool& pool,
        const boost::posix_time::time_duration& timeout,
        const config::authority& self=unspecified_network_address,
        bool relay=true);

    /// This class is not copyable.
    protocol_version(const protocol_version&) = delete;
    void operator=(const protocol_version&) = delete;
    
    /**
     * Start the protocol on the configured channel.
     * @param[in]  handle_handshake  Will be invoked upon expire or completion.
     */
    void start(handler handle_handshake);

    /**
     * Start the current blockchain height into the version template.
     * @param[in]  height      The height to set.
     * @param[in]  handle_set  Will be invoked upon set completion.
     */
    void set_height(uint64_t height, set_height_handler handle_set);

private:
    bool stopped() const;
    void handle_stop(const std::error_code& ec, handler complete);
    void handle_timer(const std::error_code& ec, handler complete) const;

    void handle_version_sent(const std::error_code& ec, handler complete) const;
    void handle_verack_sent(const std::error_code& ec, handler complete) const;
    void handle_receive_version(const std::error_code& ec, 
        const message::version& version, handler complete);
    void handle_receive_verack(const std::error_code& ec, 
        const message::verack&, handler complete) const;

    void do_set_height(uint64_t height, set_height_handler handle_set);

    channel_ptr peer_;
    dispatcher dispatch_;
    deadline::ptr deadline_;
    message::version template_;
    bool stopped_;
    bool relay_;
};

} // namespace network
} // namespace libbitcoin

#endif


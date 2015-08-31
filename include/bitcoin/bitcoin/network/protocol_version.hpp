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
#ifndef LIBBITCOIN_NETWORK_PROTOCOL_VERSION_HPP
#define LIBBITCOIN_NETWORK_PROTOCOL_VERSION_HPP

#include <cstdint>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/message/verack.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/protocol_base.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {
    
/**
 * Version protocol (handshake).
 * Attach this to a node immediately following socket creation.
 */
class BC_API protocol_version
  : public protocol_base<protocol_version>, track<protocol_version>
{
public:
    /**
     * Start a version protocol instance.
     * @param[in]  channel   The channel on which to start the protocol.
     * @param[in]  pool      The thread pool used by the protocol.
     * @param[in]  timeout   The timer period.
     * @param[in]  complete  Callback invoked upon stop or complete.
     * @param[in]  height    The current height of the local blockchain.
     * @param[in]  self      The authority that represents us to this peer.
     * @param[in]  relay     Set relay in version message to peer.
     */
    protocol_version(channel::ptr channel, threadpool& pool,
        const asio::duration& timeout, handler complete, hosts& hosts,
        const config::authority& self, uint32_t height, bool relay);

    /**
     * Starts the protocol, release any reference after calling.
     */
    void start() override;

private:
    static handler synchronizer(handler complete);

    void handle_receive_version(const code& ec,
        const message::version& version);
    void handle_receive_verack(const code& ec, const message::verack&);
    void handle_version_sent(const code& ec);
    void handle_verack_sent(const code& ec);

    static const message::version template_;

    message::version version_;
};

} // namespace network
} // namespace libbitcoin

#endif


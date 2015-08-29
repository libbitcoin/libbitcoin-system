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
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/protocol_base.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

/**
 * Address protocol.
 * Attach this to a node immediately following handshake completion.
 */
class BC_API protocol_address
  : public protocol_base, track<protocol_address>
{
public:
    /**
     * Start an address protocol instance.
     * @param[in]  peer   The channel on which to start the protocol.
     * @param[in]  pool   The thread pool used by the protocol.
     * @param[in]  hosts  The address pool that this class populates.
     * @param[in]  self   The authority that represents us to this peer.
     */
    protocol_address(channel::ptr peer, threadpool& pool, hosts& hosts,
        const config::authority& self);

    /**
     * Starts the protocol, release any reference after calling.
     */
    void start() override;

private:
    void handle_receive_address(const code& ec,
        const message::address& address);
    void handle_receive_get_address(const code& ec,
        const message::get_address& message);
    void handle_send_address(const code& ec);
    void handle_send_get_address(const code& ec);
    void handle_store_addresses(const code& ec);

    hosts& hosts_;
    const config::authority self_;
};

} // namespace network
} // namespace libbitcoin

#endif

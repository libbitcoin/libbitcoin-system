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
#ifndef LIBBITCOIN_NETWORK_PROTOCOL_SEED_HPP
#define LIBBITCOIN_NETWORK_PROTOCOL_SEED_HPP

#include <memory>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/message/address.hpp>
#include <bitcoin/bitcoin/message/get_address.hpp>
#include <bitcoin/bitcoin/network/asio.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/p2p.hpp>
#include <bitcoin/bitcoin/network/protocol_base.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

/**
 * Seeding protocol.
 * Attach this to a channel immediately following seed handshake completion.
 */
class BC_API protocol_seed
  : public protocol_base<protocol_seed>, track<protocol_seed>
{
public:
    typedef std::shared_ptr<protocol_seed> ptr;

    /**
     * Start a seed protocol instance.
     * @param[in]  pool      The thread pool used by the protocol.
     * @param[in]  network   The network interface.
     * @param[in]  settings  Configuration settings.
     * @param[in]  channel   The channel on which to start the protocol.
     * @param[in]  handler   Callback invoked upon stop or complete.
     */
    protocol_seed(threadpool& pool, p2p& network, const settings& settings,
        channel::ptr channel, completion_handler handler);

    /**
     * Starts the protocol, release any reference after calling.
     */
    void start() override;

private:
    static completion_handler synchronizer_factory(completion_handler handler);

    void handle_receive_address(const code& ec,
        const message::address& address);
    void handle_receive_get_address(const code& ec,
        const message::get_address& message);
    void handle_send_address(const code& ec);
    void handle_send_get_address(const code& ec);
    void handle_store_addresses(const code& ec);

    p2p& network_;
    const config::authority self_;
    bool disabled_;
};

} // namespace network
} // namespace libbitcoin

#endif

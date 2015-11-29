/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_NETWORK_SESSION_BATCH_HPP
#define LIBBITCOIN_NETWORK_SESSION_BATCH_HPP

#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/connector.hpp>
#include <bitcoin/bitcoin/network/network_settings.hpp>
#include <bitcoin/bitcoin/network/session.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class p2p;

/// Intermediate base class for adding batch connect sequence.
class BC_API session_batch
  : public session
{
protected:

    /// Construct an instance.
    session_batch(threadpool& pool, p2p& network, const settings& settings,
        bool persistent);

    /// Create a channel from the configured number of concurrent attempts.
    void connect(connector::ptr connect, channel_handler handler);

private:

    // Connect sequence
    void new_connect(connector::ptr connect, channel_handler handler);
    void start_connect(const code& ec, const authority& host,
        connector::ptr connect, channel_handler handler);
    void handle_connect(const code& ec, channel::ptr channel,
        const authority& host, connector::ptr connect,
        channel_handler handler);
};

} // namespace network
} // namespace libbitcoin

#endif

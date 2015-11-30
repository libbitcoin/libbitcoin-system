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

#include <cstddef>
#include <cstdint>
#include <memory>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/message/verack.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/network_settings.hpp>
#include <bitcoin/bitcoin/network/p2p.hpp>
#include <bitcoin/bitcoin/network/protocol_timer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class BC_API protocol_version
  : public protocol_timer, track<protocol_version>
{
public:
    typedef std::shared_ptr<protocol_version> ptr;

    /**
     * Construct a version protocol instance.
     * @param[in]  pool      The thread pool used by the protocol.
     * @param[in]  network   The network interface.
     * @param[in]  channel   The channel on which to start the protocol.
     */
    protocol_version(threadpool& pool, p2p&, channel::ptr channel);
    
    /**
     * Start the protocol.
     * @param[in]  settings Configuration settings.
     * @param[in]  height   Our current blockchain height.
     * @param[in]  handler  Invoked upon stop or receipt of version and verack.
     */
    void start(const settings& settings, size_t height, event_handler handler);

private:
    static message::version template_factory(
        const config::authority& authority, const settings& settings,
        uint64_t nonce, size_t height);

    void handle_version_sent(const code& ec);
    void handle_verack_sent(const code& ec);
    void handle_receive_version(const code& ec,
        const message::version& version);
    void handle_receive_verack(const code& ec, const message::verack&);

    static const message::version template_;
};

} // namespace network
} // namespace libbitcoin

#endif


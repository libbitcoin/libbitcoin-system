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
#include <bitcoin/bitcoin/network/protocol_version.hpp>

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <functional>
#include <boost/date_time.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/message/verack.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/random.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>
#include <bitcoin/bitcoin/version.hpp>

INITIALIZE_TRACK(bc::network::protocol_version);

namespace libbitcoin {
namespace network {

using namespace bc::message;
using std::placeholders::_1;
using std::placeholders::_2;
#define CLASS protocol_version

const message::version protocol_version::version_template
{
    bc::protocol_version,
    services::node_network,
    no_timestamp,
    bc::unspecified_network_address,
    bc::unspecified_network_address,
    0,
    BC_USER_AGENT,
    0,
    true
};

protocol_version::protocol_version(channel::ptr channel, threadpool& pool,
    const asio::duration& timeout, handler complete, hosts& hosts,
    const config::authority& self, bool relay)
  : version_(version_template),
    protocol_base(channel, pool, timeout, synchronize(complete, 3, "version")),
    CONSTRUCT_TRACK(protocol_version, LOG_NETWORK)
{
    // Set required transaction relay policy for the connection.
    version_.relay = relay;

    // The timestamp should not used here and services should be set in config.
    version_.address_me = self.to_network_address();

    // The timestamp should not used here and there's no need to set services.
    version_.address_you = authority().to_network_address();

    // The nonce is used to detect connections to self.
    version_.nonce = pseudo_random();

    // TODO: Add nonce to channel state for loopback detection in the context.
    channel_->set_nonce(version_.nonce);
}

void protocol_version::start()
{
    protocol_base::start();

    SUBSCRIBE2(version, handle_receive_version, _1, _2);
    SUBSCRIBE2(verack, handle_receive_verack, _1, _2);
    SEND1(version_, handle_version_sent, _1);
}

void protocol_version::handle_receive_version(const code& ec,
    const version& message)
{
    if (stopped())
        return;

    if (ec)
    {
        callback(ec);
        return;
    }

    if (message.value < bc::peer_minimum_version)
    {
        log_debug(LOG_NETWORK)
            << "Peer version (" << message.value << ") below minimum ("
            << bc::peer_minimum_version << ") [" << authority() << "]";
        callback(error::accept_failed);
        return;
    }

    // TODO: add loopback detection to the channel.
    // TODO: set the protocol version on peer (for feature degradation).
    // TODO: save relay to peer and have protocol not relay if false.
    // TODO: trace out version.version|services|user_agent.
    // peer->set_version(version);
    log_debug(LOG_NETWORK)
        << "Peer version [" << authority() << "] ("
        << message.value << ") " << message.user_agent;

    SEND1(verack(), handle_verack_sent, _1);
}

void protocol_version::handle_verack_sent(const code& ec) const
{
    if (stopped())
        return;

    // 1 of 3
    callback(ec);
}

void protocol_version::handle_version_sent(const code& ec) const
{
    if (stopped())
        return;

    // 2 of 3
    callback(ec);
}

void protocol_version::handle_receive_verack(const code& ec,
    const message::verack&) const
{
    if (stopped())
        return;

    // 3 of 3
    // We may not get the verack before timeout, in which case we can only
    //assume that our version wasn't accepted.
    callback(ec);
}

// TODO: move to session (context).
void protocol_version::set_height(uint64_t height, handler handle)
{
    if (stopped())
        return;

    dispatch_.ordered(BIND2(do_set_height, height, handle));
}

void protocol_version::do_set_height(uint64_t height, handler handle)
{
    if (stopped())
        return;

    // We type this method as uint64_t because that is what is returned by
    // fetch_last_height, which feeds directly into this method. But start_height
    // is uint32_t in the satoshi network protocol.
    BITCOIN_ASSERT(height <= bc::max_uint32);
    version_.start_height = static_cast<uint32_t>(height);
    handle(error::success);
}

#undef CLASS

} // namespace network
} // namespace libbitcoin

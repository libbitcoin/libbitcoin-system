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
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/random.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/version.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;
using boost::posix_time::time_duration;

protocol_version::protocol_version(channel::ptr peer, threadpool& pool,
    const time_duration& timeout, const config::authority& self, bool relay)
  : peer_(peer),
    dispatch_(pool),
    deadline_(std::make_shared<deadline>(pool, timeout)),
    stopped_(false),
    relay_(relay)
{
    // nonce, relay and address_you are set in start.
    // start_height is managed dynamically by channel.
    template_.value = bc::protocol_version;
    template_.services = services::node_network;
    template_.timestamp = 0;
    template_.address_me = self.to_network_address();
    template_.address_me.services = services::node_network;
    template_.address_me.timestamp = no_timestamp;
    template_.address_you = unspecified_network_address;
    template_.nonce = 0;
    template_.user_agent = BC_USER_AGENT;
    template_.start_height = 0;
    template_.relay = true;
}

// This will not fire the handshake completion until all three
// subscriptions or the timer fires. synchronize guards against the
// possiblity of conflicting timer completion callbacks.
void protocol_version::start(handler handle_handshake)
{
    // Require three callbacks (or any error) before calling handle_handshake.
    const auto complete = synchronize(handle_handshake, 3, "handshake");

    // Create a copy of the version template.
    auto session_version = template_;

    // Set required transaction relay policy for the connection.
    session_version.relay = relay_;

    // TODO: bury the address_you-setting into the proxy.
    // Set the peer's address into the outgoing version.
    // The timestamp should not used here and there's no need to set services.
    session_version.address_you = peer_->address().to_network_address();

    // TODO: bury the nonce-setting into the proxy.
    // The nonce is used to detect connections to self. It is chosen at random
    // for each connection to minimize persistent collisions.
    // Add nonce to channel state for loopback detection.
    session_version.nonce = pseudo_random();
    peer_->set_nonce(session_version.nonce);

    // Subscribe to stop messages.
    peer_->subscribe_stop(
        dispatch_.sync(&protocol_version::handle_stop,
            shared_from_this(), _1, complete));

    // 1 of 3
    // Subscribe to version messages.
    peer_->subscribe_version(
        dispatch_.sync(&protocol_version::handle_receive_version,
            shared_from_this(), _1, _2, complete));

    // 2 of 3
    // Subscribe to verack messages.
    peer_->subscribe_verack(
        dispatch_.sync(&protocol_version::handle_receive_verack,
            shared_from_this(), _1, _2, complete));

    // 3 of 3
    // Send version message.
    peer_->send(session_version,
        dispatch_.sync(&protocol_version::handle_version_sent,
            shared_from_this(), _1, complete));

    // Subscribe to timeout event.
    deadline_->start(
        std::bind(&protocol_version::handle_timer,
            shared_from_this(), _1, complete));
}

void protocol_version::handle_stop(const code& ec,
    handler complete)
{
    if (stopped())
        return;

    stopped_ = true;

    if (deadline_)
        deadline_->cancel();
}

bool protocol_version::stopped() const
{
    return stopped_;
}

void protocol_version::handle_timer(const code& ec,
    handler complete) const
{
    if (stopped())
        return;

    if (!deadline::canceled(ec))
        complete(error::channel_timeout);
}

void protocol_version::handle_version_sent(const code& ec,
    handler complete) const
{
    if (stopped())
        return;

    complete(ec);
}

void protocol_version::handle_receive_version(const code& ec,
    const message::version& version, handler complete)
{
    if (stopped())
        return;

    if (ec)
    {
        complete(ec);
        return;
    }

    if (version.value < bc::peer_minimum_version)
    {
        log_debug(LOG_NETWORK)
            << "Peer version (" << version.value << ") below minimum ("
            << bc::peer_minimum_version << ") [" << peer_->address() << "]";
        complete(error::accept_failed);
        return;
    }

    // TODO: add loopback detection to the channel.
    // TODO: set the protocol version on peer (for feature degradation).
    // TODO: save relay to peer and have protocol not relay if false.
    // TODO: trace out version.version|services|user_agent.
    // peer->set_version(version);
    log_debug(LOG_NETWORK)
        << "Peer version [" << peer_->address() << "] ("
        << version.value << ") " << version.user_agent;

    peer_->send(message::verack(),
        dispatch_.sync(&protocol_version::handle_verack_sent,
            shared_from_this(), _1, complete));
}

void protocol_version::handle_verack_sent(const code& ec,
    handler complete) const
{
    if (stopped())
        return;

    complete(ec);
}

void protocol_version::handle_receive_verack(const code& ec,
    const message::verack&, handler complete) const
{
    if (stopped())
        return;

    // We may not get the verack before timeout, in which case we can only
    //assume that our version wasn't accepted.
    complete(ec);
}

void protocol_version::set_height(uint64_t height,
    set_height_handler handle_set)
{
    if (stopped())
        return;

    dispatch_.queue(
        std::bind(&protocol_version::do_set_height,
            shared_from_this(), height, handle_set));
}

void protocol_version::do_set_height(uint64_t height,
    set_height_handler handle_set)
{
    if (stopped())
        return;

    // We type this method as uint64_t because that is what is returned by
    // fetch_last_height, which feeds directly into this method. But start_height
    // is uint32_t in the satoshi network protocol.
    BITCOIN_ASSERT(height <= bc::max_uint32);
    template_.start_height = static_cast<uint32_t>(height);
    handle_set(error::success);
}

} // namespace network
} // namespace libbitcoin

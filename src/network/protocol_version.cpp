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

#define NAME "version"
#define CLASS protocol_version
#define RELAY_TRUE true
#define GENESIS_HEIGHT 0
#define UNSPECIFIED_NONCE 0

using namespace bc::message;
using std::placeholders::_1;
using std::placeholders::_2;

const message::version protocol_version::template_
{
    bc::protocol_version,
    services::node_network,
    no_timestamp,
    bc::unspecified_network_address,
    bc::unspecified_network_address,
    UNSPECIFIED_NONCE,
    BC_USER_AGENT,
    GENESIS_HEIGHT,
    RELAY_TRUE
};

protocol_base::handler protocol_version::synchronizer(handler complete)
{
    return synchronize(complete, 3, NAME);
}

protocol_version::protocol_version(channel::ptr channel, threadpool& pool,
    const asio::duration& timeout, handler complete, hosts& hosts,
    const config::authority& self, uint32_t height, bool relay)
  : version_(template_),
    protocol_base(channel, pool, timeout, NAME, synchronizer(complete)),
    CONSTRUCT_TRACK(protocol_version, LOG_NETWORK)
{
    // The timestamp should not used here and there's no need to set services.
    version_.address_you = authority().to_network_address();

    // The timestamp should not used here and services should be set in config.
    version_.address_me = self.to_network_address();

    // It is expected that the version is constructed shortly before use.
    version_.start_height = height;

    // Set required transaction relay policy for the connection.
    version_.relay = relay;

    // The nonce is used to detect connections to self.
    version_.nonce = pseudo_random();

    // TODO: Add nonce to channel state for loopback detection in the context.
    set_identifier(version_.nonce);
}

void protocol_version::start()
{
    protocol_base::start();

    subscribe<protocol_version, version>(&protocol_version::handle_receive_version, _1, _2);
    subscribe<protocol_version, verack>(&protocol_version::handle_receive_verack, _1, _2);
    send<protocol_version>(version_, &protocol_version::handle_version_sent, _1);
}

void protocol_version::handle_receive_version(const code& ec,
    const version& message)
{
    if (stopped())
        return;

    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Failure receiving version from [" << authority() << "] "
            << ec.message();
        stop(ec);
        return;
    }

    if (message.value < bc::peer_minimum_version)
    {
        log_debug(LOG_NETWORK)
            << "Peer version (" << message.value << ") below minimum ("
            << bc::peer_minimum_version << ") [" << authority() << "]";
        stop(error::accept_failed);
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

    send<protocol_version>(verack(), &protocol_version::handle_verack_sent, _1);
}

void protocol_version::handle_verack_sent(const code& ec)
{
    if (stopped())
        return;

    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Failure sending verack to [" << authority() << "] "
            << ec.message();
        stop(ec);
        return;
    }

    // 1 of 3
    callback(error::success);
}

void protocol_version::handle_version_sent(const code& ec)
{
    if (stopped())
        return;

    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Failure sending version to [" << authority() << "] "
            << ec.message();
        stop(ec);
        return;
    }

    // 2 of 3
    callback(error::success);
}

void protocol_version::handle_receive_verack(const code& ec,
    const message::verack&)
{
    if (stopped())
        return;

    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Failure receiving verack from [" << authority() << "] "
            << ec.message();
        stop(ec);
        return;
    }

    // 3 of 3
    callback(error::success);
}

} // namespace network
} // namespace libbitcoin

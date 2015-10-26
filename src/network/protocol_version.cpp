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
#include <bitcoin/bitcoin/message/verack.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/network_settings.hpp>
#include <bitcoin/bitcoin/network/p2p.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/log.hpp>
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

protocol::completion_handler protocol_version::synchronizer_factory(
    completion_handler handler)
{
    return synchronize(handler, 3, NAME);
}

message::version protocol_version::template_factory(channel::ptr channel,
    const settings& settings, size_t height)
{
    auto version = protocol_version::template_;

    // The timestamp should not used here and there's no need to set services.
    version.address_you = channel->address().to_network_address();

    // The timestamp should not used here and services should be set in config.
    version.address_me = settings.self.to_network_address();

    // It is expected that the version is constructed shortly before use.
    BITCOIN_ASSERT_MSG(height < max_uint32, "Time to upgrade the protocol.");
    version.start_height = static_cast<uint32_t>(height);

    // Set required transaction relay policy for the connection.
    version.relay = settings.relay_transactions;

    // A non-zero nonce is used to detect connections to self.
    version.nonce = channel->nonce();
    return version;
}

protocol_version::protocol_version(threadpool& pool, p2p&,
    const settings& settings, channel::ptr channel, size_t height,
    completion_handler handler)
  : protocol_base(pool, channel, settings.channel_handshake(), NAME,
        synchronizer_factory(handler)),
    height_(height),
    version_(template_factory(channel, settings, height)),
    CONSTRUCT_TRACK(protocol_version, LOG_PROTOCOL)
{
}

void protocol_version::start()
{
    protocol_base::start();

    subscribe<version>(&protocol_version::handle_receive_version, _1, _2);
    subscribe<verack>(&protocol_version::handle_receive_verack, _1, _2);
    send(version_, &protocol_version::handle_version_sent, _1);
}

void protocol_version::handle_receive_version(const code& ec,
    const version& message)
{
    if (stopped())
        return;

    if (ec)
    {
        log::error(LOG_PROTOCOL)
            << "Failure receiving version from [" << authority() << "] "
            << ec.message();
        stop(ec);
        return;
    }

    log::debug(LOG_PROTOCOL)
        << "Peer [" << authority() << "] version (" << message.value
        << ") services (" << message.services << ") " << message.user_agent;

    set_version(message);
    send(verack(), &protocol_version::handle_verack_sent, _1);
}

void protocol_version::handle_verack_sent(const code& ec)
{
    if (stopped())
        return;

    if (ec)
    {
        log::error(LOG_PROTOCOL)
            << "Failure sending verack to [" << authority() << "] "
            << ec.message();
        stop(ec);
        return;
    }

    // 1 of 3
    complete(error::success);
}

void protocol_version::handle_version_sent(const code& ec)
{
    if (stopped())
        return;

    if (ec)
    {
        log::error(LOG_PROTOCOL)
            << "Failure sending version to [" << authority() << "] "
            << ec.message();
        stop(ec);
        return;
    }

    // 2 of 3
    complete(error::success);
}

void protocol_version::handle_receive_verack(const code& ec,
    const message::verack&)
{
    if (stopped())
        return;

    if (ec)
    {
        log::error(LOG_PROTOCOL)
            << "Failure receiving verack from [" << authority() << "] "
            << ec.message();
        stop(ec);
        return;
    }

    // 3 of 3
    complete(error::success);
}

} // namespace network
} // namespace libbitcoin

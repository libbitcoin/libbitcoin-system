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
#include <bitcoin/bitcoin/network/p2p.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/network_settings.hpp>
#include <bitcoin/bitcoin/network/pending.hpp>
#include <bitcoin/bitcoin/network/protocol_address.hpp>
#include <bitcoin/bitcoin/network/protocol_ping.hpp>
#include <bitcoin/bitcoin/network/protocol_seed.hpp>
#include <bitcoin/bitcoin/network/protocol_version.hpp>
#include <bitcoin/bitcoin/network/session_inbound.hpp>
#include <bitcoin/bitcoin/network/session_manual.hpp>
#include <bitcoin/bitcoin/network/session_outbound.hpp>
#include <bitcoin/bitcoin/network/session_seed.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/log.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

INITIALIZE_TRACK(bc::network::channel::channel_subscriber);

namespace libbitcoin {
namespace network {

#define NAME "channel::subscriber"

using std::placeholders::_1;

const settings p2p::mainnet
{
    NETWORK_THREADS,
    NETWORK_IDENTIFIER_MAINNET,
    NETWORK_INBOUND_PORT_MAINNET,
    NETWORK_INBOUND_CONNECTION_LIMIT,
    NETWORK_OUTBOUND_CONNECTIONS,
    NETWORK_CONNECT_ATTEMPTS,
    NETWORK_CONNECT_TIMEOUT_SECONDS,
    NETWORK_CHANNEL_HANDSHAKE_SECONDS,
    NETWORK_CHANNEL_REVIVAL_MINUTES,
    NETWORK_CHANNEL_HEARTBEAT_MINUTES,
    NETWORK_CHANNEL_INACTIVITY_MINUTES,
    NETWORK_CHANNEL_EXPIRATION_MINUTES,
    NETWORK_CHANNEL_GERMINATION_SECONDS,
    NETWORK_HOST_POOL_CAPACITY,
    NETWORK_RELAY_TRANSACTIONS,
    NETWORK_HOSTS_FILE,
    NETWORK_DEBUG_FILE,
    NETWORK_ERROR_FILE,
    NETWORK_SELF,
    NETWORK_BLACKLISTS,
    NETWORK_SEEDS_MAINNET
};

const settings p2p::testnet
{
    NETWORK_THREADS,
    NETWORK_IDENTIFIER_TESTNET,
    NETWORK_INBOUND_PORT_TESTNET,
    NETWORK_INBOUND_CONNECTION_LIMIT,
    NETWORK_OUTBOUND_CONNECTIONS,
    NETWORK_CONNECT_ATTEMPTS,
    NETWORK_CONNECT_TIMEOUT_SECONDS,
    NETWORK_CHANNEL_HANDSHAKE_SECONDS,
    NETWORK_CHANNEL_REVIVAL_MINUTES,
    NETWORK_CHANNEL_HEARTBEAT_MINUTES,
    NETWORK_CHANNEL_INACTIVITY_MINUTES,
    NETWORK_CHANNEL_EXPIRATION_MINUTES,
    NETWORK_CHANNEL_GERMINATION_SECONDS,
    NETWORK_HOST_POOL_CAPACITY,
    NETWORK_RELAY_TRANSACTIONS,
    NETWORK_HOSTS_FILE,
    NETWORK_DEBUG_FILE,
    NETWORK_ERROR_FILE,
    NETWORK_SELF,
    NETWORK_BLACKLISTS,
    NETWORK_SEEDS_TESTNET
};

p2p::p2p(const settings& settings)
  : stopped_(true),
    height_(0),
    settings_(settings),
    dispatch_(pool_),
    pending_(pool_),
    connections_(pool_),
    hosts_(pool_, settings_),
    subscriber_(std::make_shared<channel::channel_subscriber>(pool_, NAME,
        LOG_NETWORK))
{
}

// Properties.
// ----------------------------------------------------------------------------

// The blockchain height is set in the version message for handshake.
size_t p2p::height()
{
    return height_;
}

// The height is set externally and is safe as a naturally atomic value.
void p2p::set_height(size_t value)
{
    height_ = value;
}

bool p2p::stopped() const
{
    return stopped_;
}

// Start sequence.
// ----------------------------------------------------------------------------

void p2p::start(result_handler handler)
{
    if (!stopped())
    {
        handler(error::operation_failed);
        return;
    }

    stopped_ = false;

    pool_.join();
    pool_.spawn(settings_.threads, thread_priority::low);

    // There is no need to seed or run the service to perform manual connection.
    // This instance is retained by the stop handler and the member reference.
    manual_ = attach<session_manual>(settings_);
    manual_->start();

    hosts_.load(
        dispatch_.ordered_delegate(&p2p::handle_hosts_loaded,
            this, _1, handler));
}

void p2p::handle_hosts_loaded(const code& ec, result_handler handler)
{
    if (stopped())
    {
        handler(error::service_stopped);
        return;
    }

    if (ec)
    {
        log::error(LOG_NETWORK)
            << "Error loading host addresses: " << ec.message();
        handler(ec);
        return;
    }

    auto seeded_handler = 
        dispatch_.ordered_delegate(&p2p::handle_hosts_seeded,
            this, _1, handler);

    // The instance is retained by the stop handler (until shutdown).
    attach<session_seed>(settings_)->start(seeded_handler);
}

void p2p::handle_hosts_seeded(const code& ec, result_handler handler)
{
    if (stopped())
    {
        handler(error::service_stopped);
        return;
    }

    if (ec)
    {
        log::error(LOG_NETWORK)
            << "Error seeding host addresses: " << ec.message();
        handler(ec);
        return;
    }

    // This is the end of the start sequence.
    handler(error::success);
}

// Run sequence.
// ----------------------------------------------------------------------------

void p2p::run(result_handler handler)
{
    // These instances are retained by the stop handler (until shutdown).
    attach<session_inbound>(settings_)->start();
    attach<session_outbound>(settings_)->start();

    // This is the end of the run sequence.
    handler(error::success);
}

// Stop sequence.
// ----------------------------------------------------------------------------

void p2p::stop(result_handler handler)
{
    if (stopped())
    {
        handler(error::service_stopped);
        return;
    }

    stopped_ = true;
    manual_ = nullptr;
    relay(error::service_stopped, nullptr);
    connections_.stop(error::service_stopped);

    hosts_.save(
        dispatch_.ordered_delegate(&p2p::handle_hosts_saved,
            this, _1, handler));

    pool_.shutdown();
}

void p2p::handle_hosts_saved(const code& ec, result_handler handler)
{
    if (ec)
        log::error(LOG_NETWORK)
            << "Error saving hosts file: " << ec.message();

    // This is the end of the stop sequence.
    handler(ec);
}

// Destruct sequence.
// ----------------------------------------------------------------------------

p2p::~p2p()
{
    close();
}

void p2p::close()
{
    stop([](code){});
    pool_.join();
}

// Pending connections collection.
// ----------------------------------------------------------------------------

void p2p::pent(uint64_t version_nonce, truth_handler handler)
{
    pending_.exists(version_nonce, handler);
}

void p2p::pend(channel::ptr channel, result_handler handler)
{
    pending_.store(channel, handler);
}

void p2p::unpend(channel::ptr channel, result_handler handler)
{
    pending_.remove(channel, handler);
}

void p2p::pent_count(count_handler handler)
{
    pending_.count(handler);
}

// Connections collection.
// ----------------------------------------------------------------------------

void p2p::connected(const address& address, truth_handler handler)
{
    connections_.exists(address, handler);
}

void p2p::store(channel::ptr channel, result_handler handler)
{
    connections_.store(channel, handler);
}

void p2p::remove(channel::ptr channel, result_handler handler)
{
    connections_.remove(channel, handler);
}

void p2p::connected_count(count_handler handler)
{
    connections_.count(handler);
}

// Hosts collection.
// ----------------------------------------------------------------------------

void p2p::fetch_address(address_handler handler)
{
    hosts_.fetch(handler);
}

void p2p::store(const address& address, result_handler handler)
{
    hosts_.store(address, handler);
}

void p2p::store(const address::list& addresses, result_handler handler)
{
    hosts_.store(addresses, handler);
}

void p2p::remove(const address& address, result_handler handler)
{
    hosts_.remove(address, handler);
}

void p2p::address_count(count_handler handler)
{
    hosts_.count(handler);
}

// Channel management.
// ----------------------------------------------------------------------------

void p2p::connect(const std::string& hostname, uint16_t port)
{
    if (stopped())
        return;

    manual_->connect(hostname, port);
}

void p2p::connect(const std::string& hostname, uint16_t port,
    channel_handler handler)
{
    if (stopped())
        handler(error::service_stopped, nullptr);
    else
        manual_->connect(hostname, port, handler);
}

void p2p::subscribe(channel_handler handler)
{
    if (stopped())
        handler(error::service_stopped, nullptr);
    else
        subscriber_->subscribe(handler);
}

/// This is not intended for public use but needs to be accessible.
void p2p::relay(const code& ec, channel::ptr channel)
{
    subscriber_->relay(ec, channel);
}

} // namespace network
} // namespace libbitcoin

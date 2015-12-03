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
#include <mutex>
#include <string>
#include <vector>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/connections.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/network_settings.hpp>
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

INITIALIZE_TRACK(bc::network::p2p::channel_subscriber);

namespace libbitcoin {
namespace network {

#define NAME "p2p"

using std::placeholders::_1;

const settings p2p::mainnet
{
    NETWORK_THREADS,
    NETWORK_IDENTIFIER_MAINNET,
    NETWORK_INBOUND_PORT_MAINNET,
    NETWORK_CONNECTION_LIMIT,
    NETWORK_OUTBOUND_CONNECTIONS,
    NETWORK_MANUAL_RETRY_LIMIT,
    NETWORK_CONNECT_BATCH_SIZE,
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
    NETWORK_CONNECTION_LIMIT,
    NETWORK_OUTBOUND_CONNECTIONS,
    NETWORK_MANUAL_RETRY_LIMIT,
    NETWORK_CONNECT_BATCH_SIZE,
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
    dispatch_(pool_, NAME),
    hosts_(pool_, settings_),
    connections_(std::make_shared<connections>(pool_)),
    subscriber_(std::make_shared<channel_subscriber>(pool_, NAME "_sub"))
{
}

// Properties.
// ----------------------------------------------------------------------------

// The blockchain height is set in our version message for handshake.
size_t p2p::height() const
{
    return height_;
}

// The height is set externally and is safe as an atomic.
void p2p::set_height(size_t value)
{
    height_ = value;
}

// protected:
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

    // Start is not thread safe, so stopped_/subscriber_ is not guarded.
    stopped_ = false;

    pool_.join();
    pool_.spawn(settings_.threads, thread_priority::low);

    const auto manual_started_handler =
        std::bind(&p2p::handle_manual_started,
            this, _1, handler);

    // This instance is retained by stop handler and member references.
    auto manual = attach<session_manual>(settings_);
    manual->start(manual_started_handler);
    manual_.store(manual);
}

void p2p::handle_manual_started(const code& ec, result_handler handler)
{
    if (stopped())
    {
        handler(error::service_stopped);
        return;
    }

    if (ec)
    {
        log::error(LOG_NETWORK)
            << "Error starting manual session: " << ec.message();
        handler(ec);
        return;
    }

    hosts_.load(
        std::bind(&p2p::handle_hosts_loaded,
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

    // The instance is retained by the stop handler (until shutdown).
    attach<session_seed>(settings_)->start(
        std::bind(&p2p::handle_hosts_seeded,
            this, _1, handler));
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
    // This instance is retained by the stop handler (until shutdown).
    attach<session_inbound>(settings_)->start(
        std::bind(&p2p::handle_inbound_started,
            this, _1, handler));
}

void p2p::handle_inbound_started(const code& ec, result_handler handler)
{
    if (ec)
    {
        log::error(LOG_NETWORK)
            << "Error starting inbound session: " << ec.message();
        handler(ec);
        return;
    }

    // This instance is retained by the stop handler (until shutdown).
    attach<session_outbound>(settings_)->start(
        std::bind(&p2p::handle_outbound_started,
            this, _1, handler));
}

void p2p::handle_outbound_started(const code& ec, result_handler handler)
{
    if (ec)
    {
        log::error(LOG_NETWORK)
            << "Error starting outbound session: " << ec.message();
        handler(ec);
        return;
    }

    // This is the end of the run sequence.
    handler(error::success);
}

// Channel subscription.
// ----------------------------------------------------------------------------

void p2p::subscribe(new_channel_handler handler)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    if (true)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        // stopped_/subscriber_ is the guarded relation.
        if (!stopped())
        {
            subscriber_->subscribe(handler);
            return;
        }
    }
    ///////////////////////////////////////////////////////////////////////////

    handler(error::service_stopped, nullptr);
}

// This is not intended for public use but needs to be accessible.
void p2p::relay(const code& ec, channel::ptr channel)
{
    subscriber_->relay(ec, channel);
}

// Manual connections.
// ----------------------------------------------------------------------------

void p2p::connect(const std::string& hostname, uint16_t port)
{
    if (stopped())
        return;

    auto manual = manual_.load();
    if (manual)
        manual->connect(hostname, port);
}

void p2p::connect(const std::string& hostname, uint16_t port,
    channel_handler handler)
{
    if (stopped())
    {
        handler(error::service_stopped, nullptr);
        return;
    }

    auto manual = manual_.load();
    if (manual)
        manual->connect(hostname, port, handler);
}

// Stop sequence.
// ----------------------------------------------------------------------------
// All shutdown actions must be queued by the end of the stop call.
// IOW queued shutdown operations must not enqueue additional work.

void p2p::stop(result_handler handler)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    if (true)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        // stopped_/subscriber_ is the guarded relation.
        if (!stopped())
        {
            stopped_ = true;
            subscriber_->relay(error::service_stopped, nullptr);
            connections_->stop(error::service_stopped);
            manual_.store(nullptr);

            hosts_.save(
                std::bind(&p2p::handle_hosts_saved,
                    this, _1, handler));

            pool_.shutdown();
            return;
        }
    }
    ///////////////////////////////////////////////////////////////////////////

    handler(error::service_stopped);
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
    // A reference cycle cannot exist with this class, since we don't capture
    // shared pointers to it. Therefore this will always clear subscriptions.
    // This allows for shutdown based on destruct without need to call stop.
    close();
}

void p2p::close()
{
    stop([](code){});

    // This is the end of the destruct sequence.
    pool_.join();
}

// Connections collection.
// ----------------------------------------------------------------------------

void p2p::connected(const address& address, truth_handler handler)
{
    connections_->exists(address, handler);
}

void p2p::store(channel::ptr channel, result_handler handler)
{
    connections_->store(channel, handler);
}

void p2p::remove(channel::ptr channel, result_handler handler)
{
    connections_->remove(channel, handler);
}

void p2p::connected_count(count_handler handler)
{
    connections_->count(handler);
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

} // namespace network
} // namespace libbitcoin

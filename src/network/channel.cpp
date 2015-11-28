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
#include <bitcoin/bitcoin/network/channel.hpp>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/message/heading.hpp>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/network/network_settings.hpp>
#include <bitcoin/bitcoin/network/proxy.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/log.hpp>
#include <bitcoin/bitcoin/utility/random.hpp>

// This must be declared in the global namespace.
INITIALIZE_TRACK(bc::network::channel);

namespace libbitcoin {
namespace network {

using std::placeholders::_1;

// Factory for deadline timer pointer construction.
static deadline::ptr alarm(threadpool& pool, const asio::duration& duration)
{
    return std::make_shared<deadline>(pool, duration);
}

channel::channel(threadpool& pool, asio::socket_ptr socket,
    const settings& settings)
  : proxy(pool, socket, settings.identifier),
    nonce_(0),
    version_({ 0 }),
    located_start_(null_hash),
    located_stop_(null_hash),
    revival_handler_(nullptr),
    expiration_(alarm(pool, pseudo_randomize(settings.channel_expiration()))),
    inactivity_(alarm(pool, settings.channel_inactivity())),
    revival_(alarm(pool, settings.channel_revival())),
    CONSTRUCT_TRACK(channel)
{
}

// Talk sequence.
// ----------------------------------------------------------------------------

// public:
void channel::start(result_handler handler)
{
    proxy::start(
        std::bind(&channel::do_start,
            shared_from_base<channel>(), _1, handler));
}

// Don't start the timers until the socket is enabled.
void channel::do_start(const code& ec, result_handler handler)
{
    start_expiration();
    start_revival();
    start_inactivity();
    handler(error::success);
}

// Properties (version write is thread unsafe, isolate from read).
// ----------------------------------------------------------------------------

uint64_t channel::nonce() const
{
    return nonce_;
}

void channel::set_nonce(uint64_t value)
{
    nonce_ = value;
}

const message::version& channel::version() const
{
    return version_;
}

void channel::set_version(const message::version& value)
{
    version_ = value;
}

// Proxy pure virtual protected and ordered handlers.
// ----------------------------------------------------------------------------

// It is possible that this may be called multipled times.
void channel::handle_stopping()
{
    expiration_->cancel();
    inactivity_->cancel();
    revival_->cancel();
    revival_handler_ = nullptr;
}

void channel::handle_activity()
{
    start_inactivity();
}

// Timers (these are inherent races, requiring stranding by stop only).
// ----------------------------------------------------------------------------

void channel::start_expiration()
{
    if (stopped())
        return;

    expiration_->start(
        std::bind(&channel::handle_expiration,
            shared_from_base<channel>(), _1));
}

void channel::handle_expiration(const code& ec)
{
    if (stopped())
        return;

    log::debug(LOG_NETWORK)
        << "Channel lifetime expired [" << authority() << "]";

    stop(error::channel_timeout);
}

void channel::start_inactivity()
{
    if (stopped())
        return;

    inactivity_->start(
        std::bind(&channel::handle_inactivity,
            shared_from_base<channel>(), _1));
}

void channel::handle_inactivity(const code& ec)
{
    if (stopped())
        return;

    log::debug(LOG_NETWORK)
        << "Channel inactivity timeout [" << authority() << "]";

    stop(error::channel_timeout);
}

// Revival timer (set/reset is thread unsafe, deprecated).
// ----------------------------------------------------------------------------

// public:
void channel::reset_revival()
{
    if (stopped())
        return;

    start_revival();
}

// public:
void channel::set_revival_handler(result_handler handler)
{
    revival_handler_ = handler;
}

void channel::start_revival()
{
    if (stopped())
        return;

    revival_->start(
        std::bind(&channel::handle_revival,
            shared_from_base<channel>(), _1));
}

void channel::handle_revival(const code& ec)
{
    if (stopped())
        return;

    // Nothing to do, no handler registered.
    if (!revival_handler_)
        return;

    log::debug(LOG_NETWORK)
        << "Channel revival invoked [" << authority() << "]";

    revival_handler_(ec);
    reset_revival();
}

// Location tracking (thread unsafe, deprecated).
// ----------------------------------------------------------------------------

bool channel::located(const hash_digest& start, const hash_digest& stop) const
{
    return located_start_ == start && located_stop_ == stop;
}

void channel::set_located(const hash_digest& start, const hash_digest& stop)
{
    located_start_ = start;
    located_stop_ = stop;
}

} // namespace network
} // namespace libbitcoin

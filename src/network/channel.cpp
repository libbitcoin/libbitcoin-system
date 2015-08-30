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
#include <memory>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/message/heading.hpp>
#include <bitcoin/bitcoin/network/asio.hpp>
#include <bitcoin/bitcoin/network/proxy.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

INITIALIZE_TRACK(bc::network::channel);

namespace libbitcoin {
namespace network {

// TODO: derive channel from proxy, adding timers, tracking, nonce.
channel::channel(proxy::ptr proxy)
  : proxy_(proxy),
    nonce_(0),
    CONSTRUCT_TRACK(channel, LOG_NETWORK)
{
}

// This implements the set of proxy messsage handler methods.
////DEFINE_CHANNEL_MESSAGE_SUBSCRIBERS()

// TODO: move proxy timeouts to channel (revival deprecated).
channel::channel(threadpool& pool, asio::socket_ptr socket,
    const timeout& timeouts)
  : channel(std::make_shared<proxy>(socket, pool, timeouts))
{
}

channel::~channel()
{
    proxy_->stop(error::channel_stopped);
}

void channel::start()
{
    proxy_->start();
}

void channel::stop(const code& ec)
{
    proxy_->stop(ec);
}

config::authority channel::address() const
{
    return proxy_->address();
}

// TODO: make private, pass on notfy.
uint64_t channel::nonce() const
{
    return nonce_;
}

void channel::set_nonce(uint64_t nonce)
{
    nonce_ = nonce;
}

void channel::reset_revival()
{
    return proxy_->reset_revival();
}

void channel::set_revival_handler(proxy::handler handler)
{
    return proxy_->set_revival_handler(handler);
}

void channel::subscribe_stop(proxy::stop_handler handler)
{
    proxy_->subscribe_stop(handler);
}

} // namespace network
} // namespace libbitcoin

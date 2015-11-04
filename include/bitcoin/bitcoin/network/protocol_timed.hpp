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
#ifndef LIBBITCOIN_NETWORK_PROTOCOL_TIMED_HPP
#define LIBBITCOIN_NETWORK_PROTOCOL_TIMED_HPP

#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/asio.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/protocol_base.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

/**
 * Base class for timed protocol implementation.
 */
template <class Protocol>
class protocol_timed
  : public protocol_base<Protocol>
{
protected:

    /**
     * Construct a timed protocol instance.
     * @param[in]  pool     The thread pool used by the dispacher.
     * @param[in]  channel  The channel on which to start the protocol.
     * @param[in]  name     The instance name for logging purposes.
     */
    protocol_timed(threadpool& pool, channel::ptr channel,
        const std::string& name)
      : protocol_base(pool, channel, name)
    {
    }

    /**
     * Define the event handler and start the protocol and timer.
     * The timer is automatically canceled on stop (only).
     * @param[in]  timeout  The timer period (not automatically reset).
     * @param[in]  handler  Invoke automatically on stop and timer events.
     */
    void start(const asio::duration& timeout, event_handler handler)
    {
        deadline_ = std::make_shared<deadline>(pool(), timeout);
        reset_timer();

        protocol_base::start(bind(&protocol_timed::handle_event, _1, handler));
    }

    /**
     * Cancel the timer.
     */
    void cancel_timer()
    {
        deadline_->cancel();
    }

    /**
     * Restart the timer.
     */
    void reset_timer()
    {
        if (stopped())
            return;

        deadline_->start(bind(&protocol_timed::handle_timer, _1));
    }

private:
    void handle_timer(const code& ec)
    {
        if (stopped())
            return;

        log::debug(LOG_PROTOCOL)
            << "Fired protocol_" << name() << " timer on ["
            << authority() << "] " << ec.message();

        set_event(error::channel_timeout);
    }

    void handle_event(const code& ec, event_handler handler)
    {
        if (ec == error::channel_stopped)
            cancel_timer();

        handler(ec);
    }

    deadline::ptr deadline_;
};

} // namespace network
} // namespace libbitcoin

#endif

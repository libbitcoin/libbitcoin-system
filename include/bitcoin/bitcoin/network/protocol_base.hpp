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
#ifndef LIBBITCOIN_NETWORK_PROTOCOL_BASE_HPP
#define LIBBITCOIN_NETWORK_PROTOCOL_BASE_HPP

#include <functional>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/asio.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/protocol_dispatch.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

/**
 * Base class for stateful protocol implementation.
 */
template <class Protocol>
class BC_API protocol_base
  : public protocol_dispatch<Protocol>
{
public:
    typedef std::function<void(const code&)> event_handler;

protected:

    /**
     * Construct a protocol instance.
     * @param[in]  pool     The thread pool used by the dispacher.
     * @param[in]  channel  The channel on which to start the protocol.
     * @param[in]  name     The instance name for logging purposes.
     */
    protocol_base(threadpool& pool, channel::ptr channel,
        const std::string& name)
      : protocol_dispatch(pool, channel, name),
        event_handler_(nullptr)
    {
    }
    
    /**
     * Invoke the event handler.
     * @param[in]  ec  The error code of the preceding operation.
     */
    void set_event(const code& ec)
    {
        call(&protocol_base::do_set_event, ec);
    }

    /**
     * Start the protocol with no event handler.
     */
    void start()
    {
        const auto unhandled = [](const code& ec) {};
        start(unhandled);
    }

    /**
     * Start the protocol.
     * The event handler may be invoked any number of times until released
     * when the protocol is stopped. A channel_stopped code indicates stop.
     * @param[in]  handler  The handler to call unpon each completion event.
     */
    void start(event_handler handler)
    {
        if (event_handler_ || stopped())
        {
            handler(error::channel_stopped);
            return;
        }

        event_handler_ = handler;
        subscribe_stop(&protocol_base::handle_stop, _1);
    }

private:
    void do_set_event(const code& ec)
    {
        if (event_handler_)
            event_handler_(ec);

        if (ec == error::channel_stopped)
            event_handler_ = nullptr;
    }

    void handle_stop(const code& ec)
    {
        log::debug(LOG_PROTOCOL)
            << "Stopped protocol_" << name() << " on [" << authority() << "] "
            << ec.message();
    
        // Event handlers can depend on this code for channel stop.
        set_event(error::channel_stopped);
    }

    event_handler event_handler_;
};

} // namespace network
} // namespace libbitcoin

#endif

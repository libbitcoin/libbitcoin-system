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
#ifndef LIBBITCOIN_NETWORK_CHANNEL_HPP
#define LIBBITCOIN_NETWORK_CHANNEL_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>
#include <string>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/messages.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/network/proxy.hpp>
#include <bitcoin/bitcoin/network/asio.hpp>
#include <bitcoin/bitcoin/network/message_subscriber.hpp>
#include <bitcoin/bitcoin/network/shared_const_buffer.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {


class BC_API channel
  : public track<channel>
{
public:
    typedef std::shared_ptr<channel> ptr;

    channel(proxy::ptr proxy);
    channel(threadpool& pool, asio::socket_ptr socket,
        uint32_t network_magic, const timeout& timeouts);
    ~channel();

    /// This class is not copyable.
    channel(const channel&) = delete;
    void operator=(const channel&) = delete;

    uint64_t identifier() const;
    void set_identifier(uint64_t value);

    void start();
    void stop(const code& ec);
    config::authority address() const;
    void reset_revival();
    void set_revival_handler(proxy::handler handler);

    template <class Message, typename Handler>
    void send(Message&& packet, Handler&& handler)
    {
        proxy_->send(std::forward<Message>(packet),
            std::forward<Handler>(handler));
    }

    template <class Message, typename Handler>
    void subscribe(Handler&& handler)
    {
        proxy_->subscribe<Message>(std::forward<Handler>(handler));
    }

    void subscribe_stop(proxy::stop_handler handle);

private:
    proxy::ptr proxy_;
    uint64_t identifier_;
};

} // namespace network
} // namespace libbitcoin

#endif

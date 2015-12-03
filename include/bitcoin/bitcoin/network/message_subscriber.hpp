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
#ifndef LIBBITCOIN_NETWORK_MESSAGE_SUBSCRIBER_HPP
#define LIBBITCOIN_NETWORK_MESSAGE_SUBSCRIBER_HPP

#include <istream>
#include <map>
#include <memory>
#include <utility>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/messages.hpp>
#include <bitcoin/bitcoin/utility/resubscriber.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

#define DEFINE_SUBSCRIBER_TYPE(value) \
    typedef resubscriber<const code&, const message::value&> \
        value##_subscriber_type

#define DEFINE_SUBSCRIBER_OVERLOAD(value) \
    template <typename Handler> \
    void subscribe(message::value&&, Handler&& handler) \
    { \
        value##_subscriber_->subscribe(std::forward<Handler>(handler)); \
    }

#define DECLARE_SUBSCRIBER(value) \
    value##_subscriber_type::ptr value##_subscriber_

/// Aggregation of subscribers by messasge type, thread safe.
class BC_API message_subscriber
{
public:
    DEFINE_SUBSCRIBER_TYPE(address);
    DEFINE_SUBSCRIBER_TYPE(alert);
    DEFINE_SUBSCRIBER_TYPE(block);
    DEFINE_SUBSCRIBER_TYPE(filter_add);
    DEFINE_SUBSCRIBER_TYPE(filter_clear);
    DEFINE_SUBSCRIBER_TYPE(filter_load);
    DEFINE_SUBSCRIBER_TYPE(get_address);
    DEFINE_SUBSCRIBER_TYPE(get_blocks);
    DEFINE_SUBSCRIBER_TYPE(get_data);
    DEFINE_SUBSCRIBER_TYPE(get_headers);
    DEFINE_SUBSCRIBER_TYPE(headers);
    DEFINE_SUBSCRIBER_TYPE(inventory);
    DEFINE_SUBSCRIBER_TYPE(memory_pool);
    DEFINE_SUBSCRIBER_TYPE(merkle_block);
    DEFINE_SUBSCRIBER_TYPE(not_found);
    DEFINE_SUBSCRIBER_TYPE(ping);
    DEFINE_SUBSCRIBER_TYPE(pong);
    DEFINE_SUBSCRIBER_TYPE(reject);
    DEFINE_SUBSCRIBER_TYPE(transaction);
    DEFINE_SUBSCRIBER_TYPE(verack);
    DEFINE_SUBSCRIBER_TYPE(version);

    /**
     * Create an instance of this class.
     * @param[in]  pool  The threadpool to use for sending notifications.
     */
    message_subscriber(threadpool& pool);

    /// This class is not copyable.
    message_subscriber(const message_subscriber&) = delete;
    void operator=(const message_subscriber&) = delete;
    
    /**
     * Subscribe to receive a notification when a message of type is received.
     * The handler is unregistered when the call is made.
     * Subscribing must be immediate, we cannot switch thread contexts.
     * @param[in]  handler  The handler to register.
     */
    template <class Message, typename Handler>
    void subscribe(Handler&& handler)
    {
        subscribe(Message(), std::forward<Handler>(handler));
    }
        
    /**
     * Load a stream into a message instance and notify subscribers.
     * @param[in]  stream      The stream from which to load the message.
     * @param[in]  subscriber  The subscriber for the message type.
     * @return                 Returns error::bad_stream if failed.
     */
    template <class Message, class Subscriber>
    code load(std::istream& stream, Subscriber subscriber) const
    {
        Message message;
        const bool parsed = message.from_data(stream);
        const code ec(parsed ? error::success : error::bad_stream);
        subscriber->relay(ec, message);
        return ec;
    }

    /**
     * Broadcast a default message instance with the specified error code.
     * @param[in]  ec  The error code to broadcast.
     */
    void broadcast(const code& ec);

    /*
     * Load a stream of the specified command type.
     * Creates an instance of the indicated message type.
     * Sends the message instance to each subscriber of the type.
     * @param[in]  type    The stream message type identifier.
     * @param[in]  stream  The stream from which to load the message.
     * @return             Returns error::bad_stream if failed.
     */
    code load(message::message_type type, std::istream& stream) const;

private:
    DEFINE_SUBSCRIBER_OVERLOAD(address);
    DEFINE_SUBSCRIBER_OVERLOAD(alert);
    DEFINE_SUBSCRIBER_OVERLOAD(block);
    DEFINE_SUBSCRIBER_OVERLOAD(filter_add);
    DEFINE_SUBSCRIBER_OVERLOAD(filter_clear);
    DEFINE_SUBSCRIBER_OVERLOAD(filter_load);
    DEFINE_SUBSCRIBER_OVERLOAD(get_address);
    DEFINE_SUBSCRIBER_OVERLOAD(get_blocks);
    DEFINE_SUBSCRIBER_OVERLOAD(get_data);
    DEFINE_SUBSCRIBER_OVERLOAD(get_headers);
    DEFINE_SUBSCRIBER_OVERLOAD(headers);
    DEFINE_SUBSCRIBER_OVERLOAD(inventory);
    DEFINE_SUBSCRIBER_OVERLOAD(memory_pool);
    DEFINE_SUBSCRIBER_OVERLOAD(merkle_block);
    DEFINE_SUBSCRIBER_OVERLOAD(not_found);
    DEFINE_SUBSCRIBER_OVERLOAD(ping);
    DEFINE_SUBSCRIBER_OVERLOAD(pong);
    DEFINE_SUBSCRIBER_OVERLOAD(reject);
    DEFINE_SUBSCRIBER_OVERLOAD(transaction);
    DEFINE_SUBSCRIBER_OVERLOAD(verack);
    DEFINE_SUBSCRIBER_OVERLOAD(version);

    DECLARE_SUBSCRIBER(address);
    DECLARE_SUBSCRIBER(alert);
    DECLARE_SUBSCRIBER(block);
    DECLARE_SUBSCRIBER(filter_add);
    DECLARE_SUBSCRIBER(filter_clear);
    DECLARE_SUBSCRIBER(filter_load);
    DECLARE_SUBSCRIBER(get_address);
    DECLARE_SUBSCRIBER(get_blocks);
    DECLARE_SUBSCRIBER(get_data);
    DECLARE_SUBSCRIBER(get_headers);
    DECLARE_SUBSCRIBER(headers);
    DECLARE_SUBSCRIBER(inventory);
    DECLARE_SUBSCRIBER(memory_pool);
    DECLARE_SUBSCRIBER(merkle_block);
    DECLARE_SUBSCRIBER(not_found);
    DECLARE_SUBSCRIBER(ping);
    DECLARE_SUBSCRIBER(pong);
    DECLARE_SUBSCRIBER(reject);
    DECLARE_SUBSCRIBER(transaction);
    DECLARE_SUBSCRIBER(verack);
    DECLARE_SUBSCRIBER(version);
};

#undef DEFINE_SUBSCRIBER_TYPE
#undef DEFINE_SUBSCRIBER_OVERLOAD
#undef DECLARE_SUBSCRIBER

} // namespace network
} // namespace libbitcoin

#endif

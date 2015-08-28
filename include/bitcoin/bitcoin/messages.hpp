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
#ifndef LIBBITCOIN_MESSAGES_HPP
#define LIBBITCOIN_MESSAGES_HPP

#include <bitcoin/bitcoin/chain/block.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/message/address.hpp>
#include <bitcoin/bitcoin/message/alert.hpp>
#include <bitcoin/bitcoin/message/alert_payload.hpp>
#include <bitcoin/bitcoin/message/filter_add.hpp>
#include <bitcoin/bitcoin/message/filter_clear.hpp>
#include <bitcoin/bitcoin/message/filter_load.hpp>
#include <bitcoin/bitcoin/message/get_address.hpp>
#include <bitcoin/bitcoin/message/get_blocks.hpp>
#include <bitcoin/bitcoin/message/get_data.hpp>
#include <bitcoin/bitcoin/message/get_headers.hpp>
#include <bitcoin/bitcoin/message/header.hpp>
#include <bitcoin/bitcoin/message/inventory.hpp>
#include <bitcoin/bitcoin/message/inventory_type_id.hpp>
#include <bitcoin/bitcoin/message/inventory_vector.hpp>
#include <bitcoin/bitcoin/message/memory_pool.hpp>
#include <bitcoin/bitcoin/message/merkle_block.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/message/not_found.hpp>
#include <bitcoin/bitcoin/message/ping.hpp>
#include <bitcoin/bitcoin/message/pong.hpp>
#include <bitcoin/bitcoin/message/reject.hpp>
#include <bitcoin/bitcoin/message/verack.hpp>
#include <bitcoin/bitcoin/message/version.hpp>

// Terminator values for use in DEFINE_MESSAGE_MACROS.
#define COMMA ,
#define SEMICOLON ;
#define NONE

// Iterate over message types for other macro definitions.
#define DEFINE_MESSAGE_MACROS(Macro, Terminator) \
    Macro(chain, block)Terminator \
    Macro(chain, transaction)Terminator \
    Macro(message, address)Terminator \
    Macro(message, alert)Terminator \
    Macro(message, filter_add)Terminator \
    Macro(message, filter_clear)Terminator \
    Macro(message, filter_load)Terminator \
    Macro(message, get_address)Terminator \
    Macro(message, get_blocks)Terminator \
    Macro(message, get_data)Terminator \
    Macro(message, get_headers)Terminator \
    Macro(message, inventory)Terminator \
    Macro(message, memory_pool)Terminator \
    Macro(message, merkle_block)Terminator \
    Macro(message, not_found)Terminator \
    Macro(message, ping)Terminator \
    Macro(message, pong)Terminator \
    Macro(message, reject)Terminator \
    Macro(message, verack)Terminator \
    Macro(message, version)

// For proxy.hpp:

#define DECLARE_PROXY_MESSAGE_HANDLER_TYPE(Namespace, Message) \
    typedef std::function<void(const code&, const Namespace::Message&)> \
        receive_##Message##_handler

#define DECLARE_PROXY_MESSAGE_HANDLER_TYPES() \
    DEFINE_MESSAGE_MACROS(DECLARE_PROXY_MESSAGE_HANDLER_TYPE, SEMICOLON)

#define DECLARE_PROXY_MESSAGE_SUBSCRIBER(Namespace, Message) \
    void subscribe_##Message(receive_##Message##_handler handle)

#define DECLARE_PROXY_MESSAGE_SUBSCRIBERS() \
    DEFINE_MESSAGE_MACROS(DECLARE_PROXY_MESSAGE_SUBSCRIBER, SEMICOLON)

#define DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(Namespace, Message) \
    typedef subscriber<const code&, const Namespace::Message&> \
        Message##_subscriber

#define DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPES() \
    DEFINE_MESSAGE_MACROS(DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE, SEMICOLON)

#define DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(Namespace, Message) \
    Message##_subscriber::ptr Message##_subscriber_

#define DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTERS() \
    DEFINE_MESSAGE_MACROS(DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER, SEMICOLON)

// For proxy.cpp:

#define INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(Namespace, Message) \
    Message##_subscriber_(MAKE_SUBSCRIBER(Message, pool, LOG_NETWORK))

#define INITIALIZE_PROXY_MESSAGE_SUBSCRIBERS() \
    DEFINE_MESSAGE_MACROS(INITIALIZE_PROXY_MESSAGE_SUBSCRIBER, COMMA)

#define INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(Namespace, Message) \
    INITIALIZE_TRACK(bc::network::proxy::Message##_subscriber)

#define INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACKS() \
    DEFINE_MESSAGE_MACROS(INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK, NONE)

#define ESTABLISH_PROXY_MESSAGE_RELAY(Namespace, Message) \
    establish_relay<Namespace::Message>(Message##_subscriber_)

#define ESTABLISH_PROXY_MESSAGE_RELAYS() \
    DEFINE_MESSAGE_MACROS(ESTABLISH_PROXY_MESSAGE_RELAY, SEMICOLON)

#define CLEAR_PROXY_MESSAGE_SUBSCRIPTION(Namespace, Message) \
    notify_stop<Namespace::Message>(Message##_subscriber_)

#define CLEAR_PROXY_MESSAGE_SUBSCRIPTIONS() \
    DEFINE_MESSAGE_MACROS(CLEAR_PROXY_MESSAGE_SUBSCRIPTION, SEMICOLON)

#define DEFINE_PROXY_MESSAGE_SUBSCRIBER(Namespace, Message) \
void proxy::subscribe_##Message(receive_##Message##_handler handle) \
{ \
    if (stopped()) \
        handle(error::channel_stopped, Namespace::Message()); \
    else \
        subscribe<Namespace::Message>(Message##_subscriber_, handle); \
}

#define DEFINE_PROXY_MESSAGE_SUBSCRIBERS() \
    DEFINE_MESSAGE_MACROS(DEFINE_PROXY_MESSAGE_SUBSCRIBER, NONE)

// For channel.hpp:

#define DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(Namespace, Message) \
    void subscribe_##Message(proxy::receive_##Message##_handler handle)

#define DECLARE_CHANNEL_MESSAGE_SUBSCRIBERS() \
    DEFINE_MESSAGE_MACROS(DECLARE_CHANNEL_MESSAGE_SUBSCRIBER, SEMICOLON)

// For channel.cpp:

#define DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(Namespace, Message) \
void channel::subscribe_##Message( \
    proxy::receive_##Message##_handler handler) \
{ \
    proxy_->subscribe_##Message(handler); \
}

#define DEFINE_CHANNEL_MESSAGE_SUBSCRIBERS() \
    DEFINE_MESSAGE_MACROS(DEFINE_CHANNEL_MESSAGE_SUBSCRIBER, NONE)

#endif

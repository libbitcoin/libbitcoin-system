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

// For channel_proxy.hpp:

#define DECLARE_PROXY_MESSAGE_HANDLER_TYPE(Message) \
    typedef std::function<void(const code&, const message::Message&)> \
        receive_##Message##_handler

#define DECLARE_PROXY_MESSAGE_HANDLER_TYPES() \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(address); \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(alert); \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(filter_add); \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(filter_clear); \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(filter_load); \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(get_address); \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(get_blocks); \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(get_data); \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(get_headers); \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(inventory); \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(memory_pool); \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(merkle_block); \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(not_found); \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(ping); \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(pong); \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(reject); \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(verack); \
    DECLARE_PROXY_MESSAGE_HANDLER_TYPE(version)

#define DECLARE_PROXY_MESSAGE_SUBSCRIBER(Message) \
    void subscribe_##Message(receive_##Message##_handler handle)

#define DECLARE_PROXY_MESSAGE_SUBSCRIBERS() \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(address); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(alert); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(filter_add); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(filter_clear); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(filter_load); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(get_address); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(get_blocks); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(get_data); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(get_headers); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(inventory); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(memory_pool); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(merkle_block); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(not_found); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(ping); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(pong); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(reject); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(verack); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER(version)

#define DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(Message) \
    typedef subscriber<const code&, const message::Message&> Message##_subscriber

#define DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPES() \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(address); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(alert); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(filter_add); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(filter_clear); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(filter_load); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(get_address); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(get_blocks); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(get_data); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(get_headers); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(inventory); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(memory_pool); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(merkle_block); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(not_found); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(ping); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(pong); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(reject); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(verack); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_TYPE(version)

#define DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(Message) \
    Message##_subscriber::ptr Message##_subscriber_

#define DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTERS() \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(address); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(alert); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(filter_add); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(filter_clear); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(filter_load); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(get_address); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(get_blocks); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(get_data); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(get_headers); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(inventory); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(memory_pool); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(merkle_block); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(not_found); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(ping); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(pong); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(reject); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(verack); \
    DECLARE_PROXY_MESSAGE_SUBSCRIBER_POINTER(version)

// For channel_proxy.cpp:

#define INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(Message, pool, log) \
    Message##_subscriber_(MAKE_SUBSCRIBER(Message, pool, log))

#define INITIALIZE_PROXY_MESSAGE_SUBSCRIBERS(pool, log) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(address, pool, log), \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(alert, pool, log), \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(filter_add, pool, log), \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(filter_clear, pool, log), \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(filter_load, pool, log), \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(get_address, pool, log), \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(get_blocks, pool, log), \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(get_data, pool, log), \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(get_headers, pool, log), \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(inventory, pool, log), \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(memory_pool, pool, log), \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(merkle_block, pool, log), \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(not_found, pool, log), \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(ping, pool, log), \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(pong, pool, log), \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(reject, pool, log), \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(verack, pool, log), \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER(version, pool, log)

#define INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(Message) \
    INITIALIZE_TRACK(bc::network::channel_proxy::Message##_subscriber)

#define INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACKS() \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(address) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(alert) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(filter_add) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(filter_clear) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(filter_load) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(get_address) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(get_blocks) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(get_data) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(get_headers) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(inventory) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(memory_pool) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(merkle_block) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(not_found) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(ping) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(pong) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(reject) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(verack) \
    INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACK(version)

#define ESTABLISH_PROXY_MESSAGE_RELAY(Message) \
    establish_relay<message::Message>(Message##_subscriber_)

#define ESTABLISH_PROXY_MESSAGE_RELAYS() \
    ESTABLISH_PROXY_MESSAGE_RELAY(address); \
    ESTABLISH_PROXY_MESSAGE_RELAY(alert); \
    ESTABLISH_PROXY_MESSAGE_RELAY(filter_add); \
    ESTABLISH_PROXY_MESSAGE_RELAY(filter_clear); \
    ESTABLISH_PROXY_MESSAGE_RELAY(filter_load); \
    ESTABLISH_PROXY_MESSAGE_RELAY(get_address); \
    ESTABLISH_PROXY_MESSAGE_RELAY(get_blocks); \
    ESTABLISH_PROXY_MESSAGE_RELAY(get_data); \
    ESTABLISH_PROXY_MESSAGE_RELAY(get_headers); \
    ESTABLISH_PROXY_MESSAGE_RELAY(inventory); \
    ESTABLISH_PROXY_MESSAGE_RELAY(memory_pool); \
    ESTABLISH_PROXY_MESSAGE_RELAY(merkle_block); \
    ESTABLISH_PROXY_MESSAGE_RELAY(not_found); \
    ESTABLISH_PROXY_MESSAGE_RELAY(ping); \
    ESTABLISH_PROXY_MESSAGE_RELAY(pong); \
    ESTABLISH_PROXY_MESSAGE_RELAY(reject); \
    ESTABLISH_PROXY_MESSAGE_RELAY(verack); \
    ESTABLISH_PROXY_MESSAGE_RELAY(version)

#define CLEAR_PROXY_MESSAGE_SUBSCRIPTION(Message) \
    notify_stop<message::Message>(Message##_subscriber_)

#define CLEAR_PROXY_MESSAGE_SUBSCRIPTIONS() \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(address); \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(alert); \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(filter_add); \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(filter_clear); \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(filter_load); \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(get_address); \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(get_blocks); \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(get_data); \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(get_headers); \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(inventory); \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(memory_pool); \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(merkle_block); \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(not_found); \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(ping); \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(pong); \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(reject); \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(verack); \
    CLEAR_PROXY_MESSAGE_SUBSCRIPTION(version)

#define DEFINE_PROXY_MESSAGE_SUBSCRIBER(Message) \
void channel_proxy::subscribe_##Message(receive_##Message##_handler handle) \
{ \
    if (stopped()) \
        handle(error::channel_stopped, message::Message()); \
    else \
        subscribe<message::Message>(Message##_subscriber_, handle); \
}

#define DEFINE_PROXY_MESSAGE_SUBSCRIBERS() \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(address) \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(alert) \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(filter_add) \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(filter_clear) \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(filter_load) \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(get_address) \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(get_blocks) \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(get_data) \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(get_headers) \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(inventory) \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(memory_pool) \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(merkle_block) \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(not_found) \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(ping) \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(pong) \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(reject) \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(verack) \
    DEFINE_PROXY_MESSAGE_SUBSCRIBER(version)

// For channel.hpp:

#define DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(Message) \
    void subscribe_##Message(channel_proxy::receive_##Message##_handler handle)

#define DECLARE_CHANNEL_MESSAGE_SUBSCRIBERS() \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(address); \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(alert); \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(filter_add); \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(filter_clear); \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(filter_load); \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(get_address); \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(get_blocks); \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(get_data); \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(get_headers); \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(inventory); \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(memory_pool); \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(merkle_block); \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(not_found); \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(ping); \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(pong); \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(reject); \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(verack); \
    DECLARE_CHANNEL_MESSAGE_SUBSCRIBER(version)

#define DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(Message) \
void channel::subscribe_##Message( \
    channel_proxy::receive_##Message##_handler handler) \
{ \
    proxy_->subscribe_##Message(handler); \
}

#define DEFINE_CHANNEL_MESSAGE_SUBSCRIBERS() \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(address) \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(alert) \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(filter_add) \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(filter_clear) \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(filter_load) \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(get_address) \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(get_blocks) \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(get_data) \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(get_headers) \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(inventory) \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(memory_pool) \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(merkle_block) \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(not_found) \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(ping) \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(pong) \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(reject) \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(verack) \
    DEFINE_CHANNEL_MESSAGE_SUBSCRIBER(version)

// For channel.cpp:

#endif

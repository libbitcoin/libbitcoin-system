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
#include <bitcoin/bitcoin/network/message_subscriber.hpp>

#include <istream>
#include <memory>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/messages.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/subscriber.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

#define TRACK_SUBSCRIBER(value) \
    INITIALIZE_TRACK(bc::network::message_subscriber::value##_subscriber_type)

#define INITIALIZE_SUBSCRIBER(pool, value) \
    value##_subscriber_(std::make_shared<value##_subscriber_type>( \
        pool, #value "_sub"))

#define RELAY_MESSAGE(value) \
    value##_subscriber_->relay(ec, message::value())

#define CASE_LOAD_STREAM(value) \
    case message_type::value: \
        return load<message::value>(stream, value##_subscriber_)

TRACK_SUBSCRIBER(address)
TRACK_SUBSCRIBER(alert)
TRACK_SUBSCRIBER(block)
TRACK_SUBSCRIBER(filter_add)
TRACK_SUBSCRIBER(filter_clear)
TRACK_SUBSCRIBER(filter_load)
TRACK_SUBSCRIBER(get_address)
TRACK_SUBSCRIBER(get_blocks)
TRACK_SUBSCRIBER(get_data)
TRACK_SUBSCRIBER(get_headers)
TRACK_SUBSCRIBER(headers)
TRACK_SUBSCRIBER(inventory)
TRACK_SUBSCRIBER(memory_pool)
TRACK_SUBSCRIBER(merkle_block)
TRACK_SUBSCRIBER(not_found)
TRACK_SUBSCRIBER(ping)
TRACK_SUBSCRIBER(pong)
TRACK_SUBSCRIBER(reject)
TRACK_SUBSCRIBER(transaction)
TRACK_SUBSCRIBER(verack)
TRACK_SUBSCRIBER(version)

namespace libbitcoin {
namespace network {

using namespace message;

message_subscriber::message_subscriber(threadpool& pool)
  : INITIALIZE_SUBSCRIBER(pool, address),
    INITIALIZE_SUBSCRIBER(pool, alert),
    INITIALIZE_SUBSCRIBER(pool, block),
    INITIALIZE_SUBSCRIBER(pool, filter_add),
    INITIALIZE_SUBSCRIBER(pool, filter_clear),
    INITIALIZE_SUBSCRIBER(pool, filter_load),
    INITIALIZE_SUBSCRIBER(pool, get_address),
    INITIALIZE_SUBSCRIBER(pool, get_blocks),
    INITIALIZE_SUBSCRIBER(pool, get_data),
    INITIALIZE_SUBSCRIBER(pool, get_headers),
    INITIALIZE_SUBSCRIBER(pool, headers),
    INITIALIZE_SUBSCRIBER(pool, inventory),
    INITIALIZE_SUBSCRIBER(pool, memory_pool),
    INITIALIZE_SUBSCRIBER(pool, merkle_block),
    INITIALIZE_SUBSCRIBER(pool, not_found),
    INITIALIZE_SUBSCRIBER(pool, ping),
    INITIALIZE_SUBSCRIBER(pool, pong),
    INITIALIZE_SUBSCRIBER(pool, reject),
    INITIALIZE_SUBSCRIBER(pool, transaction),
    INITIALIZE_SUBSCRIBER(pool, verack),
    INITIALIZE_SUBSCRIBER(pool, version)
{
}

void message_subscriber::broadcast(const code& ec)
{
    RELAY_MESSAGE(address);
    RELAY_MESSAGE(alert);
    RELAY_MESSAGE(block);
    RELAY_MESSAGE(filter_add);
    RELAY_MESSAGE(filter_clear);
    RELAY_MESSAGE(filter_load);
    RELAY_MESSAGE(get_address);
    RELAY_MESSAGE(get_blocks);
    RELAY_MESSAGE(get_data);
    RELAY_MESSAGE(get_headers);
    RELAY_MESSAGE(headers);
    RELAY_MESSAGE(inventory);
    RELAY_MESSAGE(memory_pool);
    RELAY_MESSAGE(merkle_block);
    RELAY_MESSAGE(not_found);
    RELAY_MESSAGE(ping);
    RELAY_MESSAGE(pong);
    RELAY_MESSAGE(reject);
    RELAY_MESSAGE(transaction);
    RELAY_MESSAGE(verack);
    RELAY_MESSAGE(version);
}

code message_subscriber::load(message_type type, std::istream& stream) const
{
    switch (type)
    {
        CASE_LOAD_STREAM(address);
        CASE_LOAD_STREAM(alert);
        CASE_LOAD_STREAM(block);
        CASE_LOAD_STREAM(filter_add);
        CASE_LOAD_STREAM(filter_clear);
        CASE_LOAD_STREAM(filter_load);
        CASE_LOAD_STREAM(get_address);
        CASE_LOAD_STREAM(get_blocks);
        CASE_LOAD_STREAM(get_data);
        CASE_LOAD_STREAM(get_headers);
        CASE_LOAD_STREAM(headers);
        CASE_LOAD_STREAM(inventory);
        CASE_LOAD_STREAM(memory_pool);
        CASE_LOAD_STREAM(merkle_block);
        CASE_LOAD_STREAM(not_found);
        CASE_LOAD_STREAM(ping);
        CASE_LOAD_STREAM(pong);
        CASE_LOAD_STREAM(reject);
        CASE_LOAD_STREAM(transaction);
        CASE_LOAD_STREAM(verack);
        CASE_LOAD_STREAM(version);
        case message_type::unknown:
        default:
            return error::not_found;
    }
}

} // namespace network
} // namespace libbitcoin

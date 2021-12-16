/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SYSTEM_MESSAGES_MESSAGE_HPP
#define LIBBITCOIN_SYSTEM_MESSAGES_MESSAGE_HPP

#include <cstdint>
#include <cstddef>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/messages/address.hpp>
#include <bitcoin/system/messages/block.hpp>
#include <bitcoin/system/messages/client_filter.hpp>
#include <bitcoin/system/messages/client_filter_checkpoint.hpp>
#include <bitcoin/system/messages/client_filter_headers.hpp>
#include <bitcoin/system/messages/compact_block.hpp>
#include <bitcoin/system/messages/fee_filter.hpp>
#include <bitcoin/system/messages/get_address.hpp>
#include <bitcoin/system/messages/get_blocks.hpp>
#include <bitcoin/system/messages/get_client_filter_checkpoint.hpp>
#include <bitcoin/system/messages/get_client_filter_headers.hpp>
#include <bitcoin/system/messages/get_client_filters.hpp>
#include <bitcoin/system/messages/get_data.hpp>
#include <bitcoin/system/messages/get_headers.hpp>
#include <bitcoin/system/messages/heading.hpp>
#include <bitcoin/system/messages/headers.hpp>
#include <bitcoin/system/messages/inventory.hpp>
#include <bitcoin/system/messages/memory_pool.hpp>
#include <bitcoin/system/messages/merkle_block.hpp>
#include <bitcoin/system/messages/not_found.hpp>
#include <bitcoin/system/messages/ping.hpp>
#include <bitcoin/system/messages/pong.hpp>
#include <bitcoin/system/messages/reject.hpp>
#include <bitcoin/system/messages/send_compact.hpp>
#include <bitcoin/system/messages/send_headers.hpp>
#include <bitcoin/system/messages/transaction.hpp>
#include <bitcoin/system/messages/version.hpp>
#include <bitcoin/system/messages/version_acknowledge.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {

////#define DECLARE_MESSAGE_POINTER_TYPES(type) \
////typedef messages::type::ptr type##_ptr; \
////typedef messages::type::const_ptr type##_const_ptr
////
////#define DECLARE_MESSAGE_POINTER_LIST_POINTER_TYPES(type) \
////typedef messages::type::ptr_list type##_ptr_list; \
////typedef messages::type::const_ptr_list type##_const_ptr_list; \
////typedef messages::type::const_ptr_list_ptr type##_const_ptr_list_ptr; \
////typedef messages::type::const_ptr_list_const_ptr type##_const_ptr_list_const_ptr
////
////// HACK: declare these in bc namespace to reduce length.
////DECLARE_MESSAGE_POINTER_TYPES(address);
////DECLARE_MESSAGE_POINTER_TYPES(block);
////DECLARE_MESSAGE_POINTER_TYPES(client_filter);
////DECLARE_MESSAGE_POINTER_TYPES(client_filter_checkpoint);
////DECLARE_MESSAGE_POINTER_TYPES(client_filter_headers);
////DECLARE_MESSAGE_POINTER_TYPES(compact_block);
////DECLARE_MESSAGE_POINTER_TYPES(compact_transactions);
////DECLARE_MESSAGE_POINTER_TYPES(fee_filter);
////DECLARE_MESSAGE_POINTER_TYPES(get_address);
////DECLARE_MESSAGE_POINTER_TYPES(get_blocks);
////DECLARE_MESSAGE_POINTER_TYPES(get_client_filter_checkpoint);
////DECLARE_MESSAGE_POINTER_TYPES(get_client_filter_headers);
////DECLARE_MESSAGE_POINTER_TYPES(get_client_filters);
////DECLARE_MESSAGE_POINTER_TYPES(get_compact_transactions);
////DECLARE_MESSAGE_POINTER_TYPES(get_data);
////DECLARE_MESSAGE_POINTER_TYPES(get_headers);
////DECLARE_MESSAGE_POINTER_TYPES(headers);
////DECLARE_MESSAGE_POINTER_TYPES(inventory);
////DECLARE_MESSAGE_POINTER_TYPES(memory_pool);
////DECLARE_MESSAGE_POINTER_TYPES(merkle_block);
////DECLARE_MESSAGE_POINTER_TYPES(not_found);
////DECLARE_MESSAGE_POINTER_TYPES(ping);
////DECLARE_MESSAGE_POINTER_TYPES(pong);
////DECLARE_MESSAGE_POINTER_TYPES(reject);
////DECLARE_MESSAGE_POINTER_TYPES(send_compact);
////DECLARE_MESSAGE_POINTER_TYPES(send_headers);
////DECLARE_MESSAGE_POINTER_TYPES(transaction);
////DECLARE_MESSAGE_POINTER_TYPES(version);
////DECLARE_MESSAGE_POINTER_TYPES(version_acknowledge);
////DECLARE_MESSAGE_POINTER_LIST_POINTER_TYPES(block);
////DECLARE_MESSAGE_POINTER_LIST_POINTER_TYPES(header);
////DECLARE_MESSAGE_POINTER_LIST_POINTER_TYPES(transaction);
////
////#undef DECLARE_MESSAGE_POINTER_TYPES
////#undef DECLARE_MESSAGE_POINTER_LIST_POINTER_TYPES

namespace messages {

/// Network protocol constants.
///----------------------------------------------------------------------------

/// Explicit size.
constexpr size_t command_size = 12;

/// Explicit limits.
constexpr size_t max_address = 1000;
constexpr size_t max_bloom_filter_add = 520;
constexpr size_t max_filter_functions = 50;
constexpr size_t max_filter_hashes = 2000;
constexpr size_t max_bloom_filter_load = 36000;
constexpr size_t max_get_blocks = 500;
constexpr size_t max_get_headers = 2000;
constexpr size_t max_get_data = 50000;
constexpr size_t max_inventory = 50000;
constexpr size_t max_get_client_filter_headers = 1999;
constexpr size_t max_get_client_filters = 99;

/// compact filter checkpoint interval
constexpr size_t client_filter_checkpoint_interval = 1000;

/// Effective limit given a 32 bit chain height boundary: 10 + log2(2^32) + 1.
constexpr size_t max_locator = 43;

///----------------------------------------------------------------------------
/// TODO: define template concept for witnessable.

template <typename Message, if_base_of<Message, block> = true>
void serialize(Message& instance, writer& sink, uint32_t version, bool witness)
{
    instance.serialize(version, sink, witness);
}

template <typename Message, if_base_of<Message, transaction> = true>
void serialize(Message& instance, writer& sink, uint32_t version, bool witness)
{
    instance.serialize(version, sink, witness);
}

template <typename Message, if_base_of<Message, compact_block> = true>
void serialize(Message& instance, writer& sink, uint32_t version, bool witness)
{
    instance.serialize(version, sink, witness);
}

template <typename Message>
void serialize(Message& instance, writer& sink, uint32_t version, bool)
{
    instance.serialize(version, sink);
}

/// Serialize a message object to the Bitcoin wire protocol encoding.
template <typename Message>
chunk_ptr serialize(const Message& instance, uint32_t magic, uint32_t version,
    bool witness)
{
    const auto buffer = std::make_shared<data_chunk>(no_fill_byte_allocator);
    buffer->resize(heading::size() + instance.size(version));

    data_reference body(std::next(buffer->begin(), heading::size()),
        buffer->end());
    write::bytes::copy body_writer(body);
    serialize(instance, body_writer, version, witness);

    write::bytes::copy head_writer(*buffer);
    heading::factory(magic, Message::command, body).serialize(head_writer);

    return buffer;
}

template <typename Message, if_base_of<Message, block> = true>
typename Message::ptr deserialize(reader& source, uint32_t version,
    bool witness)
{
    return to_shared(Message::deserialize(version, source, witness));
}

template <typename Message, if_base_of<Message, transaction> = true>
typename Message::ptr deserialize(reader& source, uint32_t version,
    bool witness)
{
    return to_shared(Message::deserialize(version, source, witness));
}

template <typename Message, if_base_of<Message, compact_block> = true>
typename Message::ptr deserialize(reader& source, uint32_t version,
    bool witness)
{
    return to_shared(Message::deserialize(version, source, witness));
}

template <typename Message>
typename Message::ptr deserialize(reader& source, uint32_t version, bool)
{
    return to_shared(Message::deserialize(version, source));
}

template <typename Message>
typename Message::ptr deserialize(const data_chunk& data, uint32_t version,
    bool witness)
{
    read::bytes::copy source(data);
    auto message = deserialize(source, version, witness);
    return reader && reader.is_exhausted() ? message : nullptr;
}

/// Compute an internal representation of the message checksum.
BC_API uint32_t network_checksum(const data_slice& data);

} // namespace messages
} // namespace system
} // namespace libbitcoin

#endif

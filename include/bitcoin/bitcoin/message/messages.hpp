/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_MESSAGE_MESSAGES_HPP
#define LIBBITCOIN_SYSTEM_MESSAGE_MESSAGES_HPP

#include <algorithm>
#include <cstdint>
#include <cstddef>
#include <bitcoin/bitcoin/math/limits.hpp>
#include <bitcoin/bitcoin/message/address.hpp>
#include <bitcoin/bitcoin/message/alert.hpp>
#include <bitcoin/bitcoin/message/alert_payload.hpp>
#include <bitcoin/bitcoin/message/block.hpp>
#include <bitcoin/bitcoin/message/block_transactions.hpp>
#include <bitcoin/bitcoin/message/compact_block.hpp>
#include <bitcoin/bitcoin/message/fee_filter.hpp>
#include <bitcoin/bitcoin/message/filter_add.hpp>
#include <bitcoin/bitcoin/message/filter_clear.hpp>
#include <bitcoin/bitcoin/message/filter_load.hpp>
#include <bitcoin/bitcoin/message/get_address.hpp>
#include <bitcoin/bitcoin/message/get_block_transactions.hpp>
#include <bitcoin/bitcoin/message/get_blocks.hpp>
#include <bitcoin/bitcoin/message/get_data.hpp>
#include <bitcoin/bitcoin/message/get_headers.hpp>
#include <bitcoin/bitcoin/message/headers.hpp>
#include <bitcoin/bitcoin/message/heading.hpp>
#include <bitcoin/bitcoin/message/inventory.hpp>
#include <bitcoin/bitcoin/message/inventory_vector.hpp>
#include <bitcoin/bitcoin/message/memory_pool.hpp>
#include <bitcoin/bitcoin/message/merkle_block.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/message/not_found.hpp>
#include <bitcoin/bitcoin/message/ping.hpp>
#include <bitcoin/bitcoin/message/pong.hpp>
#include <bitcoin/bitcoin/message/reject.hpp>
#include <bitcoin/bitcoin/message/send_compact.hpp>
#include <bitcoin/bitcoin/message/send_headers.hpp>
#include <bitcoin/bitcoin/message/transaction.hpp>
#include <bitcoin/bitcoin/message/verack.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

// Minimum current libbitcoin protocol version:     31402
// Minimum current satoshi client protocol version: 31800

// libbitcoin-network
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// version      v2      70001           added relay field
// verack       v1
// getaddr      v1
// addr         v1
// ping         v1
// ping         v2      60001   BIP031  added nonce field
// pong         v1      60001   BIP031
// reject       v3      70002   BIP061
// ----------------------------------------------------------------------------
// alert        --                      no intent to support
// checkorder   --                      obsolete
// reply        --                      obsolete
// submitorder  --                      obsolete
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// libbitcoin-node
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getblocks    v1
// inv          v1
// getdata      v1
// getdata      v3      70001   BIP037  allows filtered_block flag
// block        v1
// tx           v1
// notfound     v2      70001
// getheaders   v3      31800
// headers      v3      31800
// mempool      --      60002   BIP035
// mempool      v3      70002           allow multiple inv messages in reply
// sendheaders  v3      70012   BIP130
// feefilter    v3      70013   BIP133
// blocktxn     v3      70014   BIP152
// cmpctblock   v3      70014   BIP152
// getblocktxn  v3      70014   BIP152
// sendcmpct    v3      70014   BIP152
// merkleblock  v3      70001   BIP037  no bloom filters so unfiltered only
// ----------------------------------------------------------------------------
// filterload   --      70001   BIP037  no intent to support, see BIP111
// filteradd    --      70001   BIP037  no intent to support, see BIP111
// filterclear  --      70001   BIP037  no intent to support, see BIP111
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

namespace libbitcoin {

#define DECLARE_MESSAGE_POINTER_TYPES(type) \
typedef message::type::ptr type##_ptr; \
typedef message::type::const_ptr type##_const_ptr

#define DECLARE_MESSAGE_POINTER_LIST_POINTER_TYPES(type) \
typedef message::type::ptr_list type##_ptr_list; \
typedef message::type::const_ptr_list type##_const_ptr_list; \
typedef message::type::const_ptr_list_ptr type##_const_ptr_list_ptr; \
typedef message::type::const_ptr_list_const_ptr type##_const_ptr_list_const_ptr

// HACK: declare these in bc namespace to reduce length.
DECLARE_MESSAGE_POINTER_TYPES(address);
DECLARE_MESSAGE_POINTER_TYPES(block);
DECLARE_MESSAGE_POINTER_TYPES(compact_block);
DECLARE_MESSAGE_POINTER_TYPES(get_address);
DECLARE_MESSAGE_POINTER_TYPES(fee_filter);
DECLARE_MESSAGE_POINTER_TYPES(get_blocks);
DECLARE_MESSAGE_POINTER_TYPES(get_data);
DECLARE_MESSAGE_POINTER_TYPES(get_headers);
DECLARE_MESSAGE_POINTER_TYPES(header);
DECLARE_MESSAGE_POINTER_TYPES(headers);
DECLARE_MESSAGE_POINTER_TYPES(inventory);
DECLARE_MESSAGE_POINTER_TYPES(memory_pool);
DECLARE_MESSAGE_POINTER_TYPES(merkle_block);
DECLARE_MESSAGE_POINTER_TYPES(not_found);
DECLARE_MESSAGE_POINTER_TYPES(ping);
DECLARE_MESSAGE_POINTER_TYPES(pong);
DECLARE_MESSAGE_POINTER_TYPES(reject);
DECLARE_MESSAGE_POINTER_TYPES(send_compact);
DECLARE_MESSAGE_POINTER_TYPES(send_headers);
DECLARE_MESSAGE_POINTER_TYPES(transaction);
DECLARE_MESSAGE_POINTER_TYPES(verack);
DECLARE_MESSAGE_POINTER_TYPES(version);
DECLARE_MESSAGE_POINTER_LIST_POINTER_TYPES(block);
DECLARE_MESSAGE_POINTER_LIST_POINTER_TYPES(header);
DECLARE_MESSAGE_POINTER_LIST_POINTER_TYPES(transaction);

#undef DECLARE_MESSAGE_POINTER_TYPES
#undef DECLARE_MESSAGE_POINTER_LIST_POINTER_TYPES

namespace message {

/// Serialize a message object to the Bitcoin wire protocol encoding.
template <typename Message>
data_chunk serialize(uint32_t version, const Message& packet,
    uint32_t magic)
{
    const auto heading_size = heading::satoshi_fixed_size();
    const auto payload_size = packet.serialized_size(version);
    const auto message_size = heading_size + payload_size;

    // Unfortunately data_sink doesn't support seek, so this is a little ugly.
    // The header requires payload size and checksum but prepends the payload.
    // Use a stream to prevent unnecessary copying of the payload.
    data_chunk data;

    // Reserve memory for the full message.
    data.reserve(message_size);

    // Size the vector for the heading so that payload insertion will follow.
    data.resize(heading_size);

    // Insert the payload after the heading and into the reservation.
    data_sink ostream(data);
    packet.to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == message_size);

    // Create the payload checksum without copying the buffer.
    data_slice slice(data.data() + heading_size, data.data() + message_size);
    const auto check = bitcoin_checksum(slice);
    const auto payload_size32 = safe_unsigned<uint32_t>(payload_size);

    // Create and serialize the heading to a temporary variable (12 bytes).
    heading head(magic, Message::command, payload_size32, check);
    auto heading = head.to_data();

    // Move the heading into the allocated beginning of the message buffer.
    std::move(heading.begin(), heading.end(), data.begin());
    return data;
}

BC_API size_t variable_uint_size(uint64_t value);

} // namespace message
} // namespace libbitcoin

#endif

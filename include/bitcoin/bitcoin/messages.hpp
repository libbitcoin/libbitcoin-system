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

#include <cstdint>
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
#include <bitcoin/bitcoin/message/headers.hpp>
#include <bitcoin/bitcoin/message/heading.hpp>
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
#include <bitcoin/bitcoin/utility/data.hpp>

// List of bitcoin messages
// ------------------------
// version
// verack
// getaddr
// addr
// inv
// getdata
// getblocks
// block
// tx
// ping
// pong
// notfound
// reject       [not yet supported]
// getheaders   [not yet supported]
// headers      [not yet supported]
// mempool      [BIP35: not yet supported]
// filterload   [BIP37: no support intended]
// filteradd    [BIP37: no support intended]
// filterclear  [BIP37: no support intended]
// merkleblock  [BIP37: no support intended]
// checkorder   [deprecated in protocol]
// submitorder  [deprecated in protocol]
// reply        [deprecated in protocol]
// alert        [no support intended]

namespace libbitcoin {
namespace message {

/// The blockchain block and tx objects are isomorphic with the corresponding
/// message block and tx objects by design. This minimizes large item copying.
typedef bc::chain::block block;
typedef bc::chain::transaction transaction;

/**
* Serialize a message object to the Bitcoin wire protocol encoding.
*/
template <typename Message>
data_chunk serialize(const Message& packet, uint32_t magic)
{
    // Serialize the payload (required for header size).
    auto payload = packet.to_data();

    // Construct the payload header.
    heading head;
    head.magic = magic;
    head.command = Message::command;
    head.payload_size = static_cast<uint32_t>(payload.size());
    head.checksum = bitcoin_checksum(payload);

    // Serialize header and copy the payload into a single message buffer.
    auto message = head.to_data();
    extend_data(message, payload);
    return message;
}

} // namespace message
} // namespace libbitcoin

#endif

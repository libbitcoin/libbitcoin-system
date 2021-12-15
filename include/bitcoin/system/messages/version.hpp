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
#ifndef LIBBITCOIN_SYSTEM_MESSAGES_VERSION_HPP
#define LIBBITCOIN_SYSTEM_MESSAGES_VERSION_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/address_item.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {

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
// ----------------------------------------------------------------------------
// cfilter      --      70015   BIP157
// getcfilters  --      70015   BIP157
// cfcheckpt    --      70015   BIP157
// getcfcheckpt --      70015   BIP157
// cfheaders    --      70015   BIP157
// getcfheaders --      70015   BIP157
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// The checksum is ignored by the version command.
struct BC_API version
{
    typedef std::shared_ptr<const version> ptr;

    enum level: uint32_t
    {
        // client filters protocol
        bip157 = 70015,

        // compact blocks protocol
        bip152 = 70014,

        // fee_filter
        bip133 = 70013,

        // send_headers
        bip130 = 70012,

        // node_bloom service bit
        bip111 = 70011,

        // node_utxo service bit (draft)
        bip64 = 70004,

        // reject (satoshi node writes version.relay starting here)
        bip61 = 70002,

        // bloom filters, merkle_block, not_found, version.relay
        bip37 = 70001,

        // memory_pool
        bip35 = 60002,

        // ping.nonce, pong
        bip31 = 60001,

        // Don't request blocks from nodes of versions 32000-32400.
        no_blocks_end = 32400,

        // Don't request blocks from nodes of versions 32000-32400.
        no_blocks_start = 32000,

        // This preceded the BIP system.
        headers = 31800,

        // We require at least this of peers, address.time fields.
        minimum = 31402,

        // We support at most this internally (bound to settings default).
        maximum = bip152,

        // Used to generate canonical size required by consensus checks.
        canonical = 0
    };

    enum service: uint64_t
    {
        // The node exposes no services.
        none = 0,

        // The node is capable of serving the block chain (full node).
        node_network = (1u << 0),

        // Requires version.value >= level::bip64 (BIP64 is draft only).
        // The node is capable of responding to the getutxo protocol request.
        node_utxo = (1u << 1),

        // Requires version.value >= level::bip111
        // The node is capable and willing to handle bloom-filtered connections.
        node_bloom = (1u << 2),

        // Independent of network protocol level.
        // The node is capable of responding to witness inventory requests.
        node_witness = (1u << 3),

        // The node is capable of responding to getcfilters, getcfheaders,
        // and getcfcheckpt protocol requests.
        node_client_filters = (1u << 6)
    };

    static const identifier id;
    static const std::string command;
    static const uint32_t version_minimum;
    static const uint32_t version_maximum;

    static version deserialize(uint32_t version, reader& source);
    void serialize(uint32_t version, writer& sink) const;
    size_t size(uint32_t version) const;

    uint32_t value;
    uint64_t services;
    uint64_t timestamp;
    address_item address_receiver;
    address_item address_sender;
    uint64_t nonce;
    std::string user_agent;
    uint32_t start_height;

    // version >= 70001
    bool relay;
};

} // namespace messages
} // namespace system
} // namespace libbitcoin

#endif

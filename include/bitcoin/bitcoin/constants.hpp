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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_CONSTANTS_HPP
#define LIBBITCOIN_CONSTANTS_HPP

#include <cstddef>
#include <cstdint>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/math/hash_number.hpp>

namespace libbitcoin {

#define BC_USER_AGENT "/libbitcoin:" LIBBITCOIN_VERSION "/"

// Generic constants.

BC_CONSTEXPR size_t command_size = 12;

BC_CONSTEXPR int64_t min_int64 = MIN_INT64;
BC_CONSTEXPR int64_t max_int64 = MAX_INT64;
BC_CONSTEXPR int32_t min_int32 = MIN_INT32;
BC_CONSTEXPR int32_t max_int32 = MAX_INT32;
BC_CONSTEXPR uint64_t max_uint64 = MAX_UINT64;
BC_CONSTEXPR uint32_t max_uint32 = MAX_UINT32;
BC_CONSTEXPR uint16_t max_uint16 = MAX_UINT16;
BC_CONSTEXPR uint8_t max_uint8 = MAX_UINT8;
BC_CONSTEXPR uint64_t max_size_t = BC_MAX_SIZE;
BC_CONSTEXPR uint8_t byte_bits = 8;

// Consensus constants.
BC_CONSTEXPR uint32_t reward_interval = 210000;
BC_CONSTEXPR uint32_t coinbase_maturity = 100;
BC_CONSTEXPR uint32_t initial_block_reward = 50;
BC_CONSTEXPR uint32_t max_work_bits = 0x1d00ffff;
BC_CONSTEXPR uint32_t max_input_sequence = max_uint32;

// Threshold for nLockTime: below this value it is interpreted as block number,
// otherwise as UNIX timestamp. [Tue Nov 5 00:53:20 1985 UTC]
BC_CONSTEXPR uint32_t locktime_threshold = 500000000;

BC_CONSTFUNC uint64_t max_money_recursive(uint64_t current)
{
    return (current > 0) ? current + max_money_recursive(current >> 1) : 0;
}

BC_CONSTFUNC uint64_t coin_price(uint64_t value=1)
{
    return value * 100000000;
}

BC_CONSTFUNC uint64_t max_money()
{
    return reward_interval *
        max_money_recursive(coin_price(initial_block_reward));
}

// For configuration settings initialization.
enum class settings
{
    none,
    mainnet,
    testnet
};

enum services: uint64_t
{
    // The node is capable of serving the block chain.
    node_network = (1 << 0),

    // Requires version >= 70004 (bip64)
    // The node is capable of responding to the getutxo protocol request.
    node_utxo = (1 << 1),

    // Requires version >= 70011 (proposed)
    // The node is capable and willing to handle bloom-filtered connections.
    bloom_filters = (1 << 2)
};

BC_CONSTEXPR uint32_t no_timestamp = 0;
BC_CONSTEXPR uint16_t unspecified_ip_port = 0;
BC_CONSTEXPR message::ip_address unspecified_ip_address
{
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00
    }
};
BC_CONSTEXPR message::network_address unspecified_network_address
{
    no_timestamp,
    services::node_network,
    unspecified_ip_address,
    unspecified_ip_port
};

// TODO: make static.
BC_API hash_number max_target();

} // namespace libbitcoin

#endif

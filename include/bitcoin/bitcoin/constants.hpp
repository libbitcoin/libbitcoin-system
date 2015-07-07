/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_CONSTANTS_HPP
#define LIBBITCOIN_CONSTANTS_HPP

#include <cstddef>
#include <cstdint>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash_number.hpp>

namespace libbitcoin {

// Generic constants.

BC_CONSTEXPR int64_t min_int64 = MIN_INT64;
BC_CONSTEXPR int64_t max_int64 = MAX_INT64;
BC_CONSTEXPR int32_t min_int32 = MIN_INT32;
BC_CONSTEXPR int32_t max_int32 = MAX_INT32;
BC_CONSTEXPR uint64_t max_uint64 = MAX_UINT64;
BC_CONSTEXPR uint32_t max_uint32 = MAX_UINT32;
BC_CONSTEXPR uint16_t max_uint16 = MAX_UINT16;
BC_CONSTEXPR uint8_t max_uint8 = MAX_UINT8;
BC_CONSTEXPR uint8_t byte_bits = 8;

// Debian complains about missing UINTPTR_MAX definition for some weird reason.
#ifdef UINTPTR_MAX
    BC_CONSTEXPR uint64_t max_size_t = UINTPTR_MAX;
#else
    BC_CONSTEXPR uint64_t max_size_t = std::numeric_limits<size_t>::max();
#endif

// Bitcoin constants.

BC_CONSTEXPR uint32_t reward_interval = 210000;
BC_CONSTEXPR uint32_t protocol_version = 60000;
BC_CONSTEXPR uint32_t coinbase_maturity = 100;
BC_CONSTEXPR uint32_t initial_block_reward = 50;
BC_CONSTEXPR uint32_t max_work_bits = 0x1d00ffff;
BC_CONSTEXPR uint32_t max_input_sequence = max_uint32;

// Deprecated (use more descriptive).
BC_CONSTEXPR uint32_t max_bits = max_work_bits;
BC_CONSTEXPR uint32_t max_sequence = max_input_sequence;
BC_CONSTEXPR uint32_t block_reward = initial_block_reward;

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

#ifdef ENABLE_TESTNET
    // Block 514 is the first block after [Feb 15 2014].
    // Testnet started bip16 before mainnet.
    BC_CONSTEXPR uint32_t bip16_switchover_timestamp = 1333238400;
    BC_CONSTEXPR uint32_t bip16_switchover_height = 514;
    BC_CONSTEXPR uint16_t protocol_port = 18333;
#else
    // [April 1 2012]
    BC_CONSTEXPR uint32_t bip16_switchover_timestamp = 1333238400;
    BC_CONSTEXPR uint32_t bip16_switchover_height = 173805;
    BC_CONSTEXPR uint16_t protocol_port = 8333;
#endif

// Constants, sort of.
BC_API hash_number max_target();
BC_API uint32_t magic_value();

} // namespace libbitcoin

#endif

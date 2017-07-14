/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_CONSTANTS_HPP
#define LIBBITCOIN_CONSTANTS_HPP

#include <cstddef>
#include <cstdint>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/config/checkpoint.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/version.hpp>

namespace libbitcoin {

// This guards assumptions within the codebase.
static_assert(sizeof(size_t) >= sizeof(uint32_t), "unsupported size_t");

#define BC_USER_AGENT "/libbitcoin:" LIBBITCOIN_VERSION "/"

// Generic constants.
//-----------------------------------------------------------------------------

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

// Consensus sentinels.
//-----------------------------------------------------------------------------

BC_CONSTEXPR uint32_t no_previous_output = max_uint32;
BC_CONSTEXPR uint32_t max_input_sequence = max_uint32;
BC_CONSTEXPR uint64_t sighash_null_value = max_uint64;

// Script/interpreter constants.
//-----------------------------------------------------------------------------

// Consensus
BC_CONSTEXPR size_t max_counted_ops = 201;
BC_CONSTEXPR size_t max_stack_size = 1000;
BC_CONSTEXPR size_t max_script_size = 10000;
BC_CONSTEXPR size_t max_push_data_size = 520;
BC_CONSTEXPR size_t max_script_public_keys = 20;
BC_CONSTEXPR size_t multisig_default_sigops = 20;
BC_CONSTEXPR size_t max_number_size = 4;
BC_CONSTEXPR size_t max_cltv_number_size = 5;

// Policy.
BC_CONSTEXPR size_t max_null_data_size = 80;

// Various validation constants.
//-----------------------------------------------------------------------------

BC_CONSTEXPR size_t min_coinbase_size = 2;
BC_CONSTEXPR size_t max_coinbase_size = 100;
BC_CONSTEXPR size_t coinbase_maturity = 100;
BC_CONSTEXPR size_t median_time_past_interval = 11;
BC_CONSTEXPR size_t locktime_threshold = 500000000;
BC_CONSTEXPR size_t max_block_size = 1000000;
BC_CONSTEXPR size_t max_sigops_factor = 50;

// Derived.
BC_CONSTEXPR size_t max_block_sigops = max_block_size / max_sigops_factor;

// Timespan constants.
//-----------------------------------------------------------------------------

BC_CONSTEXPR uint32_t retargeting_factor = 4;
BC_CONSTEXPR uint32_t easy_spacing_seconds = 20 * 60;
BC_CONSTEXPR uint32_t target_spacing_seconds = 10 * 60;
BC_CONSTEXPR uint32_t target_timespan_seconds = 2 * 7 * 24 * 60 * 60;
BC_CONSTEXPR uint32_t timestamp_future_seconds = 2 * 60 * 60;
BC_CONSTEXPR uint32_t proof_of_work_limit = 0x1d00ffff;

// The upper and lower bounds for the retargeting timespan.
BC_CONSTEXPR uint32_t min_timespan =
    target_timespan_seconds / retargeting_factor;
BC_CONSTEXPR uint32_t max_timespan =
    target_timespan_seconds * retargeting_factor;

// The target number of blocks for 2 weeks of work (2016 blocks).
BC_CONSTEXPR size_t retargeting_interval =
    target_timespan_seconds / target_spacing_seconds;

// Fork constants.
//-----------------------------------------------------------------------------

// Consensus rule change activation and enforcement parameters.
BC_CONSTEXPR size_t first_version = 1;
BC_CONSTEXPR size_t bip34_version = 2;
BC_CONSTEXPR size_t bip66_version = 3;
BC_CONSTEXPR size_t bip65_version = 4;
BC_CONSTEXPR uint32_t bip9_version_bit0 = 0x00000001;
BC_CONSTEXPR uint32_t bip9_version_base = 0x20000000;

// Mainnet activation parameters (bip34-style activations).
BC_CONSTEXPR size_t mainnet_active = 750;
BC_CONSTEXPR size_t mainnet_enforce = 950;
BC_CONSTEXPR size_t mainnet_sample = 1000;

// Testnet activation parameters (bip34-style activations).
BC_CONSTEXPR size_t testnet_active = 51;
BC_CONSTEXPR size_t testnet_enforce = 75;
BC_CONSTEXPR size_t testnet_sample = 100;

// Mainnet frozen activation heights (frozen_activations).
BC_CONSTEXPR size_t mainnet_bip65_freeze = 388381;
BC_CONSTEXPR size_t mainnet_bip66_freeze = 363725;
BC_CONSTEXPR size_t mainnet_bip34_freeze = 227931;

// Testnet frozen activation heights (frozen_activations).
BC_CONSTEXPR size_t testnet_bip65_freeze = 581885;
BC_CONSTEXPR size_t testnet_bip66_freeze = 330776;
BC_CONSTEXPR size_t testnet_bip34_freeze = 21111;

// Block 514 is the first testnet block after date-based activation.
// Block 173805 is the first mainnet block after date-based activation.
BC_CONSTEXPR uint32_t bip16_activation_time = 0x4f779a80;

// Block 170060 was mined with an invalid p2sh (code shipped late).
// bitcointalk.org/index.php?topic=63165.msg788832#msg788832
static const config::checkpoint mainnet_bip16_exception_checkpoint
{
    "00000000000002dc756eebf4f49723ed8d30cc28a5f108eb94b1ba88ac4f9c22", 170060
};

// github.com/bitcoin/bips/blob/master/bip-0030.mediawiki#specification
static const config::checkpoint mainnet_bip30_exception_checkpoint1
{
    "00000000000a4d0a398161ffc163c503763b1f4360639393e0e4c8e300e0caec", 91842
};
static const config::checkpoint mainnet_bip30_exception_checkpoint2
{
    "00000000000743f190a18c5577a3c2d2a1f610ae9601ac046a38084ccb7cd721", 91880
};

// bip90 stops checking unspent duplicates above this bip34 activation.
static const config::checkpoint mainnet_bip34_active_checkpoint
{
    "000000000000024b89b42a942fe0d9fea3bb44ab7bd1b19115dd6a759c0808b8",
    mainnet_bip34_freeze
};
static const config::checkpoint testnet_bip34_active_checkpoint
{
    "0000000023b3a96d3484e5abb3755c413e7d41500f8e2a5c3f0dd01299cd8ef8",
    testnet_bip34_freeze
};

// These cannot be reactivated in a future branch due to window expiration.
static const config::checkpoint mainnet_bip9_bit0_active_checkpoint
{
    "000000000000000004a1b34462cb8aeebd5799177f7a29cf28f2d1961716b5b5", 419328
};
static const config::checkpoint testnet_bip9_bit0_active_checkpoint
{
    "00000000025e930139bac5c6c31a403776da130831ab85be56578f3fa75369bb", 770112
};

// Network protocol constants.
//-----------------------------------------------------------------------------

// Explicit size.
BC_CONSTEXPR size_t command_size = 12;

// Explicit limits.
BC_CONSTEXPR size_t max_address = 1000;
BC_CONSTEXPR size_t max_filter_add = 520;
BC_CONSTEXPR size_t max_filter_functions = 50;
BC_CONSTEXPR size_t max_filter_load = 36000;
BC_CONSTEXPR size_t max_get_blocks = 500;
BC_CONSTEXPR size_t max_get_headers = 2000;
BC_CONSTEXPR size_t max_get_data = 50000;
BC_CONSTEXPR size_t max_inventory = 50000;

// Effective limit given a 32 bit chain height boundary: 10 + log2(2^32) + 1.
BC_CONSTEXPR size_t max_locator = 43;

// Variable integer prefix sentinels.
BC_CONSTEXPR uint8_t varint_two_bytes = 0xfd;
BC_CONSTEXPR uint8_t varint_four_bytes = 0xfe;
BC_CONSTEXPR uint8_t varint_eight_bytes = 0xff;

// String padding sentinel.
BC_CONSTEXPR uint8_t string_terminator = 0x00;

// Currency unit constants (uint64_t).
//-----------------------------------------------------------------------------

BC_CONSTFUNC uint64_t max_money_recursive(uint64_t money)
{
    return money > 0 ? money + max_money_recursive(money >> 1) : 0;
}

BC_CONSTEXPR uint64_t satoshi_per_bitcoin = 100000000;
BC_CONSTFUNC uint64_t bitcoin_to_satoshi(uint64_t bitcoin_uints=1)
{
    return bitcoin_uints * satoshi_per_bitcoin;
}

BC_CONSTEXPR uint64_t initial_block_subsidy_bitcoin = 50;
BC_CONSTFUNC uint64_t initial_block_subsidy_satoshi()
{
    return bitcoin_to_satoshi(initial_block_subsidy_bitcoin);
}

BC_CONSTEXPR uint64_t subsidy_interval = 210000;
BC_CONSTEXPR uint64_t recursive_money = 0x02540be3f5;
BC_CONSTFUNC uint64_t max_money()
{
    ////// Optimize out the derivation of recursive_money.
    ////BITCOIN_ASSERT(recursive_money == max_money_recursive(
    ////    initial_block_subsidy_satoshi()));

    return subsidy_interval * recursive_money;
}

} // namespace libbitcoin

#endif

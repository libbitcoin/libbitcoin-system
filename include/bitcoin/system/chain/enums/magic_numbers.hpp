/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_MAGIC_NUMBERS_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_MAGIC_NUMBERS_HPP

#include <cstddef>
#include <cstdint>
#include <bitcoin/system/constants.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Consensus sentinels.
//-----------------------------------------------------------------------------

constexpr uint32_t no_previous_output = max_uint32;
constexpr uint32_t max_input_sequence = max_uint32;
constexpr uint64_t sighash_null_value = max_uint64;

// Script/interpreter consensus constants.
//-----------------------------------------------------------------------------

constexpr size_t max_counted_ops = 201;
constexpr size_t max_stack_size = 1000;
constexpr size_t max_script_size = 10000;
constexpr size_t max_push_data_size = 520;
constexpr size_t max_script_public_keys = 20;
constexpr size_t multisig_default_sigops = 20;
constexpr size_t max_number_size = 4;
constexpr size_t max_check_locktime_verify_number_size = 5;
constexpr size_t max_check_sequence_verify_number_size = 5;

// Validation consensus constants.
//-----------------------------------------------------------------------------

constexpr size_t min_coinbase_size = 2;
constexpr size_t max_coinbase_size = 100;
constexpr size_t coinbase_maturity = 100;
constexpr size_t median_time_past_interval = 11;
constexpr size_t locktime_threshold = 500000000;
constexpr size_t max_block_size = 1000000;
constexpr size_t max_sigops_factor = 50;
constexpr size_t max_block_sigops = max_block_size / max_sigops_factor;
constexpr uint64_t satoshi_per_bitcoin = 100000000;

// Relative locktime consensus constants.
//-----------------------------------------------------------------------------

constexpr size_t relative_locktime_min_version = 2;
constexpr size_t relative_locktime_disabled_bit = 31;
constexpr size_t relative_locktime_time_locked_bit = 22;
constexpr size_t relative_locktime_seconds_shift_left = 9;
constexpr size_t relative_locktime_mask_left = 16;

// Witness serialization consensus constants (bip141).
//-----------------------------------------------------------------------------

constexpr size_t fast_sigops_factor = 4;
constexpr size_t max_fast_sigops = fast_sigops_factor * max_block_sigops;
constexpr size_t light_weight_factor = 4;
constexpr size_t max_block_weight = light_weight_factor * max_block_size;
constexpr size_t base_size_contribution = 3;
constexpr size_t total_size_contribution = 1;
constexpr size_t min_witness_program = 2;
constexpr size_t max_witness_program = 40;
constexpr uint32_t witness_head = 0xaa21a9ed;
constexpr uint8_t witness_marker = 0x00;
constexpr uint8_t witness_enabled = 0x01;

// Policy constants.
//-----------------------------------------------------------------------------

constexpr size_t max_null_data_size = 80;

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif

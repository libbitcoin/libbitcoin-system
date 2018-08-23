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
#ifndef LIBBITCOIN_SETTINGS_HPP
#define LIBBITCOIN_SETTINGS_HPP

#include <bitcoin/bitcoin/config/block.hpp>
#include <bitcoin/bitcoin/config/settings.hpp>
#include <bitcoin/bitcoin/define.hpp>

#include <cstdint>
#include <cstddef>
#include <string>

namespace libbitcoin {

/// Common database configuration settings, properties not thread safe.
class BC_API settings
{
public:
    settings();
    settings(config::settings context);

    // Currency unit helper methods (uint64_t).
    //--------------------------------------------------------------------------

    uint64_t bitcoin_to_satoshi(uint64_t bitcoin_units=1) const;

    uint32_t retargeting_factor;
    uint32_t target_spacing_seconds;
    uint32_t easy_spacing_seconds;
    uint32_t timestamp_future_seconds;
    uint32_t target_timespan_seconds;
    uint32_t proof_of_work_limit;

    // The upper and lower bounds for the retargeting timespan.
    uint32_t minimum_timespan;
    uint32_t maximum_timespan;

    // The target number of blocks for 2 weeks of work (2016 blocks).
    size_t retargeting_interval;

    config::block genesis_block;

    // Fork settings.
    //--------------------------------------------------------------------------

    // Consensus rule change activation and enforcement parameters.
    uint32_t first_version;
    uint32_t bip34_version;
    uint32_t bip66_version;
    uint32_t bip65_version;
    uint32_t bip9_version_bit0;
    uint32_t bip9_version_bit1;
    uint32_t bip9_version_base;

    // Activation parameters (bip34-style activations).
    size_t activation_threshold;
    size_t enforcement_threshold;
    size_t activation_sample;

    // Frozen activation heights (frozen_activations).
    size_t bip65_freeze;
    size_t bip66_freeze;
    size_t bip34_freeze;

    // Block 514 is the first testnet block after date-based activation.
    // Block 173805 is the first mainnet block after date-based activation.
    // The first mainnet activation window hardwired in satoshi 0.6.0rc1 failed.
    uint32_t bip16_activation_time;

    // bip90 stops checking unspent duplicates above this bip34 activation.
    config::checkpoint bip34_active_checkpoint;

    // This cannot be reactivated in a future branch due to window expiration.
    config::checkpoint bip9_bit0_active_checkpoint;

    // This cannot be reactivated in a future branch due to window expiration.
    config::checkpoint bip9_bit1_active_checkpoint;

    // Currency unit settings (uint64_t).
    //--------------------------------------------------------------------------

    uint64_t satoshi_per_bitcoin;
    uint64_t initial_block_subsidy_bitcoin;
    uint64_t recursive_money;
    uint64_t subsidy_interval;
    uint64_t max_money;
};

} // namespace libbitcoin

#endif

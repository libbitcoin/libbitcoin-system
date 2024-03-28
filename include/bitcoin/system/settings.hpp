/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_SETTINGS_HPP
#define LIBBITCOIN_SYSTEM_SETTINGS_HPP

#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/config/config.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Common database configuration settings, not thread safe.
class BC_API settings
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(settings);

    settings() NOEXCEPT;
    settings(chain::selection context) NOEXCEPT;

    /// Computed properties.
    /// -----------------------------------------------------------------------

    /// These are not used by the system library.
    virtual uint64_t max_money() const NOEXCEPT;
    virtual uint64_t initial_subsidy() const NOEXCEPT;
    virtual uint64_t bitcoin_to_satoshi(uint64_t value) const NOEXCEPT;

    /// These are used by chain_state (only).
    virtual uint32_t minimum_timespan() const NOEXCEPT;
    virtual uint32_t maximum_timespan() const NOEXCEPT;
    virtual size_t retargeting_interval() const NOEXCEPT;

    /// Configured forks.
    /// -----------------------------------------------------------------------
    /// These are used by chain_state (only).

    chain::chain_state::forks_t forks{};

    /// Consensus parameters.
    /// -----------------------------------------------------------------------

    /// These are not used by the system library.
    uint64_t initial_subsidy_bitcoin;
    uint32_t subsidy_interval_blocks;
    uint32_t timestamp_limit_seconds;
    chain::checkpoints checkpoints{};
    config::block genesis_block{};

    /// These are used by chain_state (only).
    uint32_t retargeting_factor;
    uint32_t retargeting_interval_seconds;
    uint32_t block_spacing_seconds;
    uint32_t proof_of_work_limit;

    /// Activation settings.
    /// -----------------------------------------------------------------------
    /// These are used by chain_state (only).

    /// Consensus rule change activation and enforcement parameters.
    uint32_t first_version;
    uint32_t bip34_version;
    uint32_t bip66_version;
    uint32_t bip65_version;
    uint32_t bip9_version_bit0;
    uint32_t bip9_version_bit1;
    uint32_t bip9_version_base;

    /// Block 514 is the first testnet block after date-based activation.
    /// Block 173805 is the first mainnet block after date-based activation.
    /// First mainnet activation window hardwired in satoshi 0.6.0rc1 failed.
    uint32_t bip16_activation_time{};

    /// Activation parameters (bip34-style activations).
    size_t bip34_activation_threshold{};
    size_t bip34_enforcement_threshold{};
    size_t bip34_activation_sample{};

    /// Frozen activation heights (frozen_activations).
    size_t bip90_bip34_height{};
    size_t bip90_bip65_height{};
    size_t bip90_bip66_height{};

    size_t bip30_reactivate_height{};
    chain::checkpoint bip30_deactivate_checkpoint{};

    /// This cannot be reactivated in a future branch due to window expiration.
    chain::checkpoint bip9_bit0_active_checkpoint{};

    /// This cannot be reactivated in a future branch due to window expiration.
    chain::checkpoint bip9_bit1_active_checkpoint{};

    /// A block that is presumed to be valid but not required to be present.
    chain::checkpoint milestone{};

    /// The minimum work for any branch to be considered valid.
    config::hash256 minimum_work{};
};

} // namespace system
} // namespace libbitcoin

#endif

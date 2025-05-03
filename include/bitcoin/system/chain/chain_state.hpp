/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_CHAIN_STATE_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_CHAIN_STATE_HPP

#include <deque>
#include <memory>
#include <bitcoin/system/chain/checkpoint.hpp>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/enums/flags.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/forks.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

class settings;

namespace chain {
    
class block;
class header;

/// system::settings are using within chain_state to compute context, but are
/// not used directly within chain classes, which would be an abstraction leak.
class BC_API chain_state
{
public:
    DELETE_COPY_MOVE_DESTRUCT(chain_state);

    typedef std::deque<uint32_t> bitss;
    typedef std::deque<uint32_t> versions;
    typedef std::deque<uint32_t> timestamps;
    typedef std::shared_ptr<const chain_state> cptr;
    typedef struct { size_t count; size_t high; } range;

    /// Heights used to identify construction requirements.
    /// All values are lower-bounded by the genesis block height.
    /// Obtaining all values even in the case where the set of queries could be
    /// short-circuited allows us to cache results for the next block,
    /// minimizing overall querying.
    struct map
    {
        // This sentinel indicates that the value was not requested.
        static const size_t unrequested = max_size_t;

        /// [block - 1, floor(block - 2016, 0)] mainnet: 1, testnet: 2016|0
        range bits{};

        /// [block - 1, floor(block - 1000, 0)] mainnet: 1000, testnet: 100
        range version{};

        /// [block - 1, floor(block - 11, 0)]
        range timestamp{};

        /// (block - (block % 2016 == 0 ? 2016 : block % 2016))
        size_t timestamp_retarget{};

        /// mainnet: 227931, testnet: 21111 (or map::unrequested)
        size_t bip30_deactivate_height{ unrequested };

        /// mainnet: 419328, testnet: 770112 (or map::unrequested)
        size_t bip9_bit0_height{ unrequested };

        /// mainnet: 481824, testnet: 834624 (or map::unrequested)
        size_t bip9_bit1_height{ unrequested };
    };

    /// Values used to populate chain state at the target height.
    struct data
    {
        /// Header values are based on this height.
        size_t height{};

        /// Hash of the candidate block or null_hash for memory pool.
        hash_digest hash{};

        /// Hash of the bip34 block that inactivates bip30, or null_hash.
        hash_digest bip30_deactivate_hash{};

        /// Hash of the bip9_bit0 block or null_hash if unrequested.
        hash_digest bip9_bit0_hash{};

        /// Hash of the bip9_bit1 block or null_hash if unrequested.
        hash_digest bip9_bit1_hash{};

        /// Sum of all work from genesis to block height.
        uint256_t cumulative_work{};

        /// Values must be ordered by height with high (block - 1) last.
        struct
        {
            uint32_t self{};
            bitss ordered{};
        } bits;

        /// Values must be ordered by height with high (block - 1) last.
        struct
        {
            uint32_t self{};
            versions ordered{};
        } version;

        /// Values must be ordered by height with high (block - 1) last.
        struct
        {
            uint32_t self{};
            uint32_t retarget{};
            timestamps ordered{};
        } timestamp;
    };

    static constexpr uint32_t minimum_timespan(
        uint32_t retargeting_interval_seconds,
        uint32_t retargeting_factor) NOEXCEPT
    {
        return retargeting_interval_seconds / retargeting_factor;
    }

    static constexpr uint32_t maximum_timespan(
        uint32_t retargeting_interval_seconds,
        uint32_t retargeting_factor) NOEXCEPT
    {
        return retargeting_interval_seconds * retargeting_factor;
    }

    static constexpr uint32_t retargeting_interval(
        uint32_t retargeting_interval_seconds,
        uint32_t block_spacing_seconds) NOEXCEPT
    {
        return retargeting_interval_seconds / block_spacing_seconds;
    }

    /// Get the map of chain data to query.
    static map get_map(size_t height,
        const system::settings& settings) NOEXCEPT;

    /// The block version to signal based on configured forks.
    static uint32_t signal_version(
        const system::settings& settings) NOEXCEPT;

    /// Create pool state from top chain top block state.
    chain_state(const chain_state& top,
        const system::settings& settings) NOEXCEPT;

    /// Create block state from tx pool chain state of same height.
    chain_state(const chain_state& pool, const chain::block& block,
        const system::settings& settings) NOEXCEPT;

    /// Create header state from header pool chain state of parent block.
    chain_state(const chain_state& parent, const chain::header& header,
        const system::settings& settings) NOEXCEPT;

    /// Forks must match those provided for map creation.
    chain_state(data&& values, const system::settings& settings) NOEXCEPT;

    /// Properties.
    chain::context context() const NOEXCEPT;
    const hash_digest& hash() const NOEXCEPT;
    const uint256_t& cumulative_work() const NOEXCEPT;
    uint32_t minimum_block_version() const NOEXCEPT;
    uint32_t work_required() const NOEXCEPT;
    uint32_t timestamp() const NOEXCEPT;
    uint32_t median_time_past() const NOEXCEPT;
    uint32_t flags() const NOEXCEPT;
    size_t height() const NOEXCEPT;

protected:
    struct activations
    {
        /// The forks that are active at this height.
        uint32_t flags;

        /// The minimum block version required at this height.
        uint32_t minimum_block_version;
    };

    /// No failure sentinel.
    static activations activation(const data& values,
        const forks& forks, const system::settings& settings) NOEXCEPT;

    /// Returns zero if data is invalid.
    static uint32_t median_time_past(const data& values,
        const forks& forks) NOEXCEPT;

    /// Returns zero if data is invalid.
    static uint32_t work_required(const data& values,
        const forks& forks, const system::settings& settings) NOEXCEPT;

private:
    static size_t bits_count(size_t height, const forks& forks,
        size_t retargeting_interval) NOEXCEPT;
    static size_t version_count(size_t height, const forks& forks,
        size_t bip34_activation_sample) NOEXCEPT;
    static size_t timestamp_count(size_t height, const forks& forks) NOEXCEPT;
    static size_t retarget_height(size_t height, const forks& forks,
        size_t retargeting_interval) NOEXCEPT;

    static size_t bip30_deactivate_height(size_t height,
        const checkpoint& bip30_deactivate_checkpoint) NOEXCEPT;
    static size_t bip9_bit0_height(size_t height,
        const checkpoint& bip9_bit0_active_checkpoint) NOEXCEPT;
    static size_t bip9_bit1_height(size_t height,
        const checkpoint& bip9_bit1_active_checkpoint) NOEXCEPT;

    static data to_pool(const chain_state& top,
        const system::settings& settings) NOEXCEPT;
    static data to_block(const chain_state& pool, const block& block,
        const system::settings& settings) NOEXCEPT;
    static data to_header(const chain_state& parent, const header& header,
        const system::settings& settings) NOEXCEPT;

    static uint32_t work_required_retarget(const data& values,
        const forks& forks, uint32_t proof_of_work_limit,
        uint32_t minimum_timespan, uint32_t maximum_timespan,
        uint32_t retargeting_interval_seconds) NOEXCEPT;
    static uint32_t retarget_timespan(const data& values,
        uint32_t minimum_timespan, uint32_t maximum_timespan) NOEXCEPT;
    static uint32_t easy_work_required(const data& values,
        size_t retargeting_interval, uint32_t proof_of_work_limit,
        uint32_t block_spacing_seconds) NOEXCEPT;

    // These are thread safe.
    const data data_;
    const forks& forks_;
    const activations activations_;
    const uint32_t work_required_;
    const uint32_t median_time_past_;
    const uint256_t cumulative_work_;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif

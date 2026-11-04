/**
 * Copyright (c) 2011-2026 libbitcoin developers
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

    /// bip9 deployment states, by period.
    enum class deployment_state : uint8_t
    {
        defined,
        started,
        locked_in,
        active,
        failed
    };

    /// The number of bip9 deployments (settings).
    static constexpr size_t deployment_count = 8;

    /// bip9 deployment parameters. A deployment is signaled if it is
    /// configured with a nonzero threshold, unless always active.
    struct deployment
    {
        /// A start of this value is active above genesis, without signaling.
        static constexpr uint32_t always_active = max_uint32;

        /// Version bit offset signaled.
        uint32_t bit{};

        /// Rule flags activated.
        uint32_t flags{};

        /// Median time past at/above which signaling starts.
        uint32_t start{};

        /// Median time past at/above which signaling fails.
        uint32_t timeout{};

        /// Signaling blocks of a period required for lock in.
        size_t threshold{};

        /// Height at/above which a lock in activates.
        size_t activation_height{};

        /// Threshold is checked before timeout (speedy trial).
        bool speedy{};

        bool operator==(const deployment& other) const NOEXCEPT = default;
    };

    /// Heights used to identify construction requirements.
    /// All values are lower-bounded by the genesis block height.
    /// Obtaining all values even in the case where the set of queries could be
    /// short-circuited allows us to cache results for the next block,
    /// minimizing overall querying.
    struct map
    {
        // This sentinel indicates that the value was not requested.
        static constexpr size_t unrequested = max_size_t;

        /// [block - 1, floor(block - 2016, 0)] mainnet: 1, testnet: 2016|0
        range bits{};

        /// [block - 1, floor(block - 1000, 0)] mainnet: 1000, testnet: 100
        range version{};

        /// [block - 1, floor(block - 11, 0)]
        range timestamp{};

        /// (block - (block % 2016 == 0 ? 2016 : block % 2016))
        size_t timestamp_retarget{};

        /// Periods of a signaled bip9 deployment, sampled from high down to
        /// the first boundary with median time past below start.
        struct signal
        {
            /// (block - (block % period)) (or map::unrequested)
            size_t high{ unrequested };
            size_t period{};
            uint32_t base{};
            uint32_t bit{};
            uint32_t start{};
            uint32_t timeout{};
        };

        /// Signal periods by deployment (if signaled).
        std::array<signal, deployment_count> signals{};
    };

    /// Values used to populate chain state at the target height.
    struct data
    {
        /// Header values are based on this height.
        size_t height{};

        /// Hash of the candidate block or null_hash for memory pool.
        hash_digest hash{};

        /// Signaling of a bip9 deployment.
        struct signal
        {
            /// Median time past at a period boundary and signal count of the
            /// period that it closes.
            struct boundary
            {
                uint32_t median_time_past{};
                size_t count{};
            };

            /// State of the block's period.
            deployment_state state{};

            /// Signaling blocks of the block's period below the block.
            size_t count{};

            /// Boundaries from map high down to the first above the last
            /// defined boundary, by descending height (consumed on
            /// construction).
            std::vector<boundary> ordered{};
        };

        /// Signaling by deployment (if signaled).
        std::array<signal, deployment_count> signals{};

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

    /// The flags of all configured forks, independent of activation.
    static uint32_t configured_flags(const forks& forks) NOEXCEPT;

    /// The median of the timestamps (zero if empty).
    static uint32_t median(const timestamps& times) NOEXCEPT;

    /// The block version signals the bip9 deployment bit.
    static bool is_signal(uint32_t version, uint32_t base,
        uint32_t bit) NOEXCEPT;

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
    uint32_t previous_timestamp() const NOEXCEPT;
    uint32_t retarget_timestamp() const NOEXCEPT;
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

    /// The deployment state of the period opened by the boundary.
    static deployment_state next_state(const deployment& deployment,
        deployment_state state, uint32_t median_time_past, size_t count,
        size_t boundary) NOEXCEPT;

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

    static map::signal bip9_signal(size_t height,
        const deployment& deployment,
        const system::settings& settings) NOEXCEPT;

    static void promote(data::signal& signal, const deployment& deployment,
        uint32_t version, uint32_t median_time_past, size_t height,
        const system::settings& settings) NOEXCEPT;
    static void reduce(data::signal& signal, const deployment& deployment,
        size_t height, size_t period) NOEXCEPT;

    static data to_pool(const chain_state& top,
        const system::settings& settings) NOEXCEPT;
    static data to_block(const chain_state& pool,
        const block& block) NOEXCEPT;
    static data to_header(const chain_state& parent, const header& header,
        const system::settings& settings) NOEXCEPT;
    static data to_values(data values,
        const system::settings& settings) NOEXCEPT;

    static uint32_t work_required_retarget_bits(const data& values,
        const forks& forks,
        uint32_t proof_of_work_limit,
        uint32_t retargeting_interval) NOEXCEPT;
    static uint32_t work_required_retarget(const data& values,
        const forks& forks, uint32_t proof_of_work_limit,
        uint32_t minimum_timespan,
        uint32_t maximum_timespan,
        uint32_t retargeting_interval,
        uint32_t retargeting_interval_seconds) NOEXCEPT;
    static uint32_t retarget_timespan(const data& values,
        uint32_t minimum_timespan,
        uint32_t maximum_timespan) NOEXCEPT;
    static uint32_t easy_work_required(const data& values,
        uint32_t retargeting_interval,
        uint32_t proof_of_work_limit,
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

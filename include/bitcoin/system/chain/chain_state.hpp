/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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

#include <memory>
#include <deque>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/enums/forks.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

class settings;

namespace chain {
    
class block;
class header;

class BC_API chain_state
{
public:
    DELETE_COPY_MOVE_DESTRUCT(chain_state);

    typedef std::deque<uint32_t> bitss;
    typedef std::deque<uint32_t> versions;
    typedef std::deque<uint32_t> timestamps;
    typedef struct { size_t count; size_t high; } range;

    typedef std::shared_ptr<chain_state> ptr;

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

        /// (block - 0), used only for populating bits.ordered on increment.
        size_t bits_self{};

        /// [block - 1, floor(block - 1000, 0)] mainnet: 1000, testnet: 100
        range version{};

        /// (block - 0)
        size_t version_self{};

        /// [block - 1, floor(block - 11, 0)]
        range timestamp{};

        /// (block - 0)
        size_t timestamp_self{};

        /// (block - (block % 2016 == 0 ? 2016 : block % 2016))
        size_t timestamp_retarget{};
    };

    /// Values used to populate chain state at the target height.
    struct data
    {
        /// Header values are based on this height.
        size_t height{};

        /// Hash of the candidate block or null_hash for memory pool.
        ////hash_digest hash;

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

    /// Checkpoints must be ordered by height with greatest at back.
    static map get_map(size_t height,
        const system::settings& settings) NOEXCEPT;

    /// The block version to signal based on active forks.
    static uint32_t signal_version(uint32_t forks,
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

    /// Checkpoints must be ordered by height with greatest at back.
    /// Forks and checkpoints must match those provided for map creation.
    chain_state(data&& values, const system::settings& settings) NOEXCEPT;

    /// Properties.
    chain::context context() const NOEXCEPT;
    uint32_t minimum_block_version() const NOEXCEPT;
    uint32_t work_required() const NOEXCEPT;
    uint32_t timestamp() const NOEXCEPT;
    uint32_t median_time_past() const NOEXCEPT;
    uint32_t forks() const NOEXCEPT;
    size_t height() const NOEXCEPT;

protected:
    struct activations
    {
        /// The forks that are active at this height.
        uint32_t forks;

        /// The minimum block version required at this height.
        uint32_t minimum_block_version;
    };

    /// No failure sentinel.
    static activations activation(const data& values, uint32_t forks,
        const system::settings& settings) NOEXCEPT;

    /// Returns zero if data is invalid.
    static uint32_t median_time_past(const data& values,
        uint32_t forks) NOEXCEPT;

    /// Returns zero if data is invalid.
    static uint32_t work_required(const data& values, uint32_t forks,
        const system::settings& settings) NOEXCEPT;

private:
    static size_t bits_count(size_t height, uint32_t forks,
        size_t retargeting_interval) NOEXCEPT;
    static size_t version_count(size_t height, uint32_t forks,
        size_t activation_sample) NOEXCEPT;
    static size_t timestamp_count(size_t height, uint32_t forks) NOEXCEPT;
    static size_t retarget_height(size_t height, uint32_t forks,
        size_t retargeting_interval) NOEXCEPT;

    static data to_pool(const chain_state& top,
        const system::settings& settings) NOEXCEPT;
    static data to_block(const chain_state& pool, const block& block) NOEXCEPT;
    static data to_header(const chain_state& parent, const header& header,
        const system::settings& settings) NOEXCEPT;

    static uint32_t work_required_retarget(const data& values, uint32_t forks,
        uint32_t proof_of_work_limit, uint32_t minimum_timespan,
        uint32_t maximum_timespan, uint32_t retargeting_interval_seconds) NOEXCEPT;
    static uint32_t retarget_timespan(const data& values,
        uint32_t minimum_timespan, uint32_t maximum_timespan) NOEXCEPT;
    static uint32_t easy_work_required(const data& values,
        size_t retargeting_interval, uint32_t proof_of_work_limit,
        uint32_t block_spacing_seconds) NOEXCEPT;

    // These are thread safe.
    const data data_;
    const uint32_t forks_;
    const activations active_;
    const uint32_t work_required_;
    const uint32_t median_time_past_;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif

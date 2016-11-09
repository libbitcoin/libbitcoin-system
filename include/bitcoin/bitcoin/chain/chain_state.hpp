/**
 * Copyright (c) 2011-2016 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_CHAIN_CHAIN_STATE_HPP
#define LIBBITCOIN_CHAIN_CHAIN_STATE_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include <bitcoin/bitcoin/chain/script/opcode.hpp>
#include <bitcoin/bitcoin/chain/script/rule_fork.hpp>
#include <bitcoin/bitcoin/config/checkpoint.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {
namespace chain {

class BC_API chain_state
{
public:
    typedef std::vector<uint32_t> bitss;
    typedef std::vector<uint32_t> versions;
    typedef std::vector<uint32_t> timestamps;
    typedef struct { size_t count; size_t high; } range;

    typedef std::shared_ptr<chain_state> ptr;
    typedef config::checkpoint::list checkpoints;

    /// Heights used to identify construction requirements.
    /// All values are lower-bounded by the genesis block height.
    /// Obtaining all values even in the case where the set of queries could be
    /// short-circuited allows us to cache results for the next block,
    /// minimizing overall querying.
    struct map
    {
        /// [block - 1, floor(block - 2016, 0)] mainnet: low == high (unused)
        range bits;

        /// [block - 1, floor(block - 1000, 0)] testnet: 100 (vs. 1000)
        range version;

        /// [block - 1, floor(block - 11 ,0)]
        range timestamp;

        /// block - 0 [mainnet: high (unused)]
        size_t timestamp_self;

        /// block - 2016 [or high (unused)]
        size_t timestamp_retarget;
    };

    /// Values used to populate chain state at the target height.
    struct data
    {
        /// Header values are based on a testnet map.
        bool testnet;

        /// All forks are enabled at the corresponding height (from cache).
        bool enabled;

        /// Header values are based on this height.
        size_t height;

        /// Hash of the candidate block or null_hash for memory pool.
        hash_digest hash;

        /// Values must be ordered by height with high (block - 1) last.
        struct { bitss ordered; } bits;

        /// Values are unordered.
        struct { versions unordered; } version;

        /// Values must be ordered by height with high (block - 1) last.
        struct
        {
            uint32_t self;
            uint32_t retarget;
            timestamps ordered;
        } timestamp;
    };

    // Compute the height map from any height and testnet flag.
    static map get_map(size_t height, bool enabled, bool testnet);

    /// Checkpoints must be ordered by height with greatest at back.
    chain_state(data&& values, const checkpoints& checkpoints);

    /// Properties.
    size_t height() const;
    uint32_t enabled_forks() const;
    uint32_t minimum_version() const;
    uint32_t median_time_past() const;
    uint32_t work_required() const;

    /// Construction with zero height or any empty array causes invalid state.
    bool is_valid() const;

    /// Determine if all forks are enabled (cache for next height construct).
    bool is_enabled() const;

    /// Determine if the flag is set in the active_forks member.
    bool is_enabled(rule_fork flag) const;

    /// Determine if the flag is set and enabled for a given block's version.
    bool is_enabled(uint32_t block_version, rule_fork flag) const;

    /// Determine if the block hash fails a checkpoint at this height.
    bool is_checkpoint_failure(const hash_digest& hash) const;

    /// This height requires full validation due to no checkpoint coverage.
    bool use_full_validation() const;

protected:
    struct activations
    {
        // The forks that are active at this height.
        uint32_t forks;

        // The minimum block version required at this height.
        uint32_t minimum_version;
    };

    static activations activation(const data& values);
    static uint32_t median_time_past(const data& values);
    static uint32_t work_required(const data& values);

private:
    // mainnet and testnet
    static uint32_t work_required_retarget(const data& values);
    static uint32_t retarget_timespan(const chain_state::data& values);

    // testnet only
    static uint32_t work_required_testnet(const data& values);
    static uint32_t elapsed_time_limit(const chain_state::data& values);
    static bool is_retarget_or_non_limit(size_t height, uint32_t bits);
    static bool is_retarget_height(size_t height);

    // This is retained as an optimization for other constructions.
    // A similar height clone can be partially computed, reducing query cost.
    const data data_;

    // These are computed on construct from sample and checkpoints.
    const activations active_;
    const uint32_t median_time_past_;
    const uint32_t work_required_;

    // Configured checkpoints are used to answer is_checkpoint_failure.
    const config::checkpoint::list& checkpoints_;
};

} // namespace chain
} // namespace libbitcoin

#endif

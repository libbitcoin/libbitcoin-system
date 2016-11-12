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
#include <bitcoin/bitcoin/chain/chain_state.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <bitcoin/bitcoin/chain/chain_state.hpp>
#include <bitcoin/bitcoin/chain/compact.hpp>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/config/checkpoint.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/math/limits.hpp>
#include <bitcoin/bitcoin/math/uint256.hpp>
#include <bitcoin/bitcoin/script/opcode.hpp>
#include <bitcoin/bitcoin/script/rule_fork.hpp>
#include <bitcoin/bitcoin/unicode/unicode.hpp>

namespace libbitcoin {
namespace chain {

// Inlines.
//-----------------------------------------------------------------------------

inline size_t version_sample_size(bool testnet)
{
    return testnet ? testnet_sample : mainnet_sample;
}

inline bool is_active(size_t count, bool testnet)
{
    return count >= (testnet ? testnet_active : mainnet_active);
}

inline bool is_enforced(size_t count, bool testnet)
{
    return count >= (testnet ? testnet_enforce : mainnet_enforce);
}

inline bool is_bip16_exception(size_t height, const hash_digest& hash,
    bool testnet)
{
    return !testnet &&
        height == mainnet_bip16_exception_checkpoint.height() &&
        hash == mainnet_bip16_exception_checkpoint.hash();
}

inline bool is_bip30_exception(size_t height, const hash_digest& hash,
    bool testnet)
{
    return !testnet &&
        (/*(height == mainnet_bip30_exception_checkpoint1.height() &&
        hash == mainnet_bip30_exception_checkpoint1.hash()) ||*/
        (height == mainnet_bip30_exception_checkpoint2.height() &&
        hash == mainnet_bip30_exception_checkpoint2.hash()));
}

inline uint32_t timestamp_high(const chain_state::data& values)
{
    return values.timestamp.ordered.back();
}

inline uint32_t bits_high(const chain_state::data& values)
{
    return values.bits.ordered.back();
}

// Statics.
//-----------------------------------------------------------------------------
// non-public

chain_state::activations chain_state::activation(const data& values)
{
    const auto testnet = values.testnet;
    const auto& history = values.version.unordered;

    // Declare version predicates.
    const auto ge_4 = [](uint32_t version) { return version >= bip65_version; };
    const auto ge_3 = [](uint32_t version) { return version >= bip66_version; };
    const auto ge_2 = [](uint32_t version) { return version >= bip34_version; };

    // Compute version summaries.
    const auto count_4 = std::count_if(history.begin(), history.end(), ge_4);
    const auto count_3 = std::count_if(history.begin(), history.end(), ge_3);
    const auto count_2 = std::count_if(history.begin(), history.end(), ge_2);

    // Initialize activation results with genesis values.
    activations result{ rule_fork::no_rules, first_version };

    // bip65 is activated based on 75% of preceding 1000 mainnet blocks.
    if (is_active(count_4, testnet))
        result.forks |= rule_fork::bip65_rule;

    // bip66 is activated based on 75% of preceding 1000 mainnet blocks.
    if (is_active(count_3, testnet))
        result.forks |= rule_fork::bip66_rule;

    // bip34 is activated based on 75% of preceding 1000 mainnet blocks.
    if (is_active(count_2, testnet))
        result.forks |= rule_fork::bip34_rule;

    // bip30 is active for all but two mainnet blocks that violate the rule.
    // These two blocks each have a coinbase transaction that exctly duplicates
    // another that is not spent by the arrival of the corresponding duplicate.
    if (!is_bip30_exception(values.height, values.hash, testnet))
        result.forks |= rule_fork::bip30_rule;

    // bip16 is activated with a one-time test on mainnet/testnet (~55% rule).
    // There was one invalid p2sh tx mined after that time (code shipped late).
    if (values.timestamp.self >= bip16_activation_time &&
        !is_bip16_exception(values.height, values.hash, testnet))
        result.forks |= rule_fork::bip16_rule;

    // version 4/3/2 enforced based on 95% of preceding 1000 mainnet blocks.
    if (is_enforced(count_4, testnet))
        result.minimum_version = bip65_version;
    else if (is_enforced(count_3, testnet))
        result.minimum_version = bip66_version;
    else if (is_enforced(count_2, testnet))
        result.minimum_version = bip34_version;
    else
        result.minimum_version = first_version;

    return result;
}

uint32_t chain_state::median_time_past(const data& values)
{
    // Create a copy for the in-place sort.
    auto times = values.timestamp.ordered;

    // Sort the times by value to obtain the median.
    std::sort(times.begin(), times.end());

    // Consensus defines median time using modulo 2 element selection.
    // This differs from arithmetic median which averages two middle values.
    return times.empty() ? 0 : times[times.size() / 2];
}

uint32_t chain_state::work_required(const data& values)
{
    // Invalid parameter via public interface, test is_valid for results.
    if (values.height == 0)
        return{};

    if (is_retarget_height(values.height))
        return work_required_retarget(values);

    if (values.testnet)
        return work_required_testnet(values);

    return bits_high(values);
}

// [CalculateNextWorkRequired]
uint32_t chain_state::work_required_retarget(const data& values)
{
    static const uint256_t pow_limit(compact{ proof_of_work_limit });

    const compact bits(bits_high(values));
    BITCOIN_ASSERT_MSG(!bits.is_overflowed(), "previous block has bad bits");

    uint256_t target(bits);
    target *= retarget_timespan(values);
    target /= target_timespan_seconds;

    // The proof_of_work_limit constant is pre-normalized.
    return target > pow_limit ? proof_of_work_limit : compact(target).normal();
}

// Get the bounded total time spanning the highest 2016 blocks.
uint32_t chain_state::retarget_timespan(const chain_state::data& values)
{
    const auto high = timestamp_high(values);
    const auto retarget = values.timestamp.retarget;
    BITCOIN_ASSERT_MSG(retarget != 0xbaadf00d, "retarget time is unpopulated");

    //*************************************************************************
    // CONSENSUS: subtract unsigned 32 bit numbers in signed 64 bit space in
    // order to prevent underflow before applying the range constraint.
    //*************************************************************************
    const auto timespan = cast_subtract<int64_t>(high, retarget);
    return range_constrain(timespan, min_timespan, max_timespan);
}

// [GetNextWorkRequired::fPowAllowMinDifficultyBlocks]
uint32_t chain_state::work_required_testnet(const data& values)
{
    BITCOIN_ASSERT(values.height != 0);

    // If the time limit has passed allow a minimum difficulty block.
    if (values.timestamp.self > elapsed_time_limit(values))
        return proof_of_work_limit;

    auto height = values.height;
    auto& bits = values.bits.ordered;

    // Reverse iterate the ordered-by-height list of header bits.
    for (auto bit = bits.rbegin(); bit != bits.rend(); ++bit)
        if (is_retarget_or_non_limit(--height, *bit))
            return *bit;

    // Since the set of heights is either a full retarget range or ends at
    // zero this is not reachable unless the data set is invalid.
    BITCOIN_ASSERT(false);
    return proof_of_work_limit;
}

uint32_t chain_state::elapsed_time_limit(const chain_state::data& values)
{
    const int64_t high = timestamp_high(values);
    const int64_t spacing = double_spacing_seconds;

    //*************************************************************************
    // CONSENSUS: add unsigned 32 bit numbers in signed 64 bit space in
    // order to prevent overflow before applying the domain constraint.
    //*************************************************************************
    return domain_constrain<uint32_t>(cast_add<int64_t>(high, spacing));
}

// A retarget height, or a block that does not have proof_of_work_limit bits.
bool chain_state::is_retarget_or_non_limit(size_t height, uint32_t bits)
{
    // Zero is a retarget height, ensuring termination before height underflow.
    // This is guaranteed, just asserting here to document the safeguard.
    BITCOIN_ASSERT_MSG(is_retarget_height(0), "loop overflow potential");

    return bits != proof_of_work_limit || is_retarget_height(height);
}

// Determine if height is a multiple of retargeting_interval.
bool chain_state::is_retarget_height(size_t height)
{
    return (height % retargeting_interval) == 0;
}

// Publics.
//-----------------------------------------------------------------------------

// static
chain_state::map chain_state::get_map(size_t height, bool enabled,
    bool testnet)
{
    // Invalid parameter in public interface, defaults indicate failure.
    if (height == 0)
        return{};

    map map;

    // Bits.
    //-------------------------------------------------------------------------
    // The height bound of the reverse (high to low) retarget search.
    map.bits.high = height - 1;

    // Mainnet doesn't do retarget search.
    map.bits.count = testnet ? std::min(height, retargeting_interval) : 1;

    // Timestamp.
    //-------------------------------------------------------------------------
    // The height bound of the median time past function.
    // Height must be a positive multiple of interval, so underflow safe.
    map.timestamp.high = height - 1;
    map.timestamp.count = std::min(height, median_time_past_interval);

    // Additional timestamps required (or timestamp_unrequested for not).
    map.timestamp_self = height;
    map.timestamp_retarget = is_retarget_height(height) ?
        height - retargeting_interval : map::timestamp_unrequested;

    // Version.
    //-------------------------------------------------------------------------
    // The height bound of the version sample for activations.
    map.version.high = height - 1;
    map.version.count = enabled ? 
        std::min(height, version_sample_size(testnet)) : 0;

    // If too small to activate set count to zero to avoid unnecessary queries.
    map.version.count = is_active(map.version.count, testnet) ?
        map.version.count : 0;

    return map;
}

// Constructor.
chain_state::chain_state(data&& values, const checkpoints& checkpoints)
  : data_(std::move(values)),
    checkpoints_(checkpoints),
    active_(activation(data_)),
    work_required_(work_required(data_)),
    median_time_past_(median_time_past(data_))
{
}

// Semantic invalidity can also arise from too many/few values in the arrays.
// The same computations used to specify the ranges could detect such errors.
// These are the conditions that would cause exception during execution.
bool chain_state::is_valid() const
{
    return data_.height != 0;
}

// Properties.
//-----------------------------------------------------------------------------

bool chain_state::is_enabled() const
{
    return is_enabled(rule_fork::bip65_rule);
}

size_t chain_state::height() const
{
    return data_.height;
}

uint32_t chain_state::enabled_forks() const
{
    return active_.forks;
}

uint32_t chain_state::minimum_version() const
{
    return active_.minimum_version;
}

uint32_t chain_state::median_time_past() const
{
    return median_time_past_;
}

uint32_t chain_state::work_required() const
{
    return work_required_;
}

// Forks.
//-----------------------------------------------------------------------------

bool chain_state::is_enabled(rule_fork flag) const
{
    return script::is_enabled(active_.forks, flag);
}

bool chain_state::is_enabled(uint32_t block_version, rule_fork flag) const
{
    return (is_enabled(flag)) &&
       ((flag == rule_fork::bip65_rule && block_version >= bip65_version) ||
        (flag == rule_fork::bip66_rule && block_version >= bip66_version) ||
        (flag == rule_fork::bip34_rule && block_version >= bip34_version));
}

bool chain_state::is_checkpoint_failure(const hash_digest& hash) const
{
    using namespace bc::config;
    return !checkpoint::validate(hash, data_.height, checkpoints_);
}

bool chain_state::use_full_validation() const
{
    // This assumes that checkponts are sorted.
    return checkpoints_.empty() ||
        data_.height > checkpoints_.back().height();
}

} // namespace chain
} // namespace libbitcoin

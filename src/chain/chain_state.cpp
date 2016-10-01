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
#include <bitcoin/bitcoin/chain/header.hpp>
#include <bitcoin/bitcoin/chain/script/opcode.hpp>
#include <bitcoin/bitcoin/chain/script/script.hpp>
#include <bitcoin/bitcoin/config/checkpoint.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/math/hash_number.hpp>
#include <bitcoin/bitcoin/math/limits.hpp>

namespace libbitcoin {
namespace chain {

//*************************************************************************
// CONSENSUS: Potential overflows/underflows that would affect consensus
// are marked inline below. These should not be modified without verifying
// changes against other implementations. Other conditions that would not
// affect consensus are strictly guarded.
//*************************************************************************

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

inline size_t bip30_exception_height1(bool testnet)
{
    return testnet ? testnet_bip30_exception_height1 :
        mainnet_bip30_exception_height1;
}

inline size_t bip30_exception_height2(bool testnet)
{
    return testnet ? testnet_bip30_exception_height2 :
        mainnet_bip30_exception_height2;
}

inline size_t bip16_activation_height(bool testnet)
{
    return testnet ? testnet_bip16_activation_height :
        mainnet_bip16_activation_height;
}

// Get the timestamp of the high height block.
inline uint32_t timestamp_high(const chain_state::data& values)
{
    return values.timestamp.ordered.back();
}

// Get the bits of the high height block.
inline uint32_t bits_high(const chain_state::data& values)
{
    return values.bits.ordered.back();
}

// Get the bounded total time spanning the highest 2016 blocks.
inline uint32_t retarget_timespan(const chain_state::data& values)
{
    // Subtract 32 bit numbers in 64 bit space and constrain result to 32 bits.
    const uint64_t high = timestamp_high(values);
    const uint64_t retarget = values.timestamp.retarget;

    //*************************************************************************
    // CONSENSUS: subtraction underflow potential (retarget > high).
    //*************************************************************************
    const uint64_t timespan = high - retarget;
    return range_constrain(timespan, min_timespan, max_timespan);
}

// Determine if height is a multiple of retargeting_interval.
inline bool is_retarget_height(size_t height)
{
    return height % retargeting_interval == 0;
}

// Statics.
//-----------------------------------------------------------------------------

// protected
chain_state::activations chain_state::activation(const data& values)
{
    const auto height = values.height;
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

    // bip30 applies to all but two mainnet blocks that violate the rule.
    if (height != bip30_exception_height1(testnet) &&
        height != bip30_exception_height2(testnet))
        result.forks |= rule_fork::bip30_rule;

    // bip16 was activated with a one-time test on mainnet/testnet (~55% rule).
    if (height >= bip16_activation_height(testnet))
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

// protected
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

// private
uint32_t chain_state::work_required_retarget(const data& values)
{
    //*************************************************************************
    // CONSENSUS: set_compact can fail but this is unguarded.
    //*************************************************************************
    hash_number retarget;
    retarget.set_compact(bits_high(values));

    hash_number maximum;
    maximum.set_compact(max_work_bits);

    //*************************************************************************
    // CONSENSUS: multiplication overflow potential.
    //*************************************************************************
    retarget *= retarget_timespan(values);
    retarget /= target_timespan_seconds;

    return retarget > maximum ? maximum.compact() : retarget.compact();
}

// A retarget point, or a block that does not have max_bits (is not special).
inline bool is_retarget_or_nonmax(size_t height, uint32_t bits)
{
    // Zero is a retarget height, ensuring termination before height underflow.
    // This is guaranteed, just asserting here to document the safeguard.
    BITCOIN_ASSERT_MSG(is_retarget_height(0), "loop overflow potential");

    return bits != max_work_bits || is_retarget_height(height);
}

// private
uint32_t chain_state::work_required_testnet(const data& values)
{
    BITCOIN_ASSERT(values.height != 0);

    //*************************************************************************
    // CONSENSUS: addition overflow potential.
    //*************************************************************************
    const auto max_time_gap = timestamp_high(values) + double_spacing_seconds;

    if (values.timestamp.self > max_time_gap)
        return max_work_bits;

    auto height = values.height;
    auto& bits = values.bits.ordered;

    // Reverse iterate the ordered-by-height list of header bits.
    for (auto bit = bits.rbegin(); bit != bits.rend(); ++bit)
        if (is_retarget_or_nonmax(--height, *bit))
            return *bit;

    // Since the set of heights is either a full retarget range or ends at
    // zero this is not reachable unless the data set is invalid.
    BITCOIN_ASSERT(false);
    return max_work_bits;
}

// protected
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

// Publics.
//-----------------------------------------------------------------------------

// static
chain_state::map chain_state::get_map(size_t height, bool activated,
    bool testnet)
{
    // Invalid parameter in public interface, defaults indicate failure.
    if (height == 0)
        return{};

    // These all refer to the block preceding this block (i.e. top).
    static const size_t max_bits_past = 1;
    static const size_t max_time_past = 1;
    static const size_t max_version_past = 1;

    // Compute the set upper bound from the size of the set (i.e. size - 1).
    const size_t min_retarget = retargeting_interval - 1;
    const size_t min_time_past = median_time_past_interval - 1;
    const size_t min_version_past = version_sample_size(testnet) - 1;

    map map;

    // Bits.
    //-------------------------------------------------------------------------
    // The height range of the reverse (high to low) retarget search.
    // Mainnet doesn't do retarget search, so use high to make redundant.
    map.bits.high = height - max_bits_past;
    map.bits.low = testnet ? floor_subtract(map.bits.high, min_retarget) :
        map.bits.high;

    // Timestamp.
    //-------------------------------------------------------------------------
    // The height range of the median time past, and retarget/self blocks.
    // Height must be a positive multiple of interval, so underflow safe.
    map.timestamp.high = height - max_time_past;
    map.timestamp.low = floor_subtract(map.timestamp.high, min_time_past);
    map.timestamp_self = testnet ? height : map.timestamp.high;
    map.timestamp_retarget = is_retarget_height(height) ?
        height - retargeting_interval : map.timestamp.high;

    // Version.
    //-------------------------------------------------------------------------
    // The height range of the version sample.
    // If too small to activate set low to high to avoid unnecessary queries.
    map.version.high = height - max_version_past;
    map.version.low = activated ?
        floor_subtract(map.version.high, min_version_past) : map.version.high;
    map.version.low = is_active(map.version.high - map.version.low, testnet) ?
        map.version.low : map.version.high;

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
    return data_.height == 0
        || data_.bits.ordered.empty()
        || data_.timestamp.ordered.empty()
        || data_.version.unordered.empty();
}

// Properties.
//-----------------------------------------------------------------------------

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

bool chain_state::is_enabled(const header& header, rule_fork flag) const
{
    return (is_enabled(flag)) &&
       ((flag == rule_fork::bip65_rule && header.version() >= bip65_version) ||
        (flag == rule_fork::bip66_rule && header.version() >= bip66_version) ||
        (flag == rule_fork::bip34_rule && header.version() >= bip34_version));
}

bool chain_state::is_checkpoint_failure(const header& header) const
{
    using namespace bc::config;
    return !checkpoint::validate(header.hash(), data_.height, checkpoints_);
}

bool chain_state::use_full_validation() const
{
    // This assumes that checkponts are sorted.
    return checkpoints_.empty() ||
        data_.height > checkpoints_.back().height();
}

} // namespace chain
} // namespace libbitcoin

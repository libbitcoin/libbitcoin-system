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
#include <bitcoin/bitcoin/chain/chain_state.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <boost/multiprecision/integer.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <bitcoin/bitcoin/chain/block.hpp>
#include <bitcoin/bitcoin/chain/chain_state.hpp>
#include <bitcoin/bitcoin/chain/compact.hpp>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/config/checkpoint.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/math/limits.hpp>
#include <bitcoin/bitcoin/machine/opcode.hpp>
#include <bitcoin/bitcoin/machine/rule_fork.hpp>
#include <bitcoin/bitcoin/settings.hpp>
#include <bitcoin/bitcoin/unicode/unicode.hpp>
#include <bitcoin/bitcoin/utility/timer.hpp>

namespace libbitcoin {
namespace chain {

using namespace bc::config;
using namespace bc::machine;
using namespace boost::adaptors;

// Inlines.
//-----------------------------------------------------------------------------

inline bool is_active(size_t count, size_t activation_threshold)
{
    return count >= activation_threshold;
}

inline bool is_enforced(size_t count, size_t enforcement_threshold)
{
    return count >= enforcement_threshold;
}

inline bool is_bip30_exception(const checkpoint& check, bool mainnet)
{
    return mainnet &&
        ((check == mainnet_bip30_exception_checkpoint1) ||
         (check == mainnet_bip30_exception_checkpoint2));
}

inline uint32_t timestamp_high(const chain_state::data& values)
{
    return values.timestamp.ordered.back();
}

inline uint32_t bits_high(const chain_state::data& values)
{
    return values.bits.ordered.back();
}

// activation
//-----------------------------------------------------------------------------

chain_state::activations chain_state::activation(const data& values,
    uint32_t forks, const bc::settings& settings)
{
    const auto height = values.height;
    const auto version = values.version.self;
    const auto& history = values.version.ordered;
    const auto frozen = script::is_enabled(forks, rule_fork::bip90_rule);
    const auto difficult = script::is_enabled(forks, rule_fork::difficult);
    const auto retarget = script::is_enabled(forks, rule_fork::retarget);
    const auto mainnet = retarget && difficult;

    //*************************************************************************
    // CONSENSUS: Though unspecified in bip34, the satoshi implementation
    // performed this comparison using the signed integer version value.
    //*************************************************************************
    const auto ge = [](uint32_t value, uint32_t version)
    {
        return static_cast<int32_t>(value) >= static_cast<int32_t>(version);
    };

    // Declare bip34-based version predicates.
    const auto ge_2 = [=](uint32_t value) { return ge(value,
        settings.bip34_version); };
    const auto ge_3 = [=](uint32_t value) { return ge(value,
        settings.bip66_version); };
    const auto ge_4 = [=](uint32_t value) { return ge(value,
        settings.bip65_version); };

    // Compute bip34-based activation version summaries.
    const auto count_2 = std::count_if(history.begin(), history.end(), ge_2);
    const auto count_3 = std::count_if(history.begin(), history.end(), ge_3);
    const auto count_4 = std::count_if(history.begin(), history.end(), ge_4);

    // Frozen activations (require version and enforce above freeze height).
    const auto bip34_ice = frozen && height >= settings.bip34_freeze;
    const auto bip66_ice = frozen && height >= settings.bip66_freeze;
    const auto bip65_ice = frozen && height >= settings.bip65_freeze;

    // Initialize activation results with genesis values.
    activations result{ rule_fork::no_rules, settings.first_version };

    // retarget is only activated via configuration (hard fork).
    result.forks |= (rule_fork::retarget & forks);

    // testnet is activated based on configuration alone (hard fork).
    result.forks |= (rule_fork::difficult & forks);

    // bip90 is activated based on configuration alone (hard fork).
    result.forks |= (rule_fork::bip90_rule & forks);

    // time_warp_patch is activated based on configuration alone (hard fork).
    result.forks |= (rule_fork::time_warp_patch & forks);

    // retarget_overflow_patch is activated based on configuration alone (hard fork).
    result.forks |= (rule_fork::retarget_overflow_patch & forks);

    // scrypt_proof_of_work is activated based on configuration alone (hard fork).
    result.forks |= (rule_fork::scrypt_proof_of_work & forks);

    // bip16 was activated based on manual inspection of history (~55% rule).
    if (values.timestamp.self >= settings.bip16_activation_time)
    {
        result.forks |= (rule_fork::bip16_rule & forks);
    }

    // bip30 is active for all but two mainnet blocks that violate the rule.
    // These two blocks each have a coinbase transaction that exctly duplicates
    // another that is not spent by the arrival of the corresponding duplicate.
    if (!is_bip30_exception({ values.hash, height }, mainnet))
    {
        result.forks |= (rule_fork::bip30_rule & forks);
    }

    // bip34 is activated based on 75% of preceding 1000 mainnet blocks.
    if (bip34_ice || (is_active(count_2, settings.activation_threshold) && version >=
        settings.bip34_version))
    {
        result.forks |= (rule_fork::bip34_rule & forks);
    }

    // bip66 is activated based on 75% of preceding 1000 mainnet blocks.
    if (bip66_ice || (is_active(count_3, settings.activation_threshold) && version >=
        settings.bip66_version))
    {
        result.forks |= (rule_fork::bip66_rule & forks);
    }

    // bip65 is activated based on 75% of preceding 1000 mainnet blocks.
    if (bip65_ice || (is_active(count_4, settings.activation_threshold) && version >=
        settings.bip65_version))
    {
        result.forks |= (rule_fork::bip65_rule & forks);
    }

    // bip9_bit0 forks are enforced above the bip9_bit0 checkpoint.
    if (values.bip9_bit0_hash == settings.bip9_bit0_active_checkpoint.hash())
    {
        result.forks |= (rule_fork::bip9_bit0_group & forks);
    }

    // bip9_bit1 forks are enforced above the bip9_bit1 checkpoint.
    if (values.bip9_bit1_hash == settings.bip9_bit1_active_checkpoint.hash())
    {
        result.forks |= (rule_fork::bip9_bit1_group & forks);
    }

    // version 4/3/2 enforced based on 95% of preceding 1000 mainnet blocks.
    if (bip65_ice || is_enforced(count_4, settings.enforcement_threshold))
    {
        result.minimum_block_version = settings.bip65_version;
    }
    else if (bip66_ice || is_enforced(count_3, settings.enforcement_threshold))
    {
        result.minimum_block_version = settings.bip66_version;
    }
    else if (bip34_ice || is_enforced(count_2, settings.enforcement_threshold))
    {
        result.minimum_block_version = settings.bip34_version;
    }
    else
    {
        result.minimum_block_version = settings.first_version;
    }

    // TODO: add configurable option to apply transaction version policy.
    result.maximum_transaction_version = max_uint32;
    return result;
}

size_t chain_state::bits_count(size_t height, uint32_t forks,
    size_t retargeting_interval)
{
    // Mainnet doesn't use bits in retargeting.
    if (script::is_enabled(forks, rule_fork::difficult))
        return 1;

    // Regtest bypasses all retargeting.
    if (!script::is_enabled(forks, rule_fork::retarget))
        return 1;

    // Testnet uses mainnet retargeting on interval.
    if (is_retarget_height(height, retargeting_interval))
        return 1;

    // Testnet requires all bits for inter-interval retargeting.
    return std::min(height, retargeting_interval);
}

size_t chain_state::version_count(size_t height, uint32_t forks,
    size_t activation_sample)
{
    if (script::is_enabled(forks, rule_fork::bip90_rule) ||
        !script::is_enabled(forks, rule_fork::bip34_activations))
    {
        return 0;
    }

    return std::min(height, activation_sample);
}

size_t chain_state::timestamp_count(size_t height, uint32_t)
{
    return std::min(height, median_time_past_interval);
}

size_t chain_state::retarget_height(size_t height, uint32_t forks,
    size_t retargeting_interval)
{
    if (!script::is_enabled(forks, rule_fork::retarget))
        return map::unrequested;

    // Height must be a positive multiple of interval, so underflow safe.
    // If not retarget height get most recent so that it may be promoted.
    return height - (is_retarget_height(height, retargeting_interval) ?
        retargeting_interval : retarget_distance(height, retargeting_interval));
}

size_t chain_state::bip9_bit0_height(size_t height,
    const config::checkpoint& bip9_bit0_active_checkpoint)
{
    const auto activation_height = bip9_bit0_active_checkpoint.height();
    // Require bip9_bit0 hash at heights above historical bip9_bit0 activation.
    return height > activation_height ? activation_height : map::unrequested;
}

size_t chain_state::bip9_bit1_height(size_t height,
    const config::checkpoint& bip9_bit1_active_checkpoint)
{
    const auto activation_height = bip9_bit1_active_checkpoint.height();

    // Require bip9_bit1 hash at heights above historical bip9_bit1 activation.
    return height > activation_height ? activation_height : map::unrequested;
}

// median_time_past
//-----------------------------------------------------------------------------

uint32_t chain_state::median_time_past(const data& values, uint32_t)
{
    // Create a copy for the in-place sort.
    auto times = values.timestamp.ordered;

    // Sort the times by value to obtain the median.
    std::sort(times.begin(), times.end());

    // Consensus defines median time using modulo 2 element selection.
    // This differs from arithmetic median which averages two middle values.
    return times.empty() ? 0 : times[times.size() / 2];
}

// work_required
//-----------------------------------------------------------------------------

uint32_t chain_state::work_required(const data& values, uint32_t forks,
    const bc::settings& settings)
{
    // Invalid parameter via public interface, test is_valid for results.
    if (values.height == 0)
        return 0;

    // Regtest bypasses all retargeting.
    if (!script::is_enabled(forks, rule_fork::retarget))
        return bits_high(values);

    // Mainnet and testnet retarget on interval.
    if (is_retarget_height(values.height, settings.retargeting_interval()))
        return work_required_retarget(values, forks,
            settings.proof_of_work_limit, settings.minimum_timespan(),
            settings.maximum_timespan(),
            settings.retargeting_interval_seconds());

    // Testnet retargets easy on inter-interval.
    if (!script::is_enabled(forks, rule_fork::difficult))
        return easy_work_required(values, settings.retargeting_interval(),
            settings.proof_of_work_limit, settings.block_spacing_seconds());

    // Mainnet not retargeting.
    return bits_high(values);
}

uint32_t chain_state::work_required_retarget(const data& values, uint32_t forks,
    uint32_t proof_of_work_limit, uint32_t minimum_timespan,
    uint32_t maximum_timespan, uint32_t retargeting_interval_seconds)
{
    static const uint256_t pow_limit(compact{ proof_of_work_limit });

    const compact bits(bits_high(values));
    BITCOIN_ASSERT_MSG(!bits.is_overflowed(), "previous block has bad bits");

    uint256_t target(bits);
    const auto retarget_overflow = script::is_enabled(forks,
        rule_fork::retarget_overflow_patch);
    using namespace boost::multiprecision;
    const auto shift = retarget_overflow && (msb(target) + 1 > msb(pow_limit)) ?
        1u : 0u;
    target >>= shift;
    target *= retarget_timespan(values, minimum_timespan, maximum_timespan);
    target /= retargeting_interval_seconds;
    target <<= shift;

    // The proof_of_work_limit constant is pre-normalized.
    return target > pow_limit ? proof_of_work_limit :
        compact(target).normal();
}

// Get the bounded total time spanning the highest 2016 blocks.
uint32_t chain_state::retarget_timespan(const data& values,
    uint32_t minimum_timespan, uint32_t maximum_timespan)
{
    const auto high = timestamp_high(values);
    const auto retarget = values.timestamp.retarget;

    //*************************************************************************
    // CONSENSUS: subtract unsigned 32 bit numbers in signed 64 bit space in
    // order to prevent underflow before applying the range constraint.
    //*************************************************************************
    const auto timespan = cast_subtract<int64_t>(high, retarget);
    return range_constrain(timespan, minimum_timespan, maximum_timespan);
}

uint32_t chain_state::easy_work_required(const data& values,
    size_t retargeting_interval, uint32_t proof_of_work_limit,
    uint32_t block_spacing_seconds)
{
    BITCOIN_ASSERT(values.height != 0);

    // Overflow allowed here since supported coins would not do so.
    const auto easy_spacing_seconds = block_spacing_seconds << 1;

    // If the time limit has passed allow a minimum difficulty block.
    if (values.timestamp.self > easy_time_limit(values, easy_spacing_seconds))
        return proof_of_work_limit;

    auto height = values.height;
    const auto& bits = values.bits.ordered;

    // Reverse iterate the ordered-by-height list of header bits.
    for (auto bit: reverse(bits))
        if (is_retarget_or_non_limit(--height, bit, retargeting_interval,
            proof_of_work_limit))
            return bit;

    // Since the set of heights is either a full retarget range or ends at
    // zero this is not reachable unless the data set is invalid.
    BITCOIN_ASSERT(false);
    return proof_of_work_limit;
}

uint32_t chain_state::easy_time_limit(const chain_state::data& values,
    int64_t spacing)
{
    const int64_t high = timestamp_high(values);

    //*************************************************************************
    // CONSENSUS: add unsigned 32 bit numbers in signed 64 bit space in
    // order to prevent overflow before applying the domain constraint.
    //*************************************************************************
    return domain_constrain<uint32_t>(cast_add<int64_t>(high, spacing));
}

// A retarget height, or a block that does not have proof_of_work_limit bits.
bool chain_state::is_retarget_or_non_limit(size_t height, uint32_t bits,
    size_t retargeting_interval, uint32_t proof_of_work_limit)
{
    // Zero is a retarget height, ensuring termination before height underflow.
    // This is guaranteed, just asserting here to document the safeguard.
    BITCOIN_ASSERT_MSG(is_retarget_height(0, retargeting_interval),
        "loop overflow potential");

    return bits != proof_of_work_limit ||
        is_retarget_height(height, retargeting_interval);
}

// Determine if height is a multiple of retargeting_interval.
bool chain_state::is_retarget_height(size_t height, size_t retargeting_interval)
{
    return retarget_distance(height, retargeting_interval) == 0;
}

// Determine the number of blocks back to the closest retarget height.
size_t chain_state::retarget_distance(size_t height,
    size_t retargeting_interval)
{
    return height % retargeting_interval;
}

// Publics.
//-----------------------------------------------------------------------------

// static
chain_state::map chain_state::get_map(size_t height,
    const checkpoints& /* checkpoints */, uint32_t forks,
    size_t retargeting_interval, size_t activation_sample,
    const config::checkpoint& bip9_bit0_active_checkpoint,
    const config::checkpoint& bip9_bit1_active_checkpoint)
{
    if (height == 0)
        return {};

    map map;

    // The height bound of the reverse (high to low) retarget search.
    map.bits_self = height;
    map.bits.high = height - 1;
    map.bits.count = bits_count(height, forks, retargeting_interval);

    // The height bound of the median time past function.
    map.timestamp_self = height;
    map.timestamp.high = height - 1;
    map.timestamp.count = timestamp_count(height, forks);

    // The height bound of the version sample for activations.
    map.version_self = height;
    map.version.high = height - 1;
    map.version.count = version_count(height, forks, activation_sample);

    // The most recent past retarget height.
    map.timestamp_retarget = retarget_height(height, forks,
        retargeting_interval);

    // The checkpoint above which bip9_bit0 rules are enforced.
    map.bip9_bit0_height = bip9_bit0_height(height,
        bip9_bit0_active_checkpoint);

    // The checkpoint above which bip9_bit1 rules are enforced.
    map.bip9_bit1_height = bip9_bit1_height(height,
        bip9_bit1_active_checkpoint);

    return map;
}

// static
uint32_t chain_state::signal_version(uint32_t forks,
    const bc::settings& settings)
{
    if (script::is_enabled(forks, rule_fork::bip65_rule))
        return settings.bip65_version;

    if (script::is_enabled(forks, rule_fork::bip66_rule))
        return settings.bip66_version;

    if (script::is_enabled(forks, rule_fork::bip34_rule))
        return settings.bip34_version;

    // TODO: these can be retired.
    // Signal bip9 bit0 if any of the group is configured.
    if (script::is_enabled(forks, rule_fork::bip9_bit0_group))
        return settings.bip9_version_base | settings.bip9_version_bit0;

    // TODO: these can be retired.
    // Signal bip9 bit1 if any of the group is configured.
    if (script::is_enabled(forks, rule_fork::bip9_bit1_group))
        return settings.bip9_version_base | settings.bip9_version_bit1;

    return settings.first_version;
}

// static
uint32_t chain_state::minimum_timespan(uint32_t retargeting_interval_seconds,
    uint32_t retargeting_factor)
{
    return retargeting_interval_seconds / retargeting_factor;
}

// static
uint32_t chain_state::maximum_timespan(uint32_t retargeting_interval_seconds,
    uint32_t retargeting_factor)
{
    return retargeting_interval_seconds * retargeting_factor;
}

// static
uint32_t chain_state::retargeting_interval(
    uint32_t retargeting_interval_seconds, uint32_t block_spacing_seconds)
{
    return retargeting_interval_seconds / block_spacing_seconds;
}

// This is promotion from a preceding height to the next.
chain_state::data chain_state::to_pool(const chain_state& top,
    const bc::settings& settings)
{
    // Alias configured forks.
    const auto forks = top.forks_;

    // Retargeting is only activated via configuration.
    const auto retarget = script::is_enabled(forks, rule_fork::retarget);

    // Copy data from presumed previous-height block state.
    auto data = top.data_;

    // If this overflows height is zero and result is handled as invalid.
    const auto height = data.height + 1u;

    // Enqueue previous block values to collections.
    data.bits.ordered.push_back(data.bits.self);
    data.version.ordered.push_back(data.version.self);
    data.timestamp.ordered.push_back(data.timestamp.self);

    // If bits collection overflows, dequeue oldest member.
    if (data.bits.ordered.size() >
        bits_count(height, forks, settings.retargeting_interval()))
        data.bits.ordered.pop_front();

    // If version collection overflows, dequeue oldest member.
    if (data.version.ordered.size() > version_count(height, forks,
            settings.activation_sample))
        data.version.ordered.pop_front();

    // If timestamp collection overflows, dequeue oldest member.
    if (data.timestamp.ordered.size() > timestamp_count(height, forks))
        data.timestamp.ordered.pop_front();

    // Regtest does not perform retargeting.
    // If promoting from retarget height, move that timestamp into retarget.
    if (retarget &&
        is_retarget_height(height - 1u, settings.retargeting_interval()))
        data.timestamp.retarget = (script::is_enabled(forks,
            rule_fork::time_warp_patch) && height != 1) ?
            *std::next(data.timestamp.ordered.crbegin()) : data.timestamp.self;

    // Replace previous block state with tx pool chain state for next height
    // Preserve top block timestamp for use in computation of staleness.
    // Preserve data.bip9_bit0_hash promotion.
    // Preserve data.bip9_bit1_hash promotion.
    // Hash and bits.self are unused.
    data.height = height;
    data.hash = null_hash;
    data.bits.self = 0;
    data.version.self = signal_version(forks, settings);
    return data;
}

// Constructor (top to pool).
// This generates a state for the pool above the presumed top block state.
chain_state::chain_state(const chain_state& top, const bc::settings& settings)
  : data_(to_pool(top, settings)),
    forks_(top.forks_),
    stale_seconds_(top.stale_seconds_),
    checkpoints_(top.checkpoints_),
    active_(activation(data_, forks_, settings)),
    work_required_(work_required(data_, forks_, settings)),
    median_time_past_(median_time_past(data_, forks_))
{
}

chain_state::data chain_state::to_block(const chain_state& pool,
    const block& block, const config::checkpoint& bip9_bit0_active_checkpoint,
    const config::checkpoint& bip9_bit1_active_checkpoint)
{
    // Copy data from presumed same-height pool state.
    auto data = pool.data_;

    // Replace pool chain state with block state at same (next) height.
    // Preserve data.timestamp.retarget promotion.
    const auto& header = block.header();
    data.hash = header.hash();
    data.bits.self = header.bits();
    data.version.self = header.version();
    data.timestamp.self = header.timestamp();

    // Cache hash of bip9 bit0 height block, otherwise use preceding state.
    if (data.height == bip9_bit0_active_checkpoint.height())
        data.bip9_bit0_hash = data.hash;

    // Cache hash of bip9 bit1 height block, otherwise use preceding state.
    if (data.height == bip9_bit1_active_checkpoint.height())
        data.bip9_bit1_hash = data.hash;

    return data;
}

// Constructor (tx pool to block).
// This assumes that the pool state is the same height as the block.
chain_state::chain_state(const chain_state& pool, const block& block,
    const bc::settings& settings)
  : data_(to_block(pool, block, settings.bip9_bit0_active_checkpoint,
        settings.bip9_bit1_active_checkpoint)),
    forks_(pool.forks_),
    stale_seconds_(pool.stale_seconds_),
    checkpoints_(pool.checkpoints_),
    active_(activation(data_, forks_, settings)),
    work_required_(work_required(data_, forks_, settings)),
    median_time_past_(median_time_past(data_, forks_))
{
}

chain_state::data chain_state::to_header(const chain_state& parent,
    const header& header, const bc::settings& settings)
{
    BITCOIN_ASSERT(header.previous_block_hash() == parent.hash());

    // Copy and promote data from presumed parent-height header/block state.
    auto data = to_pool(parent, settings);

    // Replace the pool (empty) current block state with given header state.
    // Preserve data.timestamp.retarget promotion.
    data.hash = header.hash();
    data.bits.self = header.bits();
    data.version.self = header.version();
    data.timestamp.self = header.timestamp();

    // Cache hash of bip9 bit0 height block, otherwise use preceding state.
    if (data.height == settings.bip9_bit0_active_checkpoint.height())
        data.bip9_bit0_hash = data.hash;

    // Cache hash of bip9 bit1 height block, otherwise use preceding state.
    if (data.height == settings.bip9_bit1_active_checkpoint.height())
        data.bip9_bit1_hash = data.hash;

    return data;
}

// Constructor (parent to header).
// This assumes that parent is the state of the header's previous block.
chain_state::chain_state(const chain_state& parent, const header& header,
    const bc::settings& settings)
  : data_(to_header(parent, header, settings)),
    forks_(parent.forks_),
    stale_seconds_(parent.stale_seconds_),
    checkpoints_(parent.checkpoints_),
    active_(activation(data_, forks_, settings)),
    work_required_(work_required(data_, forks_, settings)),
    median_time_past_(median_time_past(data_, forks_))
{
}

// Constructor (from raw data).
chain_state::chain_state(data&& values, const checkpoints& checkpoints,
    uint32_t forks, uint32_t stale_seconds, const bc::settings& settings)
  : data_(std::move(values)),
    forks_(forks),
    stale_seconds_(stale_seconds),
    checkpoints_(checkpoints),
    active_(activation(data_, forks_, settings)),
    work_required_(work_required(data_, forks_, settings)),
    median_time_past_(median_time_past(data_, forks_))
{
}

// Semantic invalidity can also arise from too many/few values in the arrays.
// The same computations used to specify the ranges could detect such errors.
// These are the conditions that would cause exception during execution.
bool chain_state::is_valid() const
{
    return data_.height != 0;
}

// If there is a zero limit then the chain is never considered stale.
bool chain_state::is_stale() const
{
    return stale_seconds_ != 0 && data_.timestamp.self <
        floor_subtract(static_cast<uint32_t>(zulu_time()), stale_seconds_);
}

// Properties.
//-----------------------------------------------------------------------------

const hash_digest& chain_state::hash() const
{
    return data_.hash;
}

size_t chain_state::height() const
{
    return data_.height;
}

uint32_t chain_state::enabled_forks() const
{
    return active_.forks;
}

uint32_t chain_state::minimum_block_version() const
{
    return active_.minimum_block_version;
}

uint32_t chain_state::maximum_transaction_version() const
{
    return active_.maximum_transaction_version;
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

bool chain_state::is_enabled(rule_fork fork) const
{
    return script::is_enabled(active_.forks, fork);
}

bool chain_state::is_checkpoint_conflict(const hash_digest& hash) const
{
    return !checkpoint::validate(hash, data_.height, checkpoints_);
}

bool chain_state::is_under_checkpoint() const
{
    // This assumes that the checkpoints are sorted.
    return checkpoint::covered(data_.height, checkpoints_);
}

} // namespace chain
} // namespace libbitcoin

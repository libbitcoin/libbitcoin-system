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
#include <bitcoin/system/chain/chain_state.hpp>

#include <chrono>
#include <ranges>
#include <bitcoin/system/chain/block.hpp>
#include <bitcoin/system/chain/chain_state.hpp>
#include <bitcoin/system/chain/checkpoint.hpp>
#include <bitcoin/system/chain/compact.hpp>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/enums/flags.hpp>
#include <bitcoin/system/chain/enums/policy.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/forks.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/settings.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// github.com/bitcoin/bips/blob/master/bip-0030.mediawiki#specification
// As bip30 exceptions apply only to bitcoin mainnet these can be embedded.
static const checkpoint mainnet_bip30_exception_checkpoint1
{
    "00000000000a4d0a398161ffc163c503763b1f4360639393e0e4c8e300e0caec", 91842
};
static const checkpoint mainnet_bip30_exception_checkpoint2
{
    "00000000000743f190a18c5577a3c2d2a1f610ae9601ac046a38084ccb7cd721", 91880
};

// Inlines.
// ----------------------------------------------------------------------------

constexpr bool is_active(size_t count, size_t bip34_activation_threshold) NOEXCEPT
{
    return count >= bip34_activation_threshold;
}

constexpr bool is_enforced(size_t count, size_t bip34_enforcement_threshold) NOEXCEPT
{
    return count >= bip34_enforcement_threshold;
}

// Determine the number of blocks back to the closest retarget height.
constexpr size_t retarget_distance(size_t height,
    size_t retargeting_interval) NOEXCEPT
{
    return height % retargeting_interval;
}

// Determine if height is a multiple of retargeting_interval.
constexpr bool is_retarget_height(size_t height,
    size_t retargeting_interval) NOEXCEPT
{
    return is_zero(retarget_distance(height, retargeting_interval));
}

// bip30 is active for all but two mainnet blocks that violate the rule.
// These two blocks each have a coinbase transaction that exactly duplicates
// another that is not spent by the arrival of the corresponding duplicate.
// Exceptions: block 91842 (duplicates 91812), 91880 (duplicates 91722).
inline bool is_bip30_exception(const hash_digest& hash,
    size_t height) NOEXCEPT
{
    return (hash   == mainnet_bip30_exception_checkpoint1.hash() &&
            height == mainnet_bip30_exception_checkpoint1.height()) ||
           (hash   == mainnet_bip30_exception_checkpoint2.hash() &&
            height == mainnet_bip30_exception_checkpoint2.height());
}

// The top three version bits are reserved for the bip9 version base.
constexpr uint32_t bip9_version_mask = 0xe0000000;

// The period boundary at or below the height.
inline size_t period_floor(size_t height, size_t period) NOEXCEPT
{
    return floored_divide(height, period) * period;
}

typedef std::array<chain_state::deployment,
    chain_state::deployment_count> deployments_t;

inline deployments_t deployments(const system::settings& settings) NOEXCEPT
{
    return
    {
        settings.bip9_bit0_deployment,
        settings.bip9_bit1_deployment,
        settings.bip9_bit2_deployment,
        settings.bip54_timewarp_deployment
    };
}

inline bool is_configured(const chain_state::deployment& deployment,
    uint32_t configured) NOEXCEPT
{
    return !is_zero(bit_and(deployment.flags, configured));
}

inline bool is_signaled(const chain_state::deployment& deployment,
    const system::settings& settings, uint32_t configured) NOEXCEPT
{
    const auto always = chain_state::deployment::always_active;

    return is_configured(deployment, configured)
        && deployment.start != always
        && !is_zero(deployment.threshold)
        && !is_zero(settings.bip9_activation_period)
        && !(settings.forks.bip90 && deployment.buried);
}

inline uint32_t timestamp_high(const chain_state::data& values) NOEXCEPT
{
    return values.timestamp.ordered.back();
}

inline uint32_t bits_high(const chain_state::data& values) NOEXCEPT
{
    return values.bits.ordered.back();
}

// activation
// ----------------------------------------------------------------------------

uint32_t chain_state::configured_flags(const forks& forks) NOEXCEPT
{
    uint32_t result{ flags::no_rules };
    if (forks.retarget)
        result |= flags::retarget;
    if (forks.difficult)
        result |= flags::difficult;
    if (forks.ltc_time_warp_patch)
        result |= flags::ltc_time_warp_patch;
    if (forks.ltc_retarget_overflow_patch)
        result |= flags::ltc_retarget_overflow_patch;
    if (forks.ltc_scrypt_proof_of_work)
        result |= flags::ltc_scrypt_proof_of_work;
    if (forks.time_warp_patch)
        result |= flags::time_warp_patch;
    if (forks.block_storm_patch)
        result |= flags::block_storm_patch;
    if (forks.signet)
        result |= flags::signet_rule;
    if (forks.bip16)
        result |= flags::bip16_rule;
    if (forks.bip30)
        result |= flags::bip30_rule;
    if (forks.bip34)
        result |= flags::bip34_rule;
    if (forks.bip42)
        result |= flags::bip42_rule;
    if (forks.bip65)
        result |= flags::bip65_rule;
    if (forks.bip66)
        result |= flags::bip66_rule;
    if (forks.bip68)
        result |= flags::bip68_rule;
    if (forks.bip90)
        result |= flags::bip90_rule;
    if (forks.bip112)
        result |= flags::bip112_rule;
    if (forks.bip113)
        result |= flags::bip113_rule;
    if (forks.bip141)
        result |= flags::bip141_rule;
    if (forks.bip143)
        result |= flags::bip143_rule;
    if (forks.bip147)
        result |= flags::bip147_rule;
    if (forks.bip341)
        result |= flags::bip341_rule;
    if (forks.bip342)
        result |= flags::bip342_rule;
    if (forks.bip54_timewarp)
        result |= flags::bip54_timewarp_rule;

    return result;
}

chain_state::activations chain_state::activation(const data& values,
    const forks& forks, const system::settings& settings) NOEXCEPT
{
    // There are no constraints on block version before bip34.
    activations result{ flags::no_rules, 0 };

    // regtest is only activated via configuration.
    if (forks.retarget)
    {
        result.flags |= flags::retarget;
    }

    // testnet is activated based on configuration alone.
    if (forks.difficult)
    {
        result.flags |= flags::difficult;
    }

    // time_warp_patch is activated based on configuration alone.
    if (forks.ltc_time_warp_patch)
    {
        result.flags |= flags::ltc_time_warp_patch;
    }

    // retarget_overflow_patch is activated based on configuration alone.
    if (forks.ltc_retarget_overflow_patch)
    {
        result.flags |= flags::ltc_retarget_overflow_patch;
    }

    // scrypt_proof_of_work is activated based on configuration alone.
    if (forks.ltc_scrypt_proof_of_work)
    {
        result.flags |= flags::ltc_scrypt_proof_of_work;
    }

    // bip42 is activated based on configuration alone (soft fork).
    if (forks.bip42)
    {
        result.flags |= flags::bip42_rule;
    }

    // bip90 is activated based on configuration alone (hard fork).
    if (forks.bip90)
    {
        result.flags |= flags::bip90_rule;
    }

    // time_warp_patch (testnet4) is activated based on configuration
    // alone (hard fork).
    if (forks.time_warp_patch)
    {
        result.flags |= flags::time_warp_patch;
    }

    // block_storm_patch (testnet4) is activated based on configuration
    // alone (hard fork).
    if (forks.block_storm_patch)
    {
        result.flags |= flags::block_storm_patch;
    }

    // signet is activated based on configuration above genesis (soft fork).
    if (forks.signet && !is_zero(values.height))
    {
        result.flags |= flags::signet_rule;
    }

    // bip16 was activated by manual inspection of signal history (soft fork).
    if (forks.bip16 &&
        (values.timestamp.self >= settings.bip16_activation_time))
    {
        result.flags |= flags::bip16_rule;
    }

    const auto height = values.height;
    const auto version = values.version.self;
    const auto& history = values.version.ordered;

    //*************************************************************************
    // CONSENSUS: Though unspecified in bip34, the satoshi implementation
    // performed this comparison using the signed integer version value.
    //*************************************************************************
    constexpr auto ge = [](uint32_t value, uint32_t version) NOEXCEPT
    {
        return sign_cast<int32_t>(value) >= sign_cast<int32_t>(version);
    };

    // Declare bip34-based version predicates.
    const auto ge_2 = [&](uint32_t value) NOEXCEPT
        { return ge(value, settings.bip34_version); };
    const auto ge_3 = [&](uint32_t value) NOEXCEPT
        { return ge(value, settings.bip66_version); };
    const auto ge_4 = [&](uint32_t value) NOEXCEPT
        { return ge(value, settings.bip65_version); };

    // Compute bip34-based activation version summaries (empty if disabled).
    const auto count_2 = std::count_if(history.cbegin(), history.cend(), ge_2);
    const auto count_3 = std::count_if(history.cbegin(), history.cend(), ge_3);
    const auto count_4 = std::count_if(history.cbegin(), history.cend(), ge_4);

    // Frozen activations (require version and enforce above freeze height).
    const auto bip90_bip34 = forks.bip90 && height >= settings.bip90_bip34_height;
    const auto bip90_bip65 = forks.bip90 && height >= settings.bip90_bip65_height;
    const auto bip90_bip66 = forks.bip90 && height >= settings.bip90_bip66_height;

    // bip34 activations oscillate until enforced by minimum_block_version.
    // bip90 does not require that the corresponding rules be defined.
    // bip34 is active based on 75% of preceding 1000 mainnet blocks.
    if (bip90_bip34 ||
        (is_active(count_2, settings.bip34_activation_threshold) &&
        ge(version, settings.bip34_version)))
    {
        result.flags |= flags::bip34_rule;
    }

    // bip66 is active based on 75% of preceding 1000 mainnet blocks.
    if (bip90_bip66 ||
        (is_active(count_3, settings.bip34_activation_threshold) &&
        ge(version, settings.bip66_version)))
    {
        result.flags |= flags::bip66_rule;
    }

    // bip65 is active based on 75% of preceding 1000 mainnet blocks.
    if (bip90_bip65 ||
        (is_active(count_4, settings.bip34_activation_threshold) &&
        ge(version, settings.bip65_version)))
    {
        result.flags |= flags::bip65_rule;
    }

    // version 4/3/2 enforced based on 95% of preceding 1000 mainnet blocks.
    if (bip90_bip65 ||
        is_enforced(count_4, settings.bip34_enforcement_threshold))
    {
        result.minimum_block_version = settings.bip65_version;
    }
    else if (bip90_bip66 ||
        is_enforced(count_3, settings.bip34_enforcement_threshold))
    {
        result.minimum_block_version = settings.bip66_version;
    }
    else if (bip90_bip34 ||
        is_enforced(count_2, settings.bip34_enforcement_threshold))
    {
        result.minimum_block_version = settings.bip34_version;
    }

    const auto configured = configured_flags(forks);

    // Unsignaled bip9_bit0 forks are enforced above the bip9_bit0 checkpoint.
    if (!is_signaled(settings.bip9_bit0_deployment, settings, configured) &&
        values.bip9_bit0_hash == settings.bip9_bit0_active_checkpoint.hash())
    {
        result.flags |= flags::bip68_rule;
        result.flags |= flags::bip112_rule;
        result.flags |= flags::bip113_rule;
    }

    // Unsignaled bip9_bit1 forks are enforced above the bip9_bit1 checkpoint.
    if (!is_signaled(settings.bip9_bit1_deployment, settings, configured) &&
        values.bip9_bit1_hash == settings.bip9_bit1_active_checkpoint.hash())
    {
        result.flags |= flags::bip141_rule;
        result.flags |= flags::bip143_rule;
        result.flags |= flags::bip147_rule;
    }

    // Unsignaled bip9_bit2 forks are enforced above the bip9_bit2 checkpoint.
    if (!is_signaled(settings.bip9_bit2_deployment, settings, configured) &&
        values.bip9_bit2_hash == settings.bip9_bit2_active_checkpoint.hash())
    {
        result.flags |= flags::bip341_rule;
        result.flags |= flags::bip342_rule;
    }

    // Signaled deployments are enforced once active, others if always active.
    const auto list = deployments(settings);
    for (size_t index{}; index < list.size(); ++index)
    {
        const auto& deployment = list.at(index);
        if (!is_configured(deployment, configured))
            continue;

        if (is_signaled(deployment, settings, configured) ?
            values.signals.at(index).state == deployment_state::active :
            deployment.start == deployment::always_active)
            result.flags |= deployment.flags;
    }

    // If not exception, existing duplicate coinbase must be spent.
    if (forks.bip30 && !is_bip30_exception(values.hash, height))
    {
        result.flags |= flags::bip30_rule;
    }

    return result;
}

// github.com/bitcoin/bips/blob/master/bip-0009.mediawiki#state-transitions
// github.com/bitcoin/bips/blob/master/bip-0341.mediawiki#deployment
chain_state::deployment_state chain_state::next_state(
    const deployment& deployment, deployment_state state,
    uint32_t median_time_past, size_t count, size_t boundary) NOEXCEPT
{
    const auto started = median_time_past >= deployment.start;
    const auto timed_out = median_time_past >= deployment.timeout;
    const auto locked_in = count >= deployment.threshold;

    switch (state)
    {
        // bip9 fails a deployment that times out before it starts.
        case deployment_state::defined:
        {
            if (!deployment.speedy && timed_out)
                return deployment_state::failed;

            return started ? deployment_state::started : state;
        }

        // bip9 checks timeout before threshold, speedy trial after.
        case deployment_state::started:
        {
            if (!deployment.speedy && timed_out)
                return deployment_state::failed;

            if (locked_in)
                return deployment_state::locked_in;

            return timed_out ? deployment_state::failed : state;
        }

        case deployment_state::locked_in:
        {
            return boundary >= deployment.activation_height ?
                deployment_state::active : state;
        }

        case deployment_state::active:
        case deployment_state::failed:
        default:
        {
            return state;
        }
    }
}

size_t chain_state::bits_count(size_t height, const forks& forks,
    size_t retargeting_interval) NOEXCEPT
{
    // Mainnet doesn't use bits in retargeting.
    if (forks.difficult)
        return one;

    // Regtest bypasses all retargeting.
    if (!forks.retarget)
        return one;

    // Testnet uses mainnet retargeting on interval, unless storm patched.
    if (!forks.block_storm_patch &&
        is_retarget_height(height, retargeting_interval))
        return one;

    // Testnet requires all bits for inter-interval retargeting.
    // Block storm patch requires all bits for interval retargeting [bip94].
    return std::min(height, retargeting_interval);
}

size_t chain_state::version_count(size_t height, const forks& forks,
    size_t bip34_activation_sample) NOEXCEPT
{
    if (forks.bip90 || (!forks.bip34 && !forks.bip65 && !forks.bip66))
        return zero;

    return std::min(height, bip34_activation_sample);
}

size_t chain_state::timestamp_count(size_t height, const forks&) NOEXCEPT
{
    return std::min(height, median_time_past_interval);
}

size_t chain_state::retarget_height(size_t height, const forks& forks,
    size_t retargeting_interval) NOEXCEPT
{
    if (!forks.retarget)
        return map::unrequested;

    // Height must be a positive multiple of interval, so underflow safe.
    // If not retarget height get most recent so that it may be promoted.
    return height - (is_retarget_height(height, retargeting_interval) ?
        retargeting_interval : retarget_distance(height, retargeting_interval));
}

// median_time_past
// ----------------------------------------------------------------------------

//*****************************************************************************
// CONSENSUS: satoshi associates the median time past for block N with block
// N-1, as opposed to block N. This is unnecessarily confusing. We associate
// the median time past for block N with block N. This is intuitive but
// requires care when comparing code.
//*****************************************************************************
uint32_t chain_state::median_time_past(const data& values,
    const forks&) NOEXCEPT
{
    return median(values.timestamp.ordered);
}

// work_required
// ----------------------------------------------------------------------------

uint32_t chain_state::work_required(const data& values, const forks& forks,
    const system::settings& settings) NOEXCEPT
{
    // Genesis has no preceding block data.
    if (is_zero(values.height))
        return 0;

    // Previous block has an invalid bits value.
    if (is_zero(compact::expand(bits_high(values))))
        return 0;

    // Regtest bypasses all retargeting.
    if (!forks.retarget)
        return bits_high(values);

    // Mainnet and testnet retarget on interval.
    if (is_retarget_height(values.height, settings.retargeting_interval()))
        return work_required_retarget(values, forks,
            settings.proof_of_work_limit,
            settings.minimum_timespan(),
            settings.maximum_timespan(),
            settings.retargeting_interval(),
            settings.retargeting_interval_seconds);

    // Testnet retargets easy on inter-interval.
    if (!forks.difficult)
        return easy_work_required(values,
            settings.retargeting_interval(),
            settings.proof_of_work_limit,
            settings.block_spacing_seconds);

    // Mainnet not retargeting, must exact match the previous block bits value.
    return bits_high(values);
}

// Get the bounded total time spanning the highest 2016 blocks.
uint32_t chain_state::retarget_timespan(const data& values,
    uint32_t minimum_timespan, uint32_t maximum_timespan) NOEXCEPT
{
    //*************************************************************************
    // CONSENSUS: "Subtract unsigned 32 bit numbers in signed 64 bit space".
    // This is done order to prevent underflow before applying the range
    // constraint. This is properly just a floored subtraction in 32 bit space.
    //*************************************************************************
    const auto timespan = floored_subtract(timestamp_high(values),
        values.timestamp.retarget);

    //*************************************************************************
    // CONSENSUS: Constrain the timespan to the configured consensus limits.
    //*************************************************************************
    return limit(timespan, minimum_timespan, maximum_timespan);
}

constexpr bool patch_timewarp(const forks& forks, const uint256_t& limit,
    const uint256_t& target) NOEXCEPT
{
    return forks.ltc_retarget_overflow_patch &&
        floored_log2(target) >= floored_log2(limit);
}

// A retarget height, or a block that does not have proof_of_work_limit bits.
constexpr bool is_retarget_or_non_limit(size_t height, uint32_t bits,
    uint32_t retargeting_interval,
    uint32_t proof_of_work_limit) NOEXCEPT
{
    // Zero is a retarget height, termination required before height underflow.
    // This is guaranteed, just a comment here because it may not be obvious.
    return bits != proof_of_work_limit ||
        is_retarget_height(height, retargeting_interval);
}

uint32_t chain_state::work_required_retarget_bits(const data& values,
    const forks& forks, uint32_t proof_of_work_limit,
    uint32_t retargeting_interval) NOEXCEPT
{
    if (forks.block_storm_patch)
    {
        auto height = values.height;

        // Reverse iterate the ordered-by-height list of header bits.
        const auto& bits = values.bits.ordered;
        for (auto bit: std::views::reverse(bits))
        {
            if (is_retarget_or_non_limit(--height, bit, retargeting_interval,
                proof_of_work_limit))
                return bit;
        }
    }

    return bits_high(values);
}

uint32_t chain_state::work_required_retarget(const data& values,
    const forks& forks,
    uint32_t proof_of_work_limit,
    uint32_t minimum_timespan,
    uint32_t maximum_timespan,
    uint32_t retargeting_interval,
    uint32_t retargeting_interval_seconds) NOEXCEPT
{
    const auto limit = compact::expand(proof_of_work_limit);
    auto target = compact::expand(work_required_retarget_bits(values,
        forks, proof_of_work_limit, retargeting_interval));

    // Conditionally implement retarget overflow patch (e.g. Litecoin).
    const auto timewarp = to_int(patch_timewarp(forks, limit, target));

    target >>= timewarp;
    target *= retarget_timespan(values, minimum_timespan, maximum_timespan);
    target /= retargeting_interval_seconds;
    target <<= timewarp;

    // Disallow target from falling below minimum configured.
    // All targets are a bits value normalized by compress here.
    return target > limit ? proof_of_work_limit : compact::compress(target);
}

uint32_t chain_state::easy_work_required(const data& values,
    uint32_t retargeting_interval,
    uint32_t proof_of_work_limit,
    uint32_t block_spacing_seconds) NOEXCEPT
{
    BC_ASSERT(!is_zero(values.height));

    // Overflow allowed here since supported coins would not do so.
    const auto easy_spacing_seconds = shift_left(block_spacing_seconds);

    // If the time limit has passed allow a minimum difficulty block.
    if (values.timestamp.self > ceilinged_add(timestamp_high(values),
        easy_spacing_seconds))
        return proof_of_work_limit;

    auto height = values.height;
    const auto& bits = values.bits.ordered;

    // Reverse iterate the ordered-by-height list of header bits.
    for (auto bit: std::views::reverse(bits))
    {
        if (is_retarget_or_non_limit(--height, bit, retargeting_interval,
            proof_of_work_limit))
            return bit;
    }

    // Since the set of heights is either a full retarget range or ends at
    // zero this is not reachable unless the data set is invalid.
    BC_ASSERT(false);
    return proof_of_work_limit;
}

// ****************************************************************************
// CONSENSUS: Hardcoded with bip90 activated.
// ****************************************************************************
size_t chain_state::bip9_bit0_height(size_t height,
    const checkpoint& bip9_bit0_active_checkpoint) NOEXCEPT
{
    const auto activation_height = bip9_bit0_active_checkpoint.height();

    // Require bip9_bit0 hash at heights at/above bip9_bit0 active.
    return height < activation_height ? map::unrequested : activation_height;
}

// ****************************************************************************
// CONSENSUS: Hardcoded in satoshi client (pull request #????).
// ****************************************************************************
size_t chain_state::bip9_bit1_height(size_t height,
    const checkpoint& bip9_bit1_active_checkpoint) NOEXCEPT
{
    const auto activation_height = bip9_bit1_active_checkpoint.height();

    // Require bip9_bit1 hash at heights at/above bip9_bit1 active.
    return height < activation_height ? map::unrequested : activation_height;
}

// ****************************************************************************
// CONSENSUS: Not yet hardcoded in satoshi client (as of 0.29.0).
// ****************************************************************************
size_t chain_state::bip9_bit2_height(size_t height,
    const checkpoint& bip9_bit2_active_checkpoint) NOEXCEPT
{
    const auto activation_height = bip9_bit2_active_checkpoint.height();

    // Require bip9_bit2 hash at heights at/above bip9_bit2 active.
    return height < activation_height ? map::unrequested : activation_height;
}

chain_state::map::signal chain_state::bip9_signal(size_t height,
    const deployment& deployment, const system::settings& settings) NOEXCEPT
{
    const auto period = settings.bip9_activation_period;

    // Sample periods down from the boundary at/below height.
    return
    {
        period_floor(height, period),
        period,
        settings.bip9_version_base,
        deployment.bit,
        deployment.start,
        deployment.timeout
    };
}

// Public static
// ----------------------------------------------------------------------------

chain_state::map chain_state::get_map(size_t height,
    const system::settings& settings) NOEXCEPT
{
    const auto& forks = settings.forks;
    map map{};

    // The checkpoint at/above which bip9_bit0 rules are enforced.
    if (forks.bip68 || forks.bip112 || forks.bip113)
        map.bip9_bit0_height = bip9_bit0_height(height,
            settings.bip9_bit0_active_checkpoint);

    // The checkpoint at/above which bip9_bit1 rules are enforced.
    if (forks.bip141 || forks.bip143 || forks.bip147)
        map.bip9_bit1_height = bip9_bit1_height(height,
            settings.bip9_bit1_active_checkpoint);

    // The checkpoint at/above which bip9_bit2 rules are enforced.
    if (forks.bip341 || forks.bip342)
        map.bip9_bit2_height = bip9_bit2_height(height,
            settings.bip9_bit2_active_checkpoint);

    // The signal periods of each signaled deployment.
    const auto configured = configured_flags(forks);
    const auto list = deployments(settings);
    for (size_t index{}; index < list.size(); ++index)
        if (is_signaled(list.at(index), settings, configured))
            map.signals.at(index) = bip9_signal(height, list.at(index),
                settings);

    // Genesis has no preceding block data (regtest checkpoints are at genesis).
    if (is_zero(height))
        return map;

    const auto interval = settings.retargeting_interval();

    // The height bound of the reverse (high to low) retarget search.
    map.bits.high = sub1(height);
    map.bits.count = bits_count(height, forks, interval);

    // The height bound of the median time past function.
    map.timestamp.high = sub1(height);
    map.timestamp.count = timestamp_count(height, forks);

    // The height bound of the version sample for activations.
    map.version.high = sub1(height);
    map.version.count = version_count(height, forks,
        settings.bip34_activation_sample);

    // The most recent past retarget height.
    map.timestamp_retarget = retarget_height(height, forks, interval);

    return map;
}

// Consensus defines median time using modulo 2 element selection.
// This differs from arithmetic median which averages two middle values.
uint32_t chain_state::median(const timestamps& times) NOEXCEPT
{
    const auto sorted = sort_copy(times);
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return sorted.empty() ? 0 : sorted.at(to_half(sorted.size()));
    BC_POP_WARNING()
}

bool chain_state::is_signal(uint32_t version, uint32_t base,
    uint32_t bit) NOEXCEPT
{
    return bit_and(version, bip9_version_mask) == base
        && get_right(version, bit);
}

// Only a miner signals, in the version of a block templated on the pool
// state. Configured bip9 groups are signaled unconditionally, where bip9
// defines signaling only while a deployment is started or locked in.
uint32_t chain_state::signal_version(const system::settings& settings) NOEXCEPT
{
    const auto& forks = settings.forks;

    // TODO: these can be retired.
    // Signal bip9 bit2 if any of the group is configured.
    if (forks.bip341 || forks.bip342)
        return settings.bip9_version_base | settings.bip9_version_bit2;

    // TODO: these can be retired.
    // Signal bip9 bit1 if any of the group is configured.
    if (forks.bip141 || forks.bip143 || forks.bip147)
        return settings.bip9_version_base | settings.bip9_version_bit1;

    // TODO: these can be retired.
    // Signal bip9 bit0 if any of the group is configured.
    if (forks.bip68 || forks.bip112 || forks.bip113)
        return settings.bip9_version_base | settings.bip9_version_bit0;

    if (forks.bip65)
        return settings.bip65_version;

    if (forks.bip66)
        return settings.bip66_version;

    if (forks.bip34)
        return settings.bip34_version;

    return settings.first_version;
}

// Signals.
// ----------------------------------------------------------------------------

// Count the previous block signal, and change state at a period boundary.
void chain_state::promote(data::signal& signal, const deployment& deployment,
    uint32_t version, uint32_t median_time_past, size_t height,
    const system::settings& settings) NOEXCEPT
{
    const auto period = settings.bip9_activation_period;
    if (is_signal(version, settings.bip9_version_base, deployment.bit))
        ++signal.count;

    if (is_zero(height % period))
    {
        signal.state = next_state(deployment, signal.state, median_time_past,
            signal.count, height);
        signal.count = zero;
    }
}

// Change state at each sampled boundary, from defined below the first.
void chain_state::reduce(data::signal& signal, const deployment& deployment,
    size_t height, size_t period) NOEXCEPT
{
    if (signal.ordered.empty())
        return;

    const auto high = period_floor(height, period);
    auto boundary = high - sub1(signal.ordered.size()) * period;
    for (const auto& value: std::views::reverse(signal.ordered))
    {
        signal.state = next_state(deployment, signal.state,
            value.median_time_past, value.count, boundary);
        boundary += period;
    }

    signal.ordered.clear();
}

// Constructors.
// ----------------------------------------------------------------------------

// This is promotion from a preceding height to the next.
chain_state::data chain_state::to_pool(const chain_state& top,
    const system::settings& settings) NOEXCEPT
{
    // Alias configured forks.
    const auto& forks = top.forks_;

    // Copy data from presumed previous-height block state.
    chain_state::data data{ top.data_ };

    // If this overflows height is zero and result is handled as invalid.
    const auto height = add1(data.height);

    // Enqueue previous block values to collections.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    data.bits.ordered.push_back(data.bits.self);
    data.version.ordered.push_back(data.version.self);
    data.timestamp.ordered.push_back(data.timestamp.self);
    BC_POP_WARNING()

    // If bits collection overflows, dequeue oldest member.
    if (data.bits.ordered.size() >
        bits_count(height, forks, settings.retargeting_interval()))
        data.bits.ordered.pop_front();

    // If version collection overflows, dequeue oldest member.
    if (data.version.ordered.size() > version_count(height, forks,
        settings.bip34_activation_sample))
        data.version.ordered.pop_front();

    // If timestamp collection overflows, dequeue oldest member.
    if (data.timestamp.ordered.size() > timestamp_count(height, forks))
        data.timestamp.ordered.pop_front();

    // Regtest does not perform retargeting.
    // If promoting from retarget height, move that timestamp into retarget.
    if (forks.retarget && is_retarget_height(sub1(height),
        settings.retargeting_interval()))
    {
        // Conditionally patch time warp bug (e.g. Litecoin).
        data.timestamp.retarget = (forks.ltc_time_warp_patch && !is_one(height)) ?
            *std::next(data.timestamp.ordered.crbegin()) : data.timestamp.self;
    }

    // Promote bip9 signaling from the previous block to this height.
    const auto mtp = median_time_past(data, forks);
    const auto configured = configured_flags(forks);
    const auto list = deployments(settings);
    for (size_t index{}; index < list.size(); ++index)
        if (is_signaled(list.at(index), settings, configured))
            promote(data.signals.at(index), list.at(index),
                data.version.self, mtp, height, settings);

    // Replace previous block state with tx pool chain state for next height
    // Preserve top block timestamp for use in computation of staleness.
    // Preserve data.bip9_bit0_hash promotion.
    // Preserve data.bip9_bit1_hash promotion.
    // bits.self is unused.
    data.height = height;
    data.hash = {};
    data.bits.self = 0;
    data.version.self = signal_version(settings);
    return data;
}

// Top to pool.
// This generates a state for the pool above the presumed top block state.
chain_state::chain_state(const chain_state& top,
    const system::settings& settings) NOEXCEPT
  : data_(to_pool(top, settings)),
    forks_(top.forks_),
    activations_(activation(data_, forks_, settings)),
    work_required_(work_required(data_, forks_, settings)),
    median_time_past_(median_time_past(data_, forks_))
{
}

chain_state::data chain_state::to_block(const chain_state& pool,
    const block& block, const system::settings& settings) NOEXCEPT
{
    // Copy data from presumed same-height pool state.
    chain_state::data data{ pool.data_ };

    // Replace pool chain state with block state at same (next) height.
    // Preserve data.timestamp.retarget promotion.
    const auto& header = block.header();
    data.hash = {};
    data.bits.self = header.bits();
    data.version.self = header.version();
    data.timestamp.self = header.timestamp();
    data.cumulative_work += header.proof();

    // Cache hash of bip9 bit0 height block, otherwise use preceding state.
    if (data.height == settings.bip9_bit0_active_checkpoint.height())
        data.bip9_bit0_hash = data.hash;

    // Cache hash of bip9 bit1 height block, otherwise use preceding state.
    if (data.height == settings.bip9_bit1_active_checkpoint.height())
        data.bip9_bit1_hash = data.hash;

    // Cache hash of bip9 bit2 height block, otherwise use preceding state.
    if (data.height == settings.bip9_bit2_active_checkpoint.height())
        data.bip9_bit2_hash = data.hash;

    return data;
}

// Pool to block.
// This assumes that the pool state is the same height as the block.
chain_state::chain_state(const chain_state& pool, const block& block,
    const system::settings& settings) NOEXCEPT
  : data_(to_block(pool, block, settings)),
    forks_(pool.forks_),
    activations_(activation(data_, forks_, settings)),
    work_required_(work_required(data_, forks_, settings)),
    median_time_past_(median_time_past(data_, forks_))
{
}

chain_state::data chain_state::to_header(const chain_state& parent,
    const header& header, const system::settings& settings) NOEXCEPT
{
    BC_ASSERT(header.previous_block_hash() == parent.hash());

    // Copy and promote data from presumed parent-height header/block state.
    auto data = to_pool(parent, settings);

    // Replace the parent (pool or previous) block state with given state.
    // Preserve data.timestamp.retarget promotion.
    data.hash = header.hash();
    data.bits.self = header.bits();
    data.version.self = header.version();
    data.timestamp.self = header.timestamp();
    data.cumulative_work += header.proof();

    // Cache hash of bip9 bit0 height block, otherwise use preceding state.
    if (data.height == settings.bip9_bit0_active_checkpoint.height())
        data.bip9_bit0_hash = data.hash;

    // Cache hash of bip9 bit1 height block, otherwise use preceding state.
    if (data.height == settings.bip9_bit1_active_checkpoint.height())
        data.bip9_bit1_hash = data.hash;

    // Cache hash of bip9 bit2 height block, otherwise use preceding state.
    if (data.height == settings.bip9_bit2_active_checkpoint.height())
        data.bip9_bit2_hash = data.hash;

    return data;
}

// Parent to header.
// This assumes that parent is the state of the header's previous block.
chain_state::chain_state(const chain_state& parent, const header& header,
    const system::settings& settings) NOEXCEPT
  : data_(to_header(parent, header, settings)),
    forks_(parent.forks_),
    activations_(activation(data_, forks_, settings)),
    work_required_(work_required(data_, forks_, settings)),
    median_time_past_(median_time_past(data_, forks_))
{
}

chain_state::data chain_state::to_values(data values,
    const system::settings& settings) NOEXCEPT
{
    const auto period = settings.bip9_activation_period;
    const auto configured = configured_flags(settings.forks);
    const auto list = deployments(settings);

    // Compute bip9 states from sampled boundaries.
    for (size_t index{}; index < list.size(); ++index)
        if (is_signaled(list.at(index), settings, configured))
            reduce(values.signals.at(index), list.at(index), values.height,
                period);

    return values;
}

// From scratch (e.g. raw data obtained from store).
chain_state::chain_state(data&& values,
    const system::settings& settings) NOEXCEPT
  : data_(to_values(std::move(values), settings)),
    forks_(settings.forks),
    activations_(activation(data_, forks_, settings)),
    work_required_(work_required(data_, forks_, settings)),
    median_time_past_(median_time_past(data_, forks_))
{
}

// Properties.
// ----------------------------------------------------------------------------

chain::context chain_state::context() const NOEXCEPT
{
    return
    {
        flags(),
        timestamp(),
        median_time_past(),
        possible_narrow_cast<uint32_t>(height()),
        minimum_block_version(),
        work_required(),
        previous_timestamp()
    };
}

const hash_digest& chain_state::hash() const NOEXCEPT
{
    return data_.hash;
}

const uint256_t& chain_state::cumulative_work() const NOEXCEPT
{
    return data_.cumulative_work;
}

uint32_t chain_state::minimum_block_version() const NOEXCEPT
{
    return activations_.minimum_block_version;
}

uint32_t chain_state::work_required() const NOEXCEPT
{
    return work_required_;
}

// context

uint32_t chain_state::timestamp() const NOEXCEPT
{
    return data_.timestamp.self;
}

uint32_t chain_state::previous_timestamp() const NOEXCEPT
{
    return data_.timestamp.ordered.empty() ? 0 : timestamp_high(data_);
}

uint32_t chain_state::median_time_past() const NOEXCEPT
{
    return median_time_past_;
}

uint32_t chain_state::flags() const NOEXCEPT
{
    return activations_.flags;
}

size_t chain_state::height() const NOEXCEPT
{
    return data_.height;
}

} // namespace chain
} // namespace system
} // namespace libbitcoin

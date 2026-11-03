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
#include "../test.hpp"

BC_AUTO_TEST_SUITE(chain_state_tests)

using namespace system::chain;

struct test_chain_state
  : chain::chain_state
{
    using chain::chain_state::activation;
    using chain::chain_state::median_time_past;
    using chain::chain_state::next_state;
    using chain::chain_state::work_required;
};

using state_t = chain_state::deployment_state;

// activation (version signalled soft forks)
// ----------------------------------------------------------------------------
// The bip90 fork is disabled so that version signalling is exercised.

BC_AUTO_TEST_CASE(chain_state__activation__no_signal_history__no_version_rules)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(!to_bool(state.flags & flags::bip34_rule));
    BC_REQUIRE(!to_bool(state.flags & flags::bip66_rule));
    BC_REQUIRE(!to_bool(state.flags & flags::bip65_rule));
    BC_REQUIRE_EQUAL(state.minimum_block_version, 0u);
}

BC_AUTO_TEST_CASE(chain_state__activation__bip34_at_activation_threshold__bip34_rule)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = settings.bip34_version;
    values.version.ordered = chain_state::versions(settings.bip34_activation_threshold, settings.bip34_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(to_bool(state.flags & flags::bip34_rule));
    BC_REQUIRE(!to_bool(state.flags & flags::bip66_rule));
    BC_REQUIRE_EQUAL(state.minimum_block_version, 0u);
}

BC_AUTO_TEST_CASE(chain_state__activation__bip34_below_activation_threshold__no_bip34_rule)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = settings.bip34_version;
    values.version.ordered = chain_state::versions(sub1(settings.bip34_activation_threshold), settings.bip34_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(!to_bool(state.flags & flags::bip34_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__bip34_signalled_but_low_self_version__no_bip34_rule)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = 1;
    values.version.ordered = chain_state::versions(settings.bip34_activation_threshold, settings.bip34_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(!to_bool(state.flags & flags::bip34_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__negative_self_version__no_bip34_rule)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = 0x80000000;
    values.version.ordered = chain_state::versions(settings.bip34_activation_threshold, settings.bip34_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(!to_bool(state.flags & flags::bip34_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__negative_self_version__no_bip66_rule)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = 0x80000000;
    values.version.ordered = chain_state::versions(settings.bip34_activation_threshold, settings.bip66_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(!to_bool(state.flags & flags::bip66_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__negative_self_version__no_bip65_rule)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = 0x80000000;
    values.version.ordered = chain_state::versions(settings.bip34_activation_threshold, settings.bip65_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(!to_bool(state.flags & flags::bip65_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__negative_history_versions__not_counted)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = settings.bip34_version;
    values.version.ordered = chain_state::versions(settings.bip34_activation_threshold, 0x80000000);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(!to_bool(state.flags & flags::bip34_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__bip66_signalled__bip66_and_bip34_rules)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = settings.bip66_version;
    values.version.ordered = chain_state::versions(settings.bip34_activation_threshold, settings.bip66_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(to_bool(state.flags & flags::bip34_rule));
    BC_REQUIRE(to_bool(state.flags & flags::bip66_rule));
    BC_REQUIRE(!to_bool(state.flags & flags::bip65_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__bip65_at_enforcement_threshold__minimum_version_four)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = settings.bip65_version;
    values.version.ordered = chain_state::versions(settings.bip34_enforcement_threshold, settings.bip65_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(to_bool(state.flags & flags::bip34_rule));
    BC_REQUIRE(to_bool(state.flags & flags::bip66_rule));
    BC_REQUIRE(to_bool(state.flags & flags::bip65_rule));
    BC_REQUIRE_EQUAL(state.minimum_block_version, settings.bip65_version);
}

BC_AUTO_TEST_CASE(chain_state__activation__bip66_at_enforcement_threshold__minimum_version_three)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = settings.bip66_version;
    values.version.ordered = chain_state::versions(settings.bip34_enforcement_threshold, settings.bip66_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE_EQUAL(state.minimum_block_version, settings.bip66_version);
}

// activation (bip90 freezes)
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(chain_state__activation__bip90_below_bip34_height__no_version_rules)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.height = sub1(settings.bip90_bip34_height);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(!to_bool(state.flags & flags::bip34_rule));
    BC_REQUIRE_EQUAL(state.minimum_block_version, 0u);
}

BC_AUTO_TEST_CASE(chain_state__activation__bip90_at_bip34_height__bip34_rule_frozen)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.height = settings.bip90_bip34_height;
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(to_bool(state.flags & flags::bip34_rule));
    BC_REQUIRE(!to_bool(state.flags & flags::bip66_rule));
    BC_REQUIRE(!to_bool(state.flags & flags::bip65_rule));
    BC_REQUIRE_EQUAL(state.minimum_block_version, settings.bip34_version);
}

BC_AUTO_TEST_CASE(chain_state__activation__bip90_at_bip65_height__all_version_rules_frozen)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.height = settings.bip90_bip65_height;
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(to_bool(state.flags & flags::bip34_rule));
    BC_REQUIRE(to_bool(state.flags & flags::bip66_rule));
    BC_REQUIRE(to_bool(state.flags & flags::bip65_rule));
    BC_REQUIRE_EQUAL(state.minimum_block_version, settings.bip65_version);
}

// activation (bip16)
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(chain_state__activation__at_bip16_activation_time__bip16_rule)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.timestamp.self = settings.bip16_activation_time;
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(to_bool(state.flags & flags::bip16_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__before_bip16_activation_time__no_bip16_rule)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.timestamp.self = sub1(settings.bip16_activation_time);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(!to_bool(state.flags & flags::bip16_rule));
}

// next_state (bip9 deployment transitions)
// ----------------------------------------------------------------------------

static const chain_state::deployment bip9_test{ 0, 0, 100, 200, 10, 0, false };
static const chain_state::deployment speedy_test{ 0, 0, 100, 200, 10, 4032, true };

BC_AUTO_TEST_CASE(chain_state__next_state__defined_below_start__defined)
{
    BC_REQUIRE(test_chain_state::next_state(bip9_test, state_t::defined, 99, 0, 2016) == state_t::defined);
}

BC_AUTO_TEST_CASE(chain_state__next_state__defined_at_start__started)
{
    BC_REQUIRE(test_chain_state::next_state(bip9_test, state_t::defined, 100, 0, 2016) == state_t::started);
}

BC_AUTO_TEST_CASE(chain_state__next_state__defined_at_timeout__failed)
{
    BC_REQUIRE(test_chain_state::next_state(bip9_test, state_t::defined, 200, 0, 2016) == state_t::failed);
}

BC_AUTO_TEST_CASE(chain_state__next_state__speedy_defined_at_timeout__started)
{
    BC_REQUIRE(test_chain_state::next_state(speedy_test, state_t::defined, 200, 0, 2016) == state_t::started);
}

BC_AUTO_TEST_CASE(chain_state__next_state__started_below_threshold__started)
{
    BC_REQUIRE(test_chain_state::next_state(bip9_test, state_t::started, 150, 9, 2016) == state_t::started);
}

BC_AUTO_TEST_CASE(chain_state__next_state__started_at_threshold__locked_in)
{
    BC_REQUIRE(test_chain_state::next_state(bip9_test, state_t::started, 150, 10, 2016) == state_t::locked_in);
}

BC_AUTO_TEST_CASE(chain_state__next_state__started_at_threshold_and_timeout__failed)
{
    BC_REQUIRE(test_chain_state::next_state(bip9_test, state_t::started, 200, 10, 2016) == state_t::failed);
}

BC_AUTO_TEST_CASE(chain_state__next_state__speedy_started_at_threshold_and_timeout__locked_in)
{
    BC_REQUIRE(test_chain_state::next_state(speedy_test, state_t::started, 200, 10, 2016) == state_t::locked_in);
}

BC_AUTO_TEST_CASE(chain_state__next_state__speedy_started_below_threshold_at_timeout__failed)
{
    BC_REQUIRE(test_chain_state::next_state(speedy_test, state_t::started, 200, 9, 2016) == state_t::failed);
}

BC_AUTO_TEST_CASE(chain_state__next_state__locked_in__active)
{
    BC_REQUIRE(test_chain_state::next_state(bip9_test, state_t::locked_in, 150, 0, 2016) == state_t::active);
}

BC_AUTO_TEST_CASE(chain_state__next_state__speedy_locked_in_below_activation_height__locked_in)
{
    BC_REQUIRE(test_chain_state::next_state(speedy_test, state_t::locked_in, 300, 0, 2016) == state_t::locked_in);
}

BC_AUTO_TEST_CASE(chain_state__next_state__speedy_locked_in_at_activation_height__active)
{
    BC_REQUIRE(test_chain_state::next_state(speedy_test, state_t::locked_in, 300, 0, 4032) == state_t::active);
}

BC_AUTO_TEST_CASE(chain_state__next_state__speedy_defined_below_start_at_timeout__defined)
{
    const chain_state::deployment late{ 0, 0, 300, 200, 10, 0, true };
    BC_REQUIRE(test_chain_state::next_state(late, state_t::defined, 250, 0, 2016) == state_t::defined);
}

BC_AUTO_TEST_CASE(chain_state__next_state__active__active)
{
    BC_REQUIRE(test_chain_state::next_state(bip9_test, state_t::active, 300, 0, 2016) == state_t::active);
}

BC_AUTO_TEST_CASE(chain_state__next_state__failed__failed)
{
    BC_REQUIRE(test_chain_state::next_state(bip9_test, state_t::failed, 150, 10, 2016) == state_t::failed);
}

// is_signal
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(chain_state__is_signal__base_and_bit__true)
{
    BC_REQUIRE(chain_state::is_signal(0x20000001, 0x20000000, 0));
}

BC_AUTO_TEST_CASE(chain_state__is_signal__base_without_bit__false)
{
    BC_REQUIRE(!chain_state::is_signal(0x20000002, 0x20000000, 0));
}

BC_AUTO_TEST_CASE(chain_state__is_signal__bit_without_base__false)
{
    BC_REQUIRE(!chain_state::is_signal(0x00000001, 0x20000000, 0));
}

BC_AUTO_TEST_CASE(chain_state__is_signal__other_top_bits__false)
{
    BC_REQUIRE(!chain_state::is_signal(0x60000001, 0x20000000, 0));
}

// activation (bip9 signaled)
// ----------------------------------------------------------------------------
// Boundaries descend from the boundary at/below the height.

static chain_state::data::signal::boundary started_boundary(const chain_state::deployment& deployment, size_t count)
{
    return { deployment.start, count };
}

// Construction above genesis requires the previous block bits.
static chain_state::data signaled_values(size_t height)
{
    chain_state::data values{};
    values.height = height;
    values.bits.ordered.push_back(0x1e0ffff0u);
    return values;
}

BC_AUTO_TEST_CASE(chain_state__activation__bip9_bit0_signaled_lock_in__bip68_bip112_bip113_rules)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const auto& deployment = settings.bip9_bit0_deployment;
    auto values = signaled_values(3 * 2016 + 5);
    values.signals[0].ordered = { started_boundary(deployment, 0), started_boundary(deployment, deployment.threshold), started_boundary(deployment, 0) };
    const chain_state state{ std::move(values), settings };
    BC_REQUIRE(to_bool(state.flags() & flags::bip68_rule));
    BC_REQUIRE(to_bool(state.flags() & flags::bip112_rule));
    BC_REQUIRE(to_bool(state.flags() & flags::bip113_rule));
    BC_REQUIRE(!to_bool(state.flags() & flags::bip141_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__bip9_bit0_signaled_below_threshold__no_bip68_rule)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const auto& deployment = settings.bip9_bit0_deployment;
    auto values = signaled_values(3 * 2016 + 5);
    values.signals[0].ordered = { started_boundary(deployment, 0), started_boundary(deployment, sub1(deployment.threshold)), started_boundary(deployment, 0) };
    const chain_state state{ std::move(values), settings };
    BC_REQUIRE(!to_bool(state.flags() & flags::bip68_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__bip9_bit2_signaled_below_activation_height__no_taproot_rules)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const auto& deployment = settings.bip9_bit2_deployment;
    auto values = signaled_values(3 * 2016 + 5);
    values.signals[2].ordered = { started_boundary(deployment, 0), started_boundary(deployment, deployment.threshold), started_boundary(deployment, 0) };
    const chain_state state{ std::move(values), settings };
    BC_REQUIRE(!to_bool(state.flags() & flags::bip341_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__bip9_bit2_signaled_at_activation_height__taproot_rules)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const auto& deployment = settings.bip9_bit2_deployment;
    auto values = signaled_values(add1(deployment.activation_height));
    values.signals[2].ordered = { started_boundary(deployment, 0), started_boundary(deployment, deployment.threshold), started_boundary(deployment, 0) };
    const chain_state state{ std::move(values), settings };
    BC_REQUIRE(to_bool(state.flags() & flags::bip341_rule));
    BC_REQUIRE(to_bool(state.flags() & flags::bip342_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__bip9_bit0_always_active__bip68_rule)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    settings.bip9_bit0_deployment.start = chain_state::deployment::always_active;
    const chain_state state{ signaled_values(1), settings };
    BC_REQUIRE(to_bool(state.flags() & flags::bip68_rule));
    BC_REQUIRE(!to_bool(state.flags() & flags::bip141_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__bip9_bit0_always_active_genesis__no_bip68_rule)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    settings.bip9_bit0_deployment.start = chain_state::deployment::always_active;
    const chain_state state{ chain_state::data{}, settings };
    BC_REQUIRE(!to_bool(state.flags() & flags::bip68_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__bip9_bit0_always_active_unconfigured__no_bip68_rule)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    settings.forks.bip68 = false;
    settings.forks.bip112 = false;
    settings.forks.bip113 = false;
    settings.bip9_bit0_deployment.start = chain_state::deployment::always_active;
    const chain_state state{ chain_state::data{}, settings };
    BC_REQUIRE(!to_bool(state.flags() & flags::bip68_rule));
}

// bip54 deployments are always active on regtest and never signaled elsewhere.
BC_AUTO_TEST_CASE(chain_state__activation__regtest__bip54_rules)
{
    const settings settings(selection::regtest);
    const chain_state state{ signaled_values(1), settings };
    BC_REQUIRE(to_bool(state.flags() & flags::bip54_timewarp_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__regtest_genesis__no_bip54_rules)
{
    const settings settings(selection::regtest);
    const chain_state state{ chain_state::data{}, settings };
    BC_REQUIRE(!to_bool(state.flags() & flags::bip54_timewarp_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__mainnet__no_bip54_rules)
{
    const settings settings(selection::mainnet);
    const chain_state state{ chain_state::data{}, settings };
    BC_REQUIRE(!to_bool(state.flags() & flags::bip54_timewarp_rule));
    BC_REQUIRE_EQUAL(chain_state::get_map(1000, settings).signals[3].high, chain_state::map::unrequested);
}

// activation (bip30 lifecycle)
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(chain_state__activation__default_data__bip30_rule)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(to_bool(state.flags & flags::bip30_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__bip34_height__bip30_rule)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.height = settings.bip90_bip34_height;
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(to_bool(state.flags & flags::bip30_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__bip30_exception_block__no_bip30_rule)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.hash = base16_hash("00000000000a4d0a398161ffc163c503763b1f4360639393e0e4c8e300e0caec");
    values.height = 91842;
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(!to_bool(state.flags & flags::bip30_rule));
}

// activation (configured rules)
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(chain_state__activation__mainnet_configuration__retarget_difficult_bip42)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(to_bool(state.flags & flags::retarget));
    BC_REQUIRE(to_bool(state.flags & flags::difficult));
    BC_REQUIRE(to_bool(state.flags & flags::bip42_rule));
    BC_REQUIRE(to_bool(state.flags & flags::bip90_rule));
}

// median_time_past
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(chain_state__median__even_count__upper_middle)
{
    BC_REQUIRE_EQUAL(chain_state::median({}), 0u);
    BC_REQUIRE_EQUAL(chain_state::median({ 5, 1, 3 }), 3u);
    BC_REQUIRE_EQUAL(chain_state::median({ 4, 1, 3, 2 }), 3u);
}

BC_AUTO_TEST_CASE(chain_state__median_time_past__empty__zero)
{
    settings settings(selection::mainnet);
    const chain_state::data values{};
    BC_REQUIRE_EQUAL(test_chain_state::median_time_past(values, settings.forks), 0u);
}

BC_AUTO_TEST_CASE(chain_state__median_time_past__eleven_unordered__median)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.timestamp.ordered = chain_state::timestamps{ 5, 1, 9, 3, 7, 2, 8, 4, 6, 11, 10 };
    BC_REQUIRE_EQUAL(test_chain_state::median_time_past(values, settings.forks), 6u);
}

BC_AUTO_TEST_CASE(chain_state__median_time_past__even_count__upper_middle)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.timestamp.ordered = chain_state::timestamps{ 1, 2, 3, 4 };
    BC_REQUIRE_EQUAL(test_chain_state::median_time_past(values, settings.forks), 3u);
}

// work_required
// ----------------------------------------------------------------------------

chain::chain_state::data get_values(size_t retargeting_interval)
{
    chain::chain_state::data values;
    values.height = retargeting_interval;
    values.bits.ordered.push_back(0x1e0ffff0u);
    values.timestamp.ordered.push_back(1692625u);
    values.timestamp.retarget = 0;
    return values;
}

BC_AUTO_TEST_CASE(chain_state__work_required__genesis__zero)
{
    settings settings(selection::mainnet);
    const chain_state::data values{};
    BC_REQUIRE_EQUAL(test_chain_state::work_required(values, settings.forks, settings), 0u);
}

BC_AUTO_TEST_CASE(chain_state__work_required__no_retarget__previous_bits)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    auto values = get_values(settings.retargeting_interval());
    values.height = 42;
    BC_REQUIRE_EQUAL(test_chain_state::work_required(values, settings.forks, settings), 0x1e0ffff0u);
}

BC_AUTO_TEST_CASE(chain_state__work_required__mainnet_inter_interval__previous_bits)
{
    settings settings(selection::mainnet);
    auto values = get_values(settings.retargeting_interval());
    values.height = add1<size_t>(settings.retargeting_interval());
    BC_REQUIRE_EQUAL(test_chain_state::work_required(values, settings.forks, settings), 0x1e0ffff0u);
}

BC_AUTO_TEST_CASE(chain_state__work_required__testnet_easy_time_exceeded__proof_of_work_limit)
{
    settings settings(selection::testnet3);
    auto values = get_values(settings.retargeting_interval());
    values.height = add1<size_t>(settings.retargeting_interval());
    values.timestamp.self = add1(1692625u + shift_left(settings.block_spacing_seconds));
    BC_REQUIRE_EQUAL(test_chain_state::work_required(values, settings.forks, settings), settings.proof_of_work_limit);
}

BC_AUTO_TEST_CASE(chain_state__work_required__testnet_easy_within_limit__last_non_limit_bits)
{
    settings settings(selection::testnet3);
    auto values = get_values(settings.retargeting_interval());
    values.height = settings.retargeting_interval() + 5u;
    values.bits.ordered.push_back(settings.proof_of_work_limit);
    values.timestamp.self = 1692625u;
    BC_REQUIRE_EQUAL(test_chain_state::work_required(values, settings.forks, settings), 0x1e0ffff0u);
}

// The easy allowance applies only past the limit, not at it.
BC_AUTO_TEST_CASE(chain_state__work_required__testnet_easy_time_at_limit__last_non_limit_bits)
{
    settings settings(selection::testnet3);
    auto values = get_values(settings.retargeting_interval());
    values.height = settings.retargeting_interval() + 5u;
    values.bits.ordered.push_back(settings.proof_of_work_limit);
    values.timestamp.self = 1692625u + shift_left(settings.block_spacing_seconds);
    BC_REQUIRE_EQUAL(test_chain_state::work_required(values, settings.forks, settings), 0x1e0ffff0u);
}

BC_AUTO_TEST_CASE(chain_state__work_required_retarget__mainnet_limit__proof_of_work_limit)
{
    settings settings(selection::mainnet);
    const auto values = get_values(settings.retargeting_interval());
    settings.forks.retarget = true;
    const auto work = test_chain_state::work_required(values, settings.forks, settings);
    BC_REQUIRE_EQUAL(work, settings.proof_of_work_limit);
}

BC_AUTO_TEST_CASE(chain_state__work_required_retarget__overflow_patch_disabled__lower_value)
{
    settings settings(chain::selection::mainnet);
    settings.proof_of_work_limit = 0x1e0fffff;
    const auto values = get_values(settings.retargeting_interval());
    settings.forks.retarget = true;
    const auto work = test_chain_state::work_required(values, settings.forks, settings);
    BC_REQUIRE_EQUAL(work, 0x1e0884d1u);
}

BC_AUTO_TEST_CASE(chain_state__work_required_retarget__overflow_patch_enabled__correct_value)
{
    settings settings(chain::selection::mainnet);
    settings.proof_of_work_limit = 0x1e0fffff;
    const auto values = get_values(settings.retargeting_interval());
    settings.forks.retarget = true;
    settings.forks.ltc_retarget_overflow_patch = true;
    const auto work = test_chain_state::work_required(values, settings.forks, settings);
    BC_REQUIRE_EQUAL(work, settings.proof_of_work_limit);
}

// The timespan is clamped to the configured bounds, so any two timespans
// beyond a bound retarget alike. The limit is raised so the result is not
// saturated to proof_of_work_limit.
BC_AUTO_TEST_CASE(chain_state__work_required_retarget__timespan_above_maximum__clamped)
{
    settings settings(chain::selection::mainnet);
    settings.proof_of_work_limit = 0x1e0fffff;
    auto values = get_values(settings.retargeting_interval());
    values.timestamp.ordered.push_back(add1(settings.maximum_timespan()));
    const auto first = test_chain_state::work_required(values, settings.forks, settings);

    values.timestamp.ordered.push_back(settings.maximum_timespan() * 8u);
    BC_REQUIRE_EQUAL(test_chain_state::work_required(values, settings.forks, settings), first);
}

BC_AUTO_TEST_CASE(chain_state__work_required_retarget__timespan_below_minimum__clamped)
{
    settings settings(chain::selection::mainnet);
    settings.proof_of_work_limit = 0x1e0fffff;
    auto values = get_values(settings.retargeting_interval());
    values.timestamp.ordered.push_back(sub1(settings.minimum_timespan()));
    const auto first = test_chain_state::work_required(values, settings.forks, settings);

    values.timestamp.ordered.push_back(settings.minimum_timespan() / 8u);
    BC_REQUIRE_EQUAL(test_chain_state::work_required(values, settings.forks, settings), first);
}

BC_AUTO_TEST_CASE(chain_state__work_required_retarget__timespan_clamps__differ)
{
    settings settings(chain::selection::mainnet);
    settings.proof_of_work_limit = 0x1e0fffff;
    auto values = get_values(settings.retargeting_interval());
    values.timestamp.ordered.push_back(settings.maximum_timespan() * 8u);
    const auto high = test_chain_state::work_required(values, settings.forks, settings);

    values.timestamp.ordered.push_back(settings.minimum_timespan() / 8u);
    BC_REQUIRE_NE(test_chain_state::work_required(values, settings.forks, settings), high);
}

// get_map
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(chain_state__get_map__genesis__default)
{
    const settings settings(selection::mainnet);
    const auto map = chain_state::get_map(0, settings);
    BC_REQUIRE_EQUAL(map.bits.count, 0u);
    BC_REQUIRE_EQUAL(map.version.count, 0u);
    BC_REQUIRE_EQUAL(map.timestamp.count, 0u);
}

BC_AUTO_TEST_CASE(chain_state__get_map__mainnet_height__expected_ranges)
{
    const settings settings(selection::mainnet);
    const auto map = chain_state::get_map(1000, settings);
    BC_REQUIRE_EQUAL(map.bits.high, 999u);
    BC_REQUIRE_EQUAL(map.timestamp.high, 999u);
    BC_REQUIRE_EQUAL(map.version.high, 999u);

    // Mainnet does not use bits in retargeting, and bip90 freezes versions.
    BC_REQUIRE_EQUAL(map.bits.count, 1u);
    BC_REQUIRE_EQUAL(map.version.count, 0u);
    BC_REQUIRE_EQUAL(map.timestamp.count, median_time_past_interval);
}

BC_AUTO_TEST_CASE(chain_state__get_map__below_median_time_past_interval__height_count)
{
    const settings settings(selection::mainnet);
    const auto map = chain_state::get_map(5, settings);
    BC_REQUIRE_EQUAL(map.timestamp.count, 5u);
}

BC_AUTO_TEST_CASE(chain_state__get_map__version_signalling_enabled__sample_count)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    const auto map = chain_state::get_map(10000, settings);
    BC_REQUIRE_EQUAL(map.version.count, settings.bip34_activation_sample);
}

BC_AUTO_TEST_CASE(chain_state__get_map__retarget_height__expected)
{
    const settings settings(selection::mainnet);
    const auto interval = settings.retargeting_interval();
    const auto map = chain_state::get_map(interval, settings);
    BC_REQUIRE_EQUAL(map.timestamp_retarget, 0u);

    const auto next = chain_state::get_map(add1<size_t>(interval), settings);
    BC_REQUIRE_EQUAL(next.timestamp_retarget, interval);
}

BC_AUTO_TEST_CASE(chain_state__get_map__bip9_disabled__unrequested)
{
    settings settings(selection::mainnet);
    settings.forks.bip68 = false;
    settings.forks.bip112 = false;
    settings.forks.bip113 = false;
    settings.forks.bip341 = false;
    settings.forks.bip342 = false;
    const auto map = chain_state::get_map(1000, settings);
    BC_REQUIRE_EQUAL(map.signals[0].high, chain_state::map::unrequested);
    BC_REQUIRE_EQUAL(map.signals[2].high, chain_state::map::unrequested);
}

BC_AUTO_TEST_CASE(chain_state__get_map__bip9_signaled__signal_requested)
{
    settings settings(selection::mainnet);
    const auto map = chain_state::get_map(add1(settings.bip9_activation_period), settings);
    BC_REQUIRE_EQUAL(map.signals[0].high, settings.bip9_activation_period);
    BC_REQUIRE_EQUAL(map.signals[0].period, settings.bip9_activation_period);
    BC_REQUIRE_EQUAL(map.signals[0].base, settings.bip9_version_base);
    BC_REQUIRE_EQUAL(map.signals[0].bit, settings.bip9_bit0_deployment.bit);
    BC_REQUIRE_EQUAL(map.signals[0].start, settings.bip9_bit0_deployment.start);
    BC_REQUIRE_EQUAL(map.signals[0].timeout, settings.bip9_bit0_deployment.timeout);
    BC_REQUIRE_EQUAL(map.signals[2].bit, settings.bip9_bit2_deployment.bit);
}

BC_AUTO_TEST_CASE(chain_state__get_map__bip9_always_active__signal_unrequested)
{
    const settings settings(selection::regtest);
    const auto map = chain_state::get_map(1, settings);
    BC_REQUIRE_EQUAL(map.signals[0].high, chain_state::map::unrequested);
    BC_REQUIRE_EQUAL(map.signals[1].high, chain_state::map::unrequested);
    BC_REQUIRE_EQUAL(map.signals[2].high, chain_state::map::unrequested);
}

// signal_version
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(chain_state__signal_version__mainnet__bip9_bit2)
{
    const settings settings(selection::mainnet);
    const auto expected = settings.bip9_version_base | settings.bip9_version_bit2;
    BC_REQUIRE_EQUAL(chain_state::signal_version(settings), expected);
}

BC_AUTO_TEST_CASE(chain_state__signal_version__taproot_disabled__bip9_bit1)
{
    settings settings(selection::mainnet);
    settings.forks.bip341 = false;
    settings.forks.bip342 = false;
    const auto expected = settings.bip9_version_base | settings.bip9_version_bit1;
    BC_REQUIRE_EQUAL(chain_state::signal_version(settings), expected);
}

BC_AUTO_TEST_CASE(chain_state__signal_version__segwit_disabled__bip9_bit0)
{
    settings settings(selection::mainnet);
    settings.forks.bip341 = false;
    settings.forks.bip342 = false;
    settings.forks.bip141 = false;
    settings.forks.bip143 = false;
    settings.forks.bip147 = false;
    const auto expected = settings.bip9_version_base | settings.bip9_version_bit0;
    BC_REQUIRE_EQUAL(chain_state::signal_version(settings), expected);
}

BC_AUTO_TEST_CASE(chain_state__signal_version__bip65_only__bip65_version)
{
    settings settings(selection::mainnet);
    settings.forks.bip341 = false;
    settings.forks.bip342 = false;
    settings.forks.bip141 = false;
    settings.forks.bip143 = false;
    settings.forks.bip147 = false;
    settings.forks.bip68 = false;
    settings.forks.bip112 = false;
    settings.forks.bip113 = false;
    const auto version = chain_state::signal_version(settings);
    BC_REQUIRE_EQUAL(version, settings.bip65_version);
}

BC_AUTO_TEST_CASE(chain_state__signal_version__no_forks__first_version)
{
    settings settings(selection::mainnet);
    settings.forks.bip341 = false;
    settings.forks.bip342 = false;
    settings.forks.bip141 = false;
    settings.forks.bip143 = false;
    settings.forks.bip147 = false;
    settings.forks.bip68 = false;
    settings.forks.bip112 = false;
    settings.forks.bip113 = false;
    settings.forks.bip65 = false;
    settings.forks.bip66 = false;
    settings.forks.bip34 = false;
    const auto version = chain_state::signal_version(settings);
    BC_REQUIRE_EQUAL(version, settings.first_version);
}

// configured_flags
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(chain_state__configured_flags__no_forks__no_rules)
{
    const forks forks{};
    BC_REQUIRE_EQUAL(chain_state::configured_flags(forks), flags::no_rules);
}

// Configured flags are independent of activation.
BC_AUTO_TEST_CASE(chain_state__configured_flags__mainnet__all_rules)
{
    const settings settings(selection::mainnet);
    const auto configured = chain_state::configured_flags(settings.forks);
    BC_REQUIRE(to_bool(configured & flags::bip16_rule));
    BC_REQUIRE(to_bool(configured & flags::bip34_rule));
    BC_REQUIRE(to_bool(configured & flags::bip141_rule));
    BC_REQUIRE(to_bool(configured & flags::bip341_rule));
    BC_REQUIRE(to_bool(configured & flags::retarget));
    BC_REQUIRE(to_bool(configured & flags::difficult));
}

BC_AUTO_TEST_CASE(chain_state__configured_flags__one_fork__one_rule)
{
    forks forks{};
    forks.bip16 = true;
    BC_REQUIRE_EQUAL(chain_state::configured_flags(forks), flags::bip16_rule);
}

// minimum_timespan/maximum_timespan
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(chain_state__minimum_timespan__mainnet__interval_over_factor)
{
    const settings settings(selection::mainnet);
    const auto interval = settings.retargeting_interval_seconds;
    const auto factor = settings.retargeting_factor;
    const auto expected = interval / factor;
    BC_REQUIRE_EQUAL(chain_state::minimum_timespan(interval, factor), expected);
}

BC_AUTO_TEST_CASE(chain_state__maximum_timespan__mainnet__interval_times_factor)
{
    const settings settings(selection::mainnet);
    const auto interval = settings.retargeting_interval_seconds;
    const auto factor = settings.retargeting_factor;
    const auto expected = interval * factor;
    BC_REQUIRE_EQUAL(chain_state::maximum_timespan(interval, factor), expected);
}

// State transitions.
// ----------------------------------------------------------------------------
// Retargeting is disabled so that work_required is the preceding bits.

static chain::chain_state::data transition_values()
{
    chain::chain_state::data values{};
    values.height = 42;
    values.hash = one_hash;
    values.bits.self = 0x1e0ffff0u;
    values.version.self = 4u;
    values.timestamp.self = 1000u;
    values.bits.ordered.push_back(0x1e0ffff0u);
    values.version.ordered.push_back(4u);
    values.timestamp.ordered.push_back(900u);
    values.cumulative_work = 1u;
    return values;
}

static system::chain::header transition_header(uint32_t timestamp)
{
    return { 4u, one_hash, system::hash_digest{}, timestamp, 0x1e0ffff0u, 0u };
}

BC_AUTO_TEST_CASE(chain_state__previous_timestamp__no_history__zero)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state state{ chain_state::data{}, settings };
    BC_REQUIRE_EQUAL(state.previous_timestamp(), 0u);
}

BC_AUTO_TEST_CASE(chain_state__construct__top_to_pool__next_height)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state top{ transition_values(), settings };
    const chain_state pool{ top, settings };
    BC_REQUIRE_EQUAL(pool.height(), add1(top.height()));
    BC_REQUIRE_EQUAL(pool.hash(), null_hash);
}

// The pool carries no block, so it accumulates no work.
BC_AUTO_TEST_CASE(chain_state__construct__top_to_pool__same_work)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state top{ transition_values(), settings };
    const chain_state pool{ top, settings };
    BC_REQUIRE_EQUAL(pool.cumulative_work(), top.cumulative_work());
}

// The top block timestamp is preserved for the computation of staleness.
BC_AUTO_TEST_CASE(chain_state__construct__top_to_pool__promoted_timestamps)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state top{ transition_values(), settings };
    const chain_state pool{ top, settings };
    BC_REQUIRE_EQUAL(pool.timestamp(), top.timestamp());
    BC_REQUIRE_EQUAL(pool.previous_timestamp(), top.timestamp());
}

BC_AUTO_TEST_CASE(chain_state__construct__pool_to_block__same_height)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state top{ transition_values(), settings };
    const chain_state pool{ top, settings };
    const chain::block block{ transition_header(1100u), transactions{} };
    const chain_state state{ pool, block, settings };
    BC_REQUIRE_EQUAL(state.height(), pool.height());
    BC_REQUIRE_EQUAL(state.hash(), null_hash);
}

BC_AUTO_TEST_CASE(chain_state__construct__pool_to_block__accumulated_work)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state top{ transition_values(), settings };
    const chain_state pool{ top, settings };
    const auto header = transition_header(1100u);
    const chain::block block{ header, transactions{} };
    const chain_state state{ pool, block, settings };
    const auto expected = pool.cumulative_work() + header.proof();
    BC_REQUIRE_EQUAL(state.cumulative_work(), expected);
    BC_REQUIRE_EQUAL(state.timestamp(), 1100u);
}

BC_AUTO_TEST_CASE(chain_state__construct__parent_to_header__next_height)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state parent{ transition_values(), settings };
    const auto header = transition_header(1100u);
    const chain_state state{ parent, header, settings };
    BC_REQUIRE_EQUAL(state.height(), add1(parent.height()));
    BC_REQUIRE_EQUAL(state.hash(), header.hash());
    BC_REQUIRE_EQUAL(state.timestamp(), 1100u);
}

BC_AUTO_TEST_CASE(chain_state__construct__parent_to_header__accumulated_work)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state parent{ transition_values(), settings };
    const auto header = transition_header(1100u);
    const chain_state state{ parent, header, settings };
    const auto expected = parent.cumulative_work() + header.proof();
    BC_REQUIRE_EQUAL(state.cumulative_work(), expected);
}

// The pool and header transitions promote identically apart from identity.
BC_AUTO_TEST_CASE(chain_state__construct__parent_to_header__pool_promotion)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state parent{ transition_values(), settings };
    const chain_state pool{ parent, settings };
    const auto header = transition_header(1100u);
    const chain_state state{ parent, header, settings };
    BC_REQUIRE_EQUAL(state.height(), pool.height());
    BC_REQUIRE_EQUAL(state.previous_timestamp(), pool.previous_timestamp());
    BC_REQUIRE_EQUAL(state.median_time_past(), pool.median_time_past());
}

// Two block periods with a threshold of one: locked in at boundary 2 by the
// signal of block 1, and active at boundary 4.
BC_AUTO_TEST_CASE(chain_state__construct__parent_to_header_bip9_signal__active_at_next_boundary)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    settings.bip9_activation_period = 2;
    settings.bip9_bit0_deployment.start = 1;
    settings.bip9_bit0_deployment.timeout = max_uint32;
    settings.bip9_bit0_deployment.threshold = 1;
    auto values = transition_values();
    values.height = 1;
    values.version.self = 0x20000001;
    values.signals[0].state = state_t::started;
    const chain_state parent{ std::move(values), settings };
    const chain::header header2{ 4u, one_hash, system::hash_digest{}, 1100u, 0x1e0ffff0u, 0u };
    const chain::header header3{ 4u, header2.hash(), system::hash_digest{}, 1200u, 0x1e0ffff0u, 0u };
    const chain::header header4{ 4u, header3.hash(), system::hash_digest{}, 1300u, 0x1e0ffff0u, 0u };
    const chain_state state2{ parent, header2, settings };
    const chain_state state3{ state2, header3, settings };
    const chain_state state4{ state3, header4, settings };
    BC_REQUIRE(!to_bool(state2.flags() & flags::bip68_rule));
    BC_REQUIRE(!to_bool(state3.flags() & flags::bip68_rule));
    BC_REQUIRE(to_bool(state4.flags() & flags::bip68_rule));
}

BC_AUTO_TEST_CASE(chain_state__construct__parent_to_header_bip9_no_signal__not_active)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    settings.bip9_activation_period = 2;
    settings.bip9_bit0_deployment.start = 1;
    settings.bip9_bit0_deployment.timeout = max_uint32;
    settings.bip9_bit0_deployment.threshold = 1;
    auto values = transition_values();
    values.height = 1;
    values.version.self = 0x00000001;
    values.signals[0].state = state_t::started;
    const chain_state parent{ std::move(values), settings };
    const chain::header header2{ 4u, one_hash, system::hash_digest{}, 1100u, 0x1e0ffff0u, 0u };
    const chain::header header3{ 4u, header2.hash(), system::hash_digest{}, 1200u, 0x1e0ffff0u, 0u };
    const chain::header header4{ 4u, header3.hash(), system::hash_digest{}, 1300u, 0x1e0ffff0u, 0u };
    const chain_state state2{ parent, header2, settings };
    const chain_state state3{ state2, header3, settings };
    const chain_state state4{ state3, header4, settings };
    BC_REQUIRE(!to_bool(state4.flags() & flags::bip68_rule));
}

// block storm patch
// ----------------------------------------------------------------------------
// BIP94: the retarget is computed from the first block of the period, so a
// retarget height requires the full interval of bits.

BC_AUTO_TEST_CASE(chain_state__get_map__retarget_height_block_storm_patch__full_interval)
{
    const settings settings(selection::testnet4);
    BC_REQUIRE(settings.forks.block_storm_patch);
    const auto interval = settings.retargeting_interval();
    BC_REQUIRE_EQUAL(chain_state::get_map(interval, settings).bits.count, interval);
}

BC_AUTO_TEST_CASE(chain_state__get_map__retarget_height_unpatched__one)
{
    const settings settings(selection::testnet3);
    BC_REQUIRE(!settings.forks.block_storm_patch);
    const auto interval = settings.retargeting_interval();
    BC_REQUIRE_EQUAL(chain_state::get_map(interval, settings).bits.count, 1u);
}

// The period is on target, with a minimum difficulty block preceding retarget.
static chain::chain_state::data storm_values(const settings& settings, uint32_t first)
{
    const auto interval = settings.retargeting_interval();
    chain::chain_state::data values{};
    values.height = interval;
    values.timestamp.retarget = 1000000;
    values.timestamp.ordered.push_back(1000000 + interval * settings.block_spacing_seconds);
    values.bits.ordered = chain_state::bitss(interval, settings.proof_of_work_limit);
    values.bits.ordered.front() = first;
    return values;
}

BC_AUTO_TEST_CASE(chain_state__work_required_retarget__block_storm_patch__period_first_bits)
{
    const settings settings(selection::testnet4);
    const auto values = storm_values(settings, 0x1c0ffff0);
    BC_REQUIRE_EQUAL(test_chain_state::work_required(values, settings.forks, settings), 0x1c0ffff0u);
}

// Unpatched, the minimum difficulty block lowers the retarget to the limit.
BC_AUTO_TEST_CASE(chain_state__work_required_retarget__unpatched__proof_of_work_limit)
{
    settings settings(selection::testnet4);
    settings.forks.block_storm_patch = false;
    const auto values = storm_values(settings, 0x1c0ffff0);
    BC_REQUIRE_EQUAL(test_chain_state::work_required(values, settings.forks, settings), settings.proof_of_work_limit);
}
// configured_flags (activation independent of signalling)
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(chain_state__configured__ltc_time_warp_patch__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.ltc_time_warp_patch = true;
    const auto configured = chain_state::configured_flags(settings.forks);
    BC_REQUIRE(to_bool(configured & flags::ltc_time_warp_patch));
}

BC_AUTO_TEST_CASE(chain_state__configured__ltc_retarget_overflow_patch__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.ltc_retarget_overflow_patch = true;
    const auto configured = chain_state::configured_flags(settings.forks);
    BC_REQUIRE(to_bool(configured & flags::ltc_retarget_overflow_patch));
}

BC_AUTO_TEST_CASE(chain_state__configured__ltc_scrypt_proof_of_work__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.ltc_scrypt_proof_of_work = true;
    const auto configured = chain_state::configured_flags(settings.forks);
    BC_REQUIRE(to_bool(configured & flags::ltc_scrypt_proof_of_work));
}

BC_AUTO_TEST_CASE(chain_state__configured__time_warp_patch__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.time_warp_patch = true;
    const auto configured = chain_state::configured_flags(settings.forks);
    BC_REQUIRE(to_bool(configured & flags::time_warp_patch));
}

BC_AUTO_TEST_CASE(chain_state__configured__block_storm_patch__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.block_storm_patch = true;
    const auto configured = chain_state::configured_flags(settings.forks);
    BC_REQUIRE(to_bool(configured & flags::block_storm_patch));
}

BC_AUTO_TEST_CASE(chain_state__configured__signet__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.signet = true;
    const auto configured = chain_state::configured_flags(settings.forks);
    BC_REQUIRE(to_bool(configured & flags::signet_rule));
}

// activation (configuration-only forks)
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(chain_state__activation__ltc_time_warp_patch__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.ltc_time_warp_patch = true;
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(to_bool(state.flags & flags::ltc_time_warp_patch));
}

BC_AUTO_TEST_CASE(chain_state__activation__ltc_retarget_overflow_patch__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.ltc_retarget_overflow_patch = true;
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(to_bool(state.flags & flags::ltc_retarget_overflow_patch));
}

BC_AUTO_TEST_CASE(chain_state__activation__ltc_scrypt_proof_of_work__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.ltc_scrypt_proof_of_work = true;
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(to_bool(state.flags & flags::ltc_scrypt_proof_of_work));
}

BC_AUTO_TEST_CASE(chain_state__activation__time_warp_patch__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.time_warp_patch = true;
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(to_bool(state.flags & flags::time_warp_patch));
}

BC_AUTO_TEST_CASE(chain_state__activation__block_storm_patch__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.block_storm_patch = true;
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(to_bool(state.flags & flags::block_storm_patch));
}

BC_AUTO_TEST_CASE(chain_state__activation__signet__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.signet = true;
    chain_state::data values{};
    values.height = 1;
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(to_bool(state.flags & flags::signet_rule));
}

BC_AUTO_TEST_CASE(chain_state__activation__signet_genesis__unflagged)
{
    settings settings(selection::mainnet);
    settings.forks.signet = true;
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BC_REQUIRE(!to_bool(state.flags & flags::signet_rule));
}

// signal_version
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(chain_state__signal_version__bip66_only__bip66_version)
{
    settings settings(selection::mainnet);
    settings.forks = {};
    settings.forks.bip66 = true;
    const auto version = chain_state::signal_version(settings);
    BC_REQUIRE_EQUAL(version, settings.bip66_version);
}

BC_AUTO_TEST_CASE(chain_state__signal_version__bip34_only__bip34_version)
{
    settings settings(selection::mainnet);
    settings.forks = {};
    settings.forks.bip34 = true;
    const auto version = chain_state::signal_version(settings);
    BC_REQUIRE_EQUAL(version, settings.bip34_version);
}

// get_map (regtest bypasses retargeting)
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(chain_state__get_map__no_retarget__unrequested)
{
    settings settings(selection::mainnet);
    settings.forks.difficult = false;
    settings.forks.retarget = false;
    settings.forks.bip54_timespan = false;
    const auto map = chain_state::get_map(42, settings);
    BC_REQUIRE_EQUAL(map.bits.count, one);
    BC_REQUIRE_EQUAL(map.timestamp_retarget, chain_state::map::unrequested);
}

// bip54 requires the period's first timestamp without retargeting.
BC_AUTO_TEST_CASE(chain_state__get_map__no_retarget_bip54_timespan__requested)
{
    settings settings(selection::mainnet);
    settings.forks.difficult = false;
    settings.forks.retarget = false;
    settings.forks.bip54_timespan = true;
    const auto map = chain_state::get_map(42, settings);
    BC_REQUIRE_EQUAL(map.bits.count, one);
    BC_REQUIRE_EQUAL(map.timestamp_retarget, 0u);
}

// Promotion from a retarget height carries that block's timestamp as the
// retarget timestamp of the period, without retargeting.
BC_AUTO_TEST_CASE(chain_state__construct__parent_to_header_from_retarget_height__retarget_timestamp)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    auto values = transition_values();
    values.height = settings.retargeting_interval();
    values.timestamp.retarget = 7;
    const chain_state parent{ std::move(values), settings };
    const chain_state state{ parent, transition_header(1100u), settings };
    BC_REQUIRE_EQUAL(parent.retarget_timestamp(), 7u);
    BC_REQUIRE_EQUAL(state.retarget_timestamp(), parent.timestamp());
    BC_REQUIRE_EQUAL(state.context().retarget_timestamp, 1000u);
}

BC_AUTO_TEST_CASE(chain_state__context__retarget_timestamp__expected)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    auto values = transition_values();
    values.timestamp.retarget = 7;
    const chain_state state{ std::move(values), settings };
    BC_REQUIRE_EQUAL(state.retarget_timestamp(), 7u);
    BC_REQUIRE_EQUAL(state.context().retarget_timestamp, 7u);
}

// properties
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(chain_state__context__always__matches_properties)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state state{ transition_values(), settings };
    const auto ctx = state.context();
    BC_REQUIRE_EQUAL(ctx.flags, state.flags());
    BC_REQUIRE_EQUAL(ctx.timestamp, state.timestamp());
    BC_REQUIRE_EQUAL(ctx.median_time_past, state.median_time_past());
    BC_REQUIRE_EQUAL(ctx.height, state.height());
    BC_REQUIRE_EQUAL(ctx.work_required, state.work_required());
    BC_REQUIRE_EQUAL(ctx.previous_timestamp, state.previous_timestamp());
    const auto version = state.minimum_block_version();
    BC_REQUIRE_EQUAL(ctx.minimum_block_version, version);
}

BC_AUTO_TEST_CASE(chain_state__work_required__invalid_previous_bits__zero)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    auto values = transition_values();
    values.bits.ordered.clear();
    values.bits.ordered.push_back(0u);
    const chain_state state{ std::move(values), settings };
    BC_REQUIRE_EQUAL(state.work_required(), 0u);
}

// promotion collections
// ----------------------------------------------------------------------------

// The oldest timestamp is dequeued once the median window is exceeded.
static chain::chain_state::data window_values()
{
    auto values = transition_values();
    values.timestamp.ordered.clear();
    values.timestamp.ordered.push_back(1000u);
    values.timestamp.ordered.push_back(2u);
    values.timestamp.ordered.push_back(3u);
    values.timestamp.ordered.push_back(4u);
    values.timestamp.ordered.push_back(5u);
    values.timestamp.ordered.push_back(6u);
    values.timestamp.ordered.push_back(7u);
    values.timestamp.ordered.push_back(8u);
    values.timestamp.ordered.push_back(9u);
    values.timestamp.ordered.push_back(10u);
    values.timestamp.ordered.push_back(11u);
    values.timestamp.self = 12u;
    return values;
}

BC_AUTO_TEST_CASE(chain_state__construct__top_to_pool__oldest_timestamp_dequeued)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state top{ window_values(), settings };
    const chain_state pool{ top, settings };
    BC_REQUIRE_EQUAL(pool.median_time_past(), 7u);
}

// retarget promotion
// ----------------------------------------------------------------------------

static chain::chain_state::data retarget_values()
{
    auto values = transition_values();
    values.height = 2016;
    values.timestamp.ordered.push_back(950u);
    return values;
}

BC_AUTO_TEST_CASE(chain_state__construct__top_to_pool__retarget_height__promoted)
{
    settings settings(selection::mainnet);
    const chain_state top{ retarget_values(), settings };
    const chain_state pool{ top, settings };
    BC_REQUIRE_EQUAL(pool.height(), 2017u);
    BC_REQUIRE_EQUAL(pool.previous_timestamp(), 1000u);
}

// The retargeting interval is configured down so a full period is spannable.
// bitcoind starts the period (interval - 1) blocks back, litecoin goes back
// the full interval, so the patched period start is one block earlier.

static chain::chain_state::data warp_values()
{
    chain::chain_state::data values{};
    values.height = 4;
    values.hash = one_hash;
    values.bits.self = 0x1d00ffffu;
    values.version.self = 4u;
    values.timestamp.self = 980u;
    values.bits.ordered.push_back(0x1d00ffffu);
    values.version.ordered.push_back(4u);
    values.timestamp.ordered.push_back(970u);
    values.cumulative_work = 1u;
    return values;
}

static system::chain::header warp_header(const hash_digest& previous,
    uint32_t timestamp)
{
    return { 4u, previous, system::hash_digest{}, timestamp, 0x1d00ffffu, 0u };
}

static uint32_t warp_work_required(bool patch)
{
    settings settings(selection::mainnet);
    settings.retargeting_interval_seconds = 40u;
    settings.block_spacing_seconds = 10u;
    settings.forks.ltc_time_warp_patch = patch;

    const chain_state top{ warp_values(), settings };
    const auto header5 = warp_header(top.hash(), 990u);
    const chain_state state5{ top, header5, settings };
    const auto header6 = warp_header(state5.hash(), 995u);
    const chain_state state6{ state5, header6, settings };
    const auto header7 = warp_header(state6.hash(), 1000u);
    const chain_state state7{ state6, header7, settings };
    const auto header8 = warp_header(state7.hash(), 1005u);
    const chain_state state8{ state7, header8, settings };

    BC_REQUIRE_EQUAL(state8.height(), 8u);
    return state8.work_required();
}

// Period start is height 4 (timestamp 980), so the timespan is 1000-980.
// The patched period start is height 3 (timestamp 970), timespan 1000-970.
// A longer timespan retargets to a larger (easier) target.
BC_AUTO_TEST_CASE(chain_state__work_required__ltc_time_warp_patch__easier)
{
    const auto plain = warp_work_required(false);
    const auto patched = warp_work_required(true);
    BC_REQUIRE_NE(patched, plain);
    BC_REQUIRE(chain::compact::expand(patched) > chain::compact::expand(plain));
}

BC_AUTO_TEST_SUITE_END()

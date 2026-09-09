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

BOOST_AUTO_TEST_SUITE(chain_state_tests)

using namespace system::chain;

struct test_chain_state
  : chain::chain_state
{
    using chain::chain_state::activation;
    using chain::chain_state::median_time_past;
    using chain::chain_state::work_required;
};

// activation (version signalled soft forks)
// ----------------------------------------------------------------------------
// The bip90 fork is disabled so that version signalling is exercised.

BOOST_AUTO_TEST_CASE(chain_state__activation__no_signal_history__no_version_rules)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip34_rule));
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip66_rule));
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip65_rule));
    BOOST_REQUIRE_EQUAL(state.minimum_block_version, 0u);
}

BOOST_AUTO_TEST_CASE(chain_state__activation__bip34_at_activation_threshold__bip34_rule)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = settings.bip34_version;
    values.version.ordered = chain_state::versions(settings.bip34_activation_threshold, settings.bip34_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::bip34_rule));
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip66_rule));
    BOOST_REQUIRE_EQUAL(state.minimum_block_version, 0u);
}

BOOST_AUTO_TEST_CASE(chain_state__activation__bip34_below_activation_threshold__no_bip34_rule)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = settings.bip34_version;
    values.version.ordered = chain_state::versions(sub1(settings.bip34_activation_threshold), settings.bip34_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip34_rule));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__bip34_signalled_but_low_self_version__no_bip34_rule)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = 1;
    values.version.ordered = chain_state::versions(settings.bip34_activation_threshold, settings.bip34_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip34_rule));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__negative_self_version__no_bip34_rule)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = 0x80000000;
    values.version.ordered = chain_state::versions(settings.bip34_activation_threshold, settings.bip34_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip34_rule));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__negative_self_version__no_bip66_rule)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = 0x80000000;
    values.version.ordered = chain_state::versions(settings.bip34_activation_threshold, settings.bip66_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip66_rule));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__negative_self_version__no_bip65_rule)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = 0x80000000;
    values.version.ordered = chain_state::versions(settings.bip34_activation_threshold, settings.bip65_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip65_rule));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__negative_history_versions__not_counted)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = settings.bip34_version;
    values.version.ordered = chain_state::versions(settings.bip34_activation_threshold, 0x80000000);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip34_rule));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__bip66_signalled__bip66_and_bip34_rules)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = settings.bip66_version;
    values.version.ordered = chain_state::versions(settings.bip34_activation_threshold, settings.bip66_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::bip34_rule));
    BOOST_REQUIRE(to_bool(state.flags & flags::bip66_rule));
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip65_rule));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__bip65_at_enforcement_threshold__minimum_version_four)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = settings.bip65_version;
    values.version.ordered = chain_state::versions(settings.bip34_enforcement_threshold, settings.bip65_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::bip34_rule));
    BOOST_REQUIRE(to_bool(state.flags & flags::bip66_rule));
    BOOST_REQUIRE(to_bool(state.flags & flags::bip65_rule));
    BOOST_REQUIRE_EQUAL(state.minimum_block_version, settings.bip65_version);
}

BOOST_AUTO_TEST_CASE(chain_state__activation__bip66_at_enforcement_threshold__minimum_version_three)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = settings.bip66_version;
    values.version.ordered = chain_state::versions(settings.bip34_enforcement_threshold, settings.bip66_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE_EQUAL(state.minimum_block_version, settings.bip66_version);
}

// activation (bip90 freezes)
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(chain_state__activation__bip90_below_bip34_height__no_version_rules)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.height = sub1(settings.bip90_bip34_height);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip34_rule));
    BOOST_REQUIRE_EQUAL(state.minimum_block_version, 0u);
}

BOOST_AUTO_TEST_CASE(chain_state__activation__bip90_at_bip34_height__bip34_rule_frozen)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.height = settings.bip90_bip34_height;
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::bip34_rule));
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip66_rule));
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip65_rule));
    BOOST_REQUIRE_EQUAL(state.minimum_block_version, settings.bip34_version);
}

BOOST_AUTO_TEST_CASE(chain_state__activation__bip90_at_bip65_height__all_version_rules_frozen)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.height = settings.bip90_bip65_height;
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::bip34_rule));
    BOOST_REQUIRE(to_bool(state.flags & flags::bip66_rule));
    BOOST_REQUIRE(to_bool(state.flags & flags::bip65_rule));
    BOOST_REQUIRE_EQUAL(state.minimum_block_version, settings.bip65_version);
}

// activation (bip16)
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(chain_state__activation__at_bip16_activation_time__bip16_rule)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.timestamp.self = settings.bip16_activation_time;
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::bip16_rule));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__before_bip16_activation_time__no_bip16_rule)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.timestamp.self = sub1(settings.bip16_activation_time);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip16_rule));
}

// activation (bip9 checkpoint gates)
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(chain_state__activation__no_bip9_hashes__no_bip9_rules)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip68_rule));
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip112_rule));
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip113_rule));
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip141_rule));
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip143_rule));
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip147_rule));
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip341_rule));
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip342_rule));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__bip9_bit0_hash__bip68_bip112_bip113_rules)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.bip9_bit0_hash = settings.bip9_bit0_active_checkpoint.hash();
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::bip68_rule));
    BOOST_REQUIRE(to_bool(state.flags & flags::bip112_rule));
    BOOST_REQUIRE(to_bool(state.flags & flags::bip113_rule));
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip141_rule));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__bip9_bit1_hash__segwit_rules)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.bip9_bit1_hash = settings.bip9_bit1_active_checkpoint.hash();
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::bip141_rule));
    BOOST_REQUIRE(to_bool(state.flags & flags::bip143_rule));
    BOOST_REQUIRE(to_bool(state.flags & flags::bip147_rule));
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip341_rule));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__bip9_bit2_hash__taproot_rules)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.bip9_bit2_hash = settings.bip9_bit2_active_checkpoint.hash();
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::bip341_rule));
    BOOST_REQUIRE(to_bool(state.flags & flags::bip342_rule));
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip141_rule));
}

// activation (bip30 lifecycle)
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(chain_state__activation__default_data__bip30_rule)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::bip30_rule));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__bip30_deactivate_hash__no_bip30_rule)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.bip30_deactivate_hash = settings.bip30_deactivate_checkpoint.hash();
    values.height = settings.bip30_deactivate_checkpoint.height();
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip30_rule));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__bip30_reactivate_height__bip30_rule)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.bip30_deactivate_hash = settings.bip30_deactivate_checkpoint.hash();
    values.height = settings.bip30_reactivate_height;
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::bip30_rule));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__bip30_exception_block__no_bip30_rule)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.hash = base16_hash("00000000000a4d0a398161ffc163c503763b1f4360639393e0e4c8e300e0caec");
    values.height = 91842;
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(!to_bool(state.flags & flags::bip30_rule));
}

// activation (configured rules)
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(chain_state__activation__mainnet_configuration__retarget_difficult_bip42)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::retarget));
    BOOST_REQUIRE(to_bool(state.flags & flags::difficult));
    BOOST_REQUIRE(to_bool(state.flags & flags::bip42_rule));
    BOOST_REQUIRE(to_bool(state.flags & flags::bip90_rule));
}

// median_time_past
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(chain_state__median_time_past__empty__zero)
{
    settings settings(selection::mainnet);
    const chain_state::data values{};
    BOOST_REQUIRE_EQUAL(test_chain_state::median_time_past(values, settings.forks), 0u);
}

BOOST_AUTO_TEST_CASE(chain_state__median_time_past__eleven_unordered__median)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.timestamp.ordered = chain_state::timestamps{ 5, 1, 9, 3, 7, 2, 8, 4, 6, 11, 10 };
    BOOST_REQUIRE_EQUAL(test_chain_state::median_time_past(values, settings.forks), 6u);
}

BOOST_AUTO_TEST_CASE(chain_state__median_time_past__even_count__upper_middle)
{
    settings settings(selection::mainnet);
    chain_state::data values{};
    values.timestamp.ordered = chain_state::timestamps{ 1, 2, 3, 4 };
    BOOST_REQUIRE_EQUAL(test_chain_state::median_time_past(values, settings.forks), 3u);
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

BOOST_AUTO_TEST_CASE(chain_state__work_required__genesis__zero)
{
    settings settings(selection::mainnet);
    const chain_state::data values{};
    BOOST_REQUIRE_EQUAL(test_chain_state::work_required(values, settings.forks, settings), 0u);
}

BOOST_AUTO_TEST_CASE(chain_state__work_required__no_retarget__previous_bits)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    auto values = get_values(settings.retargeting_interval());
    values.height = 42;
    BOOST_REQUIRE_EQUAL(test_chain_state::work_required(values, settings.forks, settings), 0x1e0ffff0u);
}

BOOST_AUTO_TEST_CASE(chain_state__work_required__mainnet_inter_interval__previous_bits)
{
    settings settings(selection::mainnet);
    auto values = get_values(settings.retargeting_interval());
    values.height = add1<size_t>(settings.retargeting_interval());
    BOOST_REQUIRE_EQUAL(test_chain_state::work_required(values, settings.forks, settings), 0x1e0ffff0u);
}

BOOST_AUTO_TEST_CASE(chain_state__work_required__testnet_easy_time_exceeded__proof_of_work_limit)
{
    settings settings(selection::testnet3);
    auto values = get_values(settings.retargeting_interval());
    values.height = add1<size_t>(settings.retargeting_interval());
    values.timestamp.self = add1(1692625u + shift_left(settings.block_spacing_seconds));
    BOOST_REQUIRE_EQUAL(test_chain_state::work_required(values, settings.forks, settings), settings.proof_of_work_limit);
}

BOOST_AUTO_TEST_CASE(chain_state__work_required__testnet_easy_within_limit__last_non_limit_bits)
{
    settings settings(selection::testnet3);
    auto values = get_values(settings.retargeting_interval());
    values.height = settings.retargeting_interval() + 5u;
    values.bits.ordered.push_back(settings.proof_of_work_limit);
    values.timestamp.self = 1692625u;
    BOOST_REQUIRE_EQUAL(test_chain_state::work_required(values, settings.forks, settings), 0x1e0ffff0u);
}

BOOST_AUTO_TEST_CASE(chain_state__work_required_retarget__mainnet_limit__proof_of_work_limit)
{
    settings settings(selection::mainnet);
    const auto values = get_values(settings.retargeting_interval());
    settings.forks.retarget = true;
    const auto work = test_chain_state::work_required(values, settings.forks, settings);
    BOOST_REQUIRE_EQUAL(work, settings.proof_of_work_limit);
}

BOOST_AUTO_TEST_CASE(chain_state__work_required_retarget__overflow_patch_disabled__lower_value)
{
    settings settings(chain::selection::mainnet);
    settings.proof_of_work_limit = 0x1e0fffff;
    const auto values = get_values(settings.retargeting_interval());
    settings.forks.retarget = true;
    const auto work = test_chain_state::work_required(values, settings.forks, settings);
    BOOST_REQUIRE_EQUAL(work, 0x1e0884d1u);
}

BOOST_AUTO_TEST_CASE(chain_state__work_required_retarget__overflow_patch_enabled__correct_value)
{
    settings settings(chain::selection::mainnet);
    settings.proof_of_work_limit = 0x1e0fffff;
    const auto values = get_values(settings.retargeting_interval());
    settings.forks.retarget = true;
    settings.forks.ltc_retarget_overflow_patch = true;
    const auto work = test_chain_state::work_required(values, settings.forks, settings);
    BOOST_REQUIRE_EQUAL(work, settings.proof_of_work_limit);
}

// get_map
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(chain_state__get_map__genesis__default)
{
    const settings settings(selection::mainnet);
    const auto map = chain_state::get_map(0, settings);
    BOOST_REQUIRE_EQUAL(map.bits.count, 0u);
    BOOST_REQUIRE_EQUAL(map.version.count, 0u);
    BOOST_REQUIRE_EQUAL(map.timestamp.count, 0u);
}

BOOST_AUTO_TEST_CASE(chain_state__get_map__mainnet_height__expected_ranges)
{
    const settings settings(selection::mainnet);
    const auto map = chain_state::get_map(1000, settings);
    BOOST_REQUIRE_EQUAL(map.bits.high, 999u);
    BOOST_REQUIRE_EQUAL(map.timestamp.high, 999u);
    BOOST_REQUIRE_EQUAL(map.version.high, 999u);

    // Mainnet does not use bits in retargeting, and bip90 freezes versions.
    BOOST_REQUIRE_EQUAL(map.bits.count, 1u);
    BOOST_REQUIRE_EQUAL(map.version.count, 0u);
    BOOST_REQUIRE_EQUAL(map.timestamp.count, median_time_past_interval);
}

BOOST_AUTO_TEST_CASE(chain_state__get_map__below_median_time_past_interval__height_count)
{
    const settings settings(selection::mainnet);
    const auto map = chain_state::get_map(5, settings);
    BOOST_REQUIRE_EQUAL(map.timestamp.count, 5u);
}

BOOST_AUTO_TEST_CASE(chain_state__get_map__version_signalling_enabled__sample_count)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    const auto map = chain_state::get_map(10000, settings);
    BOOST_REQUIRE_EQUAL(map.version.count, settings.bip34_activation_sample);
}

BOOST_AUTO_TEST_CASE(chain_state__get_map__retarget_height__expected)
{
    const settings settings(selection::mainnet);
    const auto interval = settings.retargeting_interval();
    const auto map = chain_state::get_map(interval, settings);
    BOOST_REQUIRE_EQUAL(map.timestamp_retarget, 0u);

    const auto next = chain_state::get_map(add1<size_t>(interval), settings);
    BOOST_REQUIRE_EQUAL(next.timestamp_retarget, interval);
}

BOOST_AUTO_TEST_CASE(chain_state__get_map__bip9_disabled__unrequested)
{
    settings settings(selection::mainnet);
    settings.forks.bip68 = false;
    settings.forks.bip112 = false;
    settings.forks.bip113 = false;
    settings.forks.bip341 = false;
    settings.forks.bip342 = false;
    const auto map = chain_state::get_map(1000, settings);
    BOOST_REQUIRE_EQUAL(map.bip9_bit0_height, chain_state::map::unrequested);
    BOOST_REQUIRE_EQUAL(map.bip9_bit2_height, chain_state::map::unrequested);
}

BOOST_AUTO_TEST_CASE(chain_state__get_map__below_bip9_checkpoint__unrequested)
{
    const settings settings(selection::mainnet);
    const auto map = chain_state::get_map(1000, settings);
    BOOST_REQUIRE_EQUAL(map.bip9_bit0_height, chain_state::map::unrequested);
}

BOOST_AUTO_TEST_CASE(chain_state__get_map__at_bip9_bit0_checkpoint__requested)
{
    const settings settings(selection::mainnet);
    const auto height = settings.bip9_bit0_active_checkpoint.height();
    const auto map = chain_state::get_map(height, settings);
    BOOST_REQUIRE_EQUAL(map.bip9_bit0_height, height);
}

// signal_version
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(chain_state__signal_version__mainnet__bip9_bit2)
{
    const settings settings(selection::mainnet);
    const auto expected = settings.bip9_version_base | settings.bip9_version_bit2;
    BOOST_REQUIRE_EQUAL(chain_state::signal_version(settings), expected);
}

BOOST_AUTO_TEST_CASE(chain_state__signal_version__taproot_disabled__bip9_bit1)
{
    settings settings(selection::mainnet);
    settings.forks.bip341 = false;
    settings.forks.bip342 = false;
    const auto expected = settings.bip9_version_base | settings.bip9_version_bit1;
    BOOST_REQUIRE_EQUAL(chain_state::signal_version(settings), expected);
}

BOOST_AUTO_TEST_CASE(chain_state__signal_version__segwit_disabled__bip9_bit0)
{
    settings settings(selection::mainnet);
    settings.forks.bip341 = false;
    settings.forks.bip342 = false;
    settings.forks.bip141 = false;
    settings.forks.bip143 = false;
    settings.forks.bip147 = false;
    const auto expected = settings.bip9_version_base | settings.bip9_version_bit0;
    BOOST_REQUIRE_EQUAL(chain_state::signal_version(settings), expected);
}

BOOST_AUTO_TEST_CASE(chain_state__signal_version__bip65_only__bip65_version)
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
    BOOST_REQUIRE_EQUAL(version, settings.bip65_version);
}

BOOST_AUTO_TEST_CASE(chain_state__signal_version__no_forks__first_version)
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
    BOOST_REQUIRE_EQUAL(version, settings.first_version);
}

// configured_flags
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(chain_state__configured_flags__no_forks__no_rules)
{
    const forks forks{};
    BOOST_REQUIRE_EQUAL(chain_state::configured_flags(forks), flags::no_rules);
}

// Configured flags are independent of activation.
BOOST_AUTO_TEST_CASE(chain_state__configured_flags__mainnet__all_rules)
{
    const settings settings(selection::mainnet);
    const auto configured = chain_state::configured_flags(settings.forks);
    BOOST_REQUIRE(to_bool(configured & flags::bip16_rule));
    BOOST_REQUIRE(to_bool(configured & flags::bip34_rule));
    BOOST_REQUIRE(to_bool(configured & flags::bip141_rule));
    BOOST_REQUIRE(to_bool(configured & flags::bip341_rule));
    BOOST_REQUIRE(to_bool(configured & flags::retarget));
    BOOST_REQUIRE(to_bool(configured & flags::difficult));
}

BOOST_AUTO_TEST_CASE(chain_state__configured_flags__one_fork__one_rule)
{
    forks forks{};
    forks.bip16 = true;
    BOOST_REQUIRE_EQUAL(chain_state::configured_flags(forks), flags::bip16_rule);
}

// minimum_timespan/maximum_timespan
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(chain_state__minimum_timespan__mainnet__interval_over_factor)
{
    const settings settings(selection::mainnet);
    const auto interval = settings.retargeting_interval_seconds;
    const auto factor = settings.retargeting_factor;
    const auto expected = interval / factor;
    BOOST_REQUIRE_EQUAL(chain_state::minimum_timespan(interval, factor), expected);
}

BOOST_AUTO_TEST_CASE(chain_state__maximum_timespan__mainnet__interval_times_factor)
{
    const settings settings(selection::mainnet);
    const auto interval = settings.retargeting_interval_seconds;
    const auto factor = settings.retargeting_factor;
    const auto expected = interval * factor;
    BOOST_REQUIRE_EQUAL(chain_state::maximum_timespan(interval, factor), expected);
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

BOOST_AUTO_TEST_CASE(chain_state__previous_timestamp__no_history__zero)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state state{ chain_state::data{}, settings };
    BOOST_REQUIRE_EQUAL(state.previous_timestamp(), 0u);
}

BOOST_AUTO_TEST_CASE(chain_state__construct__top_to_pool__next_height)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state top{ transition_values(), settings };
    const chain_state pool{ top, settings };
    BOOST_REQUIRE_EQUAL(pool.height(), add1(top.height()));
    BOOST_REQUIRE_EQUAL(pool.hash(), null_hash);
}

// The pool carries no block, so it accumulates no work.
BOOST_AUTO_TEST_CASE(chain_state__construct__top_to_pool__same_work)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state top{ transition_values(), settings };
    const chain_state pool{ top, settings };
    BOOST_REQUIRE_EQUAL(pool.cumulative_work(), top.cumulative_work());
}

// The top block timestamp is preserved for the computation of staleness.
BOOST_AUTO_TEST_CASE(chain_state__construct__top_to_pool__promoted_timestamps)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state top{ transition_values(), settings };
    const chain_state pool{ top, settings };
    BOOST_REQUIRE_EQUAL(pool.timestamp(), top.timestamp());
    BOOST_REQUIRE_EQUAL(pool.previous_timestamp(), top.timestamp());
}

BOOST_AUTO_TEST_CASE(chain_state__construct__pool_to_block__same_height)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state top{ transition_values(), settings };
    const chain_state pool{ top, settings };
    const chain::block block{ transition_header(1100u), transactions{} };
    const chain_state state{ pool, block, settings };
    BOOST_REQUIRE_EQUAL(state.height(), pool.height());
    BOOST_REQUIRE_EQUAL(state.hash(), null_hash);
}

BOOST_AUTO_TEST_CASE(chain_state__construct__pool_to_block__accumulated_work)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state top{ transition_values(), settings };
    const chain_state pool{ top, settings };
    const auto header = transition_header(1100u);
    const chain::block block{ header, transactions{} };
    const chain_state state{ pool, block, settings };
    const auto expected = pool.cumulative_work() + header.proof();
    BOOST_REQUIRE_EQUAL(state.cumulative_work(), expected);
    BOOST_REQUIRE_EQUAL(state.timestamp(), 1100u);
}

BOOST_AUTO_TEST_CASE(chain_state__construct__parent_to_header__next_height)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state parent{ transition_values(), settings };
    const auto header = transition_header(1100u);
    const chain_state state{ parent, header, settings };
    BOOST_REQUIRE_EQUAL(state.height(), add1(parent.height()));
    BOOST_REQUIRE_EQUAL(state.hash(), header.hash());
    BOOST_REQUIRE_EQUAL(state.timestamp(), 1100u);
}

BOOST_AUTO_TEST_CASE(chain_state__construct__parent_to_header__accumulated_work)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state parent{ transition_values(), settings };
    const auto header = transition_header(1100u);
    const chain_state state{ parent, header, settings };
    const auto expected = parent.cumulative_work() + header.proof();
    BOOST_REQUIRE_EQUAL(state.cumulative_work(), expected);
}

// The pool and header transitions promote identically apart from identity.
BOOST_AUTO_TEST_CASE(chain_state__construct__parent_to_header__pool_promotion)
{
    settings settings(selection::mainnet);
    settings.forks.retarget = false;
    const chain_state parent{ transition_values(), settings };
    const chain_state pool{ parent, settings };
    const auto header = transition_header(1100u);
    const chain_state state{ parent, header, settings };
    BOOST_REQUIRE_EQUAL(state.height(), pool.height());
    BOOST_REQUIRE_EQUAL(state.previous_timestamp(), pool.previous_timestamp());
    BOOST_REQUIRE_EQUAL(state.median_time_past(), pool.median_time_past());
}

// block storm patch
// ----------------------------------------------------------------------------
// BIP94: the retarget is computed from the first block of the period, so a
// retarget height requires the full interval of bits.

BOOST_AUTO_TEST_CASE(chain_state__get_map__retarget_height_block_storm_patch__full_interval)
{
    const settings settings(selection::testnet4);
    BOOST_REQUIRE(settings.forks.block_storm_patch);
    const auto interval = settings.retargeting_interval();
    BOOST_REQUIRE_EQUAL(chain_state::get_map(interval, settings).bits.count, interval);
}

BOOST_AUTO_TEST_CASE(chain_state__get_map__retarget_height_unpatched__one)
{
    const settings settings(selection::testnet3);
    BOOST_REQUIRE(!settings.forks.block_storm_patch);
    const auto interval = settings.retargeting_interval();
    BOOST_REQUIRE_EQUAL(chain_state::get_map(interval, settings).bits.count, 1u);
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

BOOST_AUTO_TEST_CASE(chain_state__work_required_retarget__block_storm_patch__period_first_bits)
{
    const settings settings(selection::testnet4);
    const auto values = storm_values(settings, 0x1c0ffff0);
    BOOST_REQUIRE_EQUAL(test_chain_state::work_required(values, settings.forks, settings), 0x1c0ffff0u);
}

// Unpatched, the minimum difficulty block lowers the retarget to the limit.
BOOST_AUTO_TEST_CASE(chain_state__work_required_retarget__unpatched__proof_of_work_limit)
{
    settings settings(selection::testnet4);
    settings.forks.block_storm_patch = false;
    const auto values = storm_values(settings, 0x1c0ffff0);
    BOOST_REQUIRE_EQUAL(test_chain_state::work_required(values, settings.forks, settings), settings.proof_of_work_limit);
}
// configured_flags (activation independent of signalling)
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(chain_state__configured__ltc_time_warp_patch__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.ltc_time_warp_patch = true;
    const auto configured = chain_state::configured_flags(settings.forks);
    BOOST_REQUIRE(to_bool(configured & flags::ltc_time_warp_patch));
}

BOOST_AUTO_TEST_CASE(chain_state__configured__ltc_retarget_overflow_patch__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.ltc_retarget_overflow_patch = true;
    const auto configured = chain_state::configured_flags(settings.forks);
    BOOST_REQUIRE(to_bool(configured & flags::ltc_retarget_overflow_patch));
}

BOOST_AUTO_TEST_CASE(chain_state__configured__ltc_scrypt_proof_of_work__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.ltc_scrypt_proof_of_work = true;
    const auto configured = chain_state::configured_flags(settings.forks);
    BOOST_REQUIRE(to_bool(configured & flags::ltc_scrypt_proof_of_work));
}

BOOST_AUTO_TEST_CASE(chain_state__configured__time_warp_patch__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.time_warp_patch = true;
    const auto configured = chain_state::configured_flags(settings.forks);
    BOOST_REQUIRE(to_bool(configured & flags::time_warp_patch));
}

BOOST_AUTO_TEST_CASE(chain_state__configured__block_storm_patch__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.block_storm_patch = true;
    const auto configured = chain_state::configured_flags(settings.forks);
    BOOST_REQUIRE(to_bool(configured & flags::block_storm_patch));
}

// activation (configuration-only forks)
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(chain_state__activation__ltc_time_warp_patch__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.ltc_time_warp_patch = true;
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::ltc_time_warp_patch));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__ltc_retarget_overflow_patch__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.ltc_retarget_overflow_patch = true;
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::ltc_retarget_overflow_patch));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__ltc_scrypt_proof_of_work__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.ltc_scrypt_proof_of_work = true;
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::ltc_scrypt_proof_of_work));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__time_warp_patch__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.time_warp_patch = true;
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::time_warp_patch));
}

BOOST_AUTO_TEST_CASE(chain_state__activation__block_storm_patch__flagged)
{
    settings settings(selection::mainnet);
    settings.forks.block_storm_patch = true;
    chain_state::data values{};
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::block_storm_patch));
}

// signal_version
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(chain_state__signal_version__bip66_only__bip66_version)
{
    settings settings(selection::mainnet);
    settings.forks = {};
    settings.forks.bip66 = true;
    const auto version = chain_state::signal_version(settings);
    BOOST_REQUIRE_EQUAL(version, settings.bip66_version);
}

BOOST_AUTO_TEST_CASE(chain_state__signal_version__bip34_only__bip34_version)
{
    settings settings(selection::mainnet);
    settings.forks = {};
    settings.forks.bip34 = true;
    const auto version = chain_state::signal_version(settings);
    BOOST_REQUIRE_EQUAL(version, settings.bip34_version);
}

// get_map (regtest bypasses retargeting)
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(chain_state__get_map__no_retarget__unrequested)
{
    settings settings(selection::mainnet);
    settings.forks.difficult = false;
    settings.forks.retarget = false;
    const auto map = chain_state::get_map(42, settings);
    BOOST_REQUIRE_EQUAL(map.bits.count, one);
    BOOST_REQUIRE_EQUAL(map.timestamp_retarget, chain_state::map::unrequested);
}

BOOST_AUTO_TEST_SUITE_END()

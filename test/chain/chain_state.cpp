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

// The history count is signed (satoshi) but the self version gate is
// unsigned, so a negative self version activates (subsumed by bip90).
BOOST_AUTO_TEST_CASE(chain_state__activation__negative_self_version__bip34_rule)
{
    settings settings(selection::mainnet);
    settings.forks.bip90 = false;
    chain_state::data values{};
    values.version.self = 0x80000000;
    values.version.ordered = chain_state::versions(settings.bip34_activation_threshold, settings.bip34_version);
    const auto state = test_chain_state::activation(values, settings.forks, settings);
    BOOST_REQUIRE(to_bool(state.flags & flags::bip34_rule));
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

BOOST_AUTO_TEST_SUITE_END()

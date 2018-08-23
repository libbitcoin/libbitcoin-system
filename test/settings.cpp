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
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(settings_tests)

uint64_t max_money_recursive(uint64_t money)
{
    return money > 0 ? money + max_money_recursive(money >> 1) : 0;
}

// constructors
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(settings__construct__default_context__expected)
{
    settings configuration;
    BOOST_REQUIRE_EQUAL(configuration.retargeting_factor, 4);
    BOOST_REQUIRE_EQUAL(configuration.target_spacing_seconds, 600);
    BOOST_REQUIRE_EQUAL(configuration.easy_spacing_seconds, 1200);
    BOOST_REQUIRE_EQUAL(configuration.timestamp_future_seconds, 7200);
    BOOST_REQUIRE_EQUAL(configuration.target_timespan_seconds, 1209600);
    BOOST_REQUIRE_EQUAL(configuration.minimum_timespan, 302400);
    BOOST_REQUIRE_EQUAL(configuration.maximum_timespan, 4838400);
    BOOST_REQUIRE_EQUAL(configuration.retargeting_interval, 2016);
    BOOST_REQUIRE_EQUAL(configuration.first_version, 1);
    BOOST_REQUIRE_EQUAL(configuration.bip34_version, 2);
    BOOST_REQUIRE_EQUAL(configuration.bip66_version, 3);
    BOOST_REQUIRE_EQUAL(configuration.bip65_version, 4);
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit0, 1u);
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit1, 2u);
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_base, 0x20000000);
    BOOST_REQUIRE_EQUAL(configuration.satoshi_per_bitcoin, 100000000);
    BOOST_REQUIRE_EQUAL(configuration.initial_block_subsidy_bitcoin, 50);
    BOOST_REQUIRE_EQUAL(configuration.recursive_money, 9999999989u);
    auto coin_subsidy = configuration.initial_block_subsidy_bitcoin;
    coin_subsidy = configuration.bitcoin_to_satoshi(coin_subsidy);
    const auto recursive_money = max_money_recursive(coin_subsidy);
    BOOST_REQUIRE_EQUAL(recursive_money, configuration.recursive_money);
    const auto satoshi_per_bitcoin = configuration.bitcoin_to_satoshi(1);
    BOOST_REQUIRE_EQUAL(satoshi_per_bitcoin, configuration.satoshi_per_bitcoin);
}

BOOST_AUTO_TEST_CASE(settings__construct__mainnet_context__expected)
{
    settings configuration(config::settings::mainnet);
    BOOST_REQUIRE_EQUAL(configuration.retargeting_factor, 4);
    BOOST_REQUIRE_EQUAL(configuration.target_spacing_seconds, 600);
    BOOST_REQUIRE_EQUAL(configuration.easy_spacing_seconds, 1200);
    BOOST_REQUIRE_EQUAL(configuration.timestamp_future_seconds, 7200);
    BOOST_REQUIRE_EQUAL(configuration.target_timespan_seconds, 1209600);
    BOOST_REQUIRE_EQUAL(configuration.proof_of_work_limit, 0x1d00ffff);
    BOOST_REQUIRE_EQUAL(configuration.minimum_timespan, 302400);
    BOOST_REQUIRE_EQUAL(configuration.maximum_timespan, 4838400);
    BOOST_REQUIRE_EQUAL(configuration.retargeting_interval, 2016);
    const chain::block genesis_block = configuration.genesis_block;
    BOOST_REQUIRE_EQUAL(genesis_block.to_data(), data_chunk({
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x3b, 0xa3, 0xed, 0xfd,
        0x7a, 0x7b, 0x12, 0xb2, 0x7a, 0xc7, 0x2c, 0x3e,
        0x67, 0x76, 0x8f, 0x61, 0x7f, 0xc8, 0x1b, 0xc3,
        0x88, 0x8a, 0x51, 0x32, 0x3a, 0x9f, 0xb8, 0xaa,
        0x4b, 0x1e, 0x5e, 0x4a, 0x29, 0xab, 0x5f, 0x49,
        0xff, 0xff, 0x00, 0x1d, 0x1d, 0xac, 0x2b, 0x7c,
        0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0xff, 0xff, 0x4d, 0x04, 0xff, 0xff, 0x00, 0x1d,
        0x01, 0x04, 0x45, 0x54, 0x68, 0x65, 0x20, 0x54,
        0x69, 0x6d, 0x65, 0x73, 0x20, 0x30, 0x33, 0x2f,
        0x4a, 0x61, 0x6e, 0x2f, 0x32, 0x30, 0x30, 0x39,
        0x20, 0x43, 0x68, 0x61, 0x6e, 0x63, 0x65, 0x6c,
        0x6c, 0x6f, 0x72, 0x20, 0x6f, 0x6e, 0x20, 0x62,
        0x72, 0x69, 0x6e, 0x6b, 0x20, 0x6f, 0x66, 0x20,
        0x73, 0x65, 0x63, 0x6f, 0x6e, 0x64, 0x20, 0x62,
        0x61, 0x69, 0x6c, 0x6f, 0x75, 0x74, 0x20, 0x66,
        0x6f, 0x72, 0x20, 0x62, 0x61, 0x6e, 0x6b, 0x73,
        0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0xf2, 0x05,
        0x2a, 0x01, 0x00, 0x00, 0x00, 0x43, 0x41, 0x04,
        0x67, 0x8a, 0xfd, 0xb0, 0xfe, 0x55, 0x48, 0x27,
        0x19, 0x67, 0xf1, 0xa6, 0x71, 0x30, 0xb7, 0x10,
        0x5c, 0xd6, 0xa8, 0x28, 0xe0, 0x39, 0x09, 0xa6,
        0x79, 0x62, 0xe0, 0xea, 0x1f, 0x61, 0xde, 0xb6,
        0x49, 0xf6, 0xbc, 0x3f, 0x4c, 0xef, 0x38, 0xc4,
        0xf3, 0x55, 0x04, 0xe5, 0x1e, 0xc1, 0x12, 0xde,
        0x5c, 0x38, 0x4d, 0xf7, 0xba, 0x0b, 0x8d, 0x57,
        0x8a, 0x4c, 0x70, 0x2b, 0x6b, 0xf1, 0x1d, 0x5f,
        0xac, 0x00, 0x00, 0x00, 0x00}));
    BOOST_REQUIRE_EQUAL(configuration.first_version, 1);
    BOOST_REQUIRE_EQUAL(configuration.bip34_version, 2);
    BOOST_REQUIRE_EQUAL(configuration.bip66_version, 3);
    BOOST_REQUIRE_EQUAL(configuration.bip65_version, 4);
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit0, 1u);
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit1, 2u);
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_base, 0x20000000);
    BOOST_REQUIRE_EQUAL(configuration.activation_threshold, 750);
    BOOST_REQUIRE_EQUAL(configuration.enforcement_threshold, 950);
    BOOST_REQUIRE_EQUAL(configuration.activation_sample, 1000);
    BOOST_REQUIRE_EQUAL(configuration.bip65_freeze, 388381);
    BOOST_REQUIRE_EQUAL(configuration.bip66_freeze, 363725);
    BOOST_REQUIRE_EQUAL(configuration.bip34_freeze, 227931);
    BOOST_REQUIRE_EQUAL(configuration.bip16_activation_time, 0x4f779a80);
    const config::checkpoint bip34_active(
        "000000000000024b89b42a942fe0d9fea3bb44ab7bd1b19115dd6a759c0808b8",
        configuration.bip34_freeze);
    BOOST_REQUIRE_EQUAL(configuration.bip34_active_checkpoint, bip34_active);
    const config::checkpoint bit0_active(
        "000000000000000004a1b34462cb8aeebd5799177f7a29cf28f2d1961716b5b5",
        419328);
    BOOST_REQUIRE_EQUAL(configuration.bip9_bit0_active_checkpoint, bit0_active);
    const config::checkpoint bit1_active(
        "0000000000000000001c8018d9cb3b742ef25114f27563e3fc4a1902167f9893",
        481824);
    BOOST_REQUIRE_EQUAL(configuration.bip9_bit1_active_checkpoint, bit1_active);
    BOOST_REQUIRE_EQUAL(configuration.satoshi_per_bitcoin, 100000000);
    BOOST_REQUIRE_EQUAL(configuration.initial_block_subsidy_bitcoin, 50);
    BOOST_REQUIRE_EQUAL(configuration.recursive_money, 9999999989u);
    BOOST_REQUIRE_EQUAL(configuration.subsidy_interval, 210000);
    auto coin_subsidy = configuration.initial_block_subsidy_bitcoin;
    coin_subsidy = configuration.bitcoin_to_satoshi(coin_subsidy);
    const auto recursive_money = max_money_recursive(coin_subsidy);
    BOOST_REQUIRE_EQUAL(recursive_money, configuration.recursive_money);
    const auto satoshi_per_bitcoin = configuration.bitcoin_to_satoshi(1);
    BOOST_REQUIRE_EQUAL(satoshi_per_bitcoin, configuration.satoshi_per_bitcoin);
    BOOST_REQUIRE_EQUAL(configuration.max_money, 2099999997690000);
}

BOOST_AUTO_TEST_CASE(settings__construct__testnet_context__expected)
{
    settings configuration(config::settings::testnet);
    BOOST_REQUIRE_EQUAL(configuration.retargeting_factor, 4);
    BOOST_REQUIRE_EQUAL(configuration.target_spacing_seconds, 600);
    BOOST_REQUIRE_EQUAL(configuration.easy_spacing_seconds, 1200);
    BOOST_REQUIRE_EQUAL(configuration.timestamp_future_seconds, 7200);
    BOOST_REQUIRE_EQUAL(configuration.target_timespan_seconds, 1209600);
    BOOST_REQUIRE_EQUAL(configuration.proof_of_work_limit, 0x1d00ffff);
    BOOST_REQUIRE_EQUAL(configuration.minimum_timespan, 302400);
    BOOST_REQUIRE_EQUAL(configuration.maximum_timespan, 4838400);
    BOOST_REQUIRE_EQUAL(configuration.retargeting_interval, 2016);
    const chain::block genesis_block = configuration.genesis_block;
    BOOST_REQUIRE_EQUAL(genesis_block.to_data(), data_chunk({
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x3b, 0xa3, 0xed, 0xfd,
        0x7a, 0x7b, 0x12, 0xb2, 0x7a, 0xc7, 0x2c, 0x3e,
        0x67, 0x76, 0x8f, 0x61, 0x7f, 0xc8, 0x1b, 0xc3,
        0x88, 0x8a, 0x51, 0x32, 0x3a, 0x9f, 0xb8, 0xaa,
        0x4b, 0x1e, 0x5e, 0x4a, 0xda, 0xe5, 0x49, 0x4d,
        0xff, 0xff, 0x00, 0x1d, 0x1a, 0xa4, 0xae, 0x18,
        0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0xff, 0xff, 0x4d, 0x04, 0xff, 0xff, 0x00, 0x1d,
        0x01, 0x04, 0x45, 0x54, 0x68, 0x65, 0x20, 0x54,
        0x69, 0x6d, 0x65, 0x73, 0x20, 0x30, 0x33, 0x2f,
        0x4a, 0x61, 0x6e, 0x2f, 0x32, 0x30, 0x30, 0x39,
        0x20, 0x43, 0x68, 0x61, 0x6e, 0x63, 0x65, 0x6c,
        0x6c, 0x6f, 0x72, 0x20, 0x6f, 0x6e, 0x20, 0x62,
        0x72, 0x69, 0x6e, 0x6b, 0x20, 0x6f, 0x66, 0x20,
        0x73, 0x65, 0x63, 0x6f, 0x6e, 0x64, 0x20, 0x62,
        0x61, 0x69, 0x6c, 0x6f, 0x75, 0x74, 0x20, 0x66,
        0x6f, 0x72, 0x20, 0x62, 0x61, 0x6e, 0x6b, 0x73,
        0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0xf2, 0x05,
        0x2a, 0x01, 0x00, 0x00, 0x00, 0x43, 0x41, 0x04,
        0x67, 0x8a, 0xfd, 0xb0, 0xfe, 0x55, 0x48, 0x27,
        0x19, 0x67, 0xf1, 0xa6, 0x71, 0x30, 0xb7, 0x10,
        0x5c, 0xd6, 0xa8, 0x28, 0xe0, 0x39, 0x09, 0xa6,
        0x79, 0x62, 0xe0, 0xea, 0x1f, 0x61, 0xde, 0xb6,
        0x49, 0xf6, 0xbc, 0x3f, 0x4c, 0xef, 0x38, 0xc4,
        0xf3, 0x55, 0x04, 0xe5, 0x1e, 0xc1, 0x12, 0xde,
        0x5c, 0x38, 0x4d, 0xf7, 0xba, 0x0b, 0x8d, 0x57,
        0x8a, 0x4c, 0x70, 0x2b, 0x6b, 0xf1, 0x1d, 0x5f,
        0xac, 0x00, 0x00, 0x00, 0x00}));
    BOOST_REQUIRE_EQUAL(configuration.first_version, 1);
    BOOST_REQUIRE_EQUAL(configuration.bip34_version, 2);
    BOOST_REQUIRE_EQUAL(configuration.bip66_version, 3);
    BOOST_REQUIRE_EQUAL(configuration.bip65_version, 4);
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit0, 1u);
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit1, 2u);
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_base, 0x20000000);
    BOOST_REQUIRE_EQUAL(configuration.activation_threshold, 51);
    BOOST_REQUIRE_EQUAL(configuration.enforcement_threshold, 75);
    BOOST_REQUIRE_EQUAL(configuration.activation_sample, 100);
    BOOST_REQUIRE_EQUAL(configuration.bip65_freeze, 581885);
    BOOST_REQUIRE_EQUAL(configuration.bip66_freeze, 330776);
    BOOST_REQUIRE_EQUAL(configuration.bip34_freeze, 21111);
    BOOST_REQUIRE_EQUAL(configuration.bip16_activation_time, 0x4f3af580);
    const config::checkpoint bip34_active(
        "0000000023b3a96d3484e5abb3755c413e7d41500f8e2a5c3f0dd01299cd8ef8",
        configuration.bip34_freeze);
    BOOST_REQUIRE_EQUAL(configuration.bip34_active_checkpoint, bip34_active);
    const config::checkpoint bit0_active(
        "00000000025e930139bac5c6c31a403776da130831ab85be56578f3fa75369bb",
        770112);
    BOOST_REQUIRE_EQUAL(configuration.bip9_bit0_active_checkpoint, bit0_active);
    const config::checkpoint bit1_active(
        "00000000002b980fcd729daaa248fd9316a5200e9b367f4ff2c42453e84201ca",
        834624);
    BOOST_REQUIRE_EQUAL(configuration.bip9_bit1_active_checkpoint, bit1_active);
    BOOST_REQUIRE_EQUAL(configuration.satoshi_per_bitcoin, 100000000);
    BOOST_REQUIRE_EQUAL(configuration.initial_block_subsidy_bitcoin, 50);
    BOOST_REQUIRE_EQUAL(configuration.recursive_money, 9999999989u);
    BOOST_REQUIRE_EQUAL(configuration.subsidy_interval, 210000);
    auto coin_subsidy = configuration.initial_block_subsidy_bitcoin;
    coin_subsidy = configuration.bitcoin_to_satoshi(coin_subsidy);
    const auto recursive_money = max_money_recursive(coin_subsidy);
    BOOST_REQUIRE_EQUAL(recursive_money, configuration.recursive_money);
    const auto satoshi_per_bitcoin = configuration.bitcoin_to_satoshi(1);
    BOOST_REQUIRE_EQUAL(satoshi_per_bitcoin, configuration.satoshi_per_bitcoin);
    BOOST_REQUIRE_EQUAL(configuration.max_money, 2099999997690000);
}

BOOST_AUTO_TEST_CASE(settings__construct__regtest_context__expected)
{
    settings configuration(config::settings::regtest);
    BOOST_REQUIRE_EQUAL(configuration.retargeting_factor, 4);
    BOOST_REQUIRE_EQUAL(configuration.target_spacing_seconds, 600);
    BOOST_REQUIRE_EQUAL(configuration.easy_spacing_seconds, 1200);
    BOOST_REQUIRE_EQUAL(configuration.timestamp_future_seconds, 7200);
    BOOST_REQUIRE_EQUAL(configuration.target_timespan_seconds, 1209600);
    BOOST_REQUIRE_EQUAL(configuration.proof_of_work_limit, 0x207fffff);
    BOOST_REQUIRE_EQUAL(configuration.minimum_timespan, 302400);
    BOOST_REQUIRE_EQUAL(configuration.maximum_timespan, 4838400);
    BOOST_REQUIRE_EQUAL(configuration.retargeting_interval, 2016);
    const chain::block genesis_block = configuration.genesis_block;
    BOOST_REQUIRE_EQUAL(genesis_block.to_data(), data_chunk({
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x3b, 0xa3, 0xed, 0xfd,
        0x7a, 0x7b, 0x12, 0xb2, 0x7a, 0xc7, 0x2c, 0x3e,
        0x67, 0x76, 0x8f, 0x61, 0x7f, 0xc8, 0x1b, 0xc3,
        0x88, 0x8a, 0x51, 0x32, 0x3a, 0x9f, 0xb8, 0xaa,
        0x4b, 0x1e, 0x5e, 0x4a, 0xda, 0xe5, 0x49, 0x4d,
        0xff, 0xff, 0x7f, 0x20, 0x02, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0xff, 0xff, 0x4d, 0x04, 0xff, 0xff, 0x00, 0x1d,
        0x01, 0x04, 0x45, 0x54, 0x68, 0x65, 0x20, 0x54,
        0x69, 0x6d, 0x65, 0x73, 0x20, 0x30, 0x33, 0x2f,
        0x4a, 0x61, 0x6e, 0x2f, 0x32, 0x30, 0x30, 0x39,
        0x20, 0x43, 0x68, 0x61, 0x6e, 0x63, 0x65, 0x6c,
        0x6c, 0x6f, 0x72, 0x20, 0x6f, 0x6e, 0x20, 0x62,
        0x72, 0x69, 0x6e, 0x6b, 0x20, 0x6f, 0x66, 0x20,
        0x73, 0x65, 0x63, 0x6f, 0x6e, 0x64, 0x20, 0x62,
        0x61, 0x69, 0x6c, 0x6f, 0x75, 0x74, 0x20, 0x66,
        0x6f, 0x72, 0x20, 0x62, 0x61, 0x6e, 0x6b, 0x73,
        0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0xf2, 0x05,
        0x2a, 0x01, 0x00, 0x00, 0x00, 0x43, 0x41, 0x04,
        0x67, 0x8a, 0xfd, 0xb0, 0xfe, 0x55, 0x48, 0x27,
        0x19, 0x67, 0xf1, 0xa6, 0x71, 0x30, 0xb7, 0x10,
        0x5c, 0xd6, 0xa8, 0x28, 0xe0, 0x39, 0x09, 0xa6,
        0x79, 0x62, 0xe0, 0xea, 0x1f, 0x61, 0xde, 0xb6,
        0x49, 0xf6, 0xbc, 0x3f, 0x4c, 0xef, 0x38, 0xc4,
        0xf3, 0x55, 0x04, 0xe5, 0x1e, 0xc1, 0x12, 0xde,
        0x5c, 0x38, 0x4d, 0xf7, 0xba, 0x0b, 0x8d, 0x57,
        0x8a, 0x4c, 0x70, 0x2b, 0x6b, 0xf1, 0x1d, 0x5f,
        0xac, 0x00, 0x00, 0x00, 0x00}));
    BOOST_REQUIRE_EQUAL(configuration.first_version, 1);
    BOOST_REQUIRE_EQUAL(configuration.bip34_version, 2);
    BOOST_REQUIRE_EQUAL(configuration.bip66_version, 3);
    BOOST_REQUIRE_EQUAL(configuration.bip65_version, 4);
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit0, 1u);
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit1, 2u);
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_base, 0x20000000);
    BOOST_REQUIRE_EQUAL(configuration.bip65_freeze, 1351);
    BOOST_REQUIRE_EQUAL(configuration.bip66_freeze, 1251);
    BOOST_REQUIRE_EQUAL(configuration.bip34_freeze, 0);
    BOOST_REQUIRE_EQUAL(configuration.bip16_activation_time, 0x4f3af580);
    const config::checkpoint genesis(genesis_block.hash(), 0);
    BOOST_REQUIRE_EQUAL(configuration.bip34_active_checkpoint, genesis);
    BOOST_REQUIRE_EQUAL(configuration.bip9_bit0_active_checkpoint, genesis);
    BOOST_REQUIRE_EQUAL(configuration.bip9_bit1_active_checkpoint, genesis);
    BOOST_REQUIRE_EQUAL(configuration.satoshi_per_bitcoin, 100000000);
    BOOST_REQUIRE_EQUAL(configuration.initial_block_subsidy_bitcoin, 50);
    BOOST_REQUIRE_EQUAL(configuration.recursive_money, 9999999989u);
    BOOST_REQUIRE_EQUAL(configuration.subsidy_interval, 150);
    auto coin_subsidy = configuration.initial_block_subsidy_bitcoin;
    coin_subsidy = configuration.bitcoin_to_satoshi(coin_subsidy);
    const auto recursive_money = max_money_recursive(coin_subsidy);
    BOOST_REQUIRE_EQUAL(recursive_money, configuration.recursive_money);
    const auto satoshi_per_bitcoin = configuration.bitcoin_to_satoshi(1);
    BOOST_REQUIRE_EQUAL(satoshi_per_bitcoin, configuration.satoshi_per_bitcoin);
    BOOST_REQUIRE_EQUAL(configuration.max_money, 1499999998350);
}

BOOST_AUTO_TEST_SUITE_END()

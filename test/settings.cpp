/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include "test.hpp"

BOOST_AUTO_TEST_SUITE(settings_tests)

const chain::checkpoint::list mainnet_checkpoints
{
    { "00000000000000004d9b4ef50f0f9d686fd69db2e03af35a100370c64632a983", 295000 },
    { "0000000000000001ae8c72a0b0c301f67e3afca10e819efa9041e458e9bd7e40", 279000 },
    { "000000000000003887df1f29024b06fc2200b55f8af8f35453d7be294df2d214", 250000 },
    { "00000000000001c108384350f74090433e7fcf79a606b8e797f065b130575932", 225430 },
    { "00000000000001b4f4b433e81ee46494af945cf96014816a4e2370f11b23df4e", 216116 },
    { "000000000000048b95347e83192f69cf0366076336c639f9b7228e9ba171342e", 210000 },
    { "000000000000059f452a5f7340de6682a977387c17010ff6e6c3bd83ca8b1317", 193000 },
    { "000000000000099e61ea72015e79632f216fe6cb33d7899acb35b75c8303b763", 168000 },
    { "00000000000005b12ffd4cd315cd34ffd4a594f430ac814c91184a0d42d2b0fe", 134444 },
    { "00000000000291ce28027faea320c8d2b054b2e0fe44a773f3eefb151d6bdc97", 105000 },
    { "0000000000573993a3c9e41ce34471c079dcf5f52a0e824a81e7f953b8661a20", 74000 },
    { "000000002dd5588a74784eaa7ab0507a18ad16a236e7b1ce69f00d7ddfb5d0a6", 33333 },
    { "0000000069e244f73d78e8fd29ba2fd2ed618bd6fa2ee92559f542fdb26e7c1d", 11111 }
};

// constructors
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(settings__construct__default_context__expected)
{
    settings configuration;
    BOOST_REQUIRE_EQUAL(configuration.block_spacing_seconds, 600u);
    BOOST_REQUIRE_EQUAL(configuration.timestamp_limit_seconds, 7200u);
    BOOST_REQUIRE_EQUAL(configuration.retargeting_interval_seconds, 1209600u);
    BOOST_REQUIRE_EQUAL(configuration.minimum_timespan(), 302400u);
    BOOST_REQUIRE_EQUAL(configuration.maximum_timespan(), 4838400u);
    BOOST_REQUIRE_EQUAL(configuration.retargeting_interval(), 2016u);
    BOOST_REQUIRE_EQUAL(configuration.first_version, 1u);
    BOOST_REQUIRE_EQUAL(configuration.bip34_version, 2u);
    BOOST_REQUIRE_EQUAL(configuration.bip66_version, 3u);
    BOOST_REQUIRE_EQUAL(configuration.bip65_version, 4u);
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit0, bit_right<uint32_t>(0));
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit1, bit_right<uint32_t>(1));
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit2, bit_right<uint32_t>(2));
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_base, 0x20000000u);
    BOOST_REQUIRE_EQUAL(configuration.initial_subsidy(), 5'000'000'000_u64);
    BOOST_REQUIRE_EQUAL(configuration.bitcoin_to_satoshi(1), 100000000u);
}

BOOST_AUTO_TEST_CASE(settings__construct__mainnet_context__expected)
{
    settings configuration(chain::selection::mainnet);
    BOOST_REQUIRE_EQUAL(configuration.block_spacing_seconds, 600u);
    BOOST_REQUIRE_EQUAL(configuration.timestamp_limit_seconds, 7200u);
    BOOST_REQUIRE_EQUAL(configuration.retargeting_interval_seconds, 1209600u);
    BOOST_REQUIRE_EQUAL(configuration.proof_of_work_limit, 486604799u);
    BOOST_REQUIRE_EQUAL(configuration.minimum_timespan(), 302400u);
    BOOST_REQUIRE_EQUAL(configuration.maximum_timespan(), 4838400u);
    BOOST_REQUIRE_EQUAL(configuration.retargeting_interval(), 2016u);
    const chain::block genesis_block = configuration.genesis_block;
    BOOST_REQUIRE_EQUAL(encode_base16(genesis_block.to_data(false)), "0100000000000000000000000000000000000000000000000000000000000000000000003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a29ab5f49ffff001d1dac2b7c0101000000010000000000000000000000000000000000000000000000000000000000000000ffffffff4d04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73ffffffff0100f2052a01000000434104678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5fac00000000");
    BOOST_REQUIRE_EQUAL(configuration.first_version, 1u);
    BOOST_REQUIRE_EQUAL(configuration.bip34_version, 2u);
    BOOST_REQUIRE_EQUAL(configuration.bip66_version, 3u);
    BOOST_REQUIRE_EQUAL(configuration.bip65_version, 4u);
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit0, bit_right<uint32_t>(0));
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit1, bit_right<uint32_t>(1));
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit2, bit_right<uint32_t>(2));
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_base, 0x20000000u);
    BOOST_REQUIRE_EQUAL(configuration.bip16_activation_time, 1333238400u);
    BOOST_REQUIRE_EQUAL(configuration.bip34_activation_threshold, 750u);
    BOOST_REQUIRE_EQUAL(configuration.bip34_enforcement_threshold, 950u);
    BOOST_REQUIRE_EQUAL(configuration.bip34_activation_sample, 1000u);
    BOOST_REQUIRE_EQUAL(configuration.bip90_bip34_height, 227931u);
    BOOST_REQUIRE_EQUAL(configuration.bip90_bip65_height, 388381u);
    BOOST_REQUIRE_EQUAL(configuration.bip90_bip66_height, 363725u);
    const chain::checkpoint bit0_active("000000000000000004a1b34462cb8aeebd5799177f7a29cf28f2d1961716b5b5", 419328u);
    BOOST_REQUIRE_EQUAL(configuration.bip9_bit0_active_checkpoint, bit0_active);
    const chain::checkpoint bit1_active("0000000000000000001c8018d9cb3b742ef25114f27563e3fc4a1902167f9893", 481824u);
    BOOST_REQUIRE_EQUAL(configuration.bip9_bit1_active_checkpoint, bit1_active);
    const chain::checkpoint bit2_active("0000000000000000000687bca986194dc2c1f949318629b44bb54ec0a94d8244", 709632);
    BOOST_REQUIRE_EQUAL(configuration.bip9_bit2_active_checkpoint, bit2_active);
    BOOST_REQUIRE_EQUAL(configuration.initial_subsidy_bitcoin, 50u);
    BOOST_REQUIRE_EQUAL(configuration.subsidy_interval_blocks, 210000u);
    BOOST_REQUIRE_EQUAL(configuration.bitcoin_to_satoshi(1), 100000000u);
    BOOST_REQUIRE_EQUAL(configuration.max_money(), 2099999997690000u);
    BOOST_REQUIRE_EQUAL(configuration.initial_subsidy(), 5'000'000'000_u64);
    BOOST_REQUIRE_EQUAL(configuration.checkpoints, mainnet_checkpoints);
    BOOST_REQUIRE_EQUAL(configuration.minimum_work, to_uintx(base16_hash("000000000000000000000000000000000000000052b2559353df4117b7348b64")));
    const chain::checkpoint milestone("000000000000000000010538edbfd2d5b809a33dd83f284aeea41c6d0d96968a", 900000u);
    BOOST_REQUIRE_EQUAL(configuration.milestone, milestone);
}

BOOST_AUTO_TEST_CASE(settings__construct__testnet_context__expected)
{
    const chain::checkpoint::list checkpoints
    {
        { "000000002a936ca763904c3c35fce2f3556c559c0214345d31b1bcebf76acb70", 546 }
    };

    settings configuration(chain::selection::testnet);
    BOOST_REQUIRE_EQUAL(configuration.block_spacing_seconds, 600u);
    BOOST_REQUIRE_EQUAL(configuration.timestamp_limit_seconds, 7200u);
    BOOST_REQUIRE_EQUAL(configuration.retargeting_interval_seconds, 1209600u);
    BOOST_REQUIRE_EQUAL(configuration.proof_of_work_limit, 486604799u);
    BOOST_REQUIRE_EQUAL(configuration.minimum_timespan(), 302400u);
    BOOST_REQUIRE_EQUAL(configuration.maximum_timespan(), 4838400u);
    BOOST_REQUIRE_EQUAL(configuration.retargeting_interval(), 2016u);
    const chain::block genesis_block = configuration.genesis_block;
    BOOST_REQUIRE_EQUAL(encode_base16(genesis_block.to_data(false)), "0100000000000000000000000000000000000000000000000000000000000000000000003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4adae5494dffff001d1aa4ae180101000000010000000000000000000000000000000000000000000000000000000000000000ffffffff4d04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73ffffffff0100f2052a01000000434104678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5fac00000000");
    BOOST_REQUIRE_EQUAL(configuration.first_version, 1u);
    BOOST_REQUIRE_EQUAL(configuration.bip34_version, 2u);
    BOOST_REQUIRE_EQUAL(configuration.bip66_version, 3u);
    BOOST_REQUIRE_EQUAL(configuration.bip65_version, 4u);
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit0, bit_right<uint32_t>(0));
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit1, bit_right<uint32_t>(1));
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit2, bit_right<uint32_t>(2));
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_base, 0x20000000u);
    BOOST_REQUIRE_EQUAL(configuration.bip16_activation_time, 1329264000u);
    BOOST_REQUIRE_EQUAL(configuration.bip34_activation_threshold, 51u);
    BOOST_REQUIRE_EQUAL(configuration.bip34_enforcement_threshold, 75u);
    BOOST_REQUIRE_EQUAL(configuration.bip34_activation_sample, 100u);
    BOOST_REQUIRE_EQUAL(configuration.bip90_bip34_height, 21111u);
    BOOST_REQUIRE_EQUAL(configuration.bip90_bip65_height, 581885u);
    BOOST_REQUIRE_EQUAL(configuration.bip90_bip66_height, 330776u);
    const chain::checkpoint bit0_active("00000000025e930139bac5c6c31a403776da130831ab85be56578f3fa75369bb", 770112u);
    BOOST_REQUIRE_EQUAL(configuration.bip9_bit0_active_checkpoint, bit0_active);
    const chain::checkpoint bit1_active("00000000002b980fcd729daaa248fd9316a5200e9b367f4ff2c42453e84201ca", 834624u);
    BOOST_REQUIRE_EQUAL(configuration.bip9_bit1_active_checkpoint, bit1_active);
    ////const chain::checkpoint bit2_active("0000000000000000000687bca986194dc2c1f949318629b44bb54ec0a94d8244", 709632);
    ////BOOST_REQUIRE_EQUAL(configuration.bip9_bit2_active_checkpoint, bit2_active);
    BOOST_REQUIRE_EQUAL(configuration.initial_subsidy_bitcoin, 50u);
    BOOST_REQUIRE_EQUAL(configuration.subsidy_interval_blocks, 210000u);
    BOOST_REQUIRE_EQUAL(configuration.bitcoin_to_satoshi(1), 100000000u);
    BOOST_REQUIRE_EQUAL(configuration.max_money(), 2099999997690000u);
    BOOST_REQUIRE_EQUAL(configuration.initial_subsidy(), 5'000'000'000_u64);
    BOOST_REQUIRE_EQUAL(configuration.checkpoints, checkpoints);
    BOOST_REQUIRE_EQUAL(configuration.minimum_work, to_uintx(base16_hash("000000000000000000000000000000000000000000000b6a51f415a67c0da307")));
    const chain::checkpoint milestone("0000000000000093bcb68c03a9a168ae252572d348a2eaeba2cdf9231d73206f", 2500000u);
    BOOST_REQUIRE_EQUAL(configuration.milestone, milestone);
}

BOOST_AUTO_TEST_CASE(settings__construct__regtest_context__expected)
{
    settings configuration(chain::selection::regtest);
    BOOST_REQUIRE_EQUAL(configuration.block_spacing_seconds, 600u);
    BOOST_REQUIRE_EQUAL(configuration.timestamp_limit_seconds, 7200u);
    BOOST_REQUIRE_EQUAL(configuration.retargeting_interval_seconds, 1209600u);
    BOOST_REQUIRE_EQUAL(configuration.proof_of_work_limit, 545259519u);
    BOOST_REQUIRE_EQUAL(configuration.minimum_timespan(), 302400u);
    BOOST_REQUIRE_EQUAL(configuration.maximum_timespan(), 4838400u);
    BOOST_REQUIRE_EQUAL(configuration.retargeting_interval(), 2016u);
    const chain::block genesis_block = configuration.genesis_block;
    BOOST_REQUIRE_EQUAL(encode_base16(genesis_block.to_data(false)), "0100000000000000000000000000000000000000000000000000000000000000000000003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4adae5494dffff7f20020000000101000000010000000000000000000000000000000000000000000000000000000000000000ffffffff4d04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73ffffffff0100f2052a01000000434104678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5fac00000000");
    BOOST_REQUIRE_EQUAL(configuration.first_version, 1u);
    BOOST_REQUIRE_EQUAL(configuration.bip34_version, 2u);
    BOOST_REQUIRE_EQUAL(configuration.bip66_version, 3u);
    BOOST_REQUIRE_EQUAL(configuration.bip65_version, 4u);
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit0, bit_right<uint32_t>(0));
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit1, bit_right<uint32_t>(1));
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_bit2, bit_right<uint32_t>(2));
    BOOST_REQUIRE_EQUAL(configuration.bip9_version_base, 0x20000000u);
    BOOST_REQUIRE_EQUAL(configuration.bip16_activation_time, 1329264000u);
    BOOST_REQUIRE_EQUAL(configuration.bip90_bip34_height, 100000000u);
    BOOST_REQUIRE_EQUAL(configuration.bip90_bip65_height, 1351u);
    BOOST_REQUIRE_EQUAL(configuration.bip90_bip66_height, 1251u);
    const chain::checkpoint genesis(genesis_block.hash(), 0u);
    BOOST_REQUIRE_EQUAL(configuration.bip9_bit0_active_checkpoint, genesis);
    BOOST_REQUIRE_EQUAL(configuration.bip9_bit1_active_checkpoint, genesis);
    BOOST_REQUIRE_EQUAL(configuration.bip9_bit2_active_checkpoint, genesis);
    BOOST_REQUIRE_EQUAL(configuration.initial_subsidy_bitcoin, 50u);
    BOOST_REQUIRE_EQUAL(configuration.subsidy_interval_blocks, 150u);
    BOOST_REQUIRE_EQUAL(configuration.bitcoin_to_satoshi(1), 100000000u);
    BOOST_REQUIRE_EQUAL(configuration.max_money(), 1499999998350u);
    BOOST_REQUIRE_EQUAL(configuration.initial_subsidy(), 5'000'000'000_u64);
    BOOST_REQUIRE(configuration.checkpoints.empty());
    BOOST_REQUIRE_EQUAL(configuration.minimum_work, to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000000000")));
    BOOST_REQUIRE_EQUAL(configuration.milestone, genesis);
}

// methods
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(settings__initial_block_subsidy_bitcoin__set_double_value__max_money_doubled)
{
    settings configuration(chain::selection::mainnet);
    const auto double_subsidy = configuration.initial_subsidy_bitcoin * 2;
    const auto double_subsidy_satoshi = configuration.bitcoin_to_satoshi(double_subsidy);
    const auto double_max_money = configuration.max_money() + double_subsidy_satoshi * configuration.subsidy_interval_blocks;
    configuration.initial_subsidy_bitcoin = double_subsidy;
    BOOST_REQUIRE_EQUAL(configuration.max_money(), double_max_money);
}

BOOST_AUTO_TEST_CASE(settings__subsidy_interval__set_double_value__max_money_doubled)
{
    settings configuration(chain::selection::mainnet);
    const auto double_subsidy_interval = configuration.subsidy_interval_blocks * 2;
    const auto double_max_money = configuration.max_money() * 2;
    configuration.subsidy_interval_blocks = double_subsidy_interval;
    BOOST_REQUIRE_EQUAL(configuration.max_money(), double_max_money);
}

BOOST_AUTO_TEST_CASE(settings__retargeting_factor__set_double_value__timespan_limits_scaled)
{
    settings configuration(chain::selection::mainnet);
    const auto double_maximum_timespan = configuration.maximum_timespan() * 2;
    const auto half_minimum_timespan = configuration.minimum_timespan() / 2;
    configuration.retargeting_factor = 8;
    BOOST_REQUIRE_EQUAL(configuration.maximum_timespan(), double_maximum_timespan);
    BOOST_REQUIRE_EQUAL(configuration.minimum_timespan(), half_minimum_timespan);
}

BOOST_AUTO_TEST_CASE(settings__retargeting_interval_seconds__set_double_value__timespan_limits_scaled)
{
    settings configuration(chain::selection::mainnet);
    const auto double_interval_seconds = configuration.retargeting_interval_seconds * 2;
    const auto double_maximum_timespan = configuration.maximum_timespan() * 2;
    const auto double_minimum_timespan = configuration.minimum_timespan() * 2;
    const auto double_retargeting_interval = configuration.retargeting_interval() * 2;
    configuration.retargeting_interval_seconds = double_interval_seconds;
    BOOST_REQUIRE_EQUAL(configuration.maximum_timespan(), double_maximum_timespan);
    BOOST_REQUIRE_EQUAL(configuration.minimum_timespan(), double_minimum_timespan);
    BOOST_REQUIRE_EQUAL(configuration.retargeting_interval(), double_retargeting_interval);
}

BOOST_AUTO_TEST_CASE(settings__block_spacing_seconds__set_double_value__retargeting_interval_halved)
{
    settings configuration(chain::selection::mainnet);
    const auto double_block_spacing_seconds = configuration.block_spacing_seconds * 2;
    const auto half_retargeting_interval = configuration.retargeting_interval() / 2;
    configuration.block_spacing_seconds = double_block_spacing_seconds;
    BOOST_REQUIRE_EQUAL(configuration.retargeting_interval(), half_retargeting_interval);
}

BOOST_AUTO_TEST_CASE(settings__sorted_checkpoints__testnet_empty__empty)
{
    settings configuration(chain::selection::testnet);
    configuration.checkpoints.clear();
    BOOST_REQUIRE(configuration.sorted_checkpoints().empty());
}

BOOST_AUTO_TEST_CASE(settings__sorted_checkpoints__mainnet_default_expected)
{
    const settings configuration(chain::selection::mainnet);
    BOOST_REQUIRE_EQUAL(configuration.checkpoints, mainnet_checkpoints);
    BOOST_REQUIRE_EQUAL(configuration.sorted_checkpoints().back(), mainnet_checkpoints.front());
    BOOST_REQUIRE_EQUAL(configuration.sorted_checkpoints().back().height(), 295000_size);
    BOOST_REQUIRE_EQUAL(configuration.sorted_checkpoints().front(), mainnet_checkpoints.back());
    BOOST_REQUIRE_EQUAL(configuration.sorted_checkpoints().front().height(), 11111_size);
}

BOOST_AUTO_TEST_CASE(settings__top_checkpoint__testnet_empty__genesis)
{
    settings configuration(chain::selection::testnet);
    configuration.checkpoints.clear();
    const chain::checkpoint genesis{ configuration.genesis_block.hash(), zero };
    BOOST_REQUIRE_EQUAL(configuration.top_checkpoint(), genesis);
}

BOOST_AUTO_TEST_CASE(settings__sorted_checkpoint__mainnet_default_expected)
{
    const settings configuration(chain::selection::mainnet);
    BOOST_REQUIRE_EQUAL(configuration.checkpoints, mainnet_checkpoints);
    BOOST_REQUIRE_EQUAL(configuration.sorted_checkpoints().back(), mainnet_checkpoints.front());
    BOOST_REQUIRE_EQUAL(configuration.sorted_checkpoints().back().height(), 295000_size);
    BOOST_REQUIRE_EQUAL(configuration.top_checkpoint(), mainnet_checkpoints.front());
}

BOOST_AUTO_TEST_SUITE_END()

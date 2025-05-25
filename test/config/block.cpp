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
#include "../test.hpp"
#include <sstream>

BOOST_AUTO_TEST_SUITE(config_block_tests)

using namespace bc::system::config;

static const std::string encoded_genesis_block =
    "01000000"
    "0000000000000000000000000000000000000000000000000000000000000000"
    "3ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a"
    "29ab5f49"
    "ffff001d"
    "1dac2b7c"
    "01"
    "01000000"
    "01"
    "0000000000000000000000000000000000000000000000000000000000000000ffffffff"
    "4d"
    "04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73"
    "ffffffff"
    "01"
    "00f2052a01000000"
    "43"
    "4104678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5fac"
    "00000000";

static const config::block genesis_config()
{
    return { encoded_genesis_block };
};

static const chain::block genesis_chain()
{
    return { genesis_config() };
};

BOOST_AUTO_TEST_CASE(block__construct__default)
{
    const block block{};
    BOOST_REQUIRE_EQUAL(block.to_string(), std::string(162, '0'));
}

BOOST_AUTO_TEST_CASE(block__construct__copy__expected)
{
    const block block(genesis_config());
    BOOST_REQUIRE_EQUAL(block, genesis_config());
}

BOOST_AUTO_TEST_CASE(block__construct__move__expected)
{
    auto genesis = genesis_config();
    const block block(std::move(genesis));
    BOOST_REQUIRE_EQUAL(block, genesis_config());
}

BOOST_AUTO_TEST_CASE(block__construct__chain_copy__expected)
{
    const block block(genesis_chain());
    BOOST_REQUIRE_EQUAL(block, genesis_config());
}

BOOST_AUTO_TEST_CASE(block__construct__chain_move__expected)
{
    chain::block genesis = genesis_config();
    const block block(std::move(genesis));
    BOOST_REQUIRE_EQUAL(block, genesis_config());
}

BOOST_AUTO_TEST_CASE(block__construct__string__expected)
{
    const block block(encoded_genesis_block);
    BOOST_REQUIRE_EQUAL(block.to_string(), encoded_genesis_block);
}

BOOST_AUTO_TEST_CASE(block__assign__copy__expected)
{
    block block{};
    block = genesis_config();
    BOOST_REQUIRE_EQUAL(block, genesis_config());
}

BOOST_AUTO_TEST_CASE(block__assign__move__expected)
{
    auto genesis = genesis_config();
    block block{};
    block = std::move(genesis);
    BOOST_REQUIRE_EQUAL(block, genesis_config());
}

BOOST_AUTO_TEST_CASE(block__assign__chain_copy__expected)
{
    block block{};
    block = genesis_chain();
    BOOST_REQUIRE_EQUAL(block, genesis_config());
}

BOOST_AUTO_TEST_CASE(block__assign__chain_move__expected)
{
    auto genesis = genesis_chain();
    block block{};
    block = std::move(genesis);
    BOOST_REQUIRE_EQUAL(block, genesis_config());
}

BOOST_AUTO_TEST_CASE(block__equality__equal__true)
{
    const block block{ genesis_config() };
    BOOST_REQUIRE(block == genesis_config());
}

BOOST_AUTO_TEST_CASE(block__equality__not_equal__false)
{
    const block block{};
    BOOST_REQUIRE(!(block == genesis_config()));
}

BOOST_AUTO_TEST_CASE(block__inequality__equal__false)
{
    const block block{ genesis_config() };
    BOOST_REQUIRE(!(block != genesis_config()));
}

BOOST_AUTO_TEST_CASE(block__inequality__not_equal__true)
{
    const block block{};
    BOOST_REQUIRE(block != genesis_config());
}

BOOST_AUTO_TEST_CASE(block__equality__chain__expected)
{
    const block block{};
    BOOST_REQUIRE(block != genesis_chain());
}

BOOST_AUTO_TEST_CASE(block__istream__populated__expected)
{
    block deserialized{};
    std::stringstream serialized(encoded_genesis_block);
    serialized >> deserialized;
    BOOST_REQUIRE_EQUAL(deserialized.to_string(), encoded_genesis_block);
}

BOOST_AUTO_TEST_CASE(block__ostream__empty__expected)
{
    std::stringstream serialized;
    serialized << block();
    BOOST_REQUIRE_EQUAL(serialized.str(), std::string(162, '0'));
}

BOOST_AUTO_TEST_CASE(block__ostream__populated__expected)
{
    std::stringstream serialized;
    serialized << genesis_config();
    BOOST_REQUIRE_EQUAL(serialized.str(), encoded_genesis_block);
}

BOOST_AUTO_TEST_SUITE_END()

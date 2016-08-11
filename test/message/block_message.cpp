/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#include <boost/iostreams/stream.hpp>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(block_message_tests)

BOOST_AUTO_TEST_CASE(from_data_fails)
{
    data_chunk data(10);
    message::block_message instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(message::version::level::minimum, data));
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(roundtrip_mainnet_genesis_block_serialization_factory_data_chunk)
{
    const auto genesis = bc::message::block_message::genesis_mainnet();
    BOOST_REQUIRE_EQUAL(genesis.serialized_size(message::version::level::minimum), 285u);
    BOOST_REQUIRE_EQUAL(genesis.header.serialized_size(false), 80u);

    // Save genesis block.
    auto raw_block_message = genesis.to_data();
    BOOST_REQUIRE_EQUAL(std::distance(raw_block_message.begin(), raw_block_message.end()), 285u);
    BOOST_REQUIRE_EQUAL(raw_block_message.size(), 285u);

    // Reload genesis block.
    const auto block = message::block_message::factory_from_data(
        message::version::level::minimum, raw_block_message);

    BOOST_REQUIRE(block.is_valid());
    BOOST_REQUIRE(genesis.header.version == block.header.version);
    BOOST_REQUIRE(genesis.header.previous_block_hash == block.header.previous_block_hash);
    BOOST_REQUIRE(genesis.header.merkle == block.header.merkle);
    BOOST_REQUIRE(genesis.header.timestamp == block.header.timestamp);
    BOOST_REQUIRE(genesis.header.bits == block.header.bits);
    BOOST_REQUIRE(genesis.header.nonce == block.header.nonce);
    BOOST_REQUIRE(genesis.header == block.header);
    BOOST_REQUIRE_EQUAL(message::version::level::minimum, block.originator());

    // Verify merkle root from transactions.
    const auto merkle = message::block_message::generate_merkle_root(block.transactions);
    BOOST_REQUIRE(genesis.header.merkle == merkle);
}

BOOST_AUTO_TEST_CASE(roundtrip_mainnet_genesis_block_serialization_factory_stream)
{
    const auto genesis = bc::message::block_message::genesis_mainnet();
    BOOST_REQUIRE_EQUAL(genesis.serialized_size(), 285u);
    BOOST_REQUIRE_EQUAL(genesis.header.serialized_size(false), 80u);

    // Save genesis block.
    auto raw_block_message = genesis.to_data();
    BOOST_REQUIRE_EQUAL(std::distance(raw_block_message.begin(), raw_block_message.end()), 285u);
    BOOST_REQUIRE_EQUAL(raw_block_message.size(), 285u);

    // Reload genesis block.
    data_source stream(raw_block_message);
    const auto block = message::block_message::factory_from_data(
        message::version::level::minimum, stream);

    BOOST_REQUIRE(block.is_valid());
    BOOST_REQUIRE(genesis.header.version == block.header.version);
    BOOST_REQUIRE(genesis.header.previous_block_hash == block.header.previous_block_hash);
    BOOST_REQUIRE(genesis.header.merkle == block.header.merkle);
    BOOST_REQUIRE(genesis.header.timestamp == block.header.timestamp);
    BOOST_REQUIRE(genesis.header.bits == block.header.bits);
    BOOST_REQUIRE(genesis.header.nonce == block.header.nonce);
    BOOST_REQUIRE(genesis.header == block.header);
    BOOST_REQUIRE_EQUAL(message::version::level::minimum, block.originator());

    // Verify merkle root from transactions.
    const auto merkle = message::block_message::generate_merkle_root(block.transactions);
    BOOST_REQUIRE(genesis.header.merkle == merkle);
}

BOOST_AUTO_TEST_CASE(roundtrip_mainnet_genesis_block_serialization_factory_reader)
{
    const auto genesis = bc::message::block_message::genesis_mainnet();
    BOOST_REQUIRE_EQUAL(genesis.serialized_size(), 285u);
    BOOST_REQUIRE_EQUAL(genesis.header.serialized_size(false), 80u);

    // Save genesis block.
    data_chunk raw_block_message = genesis.to_data();
    BOOST_REQUIRE_EQUAL(std::distance(raw_block_message.begin(), raw_block_message.end()), 285u);
    BOOST_REQUIRE_EQUAL(raw_block_message.size(), 285u);

    // Reload genesis block.
    data_source stream(raw_block_message);
    istream_reader reader(stream);
    const auto block = message::block_message::factory_from_data(
        message::version::level::minimum + 1, reader);

    BOOST_REQUIRE(block.is_valid());
    BOOST_REQUIRE(genesis.header.version == block.header.version);
    BOOST_REQUIRE(genesis.header.previous_block_hash == block.header.previous_block_hash);
    BOOST_REQUIRE(genesis.header.merkle == block.header.merkle);
    BOOST_REQUIRE(genesis.header.timestamp == block.header.timestamp);
    BOOST_REQUIRE(genesis.header.bits == block.header.bits);
    BOOST_REQUIRE(genesis.header.nonce == block.header.nonce);
    BOOST_REQUIRE(genesis.header == block.header);
    BOOST_REQUIRE_EQUAL(message::version::level::minimum + 1, block.originator());

    // Verify merkle root from transactions.
    const auto merkle = message::block_message::generate_merkle_root(block.transactions);
    BOOST_REQUIRE(genesis.header.merkle == merkle);
}

BOOST_AUTO_TEST_SUITE_END()

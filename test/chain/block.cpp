/*
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/iostreams/stream.hpp>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>
#include "../genesis_block.hpp"

using namespace bc;

BOOST_AUTO_TEST_SUITE(block_tests)

BOOST_AUTO_TEST_CASE(from_data_fails)
{
    data_chunk data(10);

    chain::block instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(data));
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(roundtrip_genesis_block_serialization_factory_data_chunk)
{
    chain::block genblk = genesis_block();
    BOOST_REQUIRE_EQUAL(genblk.satoshi_size(), 285u);
    BOOST_REQUIRE_EQUAL(genblk.header.satoshi_size(), 80u);

    // Save genesis block.
    data_chunk rawblk = genblk.to_data();
    BOOST_REQUIRE_EQUAL(std::distance(rawblk.begin(), rawblk.end()), 285u);
    BOOST_REQUIRE_EQUAL(rawblk.size(), 285u);

    // Reload genesis block.
    chain::block blk = chain::block::factory_from_data(rawblk);

    BOOST_REQUIRE(blk.is_valid());
    BOOST_REQUIRE(genblk.header.version == blk.header.version);
    BOOST_REQUIRE(genblk.header.previous_block_hash == blk.header.previous_block_hash);
    BOOST_REQUIRE(genblk.header.merkle == blk.header.merkle);
    BOOST_REQUIRE(genblk.header.timestamp == blk.header.timestamp);
    BOOST_REQUIRE(genblk.header.bits == blk.header.bits);
    BOOST_REQUIRE(genblk.header.nonce == blk.header.nonce);
    BOOST_REQUIRE(genblk.header == blk.header);

    // Verify merkle root from transactions.
    const hash_digest& merkle = chain::block::generate_merkle_root(blk.transactions);
    BOOST_REQUIRE(genblk.header.merkle == merkle);
}

BOOST_AUTO_TEST_CASE(roundtrip_genesis_block_serialization_factory_stream)
{
    chain::block genblk = genesis_block();
    BOOST_REQUIRE_EQUAL(genblk.satoshi_size(), 285u);
    BOOST_REQUIRE_EQUAL(genblk.header.satoshi_size(), 80u);

    // Save genesis block.
    data_chunk rawblk = genblk.to_data();
    BOOST_REQUIRE_EQUAL(std::distance(rawblk.begin(), rawblk.end()), 285u);
    BOOST_REQUIRE_EQUAL(rawblk.size(), 285u);

    // Reload genesis block.
    boost::iostreams::stream<byte_source<data_chunk>> stream(rawblk);
    chain::block blk = chain::block::factory_from_data(stream);

    BOOST_REQUIRE(blk.is_valid());
    BOOST_REQUIRE(genblk.header.version == blk.header.version);
    BOOST_REQUIRE(genblk.header.previous_block_hash == blk.header.previous_block_hash);
    BOOST_REQUIRE(genblk.header.merkle == blk.header.merkle);
    BOOST_REQUIRE(genblk.header.timestamp == blk.header.timestamp);
    BOOST_REQUIRE(genblk.header.bits == blk.header.bits);
    BOOST_REQUIRE(genblk.header.nonce == blk.header.nonce);
    BOOST_REQUIRE(genblk.header == blk.header);

    // Verify merkle root from transactions.
    const hash_digest& merkle = chain::block::generate_merkle_root(blk.transactions);
    BOOST_REQUIRE(genblk.header.merkle == merkle);
}

BOOST_AUTO_TEST_CASE(roundtrip_genesis_block_serialization_factory_reader)
{
    chain::block genblk = genesis_block();
    BOOST_REQUIRE_EQUAL(genblk.satoshi_size(), 285u);
    BOOST_REQUIRE_EQUAL(genblk.header.satoshi_size(), 80u);

    // Save genesis block.
    data_chunk rawblk = genblk.to_data();
    BOOST_REQUIRE_EQUAL(std::distance(rawblk.begin(), rawblk.end()), 285u);
    BOOST_REQUIRE_EQUAL(rawblk.size(), 285u);

    // Reload genesis block.
    boost::iostreams::stream<byte_source<data_chunk>> stream(rawblk);
    istream_reader reader(stream);
    chain::block blk = chain::block::factory_from_data(reader);

    BOOST_REQUIRE(blk.is_valid());
    BOOST_REQUIRE(genblk.header.version == blk.header.version);
    BOOST_REQUIRE(genblk.header.previous_block_hash == blk.header.previous_block_hash);
    BOOST_REQUIRE(genblk.header.merkle == blk.header.merkle);
    BOOST_REQUIRE(genblk.header.timestamp == blk.header.timestamp);
    BOOST_REQUIRE(genblk.header.bits == blk.header.bits);
    BOOST_REQUIRE(genblk.header.nonce == blk.header.nonce);
    BOOST_REQUIRE(genblk.header == blk.header);

    // Verify merkle root from transactions.
    const hash_digest& merkle = chain::block::generate_merkle_root(blk.transactions);
    BOOST_REQUIRE(genblk.header.merkle == merkle);
}

BOOST_AUTO_TEST_SUITE_END()

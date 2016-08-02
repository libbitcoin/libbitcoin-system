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
#include <boost/test/unit_test.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

bool operator==(const message::compact_block& left,
    const message::compact_block& right)
{
    auto raw_left = left.to_data();
    auto raw_right = right.to_data();
    return (raw_left == raw_right);
}

BOOST_AUTO_TEST_SUITE(compact_block_tests)

BOOST_AUTO_TEST_CASE(from_data_insufficient_bytes_failure)
{
    const data_chunk raw{ 0xab, 0xcd };
    message::compact_block instance{};
    BOOST_REQUIRE_EQUAL(false, instance.from_data(raw));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk)
{
    message::compact_block expected;
    expected.header.version = 10;
    expected.header.previous_block_hash = hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
    expected.header.merkle = hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    expected.header.timestamp = 531234;
    expected.header.bits = 6523454;
    expected.header.nonce = 68644;
    expected.header.transaction_count = 0;
    expected.nonce = 1234;
    expected.short_ids.push_back(base16_literal("000000121212"));
    expected.short_ids.push_back(base16_literal("121212343434"));
    expected.short_ids.push_back(base16_literal("343434565656"));
    expected.short_ids.push_back(base16_literal("565678789a9a"));
    expected.transactions.emplace_back();
    BOOST_REQUIRE(expected.transactions.back().from_data(to_chunk(base16_literal(
        "0100000001000000000000000000000000000000000000000000000000000000"
        "0000000000ffffffff2c037f75061f4d696e656420627920416e74506f6f6c20"
        "75736131202e78f5702057a05792943d0000e1320100ffffffff0186068c4a00"
        "0000001976a9142ef12bd2ac1416406d0e132e5bc8d0b02df3861b88ac000000"
        "00"))));
    expected.transactions.emplace_back();
    BOOST_REQUIRE(expected.transactions.back().from_data(to_chunk(base16_literal(
        "0100000001000000000000000000000000000000000000000000000000000000"
        "0000000000ffffffff3f037a75060004d34ba05704ea24981408ebbc9eae69e2"
        "aaae0a425720537570706f727420384d200a666973686572206a696e78696e09"
        "2f425720506f6f6c2fffffffff010c316b4d000000001976a914721afdf638d5"
        "70285d02d3076d8be6a03ee0794d88ac00000000"))));

    const auto data = expected.to_data();
    const auto result = message::compact_block::factory_from_data(data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size());
    BOOST_REQUIRE_EQUAL(expected.serialized_size(), result.serialized_size());
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream)
{
    message::compact_block expected;
    expected.header.version = 10;
    expected.header.previous_block_hash = hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
    expected.header.merkle = hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    expected.header.timestamp = 531234;
    expected.header.bits = 6523454;
    expected.header.nonce = 68644;
    expected.header.transaction_count = 0;
    expected.nonce = 1234;
    expected.short_ids.push_back(base16_literal("000000121212"));
    expected.short_ids.push_back(base16_literal("121212343434"));
    expected.short_ids.push_back(base16_literal("343434565656"));
    expected.short_ids.push_back(base16_literal("565678789a9a"));
    expected.transactions.emplace_back();
    BOOST_REQUIRE(expected.transactions.back().from_data(to_chunk(base16_literal(
        "0100000001000000000000000000000000000000000000000000000000000000"
        "0000000000ffffffff2c037f75061f4d696e656420627920416e74506f6f6c20"
        "75736131202e78f5702057a05792943d0000e1320100ffffffff0186068c4a00"
        "0000001976a9142ef12bd2ac1416406d0e132e5bc8d0b02df3861b88ac000000"
        "00"))));
    expected.transactions.emplace_back();
    BOOST_REQUIRE(expected.transactions.back().from_data(to_chunk(base16_literal(
        "0100000001000000000000000000000000000000000000000000000000000000"
        "0000000000ffffffff3f037a75060004d34ba05704ea24981408ebbc9eae69e2"
        "aaae0a425720537570706f727420384d200a666973686572206a696e78696e09"
        "2f425720506f6f6c2fffffffff010c316b4d000000001976a914721afdf638d5"
        "70285d02d3076d8be6a03ee0794d88ac00000000"))));

    const auto data = expected.to_data();
    data_source istream(data);
    auto result = message::compact_block::factory_from_data(istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size());
    BOOST_REQUIRE_EQUAL(expected.serialized_size(), result.serialized_size());
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader)
{
    message::compact_block expected;
    expected.header.version = 10;
    expected.header.previous_block_hash = hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
    expected.header.merkle = hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    expected.header.timestamp = 531234;
    expected.header.bits = 6523454;
    expected.header.nonce = 68644;
    expected.header.transaction_count = 0;
    expected.nonce = 1234;
    expected.short_ids.push_back(base16_literal("000000121212"));
    expected.short_ids.push_back(base16_literal("121212343434"));
    expected.short_ids.push_back(base16_literal("343434565656"));
    expected.short_ids.push_back(base16_literal("565678789a9a"));
    expected.transactions.emplace_back();
    BOOST_REQUIRE(expected.transactions.back().from_data(to_chunk(base16_literal(
        "0100000001000000000000000000000000000000000000000000000000000000"
        "0000000000ffffffff2c037f75061f4d696e656420627920416e74506f6f6c20"
        "75736131202e78f5702057a05792943d0000e1320100ffffffff0186068c4a00"
        "0000001976a9142ef12bd2ac1416406d0e132e5bc8d0b02df3861b88ac000000"
        "00"))));
    expected.transactions.emplace_back();
    BOOST_REQUIRE(expected.transactions.back().from_data(to_chunk(base16_literal(
        "0100000001000000000000000000000000000000000000000000000000000000"
        "0000000000ffffffff3f037a75060004d34ba05704ea24981408ebbc9eae69e2"
        "aaae0a425720537570706f727420384d200a666973686572206a696e78696e09"
        "2f425720506f6f6c2fffffffff010c316b4d000000001976a914721afdf638d5"
        "70285d02d3076d8be6a03ee0794d88ac00000000"))));

    const auto data = expected.to_data();
    data_source istream(data);
    istream_reader source(istream);
    const auto result = message::compact_block::factory_from_data(source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size());
    BOOST_REQUIRE_EQUAL(expected.serialized_size(), result.serialized_size());
}

BOOST_AUTO_TEST_SUITE_END()

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
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(header_message_tests)

BOOST_AUTO_TEST_CASE(header_message__constructor_1__always__initialized_invalid)
{
    message::header_message instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(header_message__constructor_2__always__equals_params)
{
    uint32_t version = 10u;
    hash_digest previous = hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
    hash_digest merkle = hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    uint32_t timestamp = 531234u;
    uint32_t bits = 6523454u;
    uint32_t nonce = 68644u;
    uint64_t originator = 2343u;

    message::header_message instance(version, previous, merkle, timestamp,
        bits, nonce, originator);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(version, instance.version());
    BOOST_REQUIRE_EQUAL(timestamp, instance.timestamp());
    BOOST_REQUIRE_EQUAL(bits, instance.bits());
    BOOST_REQUIRE_EQUAL(nonce, instance.nonce());
    BOOST_REQUIRE_EQUAL(originator, instance.originator());
    BOOST_REQUIRE(previous == instance.previous_block_hash());
    BOOST_REQUIRE(merkle == instance.merkle());
}

BOOST_AUTO_TEST_CASE(header_message__constructor_3__always__equals_params)
{
    uint32_t version = 10u;
    hash_digest previous = hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
    hash_digest merkle = hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    uint32_t timestamp = 531234u;
    uint32_t bits = 6523454u;
    uint32_t nonce = 68644u;
    uint64_t originator = 2343u;

    message::header_message instance(version, std::move(previous), std::move(merkle),
        timestamp, bits, nonce, originator);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(version, instance.version());
    BOOST_REQUIRE_EQUAL(timestamp, instance.timestamp());
    BOOST_REQUIRE_EQUAL(bits, instance.bits());
    BOOST_REQUIRE_EQUAL(nonce, instance.nonce());
    BOOST_REQUIRE_EQUAL(originator, instance.originator());
    BOOST_REQUIRE(previous == instance.previous_block_hash());
    BOOST_REQUIRE(merkle == instance.merkle());
}

BOOST_AUTO_TEST_CASE(header_message__constructor_4__always__equals_params)
{
    const chain::header expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        1234u);

    message::header_message instance(expected);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected == instance);
    BOOST_REQUIRE_EQUAL(0u, instance.originator());
}

BOOST_AUTO_TEST_CASE(header_message__constructor_5__always__equals_params)
{
    chain::header expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        123u);

    message::header_message instance(std::move(expected));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected == instance);
    BOOST_REQUIRE_EQUAL(0u, instance.originator());
}

BOOST_AUTO_TEST_CASE(header_message__constructor_6__always__equals_params)
{
    const message::header_message expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u,
        5555u);

    message::header_message instance(expected);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected == instance);
    BOOST_REQUIRE_EQUAL(expected.originator(), instance.originator());
}

BOOST_AUTO_TEST_CASE(header_message__constructor_7__always__equals_params)
{
    message::header_message expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u,
        4444u);

    message::header_message instance(std::move(expected));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected == instance);
    BOOST_REQUIRE_EQUAL(expected.originator(), instance.originator());
}

BOOST_AUTO_TEST_CASE(header_message__from_data__insufficient_bytes__failure)
{
    data_chunk data(10);
    message::header_message header;
    BOOST_REQUIRE_EQUAL(false, header.from_data(
        message::header_message::version_maximum, data));
    BOOST_REQUIRE_EQUAL(false, header.is_valid());
}

BOOST_AUTO_TEST_CASE(header_message__factory_from_data_1__valid_input__success)
{
    const auto version = message::header_message::version_minimum;
    message::header_message expected
    {
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u,
        333u
    };

    data_chunk data = expected.to_data(version);

    auto result = message::header_message::factory_from_data(version, data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(header_message__factory_from_data_2__valid_input__success)
{
    const auto version = message::header_message::version_minimum;
    message::header_message expected
    {
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u,
        222u
    };

    data_chunk data = expected.to_data(version);
    data_source istream(data);

    auto result = message::header_message::factory_from_data(version, istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(header_message__factory_from_data_3__valid_input__success)
{
    const auto version = message::header_message::version_minimum;
    message::header_message expected
    {
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u,
        111u
    };

    data_chunk data = expected.to_data(version);
    data_source istream(data);
    istream_reader source(istream);

    auto result = message::header_message::factory_from_data(version, source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(header_message__originator_accessor__always__returns_initialized_value)
{
    uint64_t value = 54345436u;
    message::header_message instance(
        11234u,
        hash_literal("abababababababababababababababababababababababababababababababab"),
        hash_literal("fefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefe"),
        753234u,
        4356344u,
        34564u,
        value);

    BOOST_REQUIRE_EQUAL(value, instance.originator());
}

BOOST_AUTO_TEST_CASE(header_message__originator_setter__roundtrip__success)
{
    uint64_t expected = 54345436u;
    const message::header_message instance;
    BOOST_REQUIRE(expected != instance.originator());
    instance.set_originator(expected);
    BOOST_REQUIRE(expected == instance.originator());
}

BOOST_AUTO_TEST_CASE(header_message__operator_assign_equals_1__always__matches_equivalent)
{
    chain::header value(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u);

    BOOST_REQUIRE(value.is_valid());

    message::header_message instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());

    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(value == instance);
}

BOOST_AUTO_TEST_CASE(header_message__operator_assign_equals_2__always__matches_equivalent)
{
    message::header_message value(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u,
        4345u);

    BOOST_REQUIRE(value.is_valid());

    message::header_message instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());

    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(value == instance);
}

BOOST_AUTO_TEST_CASE(header_message__operator_boolean_equals_1__duplicates__returns_true)
{
    const chain::header expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        3565u);

    message::header_message instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(header_message__operator_boolean_equals_1__differs__returns_false)
{
    const chain::header expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        6523454u,
        68644u,
        4453u);

    message::header_message instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(header_message__operator_boolean_not_equals_1__duplicates__returns_false)
{
    const chain::header expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        6523454u,
        68644u,
        2345u);

    message::header_message instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(header_message__operator_boolean_not_equals_1__differs__returns_true)
{
    const chain::header expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        68644u,
        47476u);

    message::header_message instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_CASE(header_message__operator_boolean_equals_2__duplicates__returns_true)
{
    const message::header_message expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u,
        3565u);

    message::header_message instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(header_message__operator_boolean_equals_2__differs__returns_false)
{
    const message::header_message expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u,
        4453u);

    message::header_message instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(header_message__operator_boolean_not_equals_2__duplicates__returns_false)
{
    const message::header_message expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u,
        2345u);

    message::header_message instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(header_message__operator_boolean_not_equals_2__differs__returns_true)
{
    const message::header_message expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u,
        47476u);

    message::header_message instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_SUITE_END()

/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(messages_header_tests)

BOOST_AUTO_TEST_CASE(header__constructor_1__always__initialized_invalid)
{
    messages::header instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(header__constructor_2__always__equals_params)
{
    uint32_t version = 10u;
    hash_digest previous = base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
    hash_digest merkle = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    uint32_t timestamp = 531234u;
    uint32_t bits = 6523454u;
    uint32_t nonce = 68644u;

    messages::header instance(version, previous, merkle, timestamp, bits, nonce);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(version, instance.version());
    BOOST_REQUIRE_EQUAL(timestamp, instance.timestamp());
    BOOST_REQUIRE_EQUAL(bits, instance.bits());
    BOOST_REQUIRE_EQUAL(nonce, instance.nonce());
    BOOST_REQUIRE(previous == instance.previous_block_hash());
    BOOST_REQUIRE(merkle == instance.merkle_root());
}

BOOST_AUTO_TEST_CASE(header__constructor_3__always__equals_params)
{
    uint32_t version = 10u;
    hash_digest previous = base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
    hash_digest merkle = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    uint32_t timestamp = 531234u;
    uint32_t bits = 6523454u;
    uint32_t nonce = 68644u;

    messages::header instance(version, std::move(previous), std::move(merkle), timestamp, bits, nonce);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(version, instance.version());
    BOOST_REQUIRE_EQUAL(timestamp, instance.timestamp());
    BOOST_REQUIRE_EQUAL(bits, instance.bits());
    BOOST_REQUIRE_EQUAL(nonce, instance.nonce());
    BOOST_REQUIRE(previous == instance.previous_block_hash());
    BOOST_REQUIRE(merkle == instance.merkle_root());
}

BOOST_AUTO_TEST_CASE(header__constructor_4__always__equals_params)
{
    const chain::header expected(
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        1234u);

    messages::header instance(expected);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(header__constructor_5__always__equals_params)
{
    chain::header expected(
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        123u);

    messages::header instance(std::move(expected));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(header__constructor_6__always__equals_params)
{
    const messages::header expected(
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u);

    messages::header instance(expected);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(header__constructor_7__always__equals_params)
{
    messages::header expected(
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u);

    messages::header instance(std::move(expected));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(header__from_data__insufficient_bytes__failure)
{
    data_chunk data(10);
    messages::header header;
    BOOST_REQUIRE_EQUAL(false, header.from_data(messages::header::version_maximum, data));
    BOOST_REQUIRE_EQUAL(false, header.is_valid());
}

BOOST_AUTO_TEST_CASE(header__factory_1__valid_input_canonical_version__no_transaction_count)
{
    const auto version = messages::version::level::canonical;
    messages::header expected
    {
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u
    };

    const auto data = expected.to_data(version);

    const auto result = messages::header::factory(version, data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), chain::header::serialized_size());
}

BOOST_AUTO_TEST_CASE(header__factory_1__valid_input__success)
{
    const auto version = messages::header::version_minimum;
    messages::header expected
    {
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u
    };

    const auto data = expected.to_data(version);

    const auto result = messages::header::factory(version, data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(header__factory_2__valid_input__success)
{
    const auto version = messages::header::version_minimum;
    messages::header expected
    {
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u
    };

    const auto data = expected.to_data(version);
    stream::in::copy istream(data);

    const auto result = messages::header::factory(version, istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(header__factory_3__valid_input__success)
{
    const auto version = messages::header::version_minimum;
    messages::header expected
    {
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u
    };

    const auto data = expected.to_data(version);
    read::bytes::copy source(data);

    const auto result = messages::header::factory(version, source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(header__operator_assign_equals_1__always__matches_equivalent)
{
    chain::header value(
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u);

    BOOST_REQUIRE(value.is_valid());

    messages::header instance;
    BOOST_REQUIRE(!instance.is_valid());

    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(value == instance);
}

BOOST_AUTO_TEST_CASE(header__operator_assign_equals_2__always__matches_equivalent)
{
    messages::header value(
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u);

    BOOST_REQUIRE(value.is_valid());

    messages::header instance;
    BOOST_REQUIRE(!instance.is_valid());

    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(value == instance);
}

BOOST_AUTO_TEST_CASE(header__operator_boolean_equals_1__duplicates__true)
{
    const chain::header expected(
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        3565u);

    messages::header instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(header__operator_boolean_equals_1__differs__false)
{
    const chain::header expected(
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        6523454u,
        68644u,
        4453u);

    messages::header instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(header__operator_boolean_not_equals_1__duplicates__false)
{
    const chain::header expected(
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        6523454u,
        68644u,
        2345u);

    messages::header instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(header__operator_boolean_not_equals_1__differs__true)
{
    const chain::header expected(
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        68644u,
        47476u);

    messages::header instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_CASE(header__operator_boolean_equals_2__duplicates__true)
{
    const messages::header expected(
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u);

    messages::header instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(header__operator_boolean_equals_2__differs__false)
{
    const messages::header expected(
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u);

    messages::header instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(header__operator_boolean_not_equals_2__duplicates__false)
{
    const messages::header expected(
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u);

    messages::header instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(header__operator_boolean_not_equals_2__differs__true)
{
    const messages::header expected(
        10u,
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u);

    messages::header instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_SUITE_END()

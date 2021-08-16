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

BOOST_AUTO_TEST_SUITE(pong_tests)

BOOST_AUTO_TEST_CASE(pong__constructor_1__always__invalid)
{
    messages::pong instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(pong__constructor_2__always__equals_params)
{
    uint64_t nonce = 462434u;
    messages::pong instance(nonce);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(nonce, instance.nonce());
}

BOOST_AUTO_TEST_CASE(pong__constructor_3__always__equals_params)
{
    messages::pong expected(24235u);
    BOOST_REQUIRE(expected.is_valid());
    messages::pong instance(expected);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(pong__satoshi_fixed_size__minimum_version__returns_8)
{
    const auto size = messages::pong::satoshi_fixed_size(messages::version::level::minimum);
    BOOST_REQUIRE_EQUAL(size, 8u);
}

BOOST_AUTO_TEST_CASE(pong__factory_1__minimum_version_empty_data__invalid)
{
    static const auto version = messages::version::level::minimum;
    const auto result = messages::pong::factory(version, data_chunk{});
    BOOST_REQUIRE(!result.is_valid());
}

BOOST_AUTO_TEST_CASE(pong__factory_1__round_trip__expected)
{
    static const messages::pong expected
    {
        4306550u
    };

    static const auto version = messages::version::level::minimum;
    const auto data = expected.to_data(version);
    const auto result = messages::pong::factory(version, data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(pong__factory_2__round_trip__expected)
{
    static const messages::pong expected
    {
        3100693u
    };

    static const auto version = messages::version::level::minimum;
    const auto data = expected.to_data(version);
    stream::in::copy istream(data);
    const auto result = messages::pong::factory(version, istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(pong__factory_3__round_trip__expected)
{
    static const messages::pong expected
    {
        4642675u
    };

    static const auto version = messages::version::level::minimum;
    const auto data = expected.to_data(version);
    read::bytes::copy source(data);
    const auto result = messages::pong::factory(version, source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(pong__nonce_accessor__always__returns_initialized_value)
{
    uint64_t value = 43564u;
    messages::pong instance(value);
    BOOST_REQUIRE_EQUAL(value, instance.nonce());
}

BOOST_AUTO_TEST_CASE(pong__nonce_setter__roundtrip__success)
{
    uint64_t value = 43564u;
    messages::pong instance;
    BOOST_REQUIRE(value != instance.nonce());
    instance.set_nonce(value);
    BOOST_REQUIRE_EQUAL(value, instance.nonce());
}

BOOST_AUTO_TEST_CASE(pong__operator_assign_equals__always__matches_equivalent)
{
    messages::pong value(356234u);
    BOOST_REQUIRE(value.is_valid());
    messages::pong instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(pong__operator_boolean_equals__duplicates__returns_true)
{
    const messages::pong expected(4543234u);
    messages::pong instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(pong__operator_boolean_equals__differs__returns_false)
{
    const messages::pong expected(547553u);
    messages::pong instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(pong__operator_boolean_not_equals__duplicates__returns_false)
{
    const messages::pong expected(653786u);
    messages::pong instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(pong__operator_boolean_not_equals__differs__returns_true)
{
    const messages::pong expected(89764u);
    messages::pong instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_SUITE_END()

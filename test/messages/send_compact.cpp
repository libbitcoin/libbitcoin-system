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

BOOST_AUTO_TEST_SUITE(send_compact_tests)

BOOST_AUTO_TEST_CASE(send_compact__constructor_1__always__invalid)
{
    messages::send_compact instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(send_compact__constructor_2__always__equals_params)
{
    bool mode = true;
    uint64_t version = 1245436u;
    messages::send_compact instance(mode, version);
    BOOST_REQUIRE_EQUAL(mode, instance.high_bandwidth_mode());
    BOOST_REQUIRE_EQUAL(version, instance.version());
}

BOOST_AUTO_TEST_CASE(send_compact__constructor_3__always__equals_params)
{
    const messages::send_compact expected(true, 1245436u);
    messages::send_compact instance(expected);
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(send_compact__constructor_4__always__equals_params)
{
    bool mode = true;
    uint64_t version = 1245436u;
    messages::send_compact expected(mode, version);
    BOOST_REQUIRE(expected.is_valid());

    messages::send_compact instance(std::move(expected));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(mode, instance.high_bandwidth_mode());
    BOOST_REQUIRE_EQUAL(version, instance.version());
}

BOOST_AUTO_TEST_CASE(send_compact__factory_1__valid_input__success)
{
    const messages::send_compact expected{ true, 164 };
    const auto data = expected.to_data(messages::send_compact::version_minimum);
    const auto result = messages::send_compact::factory(
        messages::send_compact::version_minimum, data);

    BOOST_REQUIRE_EQUAL(
        messages::send_compact::satoshi_fixed_size(
            messages::send_compact::version_minimum),
        data.size());
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_CASE(send_compact__factory_2__valid_input__success)
{
    const messages::send_compact expected{ false, 5 };
    const auto data = expected.to_data(messages::send_compact::version_minimum);
    stream::in::copy istream(data);
    const auto result = messages::send_compact::factory(
        messages::send_compact::version_minimum, istream);

    BOOST_REQUIRE_EQUAL(
        messages::send_compact::satoshi_fixed_size(
            messages::send_compact::version_minimum),
        data.size());
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_CASE(send_compact__factory_3__valid_input__success)
{
    const messages::send_compact expected{ true, 257 };
    const auto data = expected.to_data(messages::send_compact::version_minimum);
    read::bytes::copy source(data);
    const auto result = messages::send_compact::factory(
        messages::send_compact::version_minimum, source);

    BOOST_REQUIRE_EQUAL(
        messages::send_compact::satoshi_fixed_size(
            messages::send_compact::version_minimum),
        data.size());
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_CASE(send_compact__from_data_1__invalid_mode_byte__failure)
{
    data_chunk raw_data{ 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
    messages::send_compact msg;
    bool result = msg.from_data(messages::send_compact::version_minimum, raw_data);
    BOOST_REQUIRE(!result);
}

BOOST_AUTO_TEST_CASE(send_compact__from_data_1__insufficient_version__failure)
{
    const messages::send_compact expected{ true, 257 };
    data_chunk raw_data = expected.to_data(messages::send_compact::version_minimum);
    messages::send_compact msg;
    bool result = msg.from_data(messages::send_compact::version_minimum - 1, raw_data);
    BOOST_REQUIRE(!result);
}

BOOST_AUTO_TEST_CASE(send_compact__high_bandwidth_mode_accessor__always__initialized_value)
{
    const bool expected = true;
    const messages::send_compact instance(expected, 210u);
    BOOST_REQUIRE_EQUAL(expected, instance.high_bandwidth_mode());
}

BOOST_AUTO_TEST_CASE(send_compact__high_bandwidth_mode_setter__roundtrip__success)
{
    const bool expected = true;
    messages::send_compact instance;
    BOOST_REQUIRE(expected != instance.high_bandwidth_mode());
    instance.set_high_bandwidth_mode(expected);
    BOOST_REQUIRE_EQUAL(expected, instance.high_bandwidth_mode());
}

BOOST_AUTO_TEST_CASE(send_compact__version_accessor__always__initialized_value)
{
    const uint64_t expected = 6548u;
    const messages::send_compact instance(false, expected);
    BOOST_REQUIRE_EQUAL(expected, instance.version());
}

BOOST_AUTO_TEST_CASE(send_compact__version_setter__roundtrip__success)
{
    const uint64_t expected = 6548u;
    messages::send_compact instance;
    BOOST_REQUIRE(expected != instance.version());
    instance.set_version(expected);
    BOOST_REQUIRE_EQUAL(expected, instance.version());
}

BOOST_AUTO_TEST_CASE(send_compact__operator_assign_equals__always__matches_equivalent)
{
    bool mode = false;
    uint64_t version = 210u;
    messages::send_compact value(mode, version);
    BOOST_REQUIRE(value.is_valid());

    messages::send_compact instance;
    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(mode, instance.high_bandwidth_mode());
    BOOST_REQUIRE_EQUAL(version, instance.version());
}

BOOST_AUTO_TEST_CASE(send_compact__operator_boolean_equals__duplicates__true)
{
    const messages::send_compact expected(false, 15234u);
    messages::send_compact instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(send_compact__operator_boolean_equals__differs__false)
{
    const messages::send_compact expected(true, 979797u);
    messages::send_compact instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(send_compact__operator_boolean_not_equals__duplicates__false)
{
    const messages::send_compact expected(true, 734678u);
    messages::send_compact instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(send_compact__operator_boolean_not_equals__differs__true)
{
    const messages::send_compact expected(false, 5357534u);
    messages::send_compact instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_SUITE_END()

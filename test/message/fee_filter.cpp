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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(fee_filter_tests)

BOOST_AUTO_TEST_CASE(fee_filter__constructor_1__always__invalid)
{
    message::fee_filter instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(fee_filter__constructor_2__always__equals_params)
{
    uint64_t value = 6434u;
    message::fee_filter instance(value);
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE_EQUAL(value, instance.minimum_fee());
}

BOOST_AUTO_TEST_CASE(fee_filter__constructor_3__always__equals_params)
{
    uint64_t fee = 6434u;
    const message::fee_filter value(fee);
    message::fee_filter instance(value);
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE_EQUAL(fee, instance.minimum_fee());
    BOOST_REQUIRE(value == instance);
}

BOOST_AUTO_TEST_CASE(fee_filter__constructor_4__always__equals_params)
{
    uint64_t fee = 6434u;
    message::fee_filter value(fee);
    message::fee_filter instance(std::move(value));
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE_EQUAL(fee, instance.minimum_fee());
}

BOOST_AUTO_TEST_CASE(fee_filter__from_data__insufficient_bytes_failure)
{
    data_chunk raw = { 0xab, 0x11 };
    message::fee_filter instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(message::version::level::maximum, raw));
}

BOOST_AUTO_TEST_CASE(fee_filter__from_data__insufficient_version_failure)
{
    const message::fee_filter expected{ 1 };
    const auto data = expected.to_data(message::fee_filter::version_maximum);
    message::fee_filter instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(
        message::filter_add::version_minimum - 1, data));
}

BOOST_AUTO_TEST_CASE(fee_filter__factory_from_data_1__roundtrip__success)
{
    const message::fee_filter expected{ 123 };
    const auto data = expected.to_data(message::fee_filter::version_maximum);
    const auto result = message::fee_filter::factory_from_data(
        message::fee_filter::version_maximum, data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(message::version::level::maximum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(message::version::level::maximum),
        result.serialized_size(message::version::level::maximum));
}

BOOST_AUTO_TEST_CASE(fee_filter__factory_from_data_2__roundtrip__success)
{
    const message::fee_filter expected{ 325 };
    const auto data = expected.to_data(message::fee_filter::version_maximum);
    data_source istream(data);
    const auto result = message::fee_filter::factory_from_data(
        message::fee_filter::version_maximum, istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(message::version::level::maximum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(message::version::level::maximum),
        result.serialized_size(message::version::level::maximum));
}

BOOST_AUTO_TEST_CASE(fee_filter__factory_from_data_3__roundtrip__success)
{
    const message::fee_filter expected{ 58246 };
    const auto data = expected.to_data(message::fee_filter::version_maximum);
    data_source istream(data);
    istream_reader source(istream);
    const auto result = message::fee_filter::factory_from_data(
        message::fee_filter::version_maximum, source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(message::version::level::maximum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(message::version::level::maximum),
        result.serialized_size(message::version::level::maximum));
}

BOOST_AUTO_TEST_CASE(fee_filter__minimum_fee__roundtrip__success)
{
    const uint64_t value = 42134u;
    message::fee_filter instance;
    BOOST_REQUIRE(value != instance.minimum_fee());
    instance.set_minimum_fee(value);
    BOOST_REQUIRE_EQUAL(value, instance.minimum_fee());
}

BOOST_AUTO_TEST_CASE(fee_filter__operator_assign_equals__always__matches_equivalent)
{
    message::fee_filter value(2453u);
    BOOST_REQUIRE(value.is_valid());

    message::fee_filter instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());

    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(fee_filter__operator_boolean_equals__duplicates__returns_true)
{
    const message::fee_filter expected(2453u);
    message::fee_filter instance(expected);
    BOOST_REQUIRE_EQUAL(true, instance == expected);
}

BOOST_AUTO_TEST_CASE(fee_filter__operator_boolean_equals__differs__returns_false)
{
    const message::fee_filter expected(2453u);
    message::fee_filter instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(fee_filter__operator_boolean_not_equals__duplicates__returns_false)
{
    const message::fee_filter expected(2453u);
    message::fee_filter instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(fee_filter__operator_boolean_not_equals__differs__returns_true)
{
    const message::fee_filter expected(2453u);
    message::fee_filter instance;
    BOOST_REQUIRE_EQUAL(true, instance != expected);
}

BOOST_AUTO_TEST_SUITE_END()

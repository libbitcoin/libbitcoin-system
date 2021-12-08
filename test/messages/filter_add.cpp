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

BOOST_AUTO_TEST_SUITE(filter_add_tests)

BOOST_AUTO_TEST_CASE(filter_add__constructor_1__always__invalid)
{
    messages::filter_add instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(filter_add__constructor_2__always__equals_params)
{
    const data_chunk data = { 0x0f, 0xf0, 0x55, 0xaa };
    messages::filter_add instance(data);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(data == instance.data());
}

BOOST_AUTO_TEST_CASE(filter_add__constructor_3__always__equals_params)
{
    const data_chunk data = { 0x0f, 0xf0, 0x55, 0xaa };
    auto dup = data;
    messages::filter_add instance(std::move(dup));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(data == instance.data());
}

BOOST_AUTO_TEST_CASE(filter_add__constructor_4__always__equals_params)
{
    const data_chunk data = { 0x0f, 0xf0, 0x55, 0xaa };
    const messages::filter_add value(data);
    messages::filter_add instance(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(value == instance);
    BOOST_REQUIRE(data == instance.data());
}

BOOST_AUTO_TEST_CASE(filter_add__constructor_5__always__equals_params)
{
    const data_chunk data = { 0x0f, 0xf0, 0x55, 0xaa };
    messages::filter_add value(data);
    messages::filter_add instance(std::move(value));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(data == instance.data());
}

BOOST_AUTO_TEST_CASE(filter_add__from_data__insufficient_bytes__failure)
{
    data_chunk raw = { 0xab, 0x11 };
    messages::filter_add instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(messages::version::level::maximum, raw));
}

BOOST_AUTO_TEST_CASE(filter_add__from_data__insufficient_version__failure)
{
    const messages::filter_add expected
        {
            {
                0x1F, 0x9a, 0x0d, 0x24, 0x9a, 0xd5, 0x39, 0x89,
                0xbb, 0x85, 0x0a, 0x3d, 0x79, 0x24, 0xed, 0x0f,
                0xc3, 0x0d, 0x6f, 0x55, 0x7d, 0x71, 0x12, 0x1a,
                0x37, 0xc0, 0xb0, 0x32, 0xf0, 0xd6, 0x6e, 0xdf
            }
        };

    const auto data = expected.to_data(messages::version::level::maximum);
    messages::filter_add instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(
        messages::filter_add::version_minimum - 1, data));
}

BOOST_AUTO_TEST_CASE(filter_add__factory_1__valid_input__success)
{
    const messages::filter_add expected
    {
        {
            0x1F, 0x9a, 0x0d, 0x24, 0x9a, 0xd5, 0x39, 0x89,
            0xbb, 0x85, 0x0a, 0x3d, 0x79, 0x24, 0xed, 0x0f,
            0xc3, 0x0d, 0x6f, 0x55, 0x7d, 0x71, 0x12, 0x1a,
            0x37, 0xc0, 0xb0, 0x32, 0xf0, 0xd6, 0x6e, 0xdf
        }
    };

    const auto data = expected.to_data(messages::version::level::maximum);
    const auto result = messages::filter_add::factory(
        messages::version::level::maximum, data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(messages::version::level::maximum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(messages::version::level::maximum),
        result.serialized_size(messages::version::level::maximum));
}

BOOST_AUTO_TEST_CASE(filter_add__factory_2__valid_input__success)
{
    const messages::filter_add expected
    {
        {
            0x1F, 0x9a, 0x0d, 0x24, 0x9a, 0xd5, 0x39, 0x89,
            0xbb, 0x85, 0x0a, 0x3d, 0x79, 0x24, 0xed, 0x0f,
            0xc3, 0x0d, 0x6f, 0x55, 0x7d, 0x71, 0x12, 0x1a,
            0x37, 0xc0, 0xb0, 0x32, 0xf0, 0xd6, 0x6e, 0xdf
        }
    };

    const auto data = expected.to_data(messages::version::level::maximum);
    stream::in::copy istream(data);
    const auto result = messages::filter_add::factory(
        messages::version::level::maximum, istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(messages::version::level::maximum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(messages::version::level::maximum),
        result.serialized_size(messages::version::level::maximum));
}

BOOST_AUTO_TEST_CASE(filter_add__factory_3__valid_input__success)
{
    const messages::filter_add expected
    {
        {
            0x1F, 0x9a, 0x0d, 0x24, 0x9a, 0xd5, 0x39, 0x89,
            0xbb, 0x85, 0x0a, 0x3d, 0x79, 0x24, 0xed, 0x0f,
            0xc3, 0x0d, 0x6f, 0x55, 0x7d, 0x71, 0x12, 0x1a,
            0x37, 0xc0, 0xb0, 0x32, 0xf0, 0xd6, 0x6e, 0xdf
        }
    };

    const auto data = expected.to_data(messages::version::level::maximum);
    read::bytes::copy source(data);
    const auto result = messages::filter_add::factory(
        messages::version::level::maximum, source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(messages::version::level::maximum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(messages::version::level::maximum),
        result.serialized_size(messages::version::level::maximum));
}

BOOST_AUTO_TEST_CASE(filter_add__data_accessor_1__always__initialized_value)
{
    const data_chunk value = { 0x0f, 0xf0, 0x55, 0xaa };
    messages::filter_add instance(value);
    BOOST_REQUIRE(value == instance.data());
}

BOOST_AUTO_TEST_CASE(filter_add__data_accessor_2__always__initialized_value)
{
    const data_chunk value = { 0x0f, 0xf0, 0x55, 0xaa };
    const messages::filter_add instance(value);
    BOOST_REQUIRE(value == instance.data());
}

BOOST_AUTO_TEST_CASE(filter_add__data_setter_1__roundtrip__success)
{
    const data_chunk value = { 0x0f, 0xf0, 0x55, 0xaa };
    messages::filter_add instance;
    BOOST_REQUIRE(value != instance.data());
    instance.set_data(value);
    BOOST_REQUIRE(value == instance.data());
}

BOOST_AUTO_TEST_CASE(filter_add__data_setter_2__roundtrip__success)
{
    const data_chunk value = { 0x0f, 0xf0, 0x55, 0xaa };
    data_chunk dup = value;
    messages::filter_add instance;
    BOOST_REQUIRE(value != instance.data());
    instance.set_data(std::move(dup));
    BOOST_REQUIRE(value == instance.data());
}

BOOST_AUTO_TEST_CASE(filter_add__operator_assign_equals__always__matches_equivalent)
{
    const data_chunk data = { 0x0f, 0xf0, 0x55, 0xaa };
    messages::filter_add value(data);
    BOOST_REQUIRE(value.is_valid());
    messages::filter_add instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(data == instance.data());
}

BOOST_AUTO_TEST_CASE(filter_add__operator_boolean_equals__duplicates__true)
{
    const messages::filter_add expected({ 0x0f, 0xf0, 0x55, 0xaa });
    messages::filter_add instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(filter_add__operator_boolean_equals__differs__false)
{
    const messages::filter_add expected({ 0x0f, 0xf0, 0x55, 0xaa });
    messages::filter_add instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(filter_add__operator_boolean_not_equals__duplicates__false)
{
    const messages::filter_add expected({ 0x0f, 0xf0, 0x55, 0xaa });
    messages::filter_add instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(filter_add__operator_boolean_not_equals__differs__true)
{
    const messages::filter_add expected({ 0x0f, 0xf0, 0x55, 0xaa });
    messages::filter_add instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_SUITE_END()

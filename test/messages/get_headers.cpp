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

BOOST_AUTO_TEST_SUITE(get_headers_tests)

BOOST_AUTO_TEST_CASE(get_headers__constructor_1__always__invalid)
{
    messages::get_headers instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(get_headers__constructor_2__always__equals_params)
{
    hash_list starts = {
        base16_hash("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
        base16_hash("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"),
        base16_hash("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee")
    };

    hash_digest stop = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");

    messages::get_headers instance(starts, stop);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(starts == instance.start_hashes());
    BOOST_REQUIRE(stop == instance.stop_hash());
}

BOOST_AUTO_TEST_CASE(get_headers__constructor_3__always__equals_params)
{
    hash_list starts = {
        base16_hash("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
        base16_hash("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"),
        base16_hash("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee")
    };
    hash_list starts_duplicate = starts;

    hash_digest stop = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");

    messages::get_headers instance(std::move(starts_duplicate), std::move(stop));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(starts == instance.start_hashes());
    BOOST_REQUIRE(stop == instance.stop_hash());
}

BOOST_AUTO_TEST_CASE(get_headers__constructor_4__always__equals_params)
{
    hash_list starts = {
        base16_hash("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
        base16_hash("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"),
        base16_hash("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee")
    };

    hash_digest stop = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");

    const messages::get_headers expected(starts, stop);
    messages::get_headers instance(expected);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected == instance);
    BOOST_REQUIRE(starts == instance.start_hashes());
    BOOST_REQUIRE(stop == instance.stop_hash());
}

BOOST_AUTO_TEST_CASE(get_headers__constructor_5__always__equals_params)
{
    hash_list starts = {
        base16_hash("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
        base16_hash("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"),
        base16_hash("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee")
    };

    hash_digest stop = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");

    messages::get_headers expected(starts, stop);
    messages::get_headers instance(std::move(expected));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(starts == instance.start_hashes());
    BOOST_REQUIRE(stop == instance.stop_hash());
}

BOOST_AUTO_TEST_CASE(get_headers__from_data__insufficient_bytes__failure)
{
    const data_chunk raw{ 0xab, 0xcd };
    messages::get_headers instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(
        messages::get_headers::version_minimum, raw));
}

BOOST_AUTO_TEST_CASE(get_headers__from_data__insufficient_version__failure)
{
    const messages::get_headers expected
    {
        {
            base16_hash("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
            base16_hash("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"),
            base16_hash("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"),
            base16_hash("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"),
            base16_hash("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee")
        },
        base16_hash("7777777777777777777777777777777777777777777777777777777777777777")
    };

    const auto data = expected.to_data(messages::get_headers::version_minimum);
    messages::get_headers instance{};

    BOOST_REQUIRE_EQUAL(false, instance.from_data(
        messages::get_headers::version_minimum - 1, data));
}

BOOST_AUTO_TEST_CASE(get_headers__factory_1__valid_input__success)
{
    const messages::get_headers expected
    {
        {
            base16_hash("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
            base16_hash("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"),
            base16_hash("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"),
            base16_hash("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"),
            base16_hash("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee")
        },
        base16_hash("7777777777777777777777777777777777777777777777777777777777777777")
    };

    const auto data = expected.to_data(messages::get_headers::version_minimum);
    const auto result = messages::get_headers::factory(
        messages::get_headers::version_minimum, data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(messages::get_headers::version_minimum));
    BOOST_REQUIRE_EQUAL(
        expected.serialized_size(messages::get_headers::version_minimum),
        result.serialized_size(messages::get_headers::version_minimum));
}

BOOST_AUTO_TEST_CASE(get_headers__factory_2__valid_input__success)
{
    const messages::get_headers expected
    {
        {
            base16_hash("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
            base16_hash("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"),
            base16_hash("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"),
            base16_hash("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"),
            base16_hash("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee")
        },
        base16_hash("7777777777777777777777777777777777777777777777777777777777777777")
    };

    const auto data = expected.to_data(messages::get_headers::version_minimum);
    stream::in::copy istream(data);
    const auto result = messages::get_headers::factory(
        messages::get_headers::version_minimum, istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(messages::get_headers::version_minimum));
    BOOST_REQUIRE_EQUAL(
        expected.serialized_size(messages::get_headers::version_minimum),
        result.serialized_size(messages::get_headers::version_minimum));
}

BOOST_AUTO_TEST_CASE(get_headers__factory_3__valid_input__success)
{
    const messages::get_headers expected
    {
        {
            base16_hash("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
            base16_hash("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"),
            base16_hash("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"),
            base16_hash("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"),
            base16_hash("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee")
        },
        base16_hash("7777777777777777777777777777777777777777777777777777777777777777")
    };

    const auto data = expected.to_data(messages::get_headers::version_minimum);
    read::bytes::copy source(data);
    const auto result = messages::get_headers::factory(
        messages::get_headers::version_minimum, source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(messages::get_headers::version_minimum));
    BOOST_REQUIRE_EQUAL(
        expected.serialized_size(messages::get_headers::version_minimum),
        result.serialized_size(messages::get_headers::version_minimum));
}

BOOST_AUTO_TEST_CASE(get_headers__operator_assign_equals__always__matches_equivalent)
{
    hash_list start = {
        base16_hash("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
        base16_hash("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"),
        base16_hash("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"),
        base16_hash("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"),
        base16_hash("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee")
    };

    hash_digest stop = base16_hash("7777777777777777777777777777777777777777777777777777777777777777");

    messages::get_headers value{ start, stop };

    BOOST_REQUIRE(value.is_valid());

    messages::get_headers instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());

    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(start == instance.start_hashes());
    BOOST_REQUIRE(stop == instance.stop_hash());
}

BOOST_AUTO_TEST_CASE(get_headers__operator_boolean_equals__duplicates__true)
{
    const messages::get_headers expected
    {
        {
            base16_hash("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
            base16_hash("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"),
            base16_hash("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"),
            base16_hash("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"),
            base16_hash("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee")
        },
        base16_hash("7777777777777777777777777777777777777777777777777777777777777777")
    };

    messages::get_headers instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(get_headers__operator_boolean_equals__differs__false)
{
    const messages::get_headers expected
    {
        {
            base16_hash("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
            base16_hash("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"),
            base16_hash("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"),
            base16_hash("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"),
            base16_hash("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee")
        },
        base16_hash("7777777777777777777777777777777777777777777777777777777777777777")
    };

    messages::get_headers instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(get_headers__operator_boolean_not_equals__duplicates__false)
{
    const messages::get_headers expected
    {
        {
            base16_hash("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
            base16_hash("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"),
            base16_hash("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"),
            base16_hash("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"),
            base16_hash("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee")
        },
        base16_hash("7777777777777777777777777777777777777777777777777777777777777777")
    };

    messages::get_headers instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(get_headers__operator_boolean_not_equals__differs__true)
{
    const messages::get_headers expected
    {
        {
            base16_hash("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
            base16_hash("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"),
            base16_hash("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"),
            base16_hash("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"),
            base16_hash("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee")
        },
        base16_hash("7777777777777777777777777777777777777777777777777777777777777777")
    };

    messages::get_headers instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_SUITE_END()

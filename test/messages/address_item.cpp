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

BOOST_AUTO_TEST_SUITE(address_item_tests)

bool equal(const messages::address_item& left,
    const messages::address_item& right, bool with_timestamp)
{
    if (with_timestamp && (left.timestamp() != right.timestamp()))
        return false;

    return left == right;
}

BOOST_AUTO_TEST_CASE(address_item__constructor_1__always__invalid)
{
    messages::address_item instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(address_item__constructor_2__always__equals_params)
{
    uint32_t timestamp = 734678u;
    uint64_t services = 5357534u;
    uint16_t port = 123u;
    const messages::ip_address ip = base16_array("127544abcdefa7b6d3e91486c57000aa");

    messages::address_item instance(timestamp, services, ip, port);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(ip == instance.ip());
    BOOST_REQUIRE_EQUAL(port, instance.port());
    BOOST_REQUIRE_EQUAL(services, instance.services());
    BOOST_REQUIRE_EQUAL(timestamp, instance.timestamp());
}

BOOST_AUTO_TEST_CASE(address_item__constructor_3__always__equals_params)
{
    uint32_t timestamp = 734678u;
    uint64_t services = 5357534u;
    uint16_t port = 123u;

    messages::address_item instance(timestamp, services,
        base16_array("127544abcdefa7b6d3e91486c57000aa"), port);

    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(address_item__constructor_4__always__equals_params)
{
    const messages::address_item expected
    {
        734678u,
        5357534u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        123u
    };

    BOOST_REQUIRE(expected.is_valid());

    messages::address_item instance(expected);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(address_item__constructor_5__always__equals_params)
{
    messages::address_item expected
    {
        734678u,
        5357534u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        123u
    };

    BOOST_REQUIRE(expected.is_valid());

    messages::address_item instance(std::move(expected));
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(address_item__from_data__insufficient_bytes__failure)
{
    const data_chunk raw{ 1 };
    messages::address_item instance{};

    BOOST_REQUIRE_EQUAL(false, instance.from_data(
        messages::version::level::minimum, raw, false));
}

BOOST_AUTO_TEST_CASE(address_item__factory_1__without_timestamp__success)
{
    const messages::address_item expected
    {
        734678u,
        5357534u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        123u
    };

    const auto data = expected.to_data(messages::version::level::minimum, false);
    const auto result = messages::address_item::factory(
        messages::version::level::minimum, data, false);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, false));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(messages::version::level::minimum, false));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(messages::version::level::minimum, false),
        result.serialized_size(messages::version::level::minimum, false));
}

BOOST_AUTO_TEST_CASE(address_item__factory_2__without_timestamp__success)
{
    const messages::address_item expected
    {
        734678u,
        5357534u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        123u
    };

    const auto data = expected.to_data(messages::version::level::minimum, false);
    stream::in::copy istream(data);
    const auto result = messages::address_item::factory(
        messages::version::level::minimum, istream, false);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, false));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(messages::version::level::minimum, false));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(messages::version::level::minimum, false),
        result.serialized_size(messages::version::level::minimum, false));
}

BOOST_AUTO_TEST_CASE(address_item__factory_3__without_timestamp__success)
{
    const messages::address_item expected
    {
        734678u,
        5357534u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        123u
    };

    const auto data = expected.to_data(messages::version::level::minimum, false);
    read::bytes::copy source(data);
    const auto result = messages::address_item::factory(
        messages::version::level::minimum, source, false);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, false));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(messages::version::level::minimum, false));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(messages::version::level::minimum, false),
        result.serialized_size(messages::version::level::minimum, false));
}

BOOST_AUTO_TEST_CASE(address_item__factory_1__with_timestamp__success)
{
    const messages::address_item expected
    {
        734678u,
        5357534u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        123u
    };

    const auto data = expected.to_data(messages::version::level::minimum, true);
    const auto result = messages::address_item::factory(
        messages::version::level::minimum, data, true);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, true));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(messages::version::level::minimum, true));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(messages::version::level::minimum, true),
        result.serialized_size(messages::version::level::minimum, true));
}

BOOST_AUTO_TEST_CASE(address_item__factory_2__with_timestamp__success)
{
    const messages::address_item expected
    {
        734678u,
        5357534u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        123u
    };

    const auto data = expected.to_data(messages::version::level::minimum, true);
    stream::in::copy istream(data);
    const auto result = messages::address_item::factory(
        messages::version::level::minimum, istream, true);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, true));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(messages::version::level::minimum, true));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(messages::version::level::minimum, true),
        result.serialized_size(messages::version::level::minimum, true));
}

BOOST_AUTO_TEST_CASE(address_item__factory_3__with_timestamp__success)
{
    const messages::address_item expected
    {
        734678u,
        5357534u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        123u
    };

    const auto data = expected.to_data(messages::version::level::minimum, true);
    read::bytes::copy source(data);
    const auto result = messages::address_item::factory(
        messages::version::level::minimum, source, true);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, true));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(messages::version::level::minimum, true));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(messages::version::level::minimum, true),
        result.serialized_size(messages::version::level::minimum, true));
}

BOOST_AUTO_TEST_CASE(address_item__timestamp_accessor__always__initialized_value)
{
    const uint32_t timestamp = 734678u;
    messages::address_item instance(
        timestamp,
        5357534u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        123u
    );

    BOOST_REQUIRE(timestamp == instance.timestamp());
}

BOOST_AUTO_TEST_CASE(address_item__timestamp_setter__roundtrip__success)
{
    const uint32_t timestamp = 734678u;
    messages::address_item instance;
    BOOST_REQUIRE(timestamp != instance.timestamp());
    instance.set_timestamp(timestamp);
    BOOST_REQUIRE(timestamp == instance.timestamp());
}

BOOST_AUTO_TEST_CASE(address_item__services_accessor__always__initialized_value)
{
    const uint32_t services = 5357534u;
    messages::address_item instance(
        734678u,
        services,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        123u
    );

    BOOST_REQUIRE(services == instance.services());
}

BOOST_AUTO_TEST_CASE(address_item__services_setter__roundtrip__success)
{
    const uint64_t services = 6842368u;
    messages::address_item instance;
    BOOST_REQUIRE(services != instance.services());
    instance.set_services(services);
    BOOST_REQUIRE(services == instance.services());
}

BOOST_AUTO_TEST_CASE(address_item__ip_accessor__always__initialized_value)
{
    const messages::ip_address ip = base16_array("127544abcdefa7b6d3e91486c57000aa");

    messages::address_item instance(
        734678u,
        5357534u,
        ip,
        123u
    );

    BOOST_REQUIRE(ip == instance.ip());
}

BOOST_AUTO_TEST_CASE(address_item__ip_setter_1__roundtrip__success)
{
    const messages::ip_address ip = base16_array("127544abcdefa7b6d3e91486c57000aa");

    messages::address_item instance;
    BOOST_REQUIRE(ip != instance.ip());
    instance.set_ip(ip);
    BOOST_REQUIRE(ip == instance.ip());
}

BOOST_AUTO_TEST_CASE(address_item__ip_setter_2__roundtrip__success)
{
    const messages::ip_address ip = base16_array("127544abcdefa7b6d3e91486c57000aa");

    messages::address_item instance;
    BOOST_REQUIRE(ip != instance.ip());
    instance.set_ip(base16_array("127544abcdefa7b6d3e91486c57000aa"));
    BOOST_REQUIRE(ip == instance.ip());
}

BOOST_AUTO_TEST_CASE(address_item__port_accessor__always__initialized_value)
{
    const uint16_t port = 123u;
    messages::address_item instance(
        734678u,
        5357534u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        port
    );

    BOOST_REQUIRE(port == instance.port());
}

BOOST_AUTO_TEST_CASE(address_item__port_setter__roundtrip__success)
{
    const uint16_t port = 853u;
    messages::address_item instance;
    BOOST_REQUIRE(port != instance.port());
    instance.set_port(port);
    BOOST_REQUIRE(port == instance.port());
}

BOOST_AUTO_TEST_CASE(address_item__operator_assign_equals_1__always__matches_equivalent)
{
    messages::address_item value(
        14356u,
        54676843u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        1500u
    );

    BOOST_REQUIRE(value.is_valid());

    messages::address_item instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());

    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(address_item__operator_assign_equals_2__always__matches_equivalent)
{
    const messages::address_item value(
        14356u,
        54676843u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        1500u
    );

    BOOST_REQUIRE(value.is_valid());

    messages::address_item instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());

    instance = value;
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(value == instance);
}

BOOST_AUTO_TEST_CASE(address_item__operator_boolean_equals__duplicates__true)
{
    const messages::address_item expected(
        14356u,
        54676843u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        1500u
    );

    messages::address_item instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(address_item__operator_boolean_equals__differs_timestamp__true)
{
    const messages::address_item expected(
        14356u,
        54676843u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        1500u
    );

    messages::address_item instance(643u, expected.services(),
        expected.ip(), expected.port());
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(address_item__operator_boolean_equals__differs__false)
{
    const messages::address_item expected(
        14356u,
        54676843u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        1500u
    );

    messages::address_item instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(address_item__operator_boolean_not_equals__duplicates__false)
{
    const messages::address_item expected(
        14356u,
        54676843u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        1500u
    );

    messages::address_item instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(address_item__operator_boolean_not_equals__differs_timestamp__false)
{
    const messages::address_item expected(
        14356u,
        54676843u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        1500u
    );

    messages::address_item instance(643u, expected.services(),
        expected.ip(), expected.port());
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(address_item__operator_boolean_not_equals__differs__true)
{
    const messages::address_item expected(
        14356u,
        54676843u,
        base16_array("127544abcdefa7b6d3e91486c57000aa"),
        1500u
    );

    messages::address_item instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_SUITE_END()

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
using namespace bc::message;

BOOST_AUTO_TEST_SUITE(ping_tests)

BOOST_AUTO_TEST_CASE(ping__satoshi_fixed_size__minimum_version__zero)
{
    BOOST_REQUIRE_EQUAL(ping::satoshi_fixed_size(version::level::minimum), 0u);
}

BOOST_AUTO_TEST_CASE(ping__satoshi_fixed_size__bip31_version__8)
{
    BOOST_REQUIRE_EQUAL(ping::satoshi_fixed_size(version::level::bip31), 8u);
}

BOOST_AUTO_TEST_CASE(ping__factory_from_data1__maximum_version_empty_data__invalid)
{
    static const auto version = version::level::maximum;
    const auto result = ping::factory_from_data(version, data_chunk{});
    BOOST_REQUIRE(!result.is_valid());
}

BOOST_AUTO_TEST_CASE(ping__factory_from_data1__minimum_version_empty_data__valid)
{
    static const auto version = version::level::minimum;
    const auto result = ping::factory_from_data(version, data_chunk{});
    BOOST_REQUIRE(result.is_valid());
}

BOOST_AUTO_TEST_CASE(ping__from_data1__minimum_version__success_zero_nonce)
{
    static const ping value
    {
        213153u
    };

    // This serializes the nonce.
    const auto data = value.to_data(version::level::bip31);
    BOOST_REQUIRE_EQUAL(data.size(), 8u);

    // This leaves the nonce on the wire but otherwise succeeds with a zero nonce.
    ping instance;
    BOOST_REQUIRE(instance.from_data(ping::version_minimum, data));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.nonce, 0u);
}

BOOST_AUTO_TEST_CASE(ping__factory_from_data1__minimum_version_round_trip__zero_nonce)
{
    static const ping value
    {
        16545612u
    };

    static const auto version = version::level::minimum;
    const auto data = value.to_data(version);
    const auto result = ping::factory_from_data(version, data);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE_EQUAL(result.nonce, 0u);
}

BOOST_AUTO_TEST_CASE(ping__factory_from_data2__minimum_version_round_trip__zero_nonce)
{
    const ping value
    {
        5087222u
    };

    static const auto version = version::level::minimum;
    const auto data = value.to_data(version);
    data_source istream(data);
    const auto result = ping::factory_from_data(version, istream);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE_EQUAL(result.nonce, 0u);
}

BOOST_AUTO_TEST_CASE(ping__factory_from_data3__minimum_version_round_trip__zero_nonce)
{
    static const ping value
    {
        6456147u
    };

    static const auto version = version::level::minimum;
    const auto data = value.to_data(version);
    data_source istream(data);
    istream_reader source(istream);
    const auto result = ping::factory_from_data(version, source);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE_EQUAL(result.nonce, 0u);
}

BOOST_AUTO_TEST_CASE(ping__from_data1__maximum_version__success_expected_nonce)
{
    static const ping expected
    {
        213153u
    };

    // This serializes the nonce.
    const auto data = expected.to_data(version::level::bip31);
    BOOST_REQUIRE_EQUAL(data.size(), 8u);

    // This leaves the nonce on the wire but otherwise succeeds with a zero nonce.
    ping instance;
    BOOST_REQUIRE(instance.from_data(ping::version_maximum, data));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.nonce, expected.nonce);
}

BOOST_AUTO_TEST_CASE(ping__factory_from_data1__bip31_version_round_trip__expected_nonce)
{
    static const ping expected
    {
        16545612u
    };

    static const auto version = version::level::bip31;
    const auto data = expected.to_data(version);
    const auto result = ping::factory_from_data(version, data);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE_EQUAL(result.nonce, expected.nonce);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(ping__factory_from_data2__bip31_version_round_trip__expected_nonce)
{
    const ping expected
    {
        5087222u
    };

    static const auto version = version::level::bip31;
    const auto data = expected.to_data(version);
    data_source istream(data);
    const auto result = ping::factory_from_data(version, istream);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE_EQUAL(result.nonce, expected.nonce);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(ping__factory_from_data3__bip31_version_round_trip__expected_nonce)
{
    static const ping expected
    {
        6456147u
    };

    static const auto version = version::level::bip31;
    const auto data = expected.to_data(version);
    data_source istream(data);
    istream_reader source(istream);
    const auto result = ping::factory_from_data(version, source);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE_EQUAL(result.nonce, expected.nonce);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_SUITE_END()

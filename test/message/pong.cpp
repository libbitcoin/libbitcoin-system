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

BOOST_AUTO_TEST_SUITE(pong_tests)

BOOST_AUTO_TEST_CASE(pong__satoshi_fixed_size__minimum__sizeof_uint64_t)
{
    BOOST_REQUIRE_EQUAL(sizeof(uint64_t), pong::satoshi_fixed_size(version::level::minimum));
}

BOOST_AUTO_TEST_CASE(pong__factory_from_data1__empty_data__invalid)
{
    static const auto version = version::level::minimum;
    const auto result = pong::factory_from_data(version, data_chunk{});
    BOOST_REQUIRE(!result.is_valid());
}

BOOST_AUTO_TEST_CASE(pong__factory_from_data1__round_trip__expected)
{
    static const pong expected
    {
        4306550u
    };

    static const auto version = version::level::minimum;
    const auto data = expected.to_data(version);
    const auto result = pong::factory_from_data(version, data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(pong__factory_from_data2__round_trip__expected)
{
    static const pong expected
    {
        3100693u
    };

    static const auto version = version::level::minimum;
    const auto data = expected.to_data(version);
    data_source istream(data);
    const auto result = pong::factory_from_data(version, istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(pong__factory_from_data3__round_trip__expected)
{
    static const pong expected
    {
        4642675u
    };

    static const auto version = version::level::minimum;
    const auto data = expected.to_data(version);
    data_source istream(data);
    istream_reader source(istream);
    const auto result = pong::factory_from_data(version, source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_SUITE_END()

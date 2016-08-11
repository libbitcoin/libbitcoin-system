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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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

BOOST_AUTO_TEST_SUITE(headers_tests)

BOOST_AUTO_TEST_CASE(from_data_insufficient_bytes_failure)
{
    const data_chunk raw{ 0xab, 0xcd };
    headers instance{};
    BOOST_REQUIRE_EQUAL(false, instance.from_data(version::level::minimum, raw));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk)
{
    static const headers expected
    {
        {
            {
                10,
                hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
                hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
                531234,
                6523454,
                68644,
                65
            }
        }
    };

    static const auto version = version::level::minimum;
    const auto data = expected.to_data(version);
    const auto result = headers::factory_from_data(version, data);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream)
{
    static const headers expected
    {
        {
            {
                15,
                hash_literal("00acadae0019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
                hash_literal("4a5e1e4bbbccddee32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
                542344,
                1247742,
                34632,
                146
            }
        }
    };

    static const auto version = version::level::minimum;
    const auto data = expected.to_data(version);
    data_source istream(data);
    auto result = headers::factory_from_data(version, istream);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader)
{
    static const headers expected
    {
        {
            {
                7,
                hash_literal("1234123412341234123412341234123412341234123412341234123412341234"),
                hash_literal("4321432143214321432143214321432143214321432143214321432143214321"),
                83221,
                4353212,
                54234,
                367
            }
        }
    };

    static const auto version = version::level::minimum;
    const auto data = expected.to_data(version);
    data_source istream(data);
    istream_reader source(istream);
    const auto result = headers::factory_from_data(version, source);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_SUITE_END()

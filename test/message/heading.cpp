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

BOOST_AUTO_TEST_SUITE(heading_tests)

BOOST_AUTO_TEST_CASE(to_data_checksum_variations)
{
    heading instance
    {
        32414u,
        "foo",
        56731u,
        0u
    };

    const auto zero_checksum = instance.to_data();
    BOOST_REQUIRE_EQUAL(zero_checksum.size(), heading::serialized_size());

    instance.checksum = 123u;
    const auto nonzero_checksum = instance.to_data();
    BOOST_REQUIRE_EQUAL(nonzero_checksum.size(), heading::serialized_size());
}

BOOST_AUTO_TEST_CASE(from_data_insufficient_bytes_failure)
{
    static const data_chunk raw
    {
        0xab, 0xcd
    };

    heading instance;
    BOOST_REQUIRE(!instance.from_data(raw));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk)
{
    static const heading expected
    {
        32414u,
        "foo",
        56731u,
        0u
    };

    const auto data = expected.to_data();
    const auto result = heading::factory_from_data(data);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), heading::serialized_size());
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream)
{
    static const heading expected
    {
        29145u,
        "bar",
        79531u,
        0u
    };

    const auto data = expected.to_data();
    data_source istream(data);
    const auto result = heading::factory_from_data(istream);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), heading::serialized_size());
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader)
{
    static const heading expected
    {
        1u,
        "bazbazbazbaz",
        2u,
        0u
    };

    const auto data = expected.to_data();
    data_source istream(data);
    istream_reader source(istream);
    const auto result = heading::factory_from_data(source);
    BOOST_REQUIRE_EQUAL(data.size(), heading::serialized_size());
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_SUITE_END()

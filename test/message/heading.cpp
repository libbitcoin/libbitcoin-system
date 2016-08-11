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

BOOST_AUTO_TEST_SUITE(heading_tests)

BOOST_AUTO_TEST_CASE(to_data_checksum_variations)
{
    message::heading instance
    {
        32414u,
        "foo",
        56731u,
        0u
    };

    auto zero_checksum = instance.to_data(message::version::level::minimum);

    BOOST_REQUIRE_EQUAL(zero_checksum.size(),
        message::heading::serialized_size(message::version::level::minimum));

    instance.checksum = 123u;
    const auto nonzero_checksum = instance.to_data(message::version::level::minimum);

    BOOST_REQUIRE_EQUAL(nonzero_checksum.size(),
        message::heading::serialized_size(message::version::level::minimum));
}

BOOST_AUTO_TEST_CASE(from_data_insufficient_bytes_failure)
{
    data_chunk raw{ 0xab, 0xcd };
    message::heading instance{};

    BOOST_REQUIRE_EQUAL(false, instance.from_data(message::version::level::minimum, raw));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk)
{
    const message::heading expected
    {
        32414u,
        "foo",
        56731u,
        0u
    };

    const auto data = expected.to_data(message::version::level::minimum);
    const auto result = message::heading::factory_from_data(
        message::version::level::minimum, data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(),
        message::heading::serialized_size(message::version::level::minimum));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream)
{
    const message::heading expected
    {
        29145u,
        "bar",
        79531u,
        0u
    };

    const auto data = expected.to_data(message::version::level::minimum);
    data_source istream(data);
    const auto result = message::heading::factory_from_data(
        message::version::level::minimum, istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(),
        message::heading::serialized_size(message::version::level::minimum));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader)
{
    const  message::heading expected
    {
        1u,
        "bazbazbazbaz",
        2u,
        0u
    };

    const auto data = expected.to_data(message::version::level::minimum);
    data_source istream(data);
    istream_reader source(istream);
    const auto result = message::heading::factory_from_data(
        message::version::level::minimum, source);

    BOOST_REQUIRE_EQUAL(data.size(),
        message::heading::serialized_size(message::version::level::minimum));
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_SUITE_END()

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

bool equal(const message::network_address& left,
    const message::network_address& right, bool with_timestamp)
{
    bool matches_timestamp = with_timestamp ? 
        (left.timestamp == right.timestamp) : true;

    return matches_timestamp
        && (left.services == right.services)
        && (left.ip == right.ip)
        && (left.port == right.port);
}

BOOST_AUTO_TEST_SUITE(network_address_tests)

BOOST_AUTO_TEST_CASE(from_data_insufficient_bytes_failure)
{
    const data_chunk raw{ 1 };
    message::network_address instance{};

    BOOST_REQUIRE_EQUAL(false, instance.from_data(
        peer_minimum_version, raw, false));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk_without_timestamp)
{
    const message::network_address expected
    {
        734678u,
        5357534u,
        {
            {
                0x47, 0x81, 0x6a, 0x40, 0xbb, 0x92, 0xbd, 0xb4,
                0xe0, 0xb8, 0x25, 0x68, 0x61, 0xf9, 0x6a, 0x55
            }
        },
        123u
    };

    const auto data = expected.to_data(peer_minimum_version, false);
    const auto result = message::network_address::factory_from_data(
        peer_minimum_version, data, false);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, false));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(peer_minimum_version, false));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(peer_minimum_version, false),
        result.serialized_size(peer_minimum_version, false));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream_without_timestamp)
{
    const message::network_address expected
    {
        734678u,
        5357534u,
        {
            {
                0x47, 0x81, 0x6a, 0x40, 0xbb, 0x92, 0xbd, 0xb4,
                0xe0, 0xb8, 0x25, 0x68, 0x61, 0xf9, 0x6a, 0x55
            }
        },
        123u
    };

    const auto data = expected.to_data(peer_minimum_version, false);
    data_source istream(data);
    const auto result = message::network_address::factory_from_data(
        peer_minimum_version, istream, false);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, false));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(peer_minimum_version, false));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(peer_minimum_version, false),
        result.serialized_size(peer_minimum_version, false));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader_without_timestamp)
{
    const message::network_address expected
    {
        734678u,
        5357534u,
        {
            {
                0x47, 0x81, 0x6a, 0x40, 0xbb, 0x92, 0xbd, 0xb4,
                0xe0, 0xb8, 0x25, 0x68, 0x61, 0xf9, 0x6a, 0x55
            }
        },
        123u
    };

    const auto data = expected.to_data(peer_minimum_version, false);
    data_source istream(data);
    istream_reader source(istream);
    const auto result = message::network_address::factory_from_data(
        peer_minimum_version, source, false);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, false));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(peer_minimum_version, false));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(peer_minimum_version, false),
        result.serialized_size(peer_minimum_version, false));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk_with_timestamp)
{
    const message::network_address expected
    {
        734678u,
        5357534u,
        {
            {
                0x47, 0x81, 0x6a, 0x40, 0xbb, 0x92, 0xbd, 0xb4,
                0xe0, 0xb8, 0x25, 0x68, 0x61, 0xf9, 0x6a, 0x55
            }
        },
        123u
    };

    const auto data = expected.to_data(peer_minimum_version, true);
    const auto result = message::network_address::factory_from_data(
        peer_minimum_version, data, true);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, true));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(peer_minimum_version, true));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(peer_minimum_version, true),
        result.serialized_size(peer_minimum_version, true));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream_with_timestamp)
{
    const message::network_address expected
    {
        734678u,
        5357534u,
        {
            {
                0x47, 0x81, 0x6a, 0x40, 0xbb, 0x92, 0xbd, 0xb4,
                0xe0, 0xb8, 0x25, 0x68, 0x61, 0xf9, 0x6a, 0x55
            }
        },
        123u
    };

    const auto data = expected.to_data(peer_minimum_version, true);
    data_source istream(data);
    const auto result = message::network_address::factory_from_data(
        peer_minimum_version, istream, true);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, true));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(peer_minimum_version, true));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(peer_minimum_version, true),
        result.serialized_size(peer_minimum_version, true));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader_with_timestamp)
{
    const message::network_address expected
    {
        734678u,
        5357534u,
        {
            {
                0x47, 0x81, 0x6a, 0x40, 0xbb, 0x92, 0xbd, 0xb4,
                0xe0, 0xb8, 0x25, 0x68, 0x61, 0xf9, 0x6a, 0x55
            }
        },
        123u
    };

    const auto data = expected.to_data(peer_minimum_version, true);
    data_source istream(data);
    istream_reader source(istream);
    const auto result = message::network_address::factory_from_data(
        peer_minimum_version, source, true);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, true));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(peer_minimum_version, true));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(peer_minimum_version, true),
        result.serialized_size(peer_minimum_version, true));
}

BOOST_AUTO_TEST_SUITE_END()

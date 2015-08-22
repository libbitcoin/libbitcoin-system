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

bool equal(const message::network_address& address_a,
    const message::network_address& address_b, bool with_timestamp)
{
    bool matches_timestamp = with_timestamp ? (address_a.timestamp == address_b.timestamp) : true;

    return matches_timestamp
        && (address_a.services == address_b.services)
        && (address_a.ip == address_b.ip)
        && (address_a.port == address_b.port);
}

BOOST_AUTO_TEST_SUITE(network_address_tests)

BOOST_AUTO_TEST_CASE(from_data_insufficient_bytes_failure)
{
    data_chunk raw(1);
    message::network_address instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(raw, false));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk_without_timestamp)
{
    message::network_address expected
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

    data_chunk data = expected.to_data(false);

    auto result = message::network_address::factory_from_data(data, false);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, false));
    BOOST_REQUIRE_EQUAL(data.size(), result.satoshi_size(false));
    BOOST_REQUIRE_EQUAL(expected.satoshi_size(false), result.satoshi_size(false));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream_without_timestamp)
{
    message::network_address expected
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

    data_chunk data = expected.to_data(false);
    data_source istream(data);

    auto result = message::network_address::factory_from_data(istream, false);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, false));
    BOOST_REQUIRE_EQUAL(data.size(), result.satoshi_size(false));
    BOOST_REQUIRE_EQUAL(expected.satoshi_size(false), result.satoshi_size(false));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader_without_timestamp)
{
    message::network_address expected
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

    data_chunk data = expected.to_data(false);
    data_source istream(data);
    istream_reader source(istream);

    auto result = message::network_address::factory_from_data(source, false);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, false));
    BOOST_REQUIRE_EQUAL(data.size(), result.satoshi_size(false));
    BOOST_REQUIRE_EQUAL(expected.satoshi_size(false), result.satoshi_size(false));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk_with_timestamp)
{
    message::network_address expected
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

    data_chunk data = expected.to_data(true);

    auto result = message::network_address::factory_from_data(data, true);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, true));
    BOOST_REQUIRE_EQUAL(data.size(), result.satoshi_size(true));
    BOOST_REQUIRE_EQUAL(expected.satoshi_size(true), result.satoshi_size(true));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream_with_timestamp)
{
    message::network_address expected
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

    data_chunk data = expected.to_data(true);
    data_source istream(data);

    auto result = message::network_address::factory_from_data(istream, true);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, true));
    BOOST_REQUIRE_EQUAL(data.size(), result.satoshi_size(true));
    BOOST_REQUIRE_EQUAL(expected.satoshi_size(true), result.satoshi_size(true));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader_with_timestamp)
{
    message::network_address expected
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

    data_chunk data = expected.to_data(true);
    data_source istream(data);
    istream_reader source(istream);

    auto result = message::network_address::factory_from_data(source, true);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result, true));
    BOOST_REQUIRE_EQUAL(data.size(), result.satoshi_size(true));
    BOOST_REQUIRE_EQUAL(expected.satoshi_size(true), result.satoshi_size(true));
}

BOOST_AUTO_TEST_SUITE_END()

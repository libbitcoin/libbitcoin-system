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

bool equal_without_timestamp(const message::network_address& left,
    const message::network_address& right)
{
    return (left.services == right.services)
        && (left.ip == right.ip)
        && (left.port == right.port);
}

// Compares all values except the ipaddress timestamps.
bool equal(const message::version& left, const message::version& right)
{
    return (left.value == right.value)
        && (left.services_sender == right.services_sender)
        && (left.timestamp == right.timestamp)
        && equal_without_timestamp(left.address_recevier, right.address_recevier)
        && equal_without_timestamp(left.address_sender, right.address_sender)
        && (left.nonce == right.nonce)
        && (left.user_agent == right.user_agent)
        && (left.start_height == right.start_height)
        && (left.relay == right.relay);
}

BOOST_AUTO_TEST_SUITE(version_tests)

BOOST_AUTO_TEST_CASE(from_data_insufficient_bytes_failure)
{
    const data_chunk raw{ 0xab };
    message::version instance{};

    BOOST_REQUIRE_EQUAL(false, instance.from_data(peer_minimum_version, raw));
}

BOOST_AUTO_TEST_CASE(version__from_data_chunk__mismatched_sender_services__invalid)
{
    const auto sender_services = 1515u;
    const message::version expected
    {
        210u,
        sender_services,
        979797u,
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
        },
        {
            46324u,
            sender_services + 1,
            {
                {
                    0xab, 0xcd, 0x6a, 0x40, 0x33, 0x92, 0x77, 0xb4,
                    0xe0, 0xb8, 0xda, 0x43, 0x61, 0x66, 0x6a, 0x88
                }
            },
            351u
        },
        13626u,
        "my agent",
        100u
    };

    const auto data = expected.to_data(peer_minimum_version);
    const auto result = message::version::factory_from_data(
        peer_minimum_version, data);

    BOOST_REQUIRE(!result.is_valid());
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk)
{
    const auto sender_services = 1515u;
    const message::version expected
    {
        210u,
        sender_services,
        979797u,
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
        },
        {
            46324u,
            sender_services,
            {
                {
                    0xab, 0xcd, 0x6a, 0x40, 0x33, 0x92, 0x77, 0xb4,
                    0xe0, 0xb8, 0xda, 0x43, 0x61, 0x66, 0x6a, 0x88
                }
            },
            351u
        },
        13626u,
        "my agent",
        100u
    };

    const auto data = expected.to_data(peer_minimum_version);
    const auto result = message::version::factory_from_data(
        peer_minimum_version, data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(peer_minimum_version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(peer_minimum_version),
        result.serialized_size(peer_minimum_version));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream)
{
    const auto sender_services = 1515u;
    const message::version expected
    {
        210u,
        sender_services,
        979797u,
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
        },
        {
            46324u,
            sender_services,
            {
                {
                    0xab, 0xcd, 0x6a, 0x40, 0x33, 0x92, 0x77, 0xb4,
                    0xe0, 0xb8, 0xda, 0x43, 0x61, 0x66, 0x6a, 0x88
                }
            },
            351u
        },
        13626u,
        "my agent",
        100u
    };

    const auto data = expected.to_data(peer_minimum_version);
    data_source istream(data);
    const auto result = message::version::factory_from_data(
        peer_minimum_version, istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(peer_minimum_version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(peer_minimum_version),
        result.serialized_size(peer_minimum_version));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader)
{
    const auto sender_services = 1515u;
    const message::version expected
    {
        210u,
        sender_services,
        979797u,
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
        },
        {
            46324u,
            sender_services,
            {
                {
                    0xab, 0xcd, 0x6a, 0x40, 0x33, 0x92, 0x77, 0xb4,
                    0xe0, 0xb8, 0xda, 0x43, 0x61, 0x66, 0x6a, 0x88
                }
            },
            351u
        },
        13626u,
        "my agent",
        100u
    };

    const auto data = expected.to_data(peer_minimum_version);
    data_source istream(data);
    istream_reader source(istream);
    const auto result = message::version::factory_from_data(
        peer_minimum_version, source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(peer_minimum_version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(peer_minimum_version),
        result.serialized_size(peer_minimum_version));
}

BOOST_AUTO_TEST_SUITE_END()

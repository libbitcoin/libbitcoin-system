/*
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

bool equal_address_without_timestamp(const message::network_address& address_a,
    const message::network_address& address_b)
{
    return (address_a.services == address_b.services)
        && (address_a.ip == address_b.ip)
        && (address_a.port == address_b.port);
}

bool equal(const message::announce_version& a,
    const message::announce_version& b)
{
    return (a.version == b.version)
        && (a.services == b.services)
        && (a.timestamp == b.timestamp)
        && equal_address_without_timestamp(a.address_me, b.address_me)
        && equal_address_without_timestamp(a.address_you, b.address_you)
        && (a.nonce == b.nonce)
        && (a.user_agent == b.user_agent)
        && (a.start_height == b.start_height);
}

BOOST_AUTO_TEST_SUITE(announce_version_tests)

BOOST_AUTO_TEST_CASE(from_data_insufficient_bytes_failure)
{
    data_chunk raw{ 0xab };
    message::announce_version instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(raw));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk)
{
    message::announce_version expected{
        210u,
        1515u,
        979797u,
        message::network_address{
            734678u,
            5357534u,
            {
                0x47, 0x81, 0x6a, 0x40, 0xbb, 0x92, 0xbd, 0xb4,
                0xe0, 0xb8, 0x25, 0x68, 0x61, 0xf9, 0x6a, 0x55
            },
            123u
        },
        message::network_address{
            46324u,
            861275u,
            {
                0xab, 0xcd, 0x6a, 0x40, 0x33, 0x92, 0x77, 0xb4,
                0xe0, 0xb8, 0xda, 0x43, 0x61, 0x66, 0x6a, 0x88
            },
            351u
        },
        13626u,
        "my agent",
        100u
    };

    data_chunk data = expected.to_data();

    auto result = message::announce_version::factory_from_data(data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result));
    BOOST_REQUIRE_EQUAL(data.size(), result.satoshi_size());
    BOOST_REQUIRE_EQUAL(expected.satoshi_size(), result.satoshi_size());
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream)
{
    message::announce_version expected{
        210u,
        1515u,
        979797u,
        message::network_address{
            734678u,
            5357534u,
            {
                0x47, 0x81, 0x6a, 0x40, 0xbb, 0x92, 0xbd, 0xb4,
                0xe0, 0xb8, 0x25, 0x68, 0x61, 0xf9, 0x6a, 0x55
            },
            123u
        },
        message::network_address{
            46324u,
            861275u,
            {
                0xab, 0xcd, 0x6a, 0x40, 0x33, 0x92, 0x77, 0xb4,
                0xe0, 0xb8, 0xda, 0x43, 0x61, 0x66, 0x6a, 0x88
            },
            351u
        },
        13626u,
        "my agent",
        100u
    };

    data_chunk data = expected.to_data();
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);

    auto result = message::announce_version::factory_from_data(istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result));
    BOOST_REQUIRE_EQUAL(data.size(), result.satoshi_size());
    BOOST_REQUIRE_EQUAL(expected.satoshi_size(), result.satoshi_size());
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader)
{
    message::announce_version expected{
        210u,
        1515u,
        979797u,
        message::network_address{
            734678u,
            5357534u,
            {
                0x47, 0x81, 0x6a, 0x40, 0xbb, 0x92, 0xbd, 0xb4,
                0xe0, 0xb8, 0x25, 0x68, 0x61, 0xf9, 0x6a, 0x55
            },
            123u
        },
        message::network_address{
            46324u,
            861275u,
            {
                0xab, 0xcd, 0x6a, 0x40, 0x33, 0x92, 0x77, 0xb4,
                0xe0, 0xb8, 0xda, 0x43, 0x61, 0x66, 0x6a, 0x88
            },
            351u
        },
        13626u,
        "my agent",
        100u
    };

    data_chunk data = expected.to_data();
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    istream_reader source(istream);

    auto result = message::announce_version::factory_from_data(source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result));
    BOOST_REQUIRE_EQUAL(data.size(), result.satoshi_size());
    BOOST_REQUIRE_EQUAL(expected.satoshi_size(), result.satoshi_size());
}

BOOST_AUTO_TEST_SUITE_END()

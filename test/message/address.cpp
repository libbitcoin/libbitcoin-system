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

bool equal_address_with_timestamp(const message::network_address& address_a,
    const message::network_address& address_b)
{
    return (address_a.timestamp == address_b.timestamp)
        && (address_a.services == address_b.services)
        && (address_a.ip == address_b.ip)
        && (address_a.port == address_b.port);
}

bool equal(const message::address& a, const message::address& b)
{
    bool result = (a.addresses.size() == b.addresses.size());

    for (size_t i = 0; (i < a.addresses.size()) && result; i++)
        result = equal_address_with_timestamp(a.addresses[i], b.addresses[i]);

    return result;
}

BOOST_AUTO_TEST_SUITE(address_tests)

BOOST_AUTO_TEST_CASE(from_data_insufficient_bytes_failure)
{
    data_chunk raw{ 0xab };
    message::address instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(raw));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk)
{
    message::address expected
    {
        {
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
            }
        }
    };

    data_chunk data = expected.to_data();

    auto result = message::address::factory_from_data(data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result));
    BOOST_REQUIRE_EQUAL(data.size(), result.satoshi_size());
    BOOST_REQUIRE_EQUAL(expected.satoshi_size(), result.satoshi_size());
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream)
{
    message::address expected
    {
        {
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
            }
        }
    };

    data_chunk data = expected.to_data();
    data_source istream(data);

    auto result = message::address::factory_from_data(istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result));
    BOOST_REQUIRE_EQUAL(data.size(), result.satoshi_size());
    BOOST_REQUIRE_EQUAL(expected.satoshi_size(), result.satoshi_size());
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader)
{
    message::address expected
    {
        {
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
            }
        }
    };

    data_chunk data = expected.to_data();
    data_source istream(data);
    istream_reader source(istream);

    auto result = message::address::factory_from_data(source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result));
    BOOST_REQUIRE_EQUAL(data.size(), result.satoshi_size());
    BOOST_REQUIRE_EQUAL(expected.satoshi_size(), result.satoshi_size());
}

BOOST_AUTO_TEST_SUITE_END()

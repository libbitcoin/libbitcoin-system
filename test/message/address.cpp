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
    const message::network_address& right)
{
    return (left.timestamp == right.timestamp)
        && (left.services == right.services)
        && (left.ip == right.ip)
        && (left.port == right.port);
}

bool equal(const message::address& left, const message::address& right)
{
    bool same = (left.addresses.size() == right.addresses.size());

    for (size_t i = 0; (i < left.addresses.size()) && same; i++)
        same = equal(left.addresses[i], right.addresses[i]);

    return same;
}

BOOST_AUTO_TEST_SUITE(address_tests)

BOOST_AUTO_TEST_CASE(from_data_insufficient_bytes_failure)
{
    data_chunk raw{ 0xab };
    message::address instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(message::version::level::minimum, raw));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk)
{
    const message::address expected
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

    const auto data = expected.to_data(message::version::level::minimum);
    const auto result = message::address::factory_from_data(
        message::version::level::minimum, data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result));
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(message::version::level::minimum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(message::version::level::minimum), result.serialized_size(message::version::level::minimum));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream)
{
    const message::address expected
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

    const auto data = expected.to_data(message::version::level::minimum);
    data_source istream(data);
    const auto result = message::address::factory_from_data(
        message::version::level::minimum, istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result));
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(message::version::level::minimum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(message::version::level::minimum), result.serialized_size(message::version::level::minimum));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader)
{
    const message::address expected
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

    const data_chunk data = expected.to_data(message::version::level::minimum);
    data_source istream(data);
    istream_reader source(istream);
    const auto result = message::address::factory_from_data(
        message::version::level::minimum, source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result));
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(message::version::level::minimum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(message::version::level::minimum), result.serialized_size(message::version::level::minimum));
}

BOOST_AUTO_TEST_SUITE_END()

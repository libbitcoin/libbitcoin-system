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

BOOST_AUTO_TEST_SUITE(get_blocks_tests)

BOOST_AUTO_TEST_CASE(from_data_insufficient_bytes_failure)
{
    const data_chunk raw{ 0xab, 0xcd };
    message::get_blocks instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(message::version::level::minimum, raw));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk)
{
    const message::get_blocks expected
    {
        {
            {
                {
                    0x78, 0x51, 0x04, 0xa9, 0xcb, 0x52, 0xb3, 0x77,
                    0x53, 0x9c, 0x8a, 0x27, 0x02, 0xab, 0x70, 0xc7,
                    0xc2, 0x04, 0xee, 0xb5, 0xd2, 0x78, 0x2e, 0x04,
                    0x76, 0xef, 0xb9, 0x9d, 0xc7, 0x5c, 0xda, 0x82,
                }
            },
            {
                {
                    0xda, 0x23, 0x97, 0x09, 0x59, 0x09, 0x9b, 0x46,
                    0x42, 0x69, 0x72, 0x7b, 0x72, 0x20, 0x7b, 0xcc,
                    0xc0, 0xf7, 0x80, 0xf4, 0x2d, 0xef, 0x0f, 0x79,
                    0x7c, 0x1f, 0x89, 0xab, 0xac, 0x92, 0x0b, 0x90
                }
            }
        },
        {
            {
                0x79, 0x22, 0x6e, 0x0a, 0xf7, 0x86, 0xaa, 0x71,
                0x6d, 0x31, 0x0c, 0x84, 0xc4, 0x8b, 0x6f, 0x62,
                0xb1, 0x20, 0xa6, 0x78, 0x2a, 0x98, 0x99, 0x93,
                0xc4, 0xc6, 0x16, 0x31, 0xe2, 0x40, 0x1d, 0xf5
            }
        }
    };

    const auto data = expected.to_data(message::version::level::minimum);
    const auto result = message::get_blocks::factory_from_data(
        message::version::level::minimum, data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(message::version::level::minimum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(message::version::level::minimum),
        result.serialized_size(message::version::level::minimum));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream)
{
    const message::get_blocks expected
    {
        {
            {
                {
                    0x78, 0x51, 0x04, 0xa9, 0xcb, 0x52, 0xb3, 0x77,
                    0x53, 0x9c, 0x8a, 0x27, 0x02, 0xab, 0x70, 0xc7,
                    0xc2, 0x04, 0xee, 0xb5, 0xd2, 0x78, 0x2e, 0x04,
                    0x76, 0xef, 0xb9, 0x9d, 0xc7, 0x5c, 0xda, 0x82
                }
            },
            {
                {
                    0xda, 0x23, 0x97, 0x09, 0x59, 0x09, 0x9b, 0x46,
                    0x42, 0x69, 0x72, 0x7b, 0x72, 0x20, 0x7b, 0xcc,
                    0xc0, 0xf7, 0x80, 0xf4, 0x2d, 0xef, 0x0f, 0x79,
                    0x7c, 0x1f, 0x89, 0xab, 0xac, 0x92, 0x0b, 0x90
                }
            }
        },
        {
            {
                0x79, 0x22, 0x6e, 0x0a, 0xf7, 0x86, 0xaa, 0x71,
                0x6d, 0x31, 0x0c, 0x84, 0xc4, 0x8b, 0x6f, 0x62,
                0xb1, 0x20, 0xa6, 0x78, 0x2a, 0x98, 0x99, 0x93,
                0xc4, 0xc6, 0x16, 0x31, 0xe2, 0x40, 0x1d, 0xf5
            }
        }
    };

    const auto data = expected.to_data(message::version::level::minimum);
    data_source istream(data);
    const auto result = message::get_blocks::factory_from_data(
        message::version::level::minimum, istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(message::version::level::minimum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(message::version::level::minimum),
        result.serialized_size(message::version::level::minimum));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader)
{
    const message::get_blocks expected
    {
        {
            {
                {
                    0x78, 0x51, 0x04, 0xa9, 0xcb, 0x52, 0xb3, 0x77,
                    0x53, 0x9c, 0x8a, 0x27, 0x02, 0xab, 0x70, 0xc7,
                    0xc2, 0x04, 0xee, 0xb5, 0xd2, 0x78, 0x2e, 0x04,
                    0x76, 0xef, 0xb9, 0x9d, 0xc7, 0x5c, 0xda, 0x82
                }
            },
            {
                {
                    0xda, 0x23, 0x97, 0x09, 0x59, 0x09, 0x9b, 0x46,
                    0x42, 0x69, 0x72, 0x7b, 0x72, 0x20, 0x7b, 0xcc,
                    0xc0, 0xf7, 0x80, 0xf4, 0x2d, 0xef, 0x0f, 0x79,
                    0x7c, 0x1f, 0x89, 0xab, 0xac, 0x92, 0x0b, 0x90
                }
            }
        },
        {
            {
                0x79, 0x22, 0x6e, 0x0a, 0xf7, 0x86, 0xaa, 0x71,
                0x6d, 0x31, 0x0c, 0x84, 0xc4, 0x8b, 0x6f, 0x62,
                0xb1, 0x20, 0xa6, 0x78, 0x2a, 0x98, 0x99, 0x93,
                0xc4, 0xc6, 0x16, 0x31, 0xe2, 0x40, 0x1d, 0xf5
            }
        }
    };

    const auto data = expected.to_data(message::version::level::minimum);
    data_source istream(data);
    istream_reader source(istream);
    const auto result = message::get_blocks::factory_from_data(
        message::version::level::minimum, source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(message::version::level::minimum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(message::version::level::minimum),
        result.serialized_size(message::version::level::minimum));
}

BOOST_AUTO_TEST_SUITE_END()

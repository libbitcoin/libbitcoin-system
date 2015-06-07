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
#include "../genesis_block.hpp"

using namespace bc;

BOOST_AUTO_TEST_SUITE(block_header_tests)

BOOST_AUTO_TEST_CASE(from_data_fails)
{
    data_chunk data(10);

    chain::block_header header;

    BOOST_REQUIRE_EQUAL(false, header.from_data(data));
    BOOST_REQUIRE_EQUAL(false, header.is_valid());
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk)
{
    chain::block_header expected {
        10,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234,
        6523454,
        68644
    };

    data_chunk data = expected.to_data();

    auto result = chain::block_header::factory_from_data(data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream)
{
    chain::block_header expected {
        10,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234,
        6523454,
        68644
    };

    data_chunk data = expected.to_data();
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);

    auto result = chain::block_header::factory_from_data(istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader)
{
    chain::block_header expected {
        10,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234,
        6523454,
        68644
    };

    data_chunk data = expected.to_data();
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    istream_reader source(istream);

    auto result = chain::block_header::factory_from_data(source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_SUITE_END()

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
#include <boost/iostreams/stream.hpp>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

data_chunk valid_raw_output = to_chunk(base16_literal(
    "20300500000000001976a914905f933de850988603aafeeb2fd7fce61e66fe5d88ac"));

BOOST_AUTO_TEST_SUITE(output_tests)

BOOST_AUTO_TEST_CASE(from_data_fails)
{
    data_chunk data(2);

    chain::output instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(data));
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(factory_data_chunk_success)
{
    auto instance = chain::output::factory_from_data(valid_raw_output);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.serialized_size(), valid_raw_output.size());

    // Re-save and compare against original.
    data_chunk resave = instance.to_data();
    BOOST_REQUIRE_EQUAL(resave.size(), valid_raw_output.size());
    BOOST_REQUIRE(resave == valid_raw_output);
}

BOOST_AUTO_TEST_CASE(factory_stream_success)
{
    data_source stream(valid_raw_output);
    auto instance = chain::output::factory_from_data(stream);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.serialized_size(), valid_raw_output.size());

    // Re-save and compare against original.
    data_chunk resave = instance.to_data();
    BOOST_REQUIRE_EQUAL(resave.size(), valid_raw_output.size());
    BOOST_REQUIRE(resave == valid_raw_output);
}

BOOST_AUTO_TEST_CASE(factory_reader_success)
{
    data_source stream(valid_raw_output);
    istream_reader source(stream);
    auto instance = chain::output::factory_from_data(source);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.serialized_size(), valid_raw_output.size());

    // Re-save and compare against original.
    data_chunk resave = instance.to_data();
    BOOST_REQUIRE_EQUAL(resave.size(), valid_raw_output.size());
    BOOST_REQUIRE(resave == valid_raw_output);
}

BOOST_AUTO_TEST_SUITE_END()

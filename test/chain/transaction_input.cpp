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

data_chunk valid_raw_input = to_data_chunk(base16_literal(
    "54b755c39207d443fd96a8d12c94446a1c6f66e39c95e894c23418d7501f681b01000"
    "0006b48304502203267910f55f2297360198fff57a3631be850965344370f732950b4"
    "7795737875022100f7da90b82d24e6e957264b17d3e5042bab8946ee5fc676d15d915"
    "da450151d36012103893d5a06201d5cf61400e96fa4a7514fc12ab45166ace618d68b"
    "8066c9c585f9ffffffff"));

BOOST_AUTO_TEST_SUITE(transaction_input_tests)

BOOST_AUTO_TEST_CASE(from_data_fails)
{
    data_chunk data(2);

    chain::transaction_input instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(data));
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(from_data_valid_junk)
{
    auto junk = base16_literal(
        "000000000000005739943a9c29a1955dfae2b3f37de547005bfb9535192e5fb0"
        "000000000000005739943a9c29a1955dfae2b3f37de547005bfb9535192e5fb0");
    // data_chunk_stream_host host(junk);
    byte_source<std::array<uint8_t, 64>> source(junk);
    boost::iostreams::stream<byte_source<std::array<uint8_t, 64>>> stream(source);

    chain::transaction_input instance;
    BOOST_REQUIRE(instance.from_data(stream));
}

BOOST_AUTO_TEST_CASE(factory_data_chunk_success)
{
    auto instance = chain::transaction_input::factory_from_data(valid_raw_input);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.satoshi_size(), valid_raw_input.size());

    // Re-save and compare against original.
    data_chunk resave = instance.to_data();
    BOOST_REQUIRE_EQUAL(resave.size(), valid_raw_input.size());
    BOOST_REQUIRE(resave == valid_raw_input);
}

BOOST_AUTO_TEST_CASE(factory_stream_success)
{
    data_source stream(valid_raw_input);
    auto instance = chain::transaction_input::factory_from_data(stream);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.satoshi_size(), valid_raw_input.size());

    // Re-save and compare against original.
    data_chunk resave = instance.to_data();
    BOOST_REQUIRE_EQUAL(resave.size(), valid_raw_input.size());
    BOOST_REQUIRE(resave == valid_raw_input);
}

BOOST_AUTO_TEST_CASE(factory_reader_success)
{
    data_source stream(valid_raw_input);
    istream_reader source(stream);
    auto instance = chain::transaction_input::factory_from_data(source);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.satoshi_size(), valid_raw_input.size());

    // Re-save and compare against original.
    data_chunk resave = instance.to_data();
    BOOST_REQUIRE_EQUAL(resave.size(), valid_raw_input.size());
    BOOST_REQUIRE(resave == valid_raw_input);
}

BOOST_AUTO_TEST_SUITE_END()

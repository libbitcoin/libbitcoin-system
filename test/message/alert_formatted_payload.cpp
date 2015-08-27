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

BOOST_AUTO_TEST_SUITE(alert_formatted_payload_tests)

BOOST_AUTO_TEST_CASE(from_data_insufficient_bytes_failure)
{
    data_chunk raw{ 0xab, 0x11 };
    message::alert_formatted_payload instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(raw));
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(wiki_sample_test)
{
    const data_chunk raw
    {
        0x01, 0x00, 0x00, 0x00, 0x37, 0x66, 0x40, 0x4f, 0x00, 0x00,
        0x00, 0x00, 0xb3, 0x05, 0x43, 0x4f, 0x00, 0x00, 0x00, 0x00,
        0xf2, 0x03, 0x00, 0x00, 0xf1, 0x03, 0x00, 0x00, 0x00, 0x10,
        0x27, 0x00, 0x00, 0x48, 0xee, 0x00, 0x00, 0x00, 0x64, 0x00,
        0x00, 0x00, 0x00, 0x46, 0x53, 0x65, 0x65, 0x20, 0x62, 0x69,
        0x74, 0x63, 0x6f, 0x69, 0x6e, 0x2e, 0x6f, 0x72, 0x67, 0x2f,
        0x66, 0x65, 0x62, 0x32, 0x30, 0x20, 0x69, 0x66, 0x20, 0x79,
        0x6f, 0x75, 0x20, 0x68, 0x61, 0x76, 0x65, 0x20, 0x74, 0x72,
        0x6f, 0x75, 0x62, 0x6c, 0x65, 0x20, 0x63, 0x6f, 0x6e, 0x6e,
        0x65, 0x63, 0x74, 0x69, 0x6e, 0x67, 0x20, 0x61, 0x66, 0x74,
        0x65, 0x72, 0x20, 0x32, 0x30, 0x20, 0x46, 0x65, 0x62, 0x72,
        0x75, 0x61, 0x72, 0x79, 0x00
    };

    const message::alert_formatted_payload expected
    {
        1,
        1329620535,
        1329792435,
        1010,
        1009,
        std::vector<uint32_t>{},
        10000,
        61000,
        std::vector<std::string>{},
        100,
        "",
        "See bitcoin.org/feb20 if you have trouble connecting after 20 February",
        ""
    };

    const auto result = message::alert_formatted_payload::factory_from_data(raw);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE_EQUAL(raw.size(), result.satoshi_size());
    BOOST_REQUIRE(result == expected);

    const auto data = expected.to_data();

    BOOST_REQUIRE_EQUAL(raw.size(), data.size());
    BOOST_REQUIRE_EQUAL(data.size(), expected.satoshi_size());
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk)
{
    message::alert_formatted_payload expected
    {
        5,
        105169,
        723544,
        1779,
        1678,
        {
            10, 25256, 37, 98485, 250
        },
        75612,
        81354,
        {
            "alpha", "beta", "gamma", "delta"
        },
        781,
        "My Comment",
        "My Status Bar",
        "RESERVED?"
    };

    const auto data = expected.to_data();
    const auto result = message::alert_formatted_payload::factory_from_data(data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.satoshi_size());
    BOOST_REQUIRE_EQUAL(expected.satoshi_size(), result.satoshi_size());
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream)
{
    message::alert_formatted_payload expected
    {
        5,
        105169,
        723544,
        1779,
        1678,
        {
            10, 25256, 37, 98485, 250
        },
        75612,
        81354,
        {
            "alpha", "beta", "gamma", "delta"
        },
        781,
        "My Comment",
        "My Status Bar",
        "RESERVED?"
    };

    const auto data = expected.to_data();
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    const auto result = message::alert_formatted_payload::factory_from_data(istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.satoshi_size());
    BOOST_REQUIRE_EQUAL(expected.satoshi_size(), result.satoshi_size());
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader)
{
    const message::alert_formatted_payload expected
    {
        5,
        105169,
        723544,
        1779,
        1678,
        {
            10, 25256, 37, 98485, 250
        },
        75612,
        81354,
        {
            "alpha", "beta", "gamma", "delta"
        },
        781,
        "My Comment",
        "My Status Bar",
        "RESERVED?"
    };

    const auto data = expected.to_data();
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    istream_reader source(istream);
    const auto result = message::alert_formatted_payload::factory_from_data(source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.satoshi_size());
    BOOST_REQUIRE_EQUAL(expected.satoshi_size(), result.satoshi_size());
}

BOOST_AUTO_TEST_SUITE_END()

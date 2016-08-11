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
#include <boost/iostreams/stream.hpp>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(point_tests)

BOOST_AUTO_TEST_CASE(begin_end_test)
{
    chain::point instance{ null_hash, 0 };

    BOOST_REQUIRE(instance.begin() != instance.end());
}

BOOST_AUTO_TEST_CASE(from_data_fails)
{
    data_chunk data(10);
    chain::point instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(data));
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(to_data_from_data_roundtrip)
{
    uint32_t index = 53213;
    hash_digest hash
    {
        {
            0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
            0x01, 0x01, 0xab, 0x11, 0x11, 0xcd, 0x11, 0x11,
            0x01, 0x10, 0x11, 0xab, 0x11, 0x11, 0xcd, 0x11,
            0x01, 0x11, 0x11, 0x11, 0xab, 0x11, 0x11, 0xcd
        }
    };

    chain::point initial{ hash, index };

    BOOST_REQUIRE(initial.is_valid());
    BOOST_REQUIRE(hash == initial.hash);
    BOOST_REQUIRE(index == initial.index);

    chain::point point;

    BOOST_REQUIRE(point != initial);
    BOOST_REQUIRE(point.from_data(initial.to_data()));
    BOOST_REQUIRE(point.is_valid());
    BOOST_REQUIRE(point == initial);
}

BOOST_AUTO_TEST_CASE(from_data_to_data_roundtrip_factory_data_chunk)
{
    data_chunk rawdata = to_chunk(base16_literal(
        "46682488f0a721124a3905a1bb72445bf13493e2cd46c5c0c8db1c15afa0d58e00000000"
    ));

    BOOST_REQUIRE(rawdata == (data_chunk
    {
        0x46, 0x68, 0x24, 0x88, 0xf0, 0xa7, 0x21, 0x12, 0x4a, 0x39, 0x05, 0xa1,
        0xbb, 0x72, 0x44, 0x5b, 0xf1, 0x34, 0x93, 0xe2, 0xcd, 0x46, 0xc5, 0xc0,
        0xc8, 0xdb, 0x1c, 0x15, 0xaf, 0xa0, 0xd5, 0x8e, 0x00, 0x00, 0x00, 0x00
    }));

    auto point = chain::point::factory_from_data(rawdata);

    BOOST_REQUIRE(point.is_valid());
    BOOST_REQUIRE_EQUAL(encode_hash(point.hash), "8ed5a0af151cdbc8c0c546cde29334f15b4472bba105394a1221a7f088246846");
    BOOST_REQUIRE(point.index == 0);

    data_chunk output = point.to_data();
    BOOST_REQUIRE(output == rawdata);
}

BOOST_AUTO_TEST_CASE(from_data_to_data_roundtrip_factory_stream)
{
    data_chunk rawdata = to_chunk(base16_literal(
        "46682488f0a721124a3905a1bb72445bf13493e2cd46c5c0c8db1c15afa0d58e00000000"
    ));

    BOOST_REQUIRE(rawdata == (data_chunk
    {
        0x46, 0x68, 0x24, 0x88, 0xf0, 0xa7, 0x21, 0x12, 0x4a, 0x39, 0x05, 0xa1,
        0xbb, 0x72, 0x44, 0x5b, 0xf1, 0x34, 0x93, 0xe2, 0xcd, 0x46, 0xc5, 0xc0,
        0xc8, 0xdb, 0x1c, 0x15, 0xaf, 0xa0, 0xd5, 0x8e, 0x00, 0x00, 0x00, 0x00
    }));

    data_source istream(rawdata);
    auto point = chain::point::factory_from_data(istream);

    BOOST_REQUIRE(point.is_valid());
    BOOST_REQUIRE_EQUAL(encode_hash(point.hash), "8ed5a0af151cdbc8c0c546cde29334f15b4472bba105394a1221a7f088246846");
    BOOST_REQUIRE(point.index == 0);

    data_chunk output = point.to_data();
    BOOST_REQUIRE(output == rawdata);
}

BOOST_AUTO_TEST_CASE(from_data_to_data_roundtrip_factory_reader)
{
    data_chunk rawdata = to_chunk(base16_literal(
        "46682488f0a721124a3905a1bb72445bf13493e2cd46c5c0c8db1c15afa0d58e00000000"
    ));

    BOOST_REQUIRE(rawdata == (data_chunk
    {
        0x46, 0x68, 0x24, 0x88, 0xf0, 0xa7, 0x21, 0x12, 0x4a, 0x39, 0x05, 0xa1,
        0xbb, 0x72, 0x44, 0x5b, 0xf1, 0x34, 0x93, 0xe2, 0xcd, 0x46, 0xc5, 0xc0,
        0xc8, 0xdb, 0x1c, 0x15, 0xaf, 0xa0, 0xd5, 0x8e, 0x00, 0x00, 0x00, 0x00
    }));

    data_source istream(rawdata);
    istream_reader source(istream);
    auto point = chain::point::factory_from_data(source);

    BOOST_REQUIRE(point.is_valid());
    BOOST_REQUIRE_EQUAL(encode_hash(point.hash), "8ed5a0af151cdbc8c0c546cde29334f15b4472bba105394a1221a7f088246846");
    BOOST_REQUIRE(point.index == 0);

    data_chunk output = point.to_data();
    BOOST_REQUIRE(output == rawdata);
}

BOOST_AUTO_TEST_SUITE_END()

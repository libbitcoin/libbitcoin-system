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
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(point_tests)

BOOST_AUTO_TEST_CASE(constructor_istream_to_data_roundtrip)
{
    data_chunk rawdata = to_data_chunk(base16_literal(
        "46682488f0a721124a3905a1bb72445bf13493e2cd46c5c0c8db1c15afa0d58e00000000"
    ));

    BOOST_REQUIRE(rawdata == (data_chunk
    {
        0x46, 0x68, 0x24, 0x88, 0xf0, 0xa7, 0x21, 0x12, 0x4a, 0x39, 0x05, 0xa1,
        0xbb, 0x72, 0x44, 0x5b, 0xf1, 0x34, 0x93, 0xe2, 0xcd, 0x46, 0xc5, 0xc0,
        0xc8, 0xdb, 0x1c, 0x15, 0xaf, 0xa0, 0xd5, 0x8e, 0x00, 0x00, 0x00, 0x00
    }));

    std::stringstream stream;
    std::copy(rawdata.begin(), rawdata.end(), std::ostream_iterator<uint8_t>(stream));

    chain::point point(stream);

    BOOST_REQUIRE_EQUAL(encode_hash(point.hash()),
        "8ed5a0af151cdbc8c0c546cde29334f15b4472bba105394a1221a7f088246846");

    BOOST_REQUIRE(point.index() == 0);

    data_chunk output = point;
    BOOST_REQUIRE(output == rawdata);
}

BOOST_AUTO_TEST_SUITE_END()

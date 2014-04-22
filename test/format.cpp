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

BOOST_AUTO_TEST_CASE(encode_test)
{
    const std::string hex_str = "10a7fd15cb45bda9e90e19a15f";
    BOOST_REQUIRE(hex_str.size() % 2 == 0);
    BOOST_REQUIRE(encode_hex(decode_hex(hex_str)) == hex_str);
    const std::string padded_hex = "  \n\t10a7fd15cb45bda9e90e19a15f\n  \t";
    BOOST_REQUIRE(encode_hex(decode_hex(padded_hex)) == hex_str);
}

BOOST_AUTO_TEST_CASE(endian_test)
{
    auto le = to_little_endian<uint32_t>(123456789);
    BOOST_REQUIRE(from_little_endian<uint32_t>(le.begin()) == 123456789);

    auto be = to_big_endian<uint32_t>(123456789);
    BOOST_REQUIRE(from_big_endian<uint32_t>(be.begin()) == 123456789);

    std::reverse(le.begin(), le.end());
    BOOST_REQUIRE(from_big_endian<uint32_t>(le.begin()) == 123456789);

    auto byte = to_little_endian<uint8_t>(0xff);
    BOOST_REQUIRE(from_big_endian<uint8_t>(byte.begin()) == 0xff);

    auto quad = to_little_endian<uint64_t>(0x1122334455667788);
    BOOST_REQUIRE(from_little_endian<uint64_t>(quad.begin()) ==
        0x1122334455667788);
}

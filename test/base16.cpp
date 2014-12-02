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

BOOST_AUTO_TEST_SUITE(base16_tests)

BOOST_AUTO_TEST_CASE(base16_literal_test)
{
    auto result = base16_literal("01ff42bc");
    byte_array<4> expected = {{0x01, 0xff, 0x42, 0xbc}};
    BOOST_REQUIRE(result == expected);
}

BOOST_AUTO_TEST_CASE(base16_odd_length_invalid_test)
{
    const auto& hex_str = "10a7fd15cb45bda9e90e19a15";
    data_chunk data;
    BOOST_REQUIRE(!decode_base16(data, hex_str));
}

// TODO: these should be tested for correctness, not just round-tripping.

BOOST_AUTO_TEST_CASE(base16_round_trip_test)
{
    const auto& hex_str = "10a7fd15cb45bda9e90e19a15f";
    data_chunk data;
    BOOST_REQUIRE(decode_base16(data, hex_str));
    BOOST_REQUIRE_EQUAL(encode_base16(data), hex_str);
}

BOOST_AUTO_TEST_CASE(base16_legacy_padded_round_trip_test)
{
    const auto& padded_hex = "  \n\t10a7fd15cb45bda9e90e19a15f\n  \t";
    data_chunk data = decode_hex(padded_hex);

    const auto& unpadded_hex = "10a7fd15cb45bda9e90e19a15f";
    BOOST_REQUIRE_EQUAL(encode_hex(data), unpadded_hex);
}

BOOST_AUTO_TEST_SUITE_END()

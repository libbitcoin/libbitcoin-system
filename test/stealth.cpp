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

BOOST_AUTO_TEST_SUITE(stealth_tests)

BOOST_AUTO_TEST_CASE(verify_string_constructor)
{
    std::string repr = "01100110000";
    binary_type prefix(repr);
    BOOST_REQUIRE_EQUAL(repr.size(), prefix.size());
    for (size_t i = 0; i < repr.size(); ++i)
    {
        const bool value = repr[i] == '1';
        BOOST_REQUIRE_EQUAL(prefix[i] , value);
    }
}

BOOST_AUTO_TEST_CASE(compare_constructor_results)
{
    std::string repr = "01100111000";
    binary_type prefix(repr);
    // Binary repr as a value on the left, padded with zeros to the right.
    //  repr + 0000....000
    data_chunk blocks{{0x67, 0x00}};
    binary_type prefix2(repr.size(), blocks);
    BOOST_REQUIRE_EQUAL(prefix, prefix2);
}

BOOST_AUTO_TEST_CASE(bitfield_test1)
{
    binary_type prefix("01100111001");
    data_chunk raw_bitfield{{0x67, 0x20, 0x00, 0x0}};
    BOOST_REQUIRE_GE(raw_bitfield.size() * 8, prefix.size());
    binary_type compare(prefix.size(), raw_bitfield);
    BOOST_REQUIRE_EQUAL(prefix, compare);
}

BOOST_AUTO_TEST_CASE(bitfield_test2)
{
    data_chunk blocks1{{0x8b, 0xf4, 0x1c, 0x69}};
    binary_type prefix1(27, blocks1);
    data_chunk raw_bitfield1{ { 0x8b, 0xf4, 0x1c, 0x79 } };
    BOOST_REQUIRE_GE(raw_bitfield1.size() * 8, prefix1.size());
    binary_type compare1(prefix1.size(), raw_bitfield1);
    BOOST_REQUIRE_EQUAL(prefix1, compare1);
}

BOOST_AUTO_TEST_CASE(bitfield_test3)
{
    data_chunk blocks_bs{{0x69, 0x1c, 0xf4, 0x8b}};
    binary_type prefix_bs(32, blocks_bs);
    data_chunk raw_bitfield_bs{{0x69, 0x1c, 0xf4, 0x8b}};
    binary_type compare_bs(prefix_bs.size(), raw_bitfield_bs);
    BOOST_REQUIRE_EQUAL(prefix_bs, compare_bs);
}

BOOST_AUTO_TEST_CASE(bitfield_test4)
{
    data_chunk blocks_b{{0x69, 0x1c, 0xf4, 0x8b}};
    binary_type prefix_b(29, blocks_b);
    data_chunk raw_bitfield_bs{ { 0x69, 0x1c, 0xf4, 0x8b } };
    binary_type compare_b(prefix_b.size(), raw_bitfield_bs);
    BOOST_REQUIRE_EQUAL(prefix_b, compare_b);
}

BOOST_AUTO_TEST_SUITE_END()

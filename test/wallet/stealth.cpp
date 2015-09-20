/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
    const std::string binary = "01100110000";
    binary_type prefix(binary);
    BOOST_REQUIRE_EQUAL(binary.size(), prefix.size());
    for (size_t i = 0; i < binary.size(); ++i)
    {
        const auto value = binary[i] == '1';
        BOOST_REQUIRE_EQUAL(prefix[i] , value);
    }
}

// Binary as a value on the left, padded with zeros to the right.
BOOST_AUTO_TEST_CASE(compare_constructor_results)
{
    std::string binary = "01100111000";
    binary_type prefix(binary);
    data_chunk blocks{ { 0x67, 0x00 } };
    binary_type prefix2(binary.size(), blocks);
    BOOST_REQUIRE_EQUAL(prefix, prefix2);
}

BOOST_AUTO_TEST_CASE(bitfield_test1)
{
    binary_type prefix("01100111001");
    data_chunk raw_bitfield{ { 0x67, 0x20, 0x00, 0x0 } };
    BOOST_REQUIRE_GE(raw_bitfield.size() * 8, prefix.size());
    binary_type compare(prefix.size(), raw_bitfield);
    BOOST_REQUIRE_EQUAL(prefix, compare);
}

BOOST_AUTO_TEST_CASE(bitfield_test2)
{
    const data_chunk blocks{ { 0x8b, 0xf4, 0x1c, 0x69 } };
    const binary_type prefix(27, blocks);
    const data_chunk raw_bitfield{ { 0x8b, 0xf4, 0x1c, 0x79 } };
    BOOST_REQUIRE_GE(raw_bitfield.size() * 8, prefix.size());
    const binary_type compare(prefix.size(), raw_bitfield);
    BOOST_REQUIRE_EQUAL(prefix, compare);
}

BOOST_AUTO_TEST_CASE(bitfield_test3)
{
    const data_chunk blocks{ { 0x69, 0x1c, 0xf4, 0x8b } };
    const binary_type prefix(32, blocks);
    const data_chunk raw_bitfield{ { 0x69, 0x1c, 0xf4, 0x8b } };
    const binary_type compare(prefix.size(), raw_bitfield);
    BOOST_REQUIRE_EQUAL(prefix, compare);
}

BOOST_AUTO_TEST_CASE(bitfield_test4)
{
    const data_chunk blocks{ { 0x69, 0x1c, 0xf4, 0x8b } };
    const binary_type prefix(29, blocks);
    const data_chunk raw_bitfield{ { 0x69, 0x1c, 0xf4, 0x8b } };
    const binary_type compare(prefix.size(), raw_bitfield);
    BOOST_REQUIRE_EQUAL(prefix, compare);
}

BOOST_AUTO_TEST_SUITE_END()

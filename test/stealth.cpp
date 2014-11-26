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

BOOST_AUTO_TEST_CASE(repr_test)
{
    std::string repr = "01100110000";
    stealth_prefix prefix(repr);
    BOOST_REQUIRE(repr.size() == prefix.size());
    for (size_t i = 0; i < repr.size(); ++i)
    {
        const bool value = repr[i] == '1';
        BOOST_REQUIRE(prefix[i] == value);
    }
}

BOOST_AUTO_TEST_CASE(value_init)
{
    std::string repr = "01100110000";
    stealth_prefix prefix(repr);
    // Binary repr as a value on the left, padded with zeros to the right.
    //  repr + 0000....000
    const uint32_t value = 1711276032;
    stealth_prefix prefix2(repr.size(), value);
    BOOST_REQUIRE(prefix == prefix2);
}

BOOST_AUTO_TEST_CASE(bitfield_test)
{
    stealth_prefix prefix("01100110001");
    data_chunk raw_bitfield{{102, 32, 0, 0}};
    BITCOIN_ASSERT(raw_bitfield.size() * 8 >= prefix.size());
    stealth_prefix compare(prefix.size(), raw_bitfield);
    BOOST_REQUIRE(prefix == compare);

    stealth_prefix prefix1(27, 0x8bf41c69);
    data_chunk raw_bitfield1{{0x8b, 0xf4, 0x1c, 0x79}};
    BITCOIN_ASSERT(raw_bitfield1.size() * 8 >= prefix1.size());
    stealth_prefix compare1(prefix1.size(), raw_bitfield1);
    BOOST_REQUIRE(prefix1 == compare1);

    stealth_prefix prefix_bs(32, 0x691cf48b);
    data_chunk raw_bitfield_bs{{0x69, 0x1c, 0xf4, 0x8b}};
    stealth_prefix compare_bs(prefix_bs.size(), raw_bitfield_bs);
    BOOST_REQUIRE(prefix_bs == compare_bs);

    stealth_prefix prefix_b(29, 0x691cf48b);
    stealth_prefix compare_b(prefix_b.size(), raw_bitfield_bs);
    BOOST_REQUIRE(prefix_b == compare_b);
}

BOOST_AUTO_TEST_SUITE_END()

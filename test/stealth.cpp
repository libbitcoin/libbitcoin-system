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

BOOST_AUTO_TEST_CASE(bitfield_test)
{
    stealth_prefix prefix(27, 0x691cf48b);
    data_chunk raw_bitfield{0x8b, 0xf4, 0x1c, 0x79};
    BITCOIN_ASSERT(raw_bitfield.size() * 8 >= prefix.size());
    BOOST_REQUIRE(stealth_match(prefix, raw_bitfield.data()));

    stealth_prefix prefix_bs(32, 0x691cf48b);
    BOOST_REQUIRE(!stealth_match(prefix_bs, raw_bitfield.data()));

    stealth_prefix prefix_b(29, 0x691cf48b);
    BOOST_REQUIRE(!stealth_match(prefix_b, raw_bitfield.data()));

#if 0
    // Display stealth_prefix bits
    std::cout << prefix_bs << std::endl;
    for (size_t i = 0; i < prefix_bs.size(); ++i)
        std::cout << prefix_bs[i];
    std::cout << std::endl;

    // Display raw_bitfield bits
    uint8_t* bf = raw_bitfield.data();
    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t of = 0; of < 8; ++of)
        {
            bool value = (*bf & (1 << of)) > 0;
            std::cout << (value ? "1" : "0");
        }
        ++bf;
    }
    std::cout << std::endl;
#endif
}

BOOST_AUTO_TEST_SUITE_END()

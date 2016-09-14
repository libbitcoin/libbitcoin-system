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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace libbitcoin;

BOOST_AUTO_TEST_SUITE(hash_number_tests)

BOOST_AUTO_TEST_CASE(hash_number__simple__test)
{
    static const auto block_hash = hash_literal("00000000b873e79784647a6c82962c70d228557d24a747ea4d1b8bbe878e1206");

    hash_number target;
    BOOST_REQUIRE(target.set_compact(486604799));
    BOOST_REQUIRE(!(target <= 0));

    hash_number maximum;
    BOOST_REQUIRE(maximum.set_compact(max_work_bits));
    BOOST_REQUIRE(!(target > maximum));

    hash_number our_value(block_hash);
    BOOST_REQUIRE(!(our_value > target));
}

BOOST_AUTO_TEST_SUITE_END()

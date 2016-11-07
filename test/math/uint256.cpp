/**
 * Copyright (c) 2011-2016 libbitcoin developers (see AUTHORS)
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

#include <utility>
#include <bitcoin/bitcoin.hpp>

using namespace libbitcoin;

BOOST_AUTO_TEST_SUITE(uint256_tests)

BOOST_AUTO_TEST_CASE(uint256__constructor__simple__test)
{
    uint256_t target(hash_literal("00000000b873e79784647a6c82962c70d228557d24a747ea4d1b8bbe878e1206"));
    BOOST_REQUIRE(!target.overflow());

    uint256_t maximum(compact_number{ max_work_bits });
    BOOST_REQUIRE(!maximum.overflow());

    uint256_t check(compact_number{ 486604799 });
    BOOST_REQUIRE(!check.overflow());
    BOOST_REQUIRE(check <= target);
    BOOST_REQUIRE(check <= maximum);
}

BOOST_AUTO_TEST_SUITE_END()

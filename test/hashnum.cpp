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
#include <bitcoin/format.hpp>
#include <bitcoin/utility/hash_number.hpp>

using namespace libbitcoin;

BOOST_AUTO_TEST_SUITE(hashnum_tests)

BOOST_AUTO_TEST_CASE(simple)
{
    hash_digest block_hash = decode_hash(
        "00000000b873e79784647a6c82962c70d228557d24a747ea4d1b8bbe878e1206");
    uint32_t bits = 486604799;

    hash_number target;
    target.set_compact(bits);

    BOOST_REQUIRE((target <= 0) == false);
    BOOST_REQUIRE((target > max_target()) == false);

    hash_number our_value;
    our_value.set_hash(block_hash);
    BOOST_REQUIRE((our_value > target) == false);
}

BOOST_AUTO_TEST_SUITE_END()


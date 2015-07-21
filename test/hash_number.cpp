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

using namespace libbitcoin;

BOOST_AUTO_TEST_SUITE(hash_number_tests)

BOOST_AUTO_TEST_CASE(hash_number__simple__test)
{
    hash_number target;
    uint32_t bits = 486604799;
    target.set_compact(bits);
    hash_digest block_hash = hash_literal(
        "00000000b873e79784647a6c82962c70d228557d24a747ea4d1b8bbe878e1206");

    BOOST_REQUIRE(!(target <= 0));
    BOOST_REQUIRE(!(target > max_target()));

    hash_number our_value;
    our_value.set_hash(block_hash);
    BOOST_REQUIRE(!(our_value > target));
}

BOOST_AUTO_TEST_CASE(hash_number__work__test)
{
    hash_number orphan_work = 0;
    BOOST_REQUIRE_EQUAL(encode_base16(orphan_work.hash()), encode_base16(null_hash));
    orphan_work += block_work(486604799);
    BOOST_REQUIRE_EQUAL(encode_base16(orphan_work.hash()),
        "0100010001000000000000000000000000000000000000000000000000000000");
    hash_number main_work = 0;
    BOOST_REQUIRE(!(orphan_work <= main_work));
}

BOOST_AUTO_TEST_SUITE_END()


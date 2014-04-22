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

BOOST_AUTO_TEST_CASE(sha256_hash_test)
{
    auto genesis = genesis_block();
    auto genesis_hash = hash_block_header(genesis.header);
    BOOST_REQUIRE(encode_hex(genesis_hash) ==
        "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
}

BOOST_AUTO_TEST_CASE(ripemd_hash_test)
{
    auto ripemd_hash = bitcoin_short_hash(data_chunk{{110}});
    BOOST_REQUIRE(encode_hex(ripemd_hash) ==
        "17d040b739d639c729daaf627eaff88cfe4207f4");
}


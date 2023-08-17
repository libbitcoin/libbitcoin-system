/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "../../test.hpp"
#include <sstream>

BOOST_AUTO_TEST_SUITE(sha256x2_writer_tests)

static const auto genesis = settings(chain::selection::mainnet).genesis_block;

BOOST_AUTO_TEST_CASE(sha256x2_writer__stream__genesis_block__expected)
{
    std::ostringstream hash;
    hash::sha256x2::ostream hasher(hash);
    genesis.header().to_data(hasher);
    hasher.flush();
    BOOST_REQUIRE(hasher);
    BOOST_REQUIRE_EQUAL(hash.str(), to_string(genesis.hash()));
}

BOOST_AUTO_TEST_CASE(sha256x2_writer__copy__genesis_block__expected)
{
    hash_digest hash;
    hash::sha256x2::copy hasher(hash);
    genesis.header().to_data(hasher);
    hasher.flush();
    BOOST_REQUIRE(hasher);
    BOOST_REQUIRE_EQUAL(hash, genesis.hash());
}

BOOST_AUTO_TEST_CASE(sha256x2_writer__data__genesis_block__expected)
{
    data_chunk hash;
    hash::sha256x2::data hasher(hash);
    genesis.header().to_data(hasher);
    hasher.flush();
    BOOST_REQUIRE(hasher);
    BOOST_REQUIRE_EQUAL(hash, to_chunk(genesis.hash()));
}

BOOST_AUTO_TEST_CASE(sha256x2_writer__text__genesis_block__expected)
{
    std::string hash;
    hash::sha256x2::text hasher(hash);
    genesis.header().to_data(hasher);
    hasher.flush();
    BOOST_REQUIRE(hasher);
    BOOST_REQUIRE_EQUAL(hash, to_string(genesis.hash()));
}

BOOST_AUTO_TEST_SUITE_END()

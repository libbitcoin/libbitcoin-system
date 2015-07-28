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
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;
using namespace bc::config;
using namespace boost::program_options;

BOOST_AUTO_TEST_SUITE(checkpoint_tests)

#define CHECKPOINT_HASH_A "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"
#define CHECKPOINT_A CHECKPOINT_HASH_A ":0"
#define CHECKPOINT_B "0000000069e244f73d78e8fd29ba2fd2ed618bd6fa2ee92559f542fdb26e7c1d:11111"
#define CHECKPOINT_C "000000002dd5588a74784eaa7ab0507a18ad16a236e7b1ce69f00d7ddfb5d0a6:33333"
#define CHECKPOINT_ABC CHECKPOINT_A "\n" CHECKPOINT_B "\n" CHECKPOINT_C

// ------------------------------------------------------------------------- //

BOOST_AUTO_TEST_SUITE(checkpoint__hash)

BOOST_AUTO_TEST_CASE(checkpoint__hash__default__null_hash)
{
    const checkpoint check;
    BOOST_REQUIRE(check.hash() == null_hash);
}

BOOST_AUTO_TEST_CASE(checkpoint__hash__genesis__expected)
{
    const checkpoint genesis(CHECKPOINT_A);
    BOOST_REQUIRE_EQUAL(genesis.height(), 0u);
}


BOOST_AUTO_TEST_SUITE_END()

// ------------------------------------------------------------------------- //

BOOST_AUTO_TEST_SUITE(checkpoint__height)

BOOST_AUTO_TEST_CASE(checkpoint__height__default__zero)
{
    const checkpoint check;
    BOOST_REQUIRE_EQUAL(check.height(), 0u);
}

BOOST_AUTO_TEST_CASE(checkpoint__height__genesis__zero)
{
    const checkpoint genesis(CHECKPOINT_A);
    BOOST_REQUIRE_EQUAL(genesis.height(), 0u);
}

BOOST_AUTO_TEST_SUITE_END()

// ------------------------------------------------------------------------- //

BOOST_AUTO_TEST_SUITE(checkpoint_list)

static const checkpoint::list test_checkpoints_list(
{
    { CHECKPOINT_A },
    { CHECKPOINT_B },
    { CHECKPOINT_C }
});

BOOST_AUTO_TEST_CASE(checkpoint_list__ostream__three_elements__expected)
{
    std::stringstream serialized;
    serialized << test_checkpoints_list;
    BOOST_REQUIRE_EQUAL(serialized.str(), CHECKPOINT_ABC);
}

BOOST_AUTO_TEST_CASE(checkpoint_list__ostream__empty__expected)
{
    std::stringstream serialized;
    serialized << checkpoint::list();
    BOOST_REQUIRE_EQUAL(serialized.str(), "");
}

BOOST_AUTO_TEST_CASE(checkpoint_list__ostream__boost_lexical_cast__expected)
{
    using namespace boost;
    const auto serialized = lexical_cast<std::string>(test_checkpoints_list);
    BOOST_REQUIRE_EQUAL(serialized, CHECKPOINT_ABC);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

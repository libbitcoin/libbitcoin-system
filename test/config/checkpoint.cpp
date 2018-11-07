/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(checkpoint__construct)

BOOST_AUTO_TEST_CASE(checkpoint__construct__default__null_hash)
{
    const checkpoint check;
    BOOST_REQUIRE(check.hash() == null_hash);
    BOOST_REQUIRE_EQUAL(check.height(), 0u);
}

BOOST_AUTO_TEST_CASE(checkpoint__construct__copy__expected)
{
    const checkpoint check1(CHECKPOINT_C);
    const checkpoint check2(check1);
    BOOST_REQUIRE_EQUAL(check2.height(), check1.height());
    BOOST_REQUIRE_EQUAL(encode_hash(check2.hash()), encode_hash(check1.hash()));
}

BOOST_AUTO_TEST_CASE(checkpoint__construct__string__expected)
{
    const checkpoint genesis(CHECKPOINT_B);
    BOOST_REQUIRE_EQUAL(genesis.height(), 11111u);
    BOOST_REQUIRE_EQUAL(genesis.to_string(), CHECKPOINT_B);
}

BOOST_AUTO_TEST_CASE(checkpoint__construct__digest__expected)
{
    const size_t expected_height = 42;
    const auto expected_hash = CHECKPOINT_HASH_A;
    hash_digest digest;
    bc::decode_hash(digest, expected_hash);
    const checkpoint genesis(digest, expected_height);
    BOOST_REQUIRE_EQUAL(genesis.height(), expected_height);
    BOOST_REQUIRE_EQUAL(encode_hash(genesis.hash()), expected_hash);
}

BOOST_AUTO_TEST_CASE(checkpoint__construct__invalid_height_value__throws_invalid_option_value)
{
    // 2^64
    BOOST_REQUIRE_THROW(checkpoint(CHECKPOINT_HASH_A ":18446744073709551616"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(checkpoint__construct__invalid_height_characters__throws_invalid_option_value)
{
    // 21 characters
    BOOST_REQUIRE_THROW(checkpoint(CHECKPOINT_HASH_A ":1000000000100000000001"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(checkpoint__construct__bogus_height_characters__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(checkpoint(CHECKPOINT_HASH_A ":xxx"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(checkpoint__construct__bogus_line_hash__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(checkpoint("bogus:42"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(checkpoint__construct__bogus_hash__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(checkpoint("bogus", 42), invalid_option_value);
}

BOOST_AUTO_TEST_SUITE_END()

// ------------------------------------------------------------------------- //

BOOST_AUTO_TEST_SUITE(checkpoint__istream)

BOOST_AUTO_TEST_CASE(checkpoint__istream__empty__expected)
{
    checkpoint deserialized;
    std::stringstream serialized(CHECKPOINT_A);
    serialized >> deserialized;
    BOOST_REQUIRE_EQUAL(deserialized.to_string(), CHECKPOINT_A);
}

BOOST_AUTO_TEST_SUITE_END()

// ------------------------------------------------------------------------- //

BOOST_AUTO_TEST_SUITE(checkpoint__ostream)

static const checkpoint::list test_checkpoints_list(
{
    { CHECKPOINT_A },
    { CHECKPOINT_B },
    { CHECKPOINT_C }
});

BOOST_AUTO_TEST_CASE(checkpoint__ostream__empty__expected)
{
    std::stringstream serialized;
    serialized << checkpoint::list();
    BOOST_REQUIRE_EQUAL(serialized.str(), "");
}

BOOST_AUTO_TEST_CASE(checkpoint__ostream__populated__expected)
{
    std::stringstream serialized;
    serialized << test_checkpoints_list;
    BOOST_REQUIRE_EQUAL(serialized.str(), CHECKPOINT_ABC);
}

BOOST_AUTO_TEST_CASE(checkpoint__ostream__boost_lexical_cast__expected)
{
    using namespace boost;
    const auto serialized = lexical_cast<std::string>(test_checkpoints_list);
    BOOST_REQUIRE_EQUAL(serialized, CHECKPOINT_ABC);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

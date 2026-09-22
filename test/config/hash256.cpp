/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#include "../test.hpp"
#include <sstream>

BOOST_AUTO_TEST_SUITE(hash256_tests)

using namespace bc::system::config;

BOOST_AUTO_TEST_SUITE(hash256__construct)

BOOST_AUTO_TEST_CASE(hash256__construct__default__null_hash)
{
    const hash256 uninitialized_hash;
    BOOST_REQUIRE_EQUAL((const hash_digest&)uninitialized_hash, null_hash);
}

#define HASH256 "0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20"

BOOST_AUTO_TEST_CASE(hash256__construct__copy_digest__expected)
{
    const hash_digest digest = base16_hash(HASH256);
    const hash256 instance(digest);
    BOOST_REQUIRE_EQUAL((const hash_digest&)instance, digest);
}

BOOST_AUTO_TEST_CASE(hash256__construct__move_digest__expected)
{
    auto digest = base16_hash(HASH256);
    const auto expected = digest;
    const hash256 instance(std::move(digest));
    BOOST_REQUIRE_EQUAL((const hash_digest&)instance, expected);
}

BOOST_AUTO_TEST_CASE(hash256__construct__string__round_trips)
{
    const hash256 instance(HASH256);
    BOOST_REQUIRE_EQUAL(instance.to_string(), HASH256);
}

BOOST_AUTO_TEST_CASE(hash256__construct__invalid_string__throws_istream_exception)
{
    BOOST_REQUIRE_THROW(hash256("bogus"), istream_exception);
}

BOOST_AUTO_TEST_CASE(hash256__cast__uint256__expected)
{
    const hash256 instance("0000000000000000000000000000000000000000000000000000000000000001");
    BOOST_REQUIRE_EQUAL((uint256_t)instance, 1u);
}

BOOST_AUTO_TEST_CASE(hash256__stream__round_trip__expected)
{
    hash256 instance{};
    std::istringstream(HASH256) >> instance;

    std::ostringstream output{};
    output << instance;
    BOOST_REQUIRE_EQUAL(output.str(), HASH256);
}

BOOST_AUTO_TEST_CASE(hash256__stream__invalid__throws_istream_exception)
{
    hash256 instance{};
    BOOST_REQUIRE_THROW(std::istringstream("bogus") >> instance, istream_exception);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include "hash.hpp"

BOOST_AUTO_TEST_SUITE(sha_tests)

BOOST_AUTO_TEST_CASE(sha__sha160_hash__test_vectors__expected)
{
    for (const auto& test: sha160_tests)
    {
        const auto hash = accumulator<sha160>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

BOOST_AUTO_TEST_CASE(sha__sha256_hash__test_vectors__expected)
{
    for (const auto& test: sha256_tests)
    {
        const auto hash = accumulator<sha256>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

BOOST_AUTO_TEST_CASE(sha__sha512_hash__test_vectors__expected)
{
    for (const auto& test: sha512_tests)
    {
        const auto hash = accumulator<sha512>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

BOOST_AUTO_TEST_SUITE_END()

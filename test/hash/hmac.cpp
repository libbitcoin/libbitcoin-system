/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(hmac_tests)

// test.expected size mismatch
////BOOST_AUTO_TEST_CASE(hmac__rmd128__test_vectors__expected)
////{
////    // No test vectors, just exercising.
////    for (const auto& test : hmac_sha160_tests)
////    {
////        const auto hash = hmac<rmd128>::code(test.data, test.key);
////        BOOST_REQUIRE_NE(hash, test.expected);
////    }
////}

BOOST_AUTO_TEST_CASE(hmac__rmd160__test_vectors__expected)
{
    // No test vectors, just exercising.
    for (const auto& test: hmac_sha160_tests)
    {
        const auto hash = hmac<rmd160>::code(test.data, test.key);
        BOOST_REQUIRE_NE(hash, test.expected);
    }
}

BOOST_AUTO_TEST_CASE(hmac__sha160__test_vectors__expected)
{
    for (const auto& test: hmac_sha160_tests)
    {
        const auto hash = hmac<sha160>::code(test.data, test.key);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

BOOST_AUTO_TEST_CASE(hmac__sha256__test_vectors__expected)
{
    for (const auto& test: hmac_sha256_tests)
    {
        const auto hash = hmac<sha256>::code(test.data, test.key);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

BOOST_AUTO_TEST_CASE(hmac__sha512__test_vectors__expected)
{
    for (const auto& test: hmac_sha512_tests)
    {
        const auto hash = hmac<sha512>::code(test.data, test.key);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

BOOST_AUTO_TEST_SUITE_END()

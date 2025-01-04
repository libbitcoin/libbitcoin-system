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

BOOST_AUTO_TEST_SUITE(pbkd_tests)

// 8+ seconds of test here.
#if defined(HAVE_SLOW_TESTS)

BOOST_AUTO_TEST_CASE(pbkd__rmd128__test_vectors__expected)
{
    // No test vectors, just exercising.
    for (const auto& test: pbkd_sha160_tests)
    {
        const auto hash = pbkd<rmd128>::key<short_hash_size>(test.passphrase, test.salt, test.count);
        BOOST_REQUIRE_NE(hash, test.expected);
    }
}

BOOST_AUTO_TEST_CASE(pbkd__rmd160__test_vectors__expected)
{
    // No test vectors, just exercising.
    for (const auto& test: pbkd_sha160_tests)
    {
        const auto hash = pbkd<rmd160>::key<short_hash_size>(test.passphrase, test.salt, test.count);
        BOOST_REQUIRE_NE(hash, test.expected);
    }
}

BOOST_AUTO_TEST_CASE(pbkd__sha160__test_vectors__expected)
{
    for (const auto& test: pbkd_sha160_tests)
    {
        const auto hash = pbkd<sha160>::key<short_hash_size>(test.passphrase, test.salt, test.count);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

BOOST_AUTO_TEST_CASE(pbkd__sha256__test_vectors__expected)
{
    for (const auto& test: pbkd_sha256_tests)
    {
        const auto hash = pbkd<sha256>::key<long_hash_size>(test.passphrase, test.salt, test.count);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

BOOST_AUTO_TEST_CASE(pbkd__sha512__test_vectors__expected)
{

    for (const auto& test: pbkd_sha512_tests)
    {
        const auto hash = pbkd<sha512>::key<long_hash_size>(test.passphrase, test.salt, test.count);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

#endif // HAVE_SLOW_TESTS

BOOST_AUTO_TEST_SUITE_END()

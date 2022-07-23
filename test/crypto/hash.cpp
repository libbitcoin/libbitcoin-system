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

BOOST_AUTO_TEST_SUITE(hash_tests)

// ripemd160
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(hash__ripemd160_hash__test_vectors__expected)
{
    for (const auto& test: ripemd_tests)
    {
        const auto hash = ripemd160_hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// sha1 (160)
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(hash__sha1_hash__test_vectors__expected)
{
    for (const auto& test: sha1_tests)
    {
        const auto hash = sha1_hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// sha256
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(hash__sha256_hash__test_vectors__expected)
{
    for (const auto& test: sha256_tests)
    {
        const auto hash = sha256_hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

BOOST_AUTO_TEST_CASE(hash__hmac_sha256__test_vectors__expected)
{
    for (const auto& test: hmac_sha256_tests)
    {
        const auto hash = hmac_sha256(test.text, test.key);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

BOOST_AUTO_TEST_CASE(hash__pbkd_sha256__test_vectors__expected)
{
    for (const auto& test: pbkd_sha256_tests)
    {
        const auto chunk = pbkd_sha256(test.passphrase, test.salt, test.iterations, test.expected.size());
        BOOST_REQUIRE_EQUAL(chunk, test.expected);
    }
}

// sha512
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(hash__sha512_hash__test_vectors__expected)
{
    for (const auto& test: sha512_tests)
    {
        const auto hash = sha512_hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

BOOST_AUTO_TEST_CASE(hash__hmac_sha512__test_vectors__expected)
{
    for (const auto& test: hmac_sha512_tests)
    {
        const auto hash = hmac_sha512(test.text, test.key);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

BOOST_AUTO_TEST_CASE(hash__pbkd_sha512__test_vectors__expected)
{
    for (const auto& test: pbkd_sha512_tests)
    {
        const auto chunk = pbkd_sha512(test.passphrase, test.salt, test.iterations, test.expected.size());
        BOOST_REQUIRE_EQUAL(chunk, test.expected);
    }
}

// scrypt
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(hash__scrypt_hash__test_vectors__expected)
{
    for (const auto& test: scrypt_tests)
    {
        const auto hash = scrypt_hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// djb2
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(hash__djb2_hash__ad_hoc__0xe1669c01)
{
    const auto hash = djb2_hash("01234567890abcdefghijklmnopqrstuvwxyz");

    if constexpr (sizeof(size_t) == sizeof(uint32_t))
    {
        BOOST_REQUIRE_EQUAL(hash, 3781598209_u32);
    }
    else
    {
        BOOST_REQUIRE_EQUAL(hash, 9646636626660989953_u64);
    }
}

BOOST_AUTO_TEST_SUITE_END()

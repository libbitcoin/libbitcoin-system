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

BOOST_AUTO_TEST_SUITE(encryption_tests)

BOOST_AUTO_TEST_CASE(encryption__aes256__trivial__expected)
{
    constexpr aes256::secret key{ 1 };
    constexpr auto plaintext = base16_array("2a000000000000000000000000000000");
    constexpr auto cypertext = base16_array("c9d27d8790e5161c391fe4f61aabe8e2");

    aes256::block block{ plaintext };

    aes256::encrypt(block, key);
    BOOST_REQUIRE_EQUAL(block, cypertext);

    aes256::decrypt(block, key);
    BOOST_REQUIRE_EQUAL(block, plaintext);
}

// key length: 32
// block size: 16
// #of rounds: 14
// csrc.nist.gov/csrc/media/publications/fips/197/final/documents/fips-197.pdf
BOOST_AUTO_TEST_CASE(encryption__aes256__nist__expected)
{
    constexpr auto key = base16_array("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f");
    constexpr auto plaintext = base16_array("00112233445566778899aabbccddeeff");
    constexpr auto cypertext = base16_array("8ea2b7ca516745bfeafc49904b496089");

    aes256::block block{ plaintext };

    aes256::encrypt(block, key);
    BOOST_REQUIRE_EQUAL(block, cypertext);

    aes256::decrypt(block, key);
    BOOST_REQUIRE_EQUAL(block, plaintext);
}

BOOST_AUTO_TEST_SUITE_END()

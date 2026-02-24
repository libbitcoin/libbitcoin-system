/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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

// nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197-upd1.pdf (Appendix C)

BOOST_AUTO_TEST_SUITE(aes256_tests)

BOOST_AUTO_TEST_CASE(aes256__encrypt_ecb_decrypt_ecb__nist_1__expected)
{
    constexpr auto key = base16_array("603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4");
    constexpr aes256::block plain = base16_array("6bc1bee22e409f96e93d7e117393172a");
    constexpr aes256::block cipher = base16_array("f3eed1bdb5d2a03c064b5a7e3db181f8");

    aes256::block block{ plain };

    aes256::encrypt_ecb(block, key);
    BOOST_REQUIRE_EQUAL(block, cipher);

    aes256::decrypt_ecb(block, key);
    BOOST_REQUIRE_EQUAL(block, plain);
}

BOOST_AUTO_TEST_CASE(aes256__encrypt_ecb_decrypt_ecb__nist_2__expected)
{
    constexpr auto key = base16_array("603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4");
    constexpr aes256::block plain = base16_array("ae2d8a571e03ac9c9eb76fac45af8e51");
    constexpr aes256::block cipher = base16_array("591ccb10d410ed26dc5ba74a31362870");

    aes256::block block{ plain };

    aes256::encrypt_ecb(block, key);
    BOOST_REQUIRE_EQUAL(block, cipher);

    aes256::decrypt_ecb(block, key);
    BOOST_REQUIRE_EQUAL(block, plain);
}

BOOST_AUTO_TEST_CASE(aes256__encrypt_ecb_decrypt_ecb__nist_3__expected)
{
    constexpr auto key = base16_array("603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4");
    constexpr aes256::block plain = base16_array("30c81c46a35ce411e5fbc1191a0a52ef");
    constexpr aes256::block cipher = base16_array("b6ed21b99ca6f4f9f153e7b1beafed1d");

    aes256::block block{ plain };

    aes256::encrypt_ecb(block, key);
    BOOST_REQUIRE_EQUAL(block, cipher);

    aes256::decrypt_ecb(block, key);
    BOOST_REQUIRE_EQUAL(block, plain);
}

BOOST_AUTO_TEST_CASE(aes256__encrypt_ecb_decrypt_ecb__nist_4__expected)
{
    constexpr auto key = base16_array("603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4");
    constexpr aes256::block plain = base16_array("f69f2445df4f9b17ad2b417be66c3710");
    constexpr aes256::block cipher = base16_array("23304b7a39f9f3ff067d8d8f9e24ecc7");

    aes256::block block{ plain };

    aes256::encrypt_ecb(block, key);
    BOOST_REQUIRE_EQUAL(block, cipher);

    aes256::decrypt_ecb(block, key);
    BOOST_REQUIRE_EQUAL(block, plain);
}

BOOST_AUTO_TEST_CASE(aes256__encrypt_ecb_decrypt_ecb__nist_5__expected)
{
    constexpr auto key = base16_array("0000000000000000000000000000000000000000000000000000000000000000");
    constexpr aes256::block plain = base16_array("00000000000000000000000000000000");
    constexpr aes256::block cipher = base16_array("dc95c078a2408989ad48a21492842087");

    aes256::block block{ plain };

    aes256::encrypt_ecb(block, key);
    BOOST_REQUIRE_EQUAL(block, cipher);

    aes256::decrypt_ecb(block, key);
    BOOST_REQUIRE_EQUAL(block, plain);
}

BOOST_AUTO_TEST_CASE(aes256__encrypt_ecb_decrypt_ecb__nist_6__expected)
{
    constexpr auto key = base16_array("feffe9928665731c6d6a8f9467308308feffe9928665731c6d6a8f9467308308");
    constexpr aes256::block plain = base16_array("d9313225f88406e5a55909c5aff5269a");
    constexpr aes256::block cipher = base16_array("5fc4d4c26434a6e5b572cb421a1fe30a");

    aes256::block block{ plain };

    aes256::encrypt_ecb(block, key);
    BOOST_REQUIRE_EQUAL(block, cipher);

    aes256::decrypt_ecb(block, key);
    BOOST_REQUIRE_EQUAL(block, plain);
}

BOOST_AUTO_TEST_CASE(aes256__encrypt_ecb_decrypt_ecb__nist_7__expected)
{
    constexpr auto key = base16_array("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f");
    constexpr aes256::block plain = base16_array("00112233445566778899aabbccddeeff");
    constexpr aes256::block cipher = base16_array("8ea2b7ca516745bfeafc49904b496089");

    aes256::block block{ plain };

    aes256::encrypt_ecb(block, key);
    BOOST_REQUIRE_EQUAL(block, cipher);

    aes256::decrypt_ecb(block, key);
    BOOST_REQUIRE_EQUAL(block, plain);
}

BOOST_AUTO_TEST_SUITE_END()

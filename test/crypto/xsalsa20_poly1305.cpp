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

BOOST_AUTO_TEST_SUITE(xsalsa20_poly1305_tests)

// nacl tests/secretbox.c (key is crypto_box_beforenm of the rfc7748 pair).
const auto secretbox_key = base16_array("1b27556473e985d462cd51197a9a46c76009549eac6474f206c4ee0844f68389");
const auto secretbox_nonce = base16_array("69696ee955b62b73cd62bda875fc73d68219e0036b7a0b37");
const auto secretbox_plain = base16_chunk(
    "be075fc53c81f2d5cf141316ebeb0c7b5228c52a4c62cbd44b66849b64244ffc"
    "e5ecbaaf33bd751a1ac728d45e6c61296cdc3c01233561f41db66cce314adb31"
    "0e3be8250c46f06dceea3a7fa1348057e2f6556ad6b1318a024a838f21af1fde"
    "048977eb48f59ffd4924ca1c60902e52f0a089bc76897040e082f93776384864"
    "5e0705");
const auto secretbox_cipher = base16_chunk(
    "f3ffc7703f9400e52a7dfb4b3d3305d98e993b9f48681273c29650ba32fc76ce"
    "48332ea7164d96a4476fb8c531a1186ac0dfc17c98dce87b4da7f011ec48c972"
    "71d2c20f9b928fe2270d6fb863d51738b48eeee314a7cc8ab932164548e526ae"
    "90224368517acfeabd6bb3732bc0e9da99832b61ca01b6de56244a9e88d5f9b3"
    "7973f622a43d14a6599b1f654cb45a74e355a5");

BOOST_AUTO_TEST_CASE(xsalsa20_poly1305__encrypt__nacl_secretbox__expected)
{
    xsalsa20_poly1305 box{ secretbox_key };
    data_chunk cipher(secretbox_plain.size() + xsalsa20_poly1305::expansion);
    box.encrypt(secretbox_plain, secretbox_nonce, cipher);
    BOOST_REQUIRE_EQUAL(cipher, secretbox_cipher);
}

BOOST_AUTO_TEST_CASE(xsalsa20_poly1305__decrypt__nacl_secretbox__expected)
{
    xsalsa20_poly1305 box{ secretbox_key };
    data_chunk plain(secretbox_cipher.size() - xsalsa20_poly1305::expansion);
    BOOST_REQUIRE(box.decrypt(plain, secretbox_nonce, secretbox_cipher));
    BOOST_REQUIRE_EQUAL(plain, secretbox_plain);
}

BOOST_AUTO_TEST_CASE(xsalsa20_poly1305__decrypt__tampered__false_and_cleared)
{
    auto cipher = secretbox_cipher;
    cipher.back() ^= 0x01;

    xsalsa20_poly1305 box{ secretbox_key };
    data_chunk plain(cipher.size() - xsalsa20_poly1305::expansion, 0xff);
    BOOST_REQUIRE(!box.decrypt(plain, secretbox_nonce, cipher));
    BOOST_REQUIRE_EQUAL(plain, data_chunk(plain.size(), 0x00));
}

BOOST_AUTO_TEST_CASE(xsalsa20_poly1305__decrypt__wrong_nonce__false)
{
    auto nonce = secretbox_nonce;
    nonce.front() ^= 0x01;

    xsalsa20_poly1305 box{ secretbox_key };
    data_chunk plain(secretbox_cipher.size() - xsalsa20_poly1305::expansion);
    BOOST_REQUIRE(!box.decrypt(plain, nonce, secretbox_cipher));
}

BOOST_AUTO_TEST_CASE(xsalsa20_poly1305__encrypt__empty__tag_only_round_trip)
{
    xsalsa20_poly1305 box{ secretbox_key };
    data_chunk cipher(xsalsa20_poly1305::expansion);
    box.encrypt({}, secretbox_nonce, cipher);

    data_chunk plain{};
    BOOST_REQUIRE(box.decrypt(plain, secretbox_nonce, cipher));
}

// nacl tests/box.c: crypto_box is the secretbox under the hsalsa20 of the x25519 shared secret.
BOOST_AUTO_TEST_CASE(xsalsa20_poly1305__encrypt__nacl_box_composition__expected)
{
    const auto alice_secret = base16_array("77076d0a7318a57d3c16c17251b26645df4c2f87ebc0992ab177fba51db92c2a");
    const auto bob_public = base16_array("de9edb7d7b7dc1b4d35b61c2ece435373f8343c85b78674dadfc7e146f882b4f");

    x25519::key shared{};
    BOOST_REQUIRE(x25519::multiply(shared, alice_secret, bob_public));

    salsa20::secret key{};
    salsa20::hsalsa20(key, shared, salsa20::input{});
    BOOST_REQUIRE_EQUAL(key, secretbox_key);

    xsalsa20_poly1305 box{ key };
    data_chunk cipher(secretbox_plain.size() + xsalsa20_poly1305::expansion);
    box.encrypt(secretbox_plain, secretbox_nonce, cipher);
    BOOST_REQUIRE_EQUAL(cipher, secretbox_cipher);
}

BOOST_AUTO_TEST_SUITE_END()

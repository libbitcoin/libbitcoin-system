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

BOOST_AUTO_TEST_SUITE(salsa20_tests)

// ecrypt salsa20 (256-bit key) set 1, vector 0, stream[0..63].
BOOST_AUTO_TEST_CASE(salsa20__stream__ecrypt_set1_vector0__expected)
{
    const auto key = base16_array("8000000000000000000000000000000000000000000000000000000000000000");
    const auto expected = base16_array(
        "e3be8fdd8beca2e3ea8ef9475b29a6e7003951e1097a5c38d23b7a5fad9f6844"
        "b22c97559e2723c7cbbd3fe4fc8d9a0744652a83e72a9c461876af4d7ef1a117");

    salsa20 cipher{ key };
    salsa20::block out{};
    cipher.stream(out);
    BOOST_REQUIRE_EQUAL(out, expected);
}

// The keystream is continuous across calls of any size.
BOOST_AUTO_TEST_CASE(salsa20__stream__segmented__continuous)
{
    const auto key = base16_array("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f");
    data_array<200> whole{};
    data_array<200> pieces{};

    salsa20 cipher{ key };
    cipher.seek(42, 7);
    cipher.stream(whole);

    salsa20 other{ key };
    other.seek(42, 7);
    other.stream(std::span<uint8_t>(pieces).first(1));
    other.stream(std::span<uint8_t>(pieces).subspan(1, 63));
    other.stream(std::span<uint8_t>(pieces).subspan(64, 100));
    other.stream(std::span<uint8_t>(pieces).subspan(164));
    BOOST_REQUIRE_EQUAL(whole, pieces);
}

// Encryption is its own inverse.
BOOST_AUTO_TEST_CASE(salsa20__crypt__round_trip__expected)
{
    const auto key = base16_array("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f");
    const auto plain = to_chunk("The quick brown fox jumps over the lazy dog, twice, and then some more.");

    salsa20 cipher{ key };
    cipher.seek(1, 0);
    data_chunk encrypted(plain.size());
    cipher.crypt(plain, encrypted);
    BOOST_REQUIRE_NE(encrypted, plain);

    cipher.seek(1, 0);
    data_chunk decrypted(plain.size());
    cipher.crypt(encrypted, decrypted);
    BOOST_REQUIRE_EQUAL(decrypted, plain);
}

// nacl core1 (crypto_core_hsalsa20 of the rfc7748 shared secret, zero input).
BOOST_AUTO_TEST_CASE(salsa20__hsalsa20__nacl_core1__expected)
{
    const auto shared = base16_array("4a5d9d5ba4ce2de1728e3bf480350f25e07e21c947d19e3376f09b3c1e161742");
    const auto expected = base16_array("1b27556473e985d462cd51197a9a46c76009549eac6474f206c4ee0844f68389");
    const salsa20::input zero{};

    salsa20::secret out{};
    salsa20::hsalsa20(out, shared, zero);
    BOOST_REQUIRE_EQUAL(out, expected);
}

// XSalsa20 is Salsa20 under the HSalsa20 subkey and the trailing nonce.
BOOST_AUTO_TEST_CASE(salsa20__extended_nonce__matches_derivation__expected)
{
    const auto key = base16_array("1b27556473e985d462cd51197a9a46c76009549eac6474f206c4ee0844f68389");
    const auto nonce = base16_array("69696ee955b62b73cd62bda875fc73d68219e0036b7a0b37");

    salsa20::secret subkey{};
    salsa20::hsalsa20(subkey, key, slice<0, 16>(nonce));
    salsa20 derived{ subkey };
    derived.seek(from_little_array<uint64_t>(slice<16, 24>(nonce)), 0);
    data_array<100> expected{};
    derived.stream(expected);

    salsa20 extended{ key, nonce };
    data_array<100> out{};
    extended.stream(out);
    BOOST_REQUIRE_EQUAL(out, expected);
}

BOOST_AUTO_TEST_SUITE_END()

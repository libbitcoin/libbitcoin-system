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

BOOST_AUTO_TEST_SUITE(chacha20_tests)

// rfc8439 test vectors (as compiled in bitcoin core test framework).

struct chacha20_vector
{
    chacha20::secret key;
    uint32_t nonce32;
    uint64_t nonce64;
    uint32_t counter;
    data_array<64> keystream;
};

const std_vector<chacha20_vector> chacha20_vectors
{
    {
        base16_array("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"),
        0x09000000, 0x4a000000, 1,
        base16_array("10f1e7e4d13b5915500fdd1fa32071c4c7d1f4c733c068030422aa9ac3d46c4e"
                     "d2826446079faa0914c2d705d98b02a2b5129cd1de164eb9cbd083e8a2503c4e")
    },
    {
        base16_array("0000000000000000000000000000000000000000000000000000000000000000"),
        0, 0, 0,
        base16_array("76b8e0ada0f13d90405d6ae55386bd28bdd219b8a08ded1aa836efcc8b770dc7"
                     "da41597c5157488d7724e03fb8d84a376a43b8f41518a11cc387b669b2ee6586")
    },
    {
        base16_array("0000000000000000000000000000000000000000000000000000000000000000"),
        0, 0, 1,
        base16_array("9f07e7be5551387a98ba977c732d080dcb0f29a048e3656912c6533e32ee7aed"
                     "29b721769ce64e43d57133b074d839d531ed1f28510afb45ace10a1f4b794d6f")
    },
    {
        base16_array("0000000000000000000000000000000000000000000000000000000000000001"),
        0, 0, 1,
        base16_array("3aeb5224ecf849929b9d828db1ced4dd832025e8018b8160b82284f3c949aa5a"
                     "8eca00bbb4a73bdad192b5c42f73f2fd4e273644c8b36125a64addeb006c13a0")
    },
    {
        base16_array("00ff000000000000000000000000000000000000000000000000000000000000"),
        0, 0, 2,
        base16_array("72d54dfbf12ec44b362692df94137f328fea8da73990265ec1bbbea1ae9af0ca"
                     "13b25aa26cb4a648cb9b9d1be65b2c0924a66c54d545ec1b7374f4872e99f096")
    },
    {
        base16_array("0000000000000000000000000000000000000000000000000000000000000000"),
        0, 0x0200000000000000, 0,
        base16_array("c2c64d378cd536374ae204b9ef933fcd1a8b2288b3dfa49672ab765b54ee27c7"
                     "8a970e0e955c14f3a88e741b97c286f75f8fc299e8148362fa198a39531bed6d")
    }
};

BOOST_AUTO_TEST_CASE(chacha20__stream__rfc8439_vectors__expected)
{
    for (const auto& vector: chacha20_vectors)
    {
        chacha20 cipher{ vector.key };
        cipher.seek(vector.nonce32, vector.nonce64, vector.counter);

        data_array<64> keystream{};
        cipher.stream(keystream);
        BOOST_REQUIRE_EQUAL(keystream, vector.keystream);
    }
}

BOOST_AUTO_TEST_CASE(chacha20__crypt__zeros__keystream)
{
    const auto& vector = chacha20_vectors.front();
    chacha20 cipher{ vector.key };
    cipher.seek(vector.nonce32, vector.nonce64, vector.counter);

    const data_array<64> zeros{};
    data_array<64> encrypted{};
    cipher.crypt(zeros, encrypted);
    BOOST_REQUIRE_EQUAL(encrypted, vector.keystream);
}

BOOST_AUTO_TEST_CASE(chacha20__crypt__round_trip__expected)
{
    const auto plain = base16_chunk("000102030405060708090a0b0c0d0e0f1011"
        "12131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f3031");
    const auto key = base16_array(
        "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f");

    chacha20 encrypter{ key };
    encrypter.seek(42, 42, 0);
    data_chunk encrypted(plain.size());
    encrypter.crypt(plain, encrypted);
    BOOST_REQUIRE_NE(encrypted, plain);

    chacha20 decrypter{ key };
    decrypter.seek(42, 42, 0);
    data_chunk decrypted(encrypted.size());
    decrypter.crypt(encrypted, decrypted);
    BOOST_REQUIRE_EQUAL(decrypted, plain);
}

BOOST_AUTO_TEST_CASE(chacha20__crypt__split_stream__continuous)
{
    const auto& vector = chacha20_vectors.front();

    // Crypt in odd-sized pieces, keystream must remain continuous.
    chacha20 cipher{ vector.key };
    cipher.seek(vector.nonce32, vector.nonce64, vector.counter);

    const data_array<64> zeros{};
    data_array<64> encrypted{};
    cipher.crypt({ zeros.data(), 1 }, { encrypted.data(), 1 });
    cipher.crypt({ std::next(zeros.data(), 1), 41 }, { std::next(encrypted.data(), 1), 41 });
    cipher.crypt({ std::next(zeros.data(), 42), 22 }, { std::next(encrypted.data(), 42), 22 });
    BOOST_REQUIRE_EQUAL(encrypted, vector.keystream);
}

BOOST_AUTO_TEST_CASE(chacha20__stream__long__matches_short_reference)
{
    const auto& vector = chacha20_vectors.front();
    constexpr size_t size = 1027;

    // Long request (exercises the concurrent block path where compiled).
    chacha20 cipher{ vector.key };
    cipher.seek(vector.nonce32, vector.nonce64, vector.counter);
    data_chunk keystream(size);
    cipher.stream(keystream);

    // Byte at a time request (scalar buffered reference).
    chacha20 reference{ vector.key };
    reference.seek(vector.nonce32, vector.nonce64, vector.counter);
    data_chunk expected(size);

    for (size_t byte{}; byte < size; ++byte)
        reference.stream({ std::next(expected.data(), byte), 1 });

    BOOST_REQUIRE_EQUAL(keystream, expected);
}

BOOST_AUTO_TEST_CASE(chacha20__crypt__long__matches_short_reference)
{
    const auto& vector = chacha20_vectors.front();
    constexpr size_t size = 1600;

    data_chunk plain(size);
    for (size_t byte{}; byte < size; ++byte)
        plain[byte] = narrow_cast<uint8_t>(byte);

    // Drained partial buffer followed by a long request.
    chacha20 cipher{ vector.key };
    cipher.seek(vector.nonce32, vector.nonce64, vector.counter);
    data_chunk encrypted(size);
    cipher.crypt({ plain.data(), 5 }, { encrypted.data(), 5 });
    cipher.crypt({ std::next(plain.data(), 5), size - 5 },
        { std::next(encrypted.data(), 5), size - 5 });

    // Byte at a time request (scalar buffered reference).
    chacha20 reference{ vector.key };
    reference.seek(vector.nonce32, vector.nonce64, vector.counter);
    data_chunk expected(size);

    for (size_t byte{}; byte < size; ++byte)
        reference.crypt({ std::next(plain.data(), byte), 1 },
            { std::next(expected.data(), byte), 1 });

    BOOST_REQUIRE_EQUAL(encrypted, expected);
}

BOOST_AUTO_TEST_SUITE_END()

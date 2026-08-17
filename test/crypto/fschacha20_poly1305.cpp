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

BOOST_AUTO_TEST_SUITE(fschacha20_poly1305_tests)

BOOST_AUTO_TEST_CASE(fschacha20_poly1305__encrypt__seek_500__expected)
{
    const auto plain = base16_chunk(
        "d6a4cb04ef0f7c09c1866ed29dc24d820e75b0491032a51b4c3366f9ca35c19e"
        "a3047ec6be9d45f9637b63e1cf9eb4c2523a5aab7b851ebeba87199db0e839cf"
        "0d5c25e50168306377aedbe9089fd2463ded88b83211cf51b73b150608cc7a60"
        "0d0f11b9a742948482e1b109d8faf15b450aa7322e892fa2208c6691e3fecf4c"
        "711191b14d75a72147");
    const auto aad = base16_chunk("786cb9b6ebf44288974cf0");
    const auto key = base16_array(
        "5c9e1c3951a74fba66708bf9d2c217571684556b6a6a3573bff2847d38612654");
    const auto expected = base16_chunk(
        "9dcebbd3281ea3dd8e9a1ef7d55a97abd6743e56ebc0c190cb2c4e14160b385e"
        "0bf508dddf754bd02c7c208447c131ce23e47a4a14dfaf5dd8bc601323950f75"
        "4e05d46e9232f83fc5120fbbef6f5347a826ec79a93820718d4ec7a2b7cfaaa4"
        "4b21e16d726448b62f803811aff4f6d827ed78e738ce8a507b81a8ae13131192"
        "8039213de18a5120dc9b7370baca878f50ff254418de3da50c");
    constexpr uint32_t interval = 224;
    constexpr uint32_t index = 500;

    fschacha20_poly1305 encrypter{ key, interval };
    fschacha20_poly1305 decrypter{ key, interval };
    data_chunk empty_cipher(fschacha20_poly1305::expansion);

    // Seek to the numbered message with empty encryptions.
    for (uint32_t message{}; message < index; ++message)
        encrypter.encrypt({}, {}, empty_cipher);

    data_chunk cipher(plain.size() + fschacha20_poly1305::expansion);
    encrypter.encrypt(plain, aad, cipher);
    BOOST_REQUIRE_EQUAL(cipher, expected);

    // Decrypt round trip at the same message index.
    data_chunk empty_plain{};
    for (uint32_t message{}; message < index; ++message)
        decrypter.decrypt(empty_plain, {}, empty_cipher);

    data_chunk decrypted(plain.size());
    BOOST_REQUIRE(decrypter.decrypt(decrypted, aad, cipher));
    BOOST_REQUIRE_EQUAL(decrypted, plain);
}

BOOST_AUTO_TEST_CASE(fschacha20_poly1305__encrypt__seek_60000__expected)
{
    const auto plain = base16_chunk(
        "8349b7a2690b63d01204800c288ff1138a1d473c832c90ea8b3fc102d0bb3adc"
        "44261b247c7c3d6760bfbe979d061c305f46d94c0582ac3099f0bf249f8cb234");
    const data_chunk aad{};
    const auto key = base16_array(
        "3bd2093fcbcb0d034d8c569583c5425c1a53171ea299f8cc3bbf9ae3530adfce");
    const auto expected = base16_chunk(
        "30a6757ff8439b975363f166a0fa0e36722ab35936abd704297948f45083f4d4"
        "99433137ce931f7fca28a0acd3bc30f57b550acbc21cbd45bbef0739d9caf30c"
        "14b94829deb27f0b1923a2af704ae5d6");
    constexpr uint32_t interval = 224;
    constexpr uint32_t index = 60000;

    fschacha20_poly1305 encrypter{ key, interval };
    data_chunk empty_cipher(fschacha20_poly1305::expansion);

    for (uint32_t message{}; message < index; ++message)
        encrypter.encrypt({}, {}, empty_cipher);

    data_chunk cipher(plain.size() + fschacha20_poly1305::expansion);
    encrypter.encrypt(plain, aad, cipher);
    BOOST_REQUIRE_EQUAL(cipher, expected);
}

BOOST_AUTO_TEST_SUITE_END()

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

BOOST_AUTO_TEST_SUITE(chacha20_poly1305_tests)

// rfc8439 section 2.8.2 example.
BOOST_AUTO_TEST_CASE(chacha20_poly1305__encrypt__rfc8439_example__expected)
{
    const auto plain = base16_chunk(
        "4c616469657320616e642047656e746c656d656e206f662074686520636c6173"
        "73206f66202739393a204966204920636f756c64206f6666657220796f75206f"
        "6e6c79206f6e652074697020666f7220746865206675747572652c2073756e73"
        "637265656e20776f756c642062652069742e");
    const auto aad = base16_chunk("50515253c0c1c2c3c4c5c6c7");
    const auto key = base16_array(
        "808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f");
    const auto expected = base16_chunk(
        "d31a8d34648e60db7b86afbc53ef7ec2a4aded51296e08fea9e2b5a736ee62d6"
        "3dbea45e8ca9671282fafb69da92728b1a71de0a9e060b2905d6a5b67ecd3b36"
        "92ddbd7f2d778b8c9803aee328091b58fab324e4fad675945585808b4831d7bc"
        "3ff4def08e4b7a9de576d26586cec64b61161ae10b594f09e26a7e902ecbd060"
        "0691");
    constexpr uint32_t nonce32 = 7;
    constexpr uint64_t nonce64 = 0x4746454443424140;

    chacha20_poly1305 aead{ key };
    data_chunk cipher(plain.size() + chacha20_poly1305::expansion);
    aead.encrypt(plain, aad, nonce32, nonce64, cipher);
    BOOST_REQUIRE_EQUAL(cipher, expected);

    // Decrypt round trip.
    data_chunk decrypted(plain.size());
    BOOST_REQUIRE(aead.decrypt(decrypted, aad, nonce32, nonce64, cipher));
    BOOST_REQUIRE_EQUAL(decrypted, plain);
}

// rfc8439 appendix A.5.
BOOST_AUTO_TEST_CASE(chacha20_poly1305__decrypt__rfc8439_a5__expected)
{
    const auto expected = base16_chunk(
        "496e7465726e65742d4472616674732061726520647261667420646f63756d65"
        "6e74732076616c696420666f722061206d6178696d756d206f6620736978206d"
        "6f6e74687320616e64206d617920626520757064617465642c207265706c6163"
        "65642c206f72206f62736f6c65746564206279206f7468657220646f63756d65"
        "6e747320617420616e792074696d652e20497420697320696e617070726f7072"
        "6961746520746f2075736520496e7465726e65742d4472616674732061732072"
        "65666572656e6365206d6174657269616c206f7220746f206369746520746865"
        "6d206f74686572207468616e206173202fe2809c776f726b20696e2070726f67"
        "726573732e2fe2809d");
    const auto aad = base16_chunk("f33388860000000000004e91");
    const auto key = base16_array(
        "1c9240a5eb55d38af333888604f6b5f0473917c1402b80099dca5cbc207075c0");
    const auto cipher = base16_chunk(
        "64a0861575861af460f062c79be643bd5e805cfd345cf389f108670ac76c8cb2"
        "4c6cfc18755d43eea09ee94e382d26b0bdb7b73c321b0100d4f03b7f355894cf"
        "332f830e710b97ce98c8a84abd0b948114ad176e008d33bd60f982b1ff37c855"
        "9797a06ef4f0ef61c186324e2b3506383606907b6a7c02b0f9f6157b53c867e4"
        "b9166c767b804d46a59b5216cde7a4e99040c5a40433225ee282a1b0a06c523e"
        "af4534d7f83fa1155b0047718cbc546a0d072b04b3564eea1b422273f548271a"
        "0bb2316053fa76991955ebd63159434ecebb4e466dae5a1073a6727627097a10"
        "49e617d91d361094fa68f0ff77987130305beaba2eda04df997b714d6c6f2c29"
        "a6ad5cb4022b02709beead9d67890cbb22392336fea1851f38");
    constexpr uint32_t nonce32 = 0;
    constexpr uint64_t nonce64 = 0x0807060504030201;

    chacha20_poly1305 aead{ key };
    data_chunk plain(cipher.size() - chacha20_poly1305::expansion);
    BOOST_REQUIRE(aead.decrypt(plain, aad, nonce32, nonce64, cipher));
    BOOST_REQUIRE_EQUAL(plain, expected);
}

BOOST_AUTO_TEST_CASE(chacha20_poly1305__decrypt__tampered__false_and_cleared)
{
    const auto plain = base16_chunk("00112233445566778899aabbccddeeff");
    const auto aad = base16_chunk("f00d");
    const auto key = base16_array(
        "808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f");

    chacha20_poly1305 aead{ key };
    data_chunk cipher(plain.size() + chacha20_poly1305::expansion);
    aead.encrypt(plain, aad, 1, 2, cipher);

    // Flip one ciphertext bit.
    cipher.front() ^= 0x01_u8;

    data_chunk decrypted(plain.size());
    BOOST_REQUIRE(!aead.decrypt(decrypted, aad, 1, 2, cipher));
    BOOST_REQUIRE_EQUAL(decrypted, data_chunk(plain.size(), 0x00_u8));

    // Restore and decrypt (fresh instance, same nonce).
    cipher.front() ^= 0x01_u8;
    BOOST_REQUIRE(aead.decrypt(decrypted, aad, 1, 2, cipher));
    BOOST_REQUIRE_EQUAL(decrypted, plain);
}

BOOST_AUTO_TEST_CASE(chacha20_poly1305__encrypt__two_span__matches_single)
{
    const auto plain = base16_chunk(
        "4c616469657320616e642047656e746c656d656e206f662074686520636c6173"
        "73206f66202739393a204966204920636f756c64206f6666657220796f75206f"
        "6e6c79206f6e652074697020666f7220746865206675747572652c2073756e73"
        "637265656e20776f756c642062652069742e");
    const auto aad = base16_chunk("50515253c0c1c2c3c4c5c6c7");
    const auto key = base16_array(
        "808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f");
    constexpr uint32_t nonce32 = 7;
    constexpr uint64_t nonce64 = 0x4746454443424140;

    // The single span encryption.
    chacha20_poly1305 single{ key };
    data_chunk expected(plain.size() + chacha20_poly1305::expansion);
    single.encrypt(plain, aad, nonce32, nonce64, expected);

    // The same plaintext split as one byte followed by the remainder.
    const auto span = const_byte_span{ plain };
    const auto plain1 = span.first(one);
    const auto plain2 = span.subspan(one);

    chacha20_poly1305 split{ key };
    data_chunk cipher(plain.size() + chacha20_poly1305::expansion);
    split.encrypt(plain1, plain2, aad, nonce32, nonce64, cipher);
    BOOST_REQUIRE_EQUAL(cipher, expected);
}

// bip324 fschacha20_poly1305 test vectors (bitcoin core test framework).

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

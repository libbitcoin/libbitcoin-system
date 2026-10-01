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
#include "aes_gcm.hpp"

BOOST_AUTO_TEST_SUITE(aes128_gcm_tests)

// Wycheproof.

BOOST_AUTO_TEST_CASE(aes128_gcm__encrypt__wycheproof_valid__expected)
{
    for (const auto& vector: aes128_gcm_valid_vectors)
    {
        aes128_gcm cipher{ vector.key };
        data_chunk out(vector.plain.size() + aes128_gcm::expansion);
        cipher.encrypt(vector.plain, vector.aad, vector.nonce, out);
        BOOST_REQUIRE_MESSAGE(out == splice(vector.cipher, vector.tag), vector.id);
    }
}

BOOST_AUTO_TEST_CASE(aes128_gcm__decrypt__wycheproof_valid__true_expected)
{
    for (const auto& vector: aes128_gcm_valid_vectors)
    {
        aes128_gcm cipher{ vector.key };
        data_chunk plain(vector.plain.size());
        BOOST_REQUIRE_MESSAGE(cipher.decrypt(plain, vector.aad, vector.nonce, splice(vector.cipher, vector.tag)), vector.id);
        BOOST_REQUIRE_MESSAGE(plain == vector.plain, vector.id);
    }
}

BOOST_AUTO_TEST_CASE(aes128_gcm__decrypt__wycheproof_invalid__false_cleared)
{
    for (const auto& vector: aes128_gcm_invalid_vectors)
    {
        aes128_gcm cipher{ vector.key };
        data_chunk plain(vector.plain.size(), 0xff);
        BOOST_REQUIRE_MESSAGE(!cipher.decrypt(plain, vector.aad, vector.nonce, splice(vector.cipher, vector.tag)), vector.id);
        BOOST_REQUIRE_MESSAGE(plain == data_chunk(vector.plain.size(), 0x00), vector.id);
    }
}

// gcm spec test cases 2 and 4.

BOOST_AUTO_TEST_CASE(aes128_gcm__encrypt__gcm_spec_2__expected)
{
    constexpr aes128_gcm::secret key{};
    constexpr aes128_gcm::nonce iv{};
    const data_chunk plain(16, 0x00);

    aes128_gcm cipher{ key };
    data_chunk out(plain.size() + aes128_gcm::expansion);
    cipher.encrypt(plain, {}, iv, out);
    BOOST_REQUIRE_EQUAL(out, base16_chunk("0388dace60b6a392f328c2b971b2fe78ab6e47d42cec13bdf53a67b21257bddf"));
}

BOOST_AUTO_TEST_CASE(aes128_gcm__encrypt_decrypt__gcm_spec_4__expected)
{
    constexpr auto key = base16_array("feffe9928665731c6d6a8f9467308308");
    constexpr auto iv = base16_array("cafebabefacedbaddecaf888");
    const auto aad = base16_chunk("feedfacedeadbeeffeedfacedeadbeefabaddad2");
    const auto plain = base16_chunk("d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39");
    const auto expected = base16_chunk("42831ec2217774244b7221b784d0d49ce3aa212f2c02a4e035c17e2329aca12e21d514b25466931c7d8f6a5aac84aa051ba30b396a0aac973d58e0915bc94fbc3221a5db94fae95ae7121a47");

    aes128_gcm cipher{ key };
    data_chunk out(plain.size() + aes128_gcm::expansion);
    cipher.encrypt(plain, aad, iv, out);
    BOOST_REQUIRE_EQUAL(out, expected);

    data_chunk decrypted(plain.size());
    BOOST_REQUIRE(cipher.decrypt(decrypted, aad, iv, out));
    BOOST_REQUIRE_EQUAL(decrypted, plain);
}

// Segments, rekey, tamper.

// Split points of a 300 byte plain text, at and around block boundaries.
constexpr std_array<size_t, 7> splits{ 0, 1, 15, 16, 17, 299, 300 };

BOOST_AUTO_TEST_CASE(aes128_gcm__encrypt__segments__same_as_whole)
{
    constexpr auto key = base16_array("feffe9928665731c6d6a8f9467308308");
    constexpr auto iv = base16_array("cafebabefacedbaddecaf888");
    const auto aad = base16_chunk("feedfacedeadbeef");
    data_chunk plain(splits.back());
    chacha20{ sha256_hash(key) }.stream(plain);

    aes128_gcm cipher{ key };
    data_chunk whole(plain.size() + aes128_gcm::expansion);
    cipher.encrypt(plain, aad, iv, whole);

    for (const auto split: splits)
    {
        const auto first = const_byte_span{ plain }.first(split);
        const auto second = const_byte_span{ plain }.subspan(split);
        data_chunk out(plain.size() + aes128_gcm::expansion);
        cipher.encrypt(first, second, aad, iv, out);
        BOOST_REQUIRE_EQUAL(out, whole);
    }
}

BOOST_AUTO_TEST_CASE(aes128_gcm__set_key__rekey__same_as_new)
{
    constexpr aes128_gcm::secret key1{};
    constexpr auto key2 = base16_array("feffe9928665731c6d6a8f9467308308");
    constexpr aes128_gcm::nonce iv{};
    const data_chunk plain(40, 0x42);

    aes128_gcm cipher1{ key1 };
    aes128_gcm cipher2{ key2 };
    cipher1.set_key(key2);

    data_chunk out1(plain.size() + aes128_gcm::expansion);
    data_chunk out2(plain.size() + aes128_gcm::expansion);
    cipher1.encrypt(plain, {}, iv, out1);
    cipher2.encrypt(plain, {}, iv, out2);
    BOOST_REQUIRE_EQUAL(out1, out2);
}

BOOST_AUTO_TEST_CASE(aes128_gcm__decrypt__tampered_aad__false_cleared)
{
    constexpr auto key = base16_array("feffe9928665731c6d6a8f9467308308");
    constexpr auto iv = base16_array("cafebabefacedbaddecaf888");
    const data_chunk plain(20, 0x42);
    const auto aad = base16_chunk("feedfacedeadbeef");
    const auto tampered = base16_chunk("ffedfacedeadbeef");

    aes128_gcm cipher{ key };
    data_chunk out(plain.size() + aes128_gcm::expansion);
    cipher.encrypt(plain, aad, iv, out);

    data_chunk decrypted(plain.size(), 0xff);
    BOOST_REQUIRE(!cipher.decrypt(decrypted, tampered, iv, out));
    BOOST_REQUIRE_EQUAL(decrypted, data_chunk(plain.size(), 0x00));
}

BOOST_AUTO_TEST_CASE(aes128_gcm__encrypt_decrypt__unaligned__same_as_aligned)
{
    constexpr std_array<size_t, 5> offsets{ 1, 5, 16, 32, 48 };
    const aes128_gcm::secret key{ 0x01, 0x02, 0x03 };
    const aes128_gcm::nonce nonce{ 0x04, 0x05, 0x06 };
    const data_chunk aad{ 0x17, 0x03, 0x03, 0x04, 0x5c };
    const data_chunk plain(1100, 0x2a);
    const auto size = plain.size() + aes128_gcm::expansion;

    aes128_gcm cipher{ key };
    data_chunk expected(size);
    cipher.encrypt(plain, aad, nonce, expected);

    std::for_each(offsets.cbegin(), offsets.cend(), [&](size_t offset)
    {
        data_chunk record(offset + size);
        const auto out = byte_span{ record }.subspan(offset);
        cipher.encrypt(plain, aad, nonce, out);
        BOOST_REQUIRE_EQUAL(data_chunk(out.begin(), out.end()), expected);

        data_chunk buffer(offset + plain.size());
        const auto text = byte_span{ buffer }.subspan(offset);
        BOOST_REQUIRE(cipher.decrypt(text, aad, nonce, out));
        BOOST_REQUIRE_EQUAL(data_chunk(text.begin(), text.end()), plain);
    });
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(aes256_gcm_tests)

// Wycheproof.

BOOST_AUTO_TEST_CASE(aes256_gcm__encrypt__wycheproof_valid__expected)
{
    for (const auto& vector: aes256_gcm_valid_vectors)
    {
        aes256_gcm cipher{ vector.key };
        data_chunk out(vector.plain.size() + aes256_gcm::expansion);
        cipher.encrypt(vector.plain, vector.aad, vector.nonce, out);
        BOOST_REQUIRE_MESSAGE(out == splice(vector.cipher, vector.tag), vector.id);
    }
}

BOOST_AUTO_TEST_CASE(aes256_gcm__decrypt__wycheproof_valid__true_expected)
{
    for (const auto& vector: aes256_gcm_valid_vectors)
    {
        aes256_gcm cipher{ vector.key };
        data_chunk plain(vector.plain.size());
        BOOST_REQUIRE_MESSAGE(cipher.decrypt(plain, vector.aad, vector.nonce, splice(vector.cipher, vector.tag)), vector.id);
        BOOST_REQUIRE_MESSAGE(plain == vector.plain, vector.id);
    }
}

BOOST_AUTO_TEST_CASE(aes256_gcm__decrypt__wycheproof_invalid__false_cleared)
{
    for (const auto& vector: aes256_gcm_invalid_vectors)
    {
        aes256_gcm cipher{ vector.key };
        data_chunk plain(vector.plain.size(), 0xff);
        BOOST_REQUIRE_MESSAGE(!cipher.decrypt(plain, vector.aad, vector.nonce, splice(vector.cipher, vector.tag)), vector.id);
        BOOST_REQUIRE_MESSAGE(plain == data_chunk(vector.plain.size(), 0x00), vector.id);
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ghash_tests)

using ghash_native = aes::ghash<true>;
using ghash_sliced = aes::ghash<false>;

// gcm spec test case 2: H = E(0, 0), GHASH(C || lengths).
constexpr auto hash_key = base16_array("66e94bd4ef8a2c3b884cfa59ca342b2e");

// Aggregate (4 block) boundaries, with remainders.
constexpr std_array<size_t, 16> sizes{ 0, 1, 15, 16, 17, 47, 48, 63, 64, 65, 79, 80, 127, 128, 129, 300 };

constexpr data_array<16> sliced_hash(const data_array<16>& key, const data_array<32>& data)
{
    ghash_sliced hash{ key };
    hash.write(data);
    return hash.flush();
}

BOOST_AUTO_TEST_CASE(ghash__flush__constexpr__native_same)
{
    constexpr auto data = base16_array("0388dace60b6a392f328c2b971b2fe7800000000000000000000000000000080");
    constexpr auto expected = sliced_hash(hash_key, data);

    ghash_native hash{ hash_key };
    hash.write(data);
    BOOST_REQUIRE_EQUAL(hash.flush(), expected);
}

BOOST_AUTO_TEST_CASE(ghash__write__sizes__variants_agree)
{
    data_chunk data(sizes.back());
    chacha20{ sha256_hash(hash_key) }.stream(data);

    for (const auto size: sizes)
    {
        const auto text = const_byte_span{ data }.first(size);
        ghash_native native{ hash_key };
        ghash_sliced sliced{ hash_key };
        native.write(text);
        sliced.write(text);
        native.write(text);
        sliced.write(text);
        BOOST_REQUIRE_EQUAL(native.flush(), sliced.flush());
    }
}

BOOST_AUTO_TEST_CASE(ghash__flush__empty__zero)
{
    ghash_native hash{ hash_key };
    hash.write({});
    BOOST_REQUIRE_EQUAL(hash.flush(), data_array<16>{});
}

BOOST_AUTO_TEST_SUITE_END()

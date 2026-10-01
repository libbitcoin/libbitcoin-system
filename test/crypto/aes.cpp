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

// nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197-upd1.pdf (Appendix A, C)
// nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38a.pdf (F)

BOOST_AUTO_TEST_SUITE(aes_tests)

using aes128_native = aes::algorithm<aes::k128, true, false>;
using aes128_vector = aes::algorithm<aes::k128, false, true>;
using aes128_sliced = aes::algorithm<aes::k128, false, false>;
using aes256_native = aes::algorithm<aes::k256, true, false>;
using aes256_vector = aes::algorithm<aes::k256, false, true>;
using aes256_sliced = aes::algorithm<aes::k256, false, false>;

// Exposes the bitsliced substitution for exhaustive comparison.
struct accessor
  : aes128
{
    using aes128::sbox;
    using aes128::inverse_sbox;
    using aes128::ortho;
};

constexpr data_array<256> fips_sbox = base16_array(
    "637c777bf26b6fc53001672bfed7ab76ca82c97dfa5947f0add4a2af9ca472c0"
    "b7fd9326363ff7cc34a5e5f171d8311504c723c31896059a071280e2eb27b275"
    "09832c1a1b6e5aa0523bd6b329e32f8453d100ed20fcb15b6acbbe394a4c58cf"
    "d0efaafb434d338545f9027f503c9fa851a3408f929d38f5bcb6da2110fff3d2"
    "cd0c13ec5f974417c4a77e3d645d197360814fdc222a908846eeb814de5e0bdb"
    "e0323a0a4906245cc2d3ac629195e479e7c8376d8dd54ea96c56f4ea657aae08"
    "ba78252e1ca6b4c6e8dd741f4bbd8b8a703eb5664803f60e613557b986c11d9e"
    "e1f8981169d98e949b1e87e9ce5528df8ca1890dbfe6426841992d0fb054bb16");

constexpr data_array<16> key128 = base16_array("2b7e151628aed2a6abf7158809cf4f3c");
constexpr data_array<32> key256 = base16_array("603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4");
constexpr data_array<64> plain = base16_array("6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710");

// Lane and pass boundaries of the native (8, 16, 32 block) and bitsliced
// (4, 8, 16, 32 block) passes, with remainders.
constexpr std_array<size_t, 24> sizes
{
    0, 1, 15, 16, 17, 63, 64, 65, 127, 128, 129, 255, 256, 257, 511, 512, 513, 575, 576, 577, 1023, 1024, 1025, 1100
};

template <size_t Size>
struct block_vector
{
    data_array<Size> key;
    data_array<16> plain;
    data_array<16> cipher;
};

// fips197 (appendix C) and sp800-38a (F.1.1, F.1.5).
const std_vector<block_vector<16>> aes128_vectors
{
    { base16_array("000102030405060708090a0b0c0d0e0f"), base16_array("00112233445566778899aabbccddeeff"), base16_array("69c4e0d86a7b0430d8cdb78070b4c55a") },
    { key128, base16_array("6bc1bee22e409f96e93d7e117393172a"), base16_array("3ad77bb40d7a3660a89ecaf32466ef97") },
    { key128, base16_array("ae2d8a571e03ac9c9eb76fac45af8e51"), base16_array("f5d3d58503b9699de785895a96fdbaaf") },
    { key128, base16_array("30c81c46a35ce411e5fbc1191a0a52ef"), base16_array("43b1cd7f598ece23881b00e3ed030688") },
    { key128, base16_array("f69f2445df4f9b17ad2b417be66c3710"), base16_array("7b0c785e27e8ad3f8223207104725dd4") }
};

const std_vector<block_vector<32>> aes256_vectors
{
    { base16_array("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"), base16_array("00112233445566778899aabbccddeeff"), base16_array("8ea2b7ca516745bfeafc49904b496089") },
    { key256, base16_array("6bc1bee22e409f96e93d7e117393172a"), base16_array("f3eed1bdb5d2a03c064b5a7e3db181f8") },
    { key256, base16_array("ae2d8a571e03ac9c9eb76fac45af8e51"), base16_array("591ccb10d410ed26dc5ba74a31362870") },
    { key256, base16_array("30c81c46a35ce411e5fbc1191a0a52ef"), base16_array("b6ed21b99ca6f4f9f153e7b1beafed1d") },
    { key256, base16_array("f69f2445df4f9b17ad2b417be66c3710"), base16_array("23304b7a39f9f3ff067d8d8f9e24ecc7") }
};

static data_array<16> block_at(const data_slice& data, size_t index)
{
    data_array<16> out{};
    std::copy_n(std::next(data.begin(), index * 16u), 16u, out.begin());
    return out;
}

template <typename Algorithm>
constexpr data_array<16> encrypted(const typename Algorithm::key_t& key, data_array<16> block)
{
    Algorithm::encrypt(block, key);
    return block;
}

template <typename Algorithm>
constexpr data_array<16> decrypted(const typename Algorithm::key_t& key, data_array<16> block)
{
    Algorithm::decrypt(block, key);
    return block;
}

template <typename Algorithm>
static data_chunk crypt(const typename Algorithm::key_t& key, typename Algorithm::block_t& counter, const_byte_span in)
{
    data_chunk out(in.size());
    Algorithm::ctr(out, in, counter, Algorithm::expand(key));
    return out;
}

// substitution

constexpr data_array<256> identity = base16_array(
    "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"
    "202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f"
    "404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f"
    "606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f"
    "808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f"
    "a0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
    "c0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedf"
    "e0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff");

template <size_t Start>
constexpr data_array<64> quarter(const data_array<256>& table)
{
    return slice<Start, Start + 64>(table);
}

// Each of the 64 bytes occupies one byte position of the eight words, so
// the transposed planes carry 64 independent bytes.
static data_array<64> substitute(const data_array<64>& bytes)
{
    auto q = from_little_endians(array_cast<uint64_t>(bytes));
    accessor::ortho(q);
    accessor::sbox(q);
    accessor::ortho(q);
    return array_cast<uint8_t>(to_little_endians(q));
}

static data_array<64> inverse_substitute(const data_array<64>& bytes)
{
    auto q = from_little_endians(array_cast<uint64_t>(bytes));
    accessor::ortho(q);
    accessor::inverse_sbox(q);
    accessor::ortho(q);
    return array_cast<uint8_t>(to_little_endians(q));
}

BOOST_AUTO_TEST_CASE(aes__sbox__all_bytes__fips197)
{
    BOOST_REQUIRE_EQUAL(substitute(quarter<0>(identity)), quarter<0>(fips_sbox));
    BOOST_REQUIRE_EQUAL(substitute(quarter<64>(identity)), quarter<64>(fips_sbox));
    BOOST_REQUIRE_EQUAL(substitute(quarter<128>(identity)), quarter<128>(fips_sbox));
    BOOST_REQUIRE_EQUAL(substitute(quarter<192>(identity)), quarter<192>(fips_sbox));
}

BOOST_AUTO_TEST_CASE(aes__inverse_sbox__all_bytes__fips197)
{
    BOOST_REQUIRE_EQUAL(inverse_substitute(quarter<0>(fips_sbox)), quarter<0>(identity));
    BOOST_REQUIRE_EQUAL(inverse_substitute(quarter<64>(fips_sbox)), quarter<64>(identity));
    BOOST_REQUIRE_EQUAL(inverse_substitute(quarter<128>(fips_sbox)), quarter<128>(identity));
    BOOST_REQUIRE_EQUAL(inverse_substitute(quarter<192>(fips_sbox)), quarter<192>(identity));
}

// expand

BOOST_AUTO_TEST_CASE(aes__expand__aes128__fips197_a1)
{
    constexpr auto schedule = aes128::expand(key128);
    static_assert(schedule.front() == key128);
    static_assert(schedule.back() == base16_array("d014f9a8c9ee2589e13f0cc8b6630ca6"));
    BOOST_REQUIRE_EQUAL(aes128::expand(key128), schedule);
}

BOOST_AUTO_TEST_CASE(aes__expand__aes256__fips197_a3)
{
    constexpr auto schedule = aes256::expand(key256);
    static_assert(schedule[0] == base16_array("603deb1015ca71be2b73aef0857d7781"));
    static_assert(schedule[1] == base16_array("1f352c073b6108d72d9810a30914dff4"));
    static_assert(schedule.back() == base16_array("fe4890d1e6188d0b046df344706c631e"));
    BOOST_REQUIRE_EQUAL(aes256::expand(key256), schedule);
}

// encrypt/decrypt

template <typename Algorithm>
static void test_cipher(const typename Algorithm::key_t& key, const data_array<16>& in, const data_array<16>& out)
{
    auto block = in;
    Algorithm::encrypt(block, key);
    BOOST_REQUIRE_EQUAL(block, out);

    Algorithm::decrypt(block, key);
    BOOST_REQUIRE_EQUAL(block, in);

    const auto schedule = Algorithm::expand(key);
    Algorithm::encrypt(block, schedule);
    BOOST_REQUIRE_EQUAL(block, out);

    Algorithm::decrypt(block, schedule);
    BOOST_REQUIRE_EQUAL(block, in);
}

BOOST_AUTO_TEST_CASE(aes__encrypt_decrypt__aes128__vectors__expected)
{
    for (const auto& vector: aes128_vectors)
    {
        test_cipher<aes128_native>(vector.key, vector.plain, vector.cipher);
        test_cipher<aes128_vector>(vector.key, vector.plain, vector.cipher);
        test_cipher<aes128_sliced>(vector.key, vector.plain, vector.cipher);
    }
}

BOOST_AUTO_TEST_CASE(aes__encrypt_decrypt__aes256__vectors__expected)
{
    for (const auto& vector: aes256_vectors)
    {
        test_cipher<aes256_native>(vector.key, vector.plain, vector.cipher);
        test_cipher<aes256_vector>(vector.key, vector.plain, vector.cipher);
        test_cipher<aes256_sliced>(vector.key, vector.plain, vector.cipher);
    }
}

BOOST_AUTO_TEST_CASE(aes__encrypt_decrypt__constexpr__fips197_c1)
{
    constexpr auto key = base16_array("000102030405060708090a0b0c0d0e0f");
    constexpr auto in = base16_array("00112233445566778899aabbccddeeff");
    constexpr auto out = base16_array("69c4e0d86a7b0430d8cdb78070b4c55a");
    static_assert(encrypted<aes128>(key, in) == out);
    static_assert(decrypted<aes128>(key, out) == in);
    BOOST_REQUIRE_EQUAL(encrypted<aes128>(key, in), out);
}

// ctr

BOOST_AUTO_TEST_CASE(aes__ctr__aes128__sp800_38a_f51)
{
    constexpr auto initial = base16_array("f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff");
    constexpr auto next = base16_array("f0f1f2f3f4f5f6f7f8f9fafbfcfdff03");
    const auto expected = base16_chunk("874d6191b620e3261bef6864990db6ce9806f66b7970fdff8617187bb9fffdff5ae4df3edbd5d35e5b4f09020db03eab1e031dda2fbe03d1792170a0f3009cee");

    auto counter1 = initial;
    auto counter2 = initial;
    auto counter3 = initial;
    BOOST_REQUIRE_EQUAL(crypt<aes128_native>(key128, counter1, plain), expected);
    BOOST_REQUIRE_EQUAL(crypt<aes128_vector>(key128, counter2, plain), expected);
    BOOST_REQUIRE_EQUAL(crypt<aes128_sliced>(key128, counter3, plain), expected);
    BOOST_REQUIRE_EQUAL(counter1, next);
    BOOST_REQUIRE_EQUAL(counter2, next);
    BOOST_REQUIRE_EQUAL(counter3, next);
}

BOOST_AUTO_TEST_CASE(aes__ctr__aes256__sp800_38a_f55)
{
    constexpr auto initial = base16_array("f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff");
    const auto expected = base16_chunk("601ec313775789a5b7a7f504bbf3d228f443e3ca4d62b59aca84e990cacaf5c52b0930daa23de94ce87017ba2d84988ddfc9c58db67aada613c2dd08457941a6");

    auto counter1 = initial;
    auto counter2 = initial;
    auto counter3 = initial;
    BOOST_REQUIRE_EQUAL(crypt<aes256_native>(key256, counter1, plain), expected);
    BOOST_REQUIRE_EQUAL(crypt<aes256_vector>(key256, counter2, plain), expected);
    BOOST_REQUIRE_EQUAL(crypt<aes256_sliced>(key256, counter3, plain), expected);
}

BOOST_AUTO_TEST_CASE(aes__ctr__sizes__variants_agree)
{
    constexpr auto initial = base16_array("000102030405060708090a0bfffffff0");
    data_chunk data(sizes.back());
    chacha20{ key256 }.stream(data);

    for (const auto size: sizes)
    {
        const auto text = const_byte_span{ data }.first(size);
        auto counter1 = initial;
        auto counter2 = initial;
        auto counter3 = initial;
        const auto expected = crypt<aes256_sliced>(key256, counter1, text);
        BOOST_REQUIRE_EQUAL(crypt<aes256_native>(key256, counter2, text), expected);
        BOOST_REQUIRE_EQUAL(crypt<aes256_vector>(key256, counter3, text), expected);
        BOOST_REQUIRE_EQUAL(counter2, counter1);
        BOOST_REQUIRE_EQUAL(counter3, counter1);
    }
}

BOOST_AUTO_TEST_CASE(aes__ctr__unaligned__variants_agree)
{
    constexpr auto initial = base16_array("000102030405060708090a0bfffffff0");
    constexpr std_array<size_t, 5> offsets{ 1, 5, 16, 32, 48 };
    alignas(64) data_array<sizes.back() + 64> data{};
    alignas(64) data_array<sizes.back() + 64> out{};
    chacha20{ key256 }.stream(data);

    std::for_each(offsets.cbegin(), offsets.cend(), [&](size_t offset)
    {
        std::for_each(sizes.cbegin(), sizes.cend(), [&](size_t size)
        {
            const auto text = const_byte_span{ data }.subspan(offset, size);
            const auto cipher = byte_span{ out }.subspan(offset, size);
            auto counter1 = initial;
            auto counter2 = initial;
            auto counter3 = initial;
            auto counter4 = initial;
            const auto expected = crypt<aes256_sliced>(key256, counter1, text);

            aes256_native::ctr(cipher, text, counter2, aes256_native::expand(key256));
            BOOST_REQUIRE_EQUAL(data_chunk(cipher.begin(), cipher.end()), expected);

            aes256_vector::ctr(cipher, text, counter3, aes256_vector::expand(key256));
            BOOST_REQUIRE_EQUAL(data_chunk(cipher.begin(), cipher.end()), expected);

            // In place.
            std::copy(text.begin(), text.end(), cipher.begin());
            aes256_native::ctr(cipher, cipher, counter4, aes256_native::expand(key256));
            BOOST_REQUIRE_EQUAL(data_chunk(cipher.begin(), cipher.end()), expected);
            BOOST_REQUIRE_EQUAL(counter2, counter1);
            BOOST_REQUIRE_EQUAL(counter3, counter1);
            BOOST_REQUIRE_EQUAL(counter4, counter1);
        });
    });
}

BOOST_AUTO_TEST_CASE(aes__ctr__keystream__encrypted_counters)
{
    constexpr auto initial = base16_array("000102030405060708090a0bfffffffe");
    const auto schedule = aes128::expand(key128);

    data_array<48> stream{};
    auto counter = initial;
    aes128::ctr(stream, {}, counter, schedule);

    auto block0 = initial;
    auto block1 = base16_array("000102030405060708090a0bffffffff");
    auto block2 = base16_array("000102030405060708090a0b00000000");
    aes128::encrypt(block0, schedule);
    aes128::encrypt(block1, schedule);
    aes128::encrypt(block2, schedule);
    BOOST_REQUIRE_EQUAL(block_at(stream, 0), block0);
    BOOST_REQUIRE_EQUAL(block_at(stream, 1), block1);
    BOOST_REQUIRE_EQUAL(block_at(stream, 2), block2);
    BOOST_REQUIRE_EQUAL(counter, base16_array("000102030405060708090a0b00000001"));
}

BOOST_AUTO_TEST_CASE(aes__ctr__long_keystream__encrypted_zeros)
{
    constexpr auto initial = base16_array("000102030405060708090a0bfffffffe");
    const auto schedule = aes128::expand(key128);

    data_array<256> stream{};
    auto counter1 = initial;
    aes128::ctr(stream, {}, counter1, schedule);

    data_array<256> zeros{};
    auto counter2 = initial;
    aes128::ctr(zeros, zeros, counter2, schedule);
    BOOST_REQUIRE_EQUAL(stream, zeros);
    BOOST_REQUIRE_EQUAL(counter1, counter2);
}

BOOST_AUTO_TEST_CASE(aes__ctr__in_place__expected)
{
    constexpr auto initial = base16_array("f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff");
    const auto expected = base16_chunk("874d6191b620e3261bef6864990db6ce9806f66b7970fdff8617187bb9fffdff5ae4df3edbd5d35e5b4f09020db03eab1e031dda2fbe03d1792170a0f3009cee");

    auto text = plain;
    auto counter = initial;
    aes128::ctr(text, text, counter, aes128::expand(key128));
    BOOST_REQUIRE_EQUAL(to_chunk(text), expected);
}

// cbc_decrypt

template <typename Algorithm>
static void test_cbc(const typename Algorithm::key_t& key, const data_chunk& cipher)
{
    constexpr auto initial = base16_array("000102030405060708090a0b0c0d0e0f");
    const auto schedule = Algorithm::expand(key);

    data_chunk out(cipher.size());
    auto iv = initial;
    Algorithm::cbc_decrypt(out, cipher, iv, schedule);
    BOOST_REQUIRE_EQUAL(out, to_chunk(plain));
    BOOST_REQUIRE_EQUAL(iv, block_at(cipher, 3));

    // In place, as a continuation of two calls.
    auto text = cipher;
    auto chained = initial;
    const std::span<uint8_t> all{ text };
    Algorithm::cbc_decrypt(all.first(16), all.first(16), chained, schedule);
    Algorithm::cbc_decrypt(all.subspan(16), all.subspan(16), chained, schedule);
    BOOST_REQUIRE_EQUAL(text, to_chunk(plain));
}

BOOST_AUTO_TEST_CASE(aes__cbc_decrypt__aes128__sp800_38a_f22)
{
    const auto cipher = base16_chunk("7649abac8119b246cee98e9b12e9197d5086cb9b507219ee95db113a917678b273bed6b8e3c1743b7116e69e222295163ff1caa1681fac09120eca307586e1a7");
    test_cbc<aes128_native>(key128, cipher);
    test_cbc<aes128_vector>(key128, cipher);
    test_cbc<aes128_sliced>(key128, cipher);
}

BOOST_AUTO_TEST_CASE(aes__cbc_decrypt__aes256__sp800_38a_f26)
{
    const auto cipher = base16_chunk("f58c4c04d6e5f1ba779eabfb5f7bfbd69cfc4e967edb808d679f777bc6702c7d39f23369a9d9bacfa530e26304231461b2eb05e2c39be9fcda6c19078c6a9d1b");
    test_cbc<aes256_native>(key256, cipher);
    test_cbc<aes256_vector>(key256, cipher);
    test_cbc<aes256_sliced>(key256, cipher);
}

BOOST_AUTO_TEST_CASE(aes__cbc_decrypt__empty__unchanged)
{
    constexpr auto expected = base16_array("000102030405060708090a0b0c0d0e0f");
    auto iv = expected;
    aes128::cbc_decrypt({}, {}, iv, aes128::expand(key128));
    BOOST_REQUIRE_EQUAL(iv, expected);
}

BOOST_AUTO_TEST_SUITE_END()

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
    
BOOST_AUTO_TEST_SUITE(sha_algorithm_tests)

using sha_160 = sha::algorithm<sha::h160>;
using sha_256 = sha::algorithm<sha::h256<>>;
using sha_512 = sha::algorithm<sha::h512<>>;

// constexpr for all variants of sha160/256/512!
static_assert(sha_160::hash(sha_160::half_t{})  == base16_array("de8a847bff8c343d69b853a215e6ee775ef2ef96"));
static_assert(sha_160::hash(sha_160::block_t{}) == base16_array("c8d7d0ef0eedfa82d2ea1aa592845b9a6d4b02b7"));
static_assert(sha_256::hash(sha_256::half_t{})  == base16_array("66687aadf862bd776c8fc18b8e9f8e20089714856ee233b3902a591d0d5f2925"));
static_assert(sha_256::hash(sha_256::block_t{}) == base16_array("f5a5fd42d16a20302798ef6ed309979b43003d2320d9f0e8ea9831a92759fb4b"));
static_assert(sha_512::hash(sha_512::half_t{})  == base16_array("7be9fda48f4179e611c698a73cff09faf72869431efee6eaad14de0cb44bbf66503f752b7a8eb17083355f3ce6eb7d2806f236b25af96a24e22b887405c20081"));
static_assert(sha_512::hash(sha_512::block_t{}) == base16_array("ab942f526272e456ed68a979f50202905ca903a141ed98443567b11ef0bf25a552d639051a01be58558122c58e3de07d749ee59ded36acf0c55cd91924d6ba11"));

// sha160
// ----------------------------------------------------------------------------

constexpr auto half160 = sha_160::half_t{};
constexpr auto full160 = sha_160::block_t{};
constexpr auto pair160 = std_array<uint8_t, array_count<sha_160::block_t> * two>{};
constexpr auto expected_half160 = base16_array("de8a847bff8c343d69b853a215e6ee775ef2ef96");
constexpr auto expected_full160 = base16_array("c8d7d0ef0eedfa82d2ea1aa592845b9a6d4b02b7");
constexpr auto expected_pair160 = base16_array("0ae4f711ef5d6e9d26c611fd2c8c8ac45ecbf9e7");
constexpr auto expected_merk160 = base16_array("a8782546751527a4a1ad999058165d6af22421c8");

BOOST_AUTO_TEST_CASE(algorithm__hash_half160__null_hash__expected)
{
    static_assert(sha_160::hash(half160) == expected_half160);
    BOOST_CHECK_EQUAL(sha_160::hash(half160), expected_half160);
    BOOST_CHECK_EQUAL(system::sha1_hash(half160), expected_half160);
}

BOOST_AUTO_TEST_CASE(algorithm__hash_full160__null_hash__expected)
{
    static_assert(sha_160::hash(full160) == expected_full160);
    BOOST_CHECK_EQUAL(sha_160::hash(full160), expected_full160);
    BOOST_CHECK_EQUAL(system::sha1_hash(full160), expected_full160);
}

BOOST_AUTO_TEST_CASE(algorithm__hash_blocks160__null_hash__expected)
{
    const std_vector<cref<sha_160::block_t>> pair
    {
        std::cref(full160), std::cref(full160)
    };

    BOOST_CHECK_EQUAL(sha_160::hash(pair), expected_pair160);
    BOOST_CHECK_EQUAL(system::sha1_hash(pair160), expected_pair160);
}

BOOST_AUTO_TEST_CASE(algorithm__double_hash160__null_hash__expected)
{
    static_assert(sha_160::double_hash(full160) == expected_merk160);
    BOOST_CHECK_EQUAL(sha_160::double_hash(full160), expected_merk160);
    BOOST_CHECK_EQUAL(system::sha1_hash(system::sha1_hash(full160)), expected_merk160);
}

BOOST_AUTO_TEST_CASE(algorithm__double_hash_blocks160__null_hash__expected)
{
    const sha_160::digests_t expected
    {
        expected_merk160, expected_merk160
    };

    const sha_160::set_t set
    {
        std::cref(full160), std::cref(full160)
    };

    BOOST_CHECK_EQUAL(sha_160::double_hash(set), expected);
}

// sha256
// ----------------------------------------------------------------------------

constexpr auto half256 = sha_256::half_t{};
constexpr auto full256 = sha_256::block_t{};
constexpr auto pair256 = std_array<uint8_t, array_count<sha_256::block_t> * two>{};
constexpr auto expected_half256 = base16_array("66687aadf862bd776c8fc18b8e9f8e20089714856ee233b3902a591d0d5f2925");
constexpr auto expected_full256 = base16_array("f5a5fd42d16a20302798ef6ed309979b43003d2320d9f0e8ea9831a92759fb4b");
constexpr auto expected_pair256 = base16_array("38723a2e5e8a17aa7950dc008209944e898f69a7bd10a23c839d341e935fd5ca");
constexpr auto expected_merk256 = base16_array("e2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf9");

BOOST_AUTO_TEST_CASE(algorithm__hash_half256__null_hash__expected)
{
    static_assert(sha_256::hash(half256) == expected_half256);
    BOOST_CHECK_EQUAL(sha_256::hash(half256), expected_half256);
    BOOST_CHECK_EQUAL(system::sha256_hash(half256), expected_half256);
}

BOOST_AUTO_TEST_CASE(algorithm__hash_full256__null_hash__expected)
{
    static_assert(sha_256::hash(full256) == expected_full256);
    BOOST_CHECK_EQUAL(sha_256::hash(full256), expected_full256);
    BOOST_CHECK_EQUAL(system::sha256_hash(full256), expected_full256);
}

BOOST_AUTO_TEST_CASE(algorithm__hash_blocks256__null_hash__expected)
{
    const std_vector<cref<sha_256::block_t>> pair
    {
        std::cref(full256), std::cref(full256)
    };

    BOOST_CHECK_EQUAL(sha_256::hash(pair), expected_pair256);
    BOOST_CHECK_EQUAL(system::sha256_hash(pair256), expected_pair256);
}

BOOST_AUTO_TEST_CASE(algorithm__double_hash256__null_hash__expected)
{
    static_assert(sha_256::double_hash(full256) == expected_merk256);
    BOOST_CHECK_EQUAL(sha_256::double_hash(full256), expected_merk256);
    BOOST_CHECK_EQUAL(system::sha256_hash(system::sha256_hash(full256)), expected_merk256);
}

BOOST_AUTO_TEST_CASE(algorithm__double_hash_blocks256__null_hash__expected)
{
    const sha_256::digests_t expected
    {
        expected_merk256, expected_merk256
    };

    const sha_256::set_t set
    {
        std::cref(full256), std::cref(full256)
    };

    BOOST_CHECK_EQUAL(sha_256::double_hash(set), expected);
}

// sha512
// ----------------------------------------------------------------------------

constexpr auto half512 = sha_512::half_t{};
constexpr auto full512 = sha_512::block_t{};
constexpr auto pair512 = std_array<uint8_t, array_count<sha_512::block_t> * two>{};
constexpr auto expected_half512 = base16_array("7be9fda48f4179e611c698a73cff09faf72869431efee6eaad14de0cb44bbf66503f752b7a8eb17083355f3ce6eb7d2806f236b25af96a24e22b887405c20081");
constexpr auto expected_full512 = base16_array("ab942f526272e456ed68a979f50202905ca903a141ed98443567b11ef0bf25a552d639051a01be58558122c58e3de07d749ee59ded36acf0c55cd91924d6ba11");
constexpr auto expected_pair512 = base16_array("693f95d58383a6162d2aab49eb60395dcc4bb22295120caf3f21e3039003230b287c566a03c7a0ca5accaed2133c700b1cb3f82edf8adcbddc92b4f9fb9910c6");
constexpr auto expected_merk512 = base16_array("56d3e5825edf06e467e50dfeb09c1df2d9940121c05d61a162bfcb80aea3aa5fe958d917ac993d76cd3ea86240fedbb79520ce7b9c275793e3c75a82116cc320");

BOOST_AUTO_TEST_CASE(algorithm__hash_half512__null_hash__expected)
{
    static_assert(sha_512::hash(half512) == expected_half512);
    BOOST_CHECK_EQUAL(sha_512::hash(half512), expected_half512);
    BOOST_CHECK_EQUAL(system::sha512_hash(half512), expected_half512);
}

BOOST_AUTO_TEST_CASE(algorithm__hash_full512__null_hash__expected)
{
    static_assert(sha_512::hash(full512) == expected_full512);
    BOOST_CHECK_EQUAL(sha_512::hash(full512), expected_full512);
    BOOST_CHECK_EQUAL(system::sha512_hash(full512), expected_full512);
}

BOOST_AUTO_TEST_CASE(algorithm__hash_blocks512__null_hashes__expected)
{
    const std_vector<cref<sha_512::block_t>> pair
    {
        std::cref(full512), std::cref(full512)
    };

    BOOST_CHECK_EQUAL(sha_512::hash(pair), expected_pair512);
    BOOST_CHECK_EQUAL(system::sha512_hash(pair512), expected_pair512);
}

BOOST_AUTO_TEST_CASE(algorithm__double_hash512__null_hash__expected)
{
    static_assert(sha_512::double_hash(full512) == expected_merk512);
    BOOST_CHECK_EQUAL(sha_512::double_hash(full512), expected_merk512);
    BOOST_CHECK_EQUAL(system::sha512_hash(system::sha512_hash(full512)), expected_merk512);
}

BOOST_AUTO_TEST_CASE(algorithm__double_hash_blocks512__null_hashes__expected)
{
    const sha_512::digests_t expected
    {
        expected_merk512, expected_merk512
    };

    const sha_512::set_t set
    {
        std::cref(full512), std::cref(full512)
    };

    BOOST_CHECK_EQUAL(sha_512::double_hash(set), expected);
}

// Verify types.
// ----------------------------------------------------------------------------

using namespace sha;

// k<,>
static_assert(k<160, 80>::size == 256);
static_assert(k<160, 80>::rounds == 80);
static_assert(k<160, 80>::strength == 160);
static_assert(k<256, 64>::size == 256);
static_assert(k<256, 64>::rounds == 64);
static_assert(k<256, 64>::strength == 256);
static_assert(k<512, 80>::size == 512);
static_assert(k<512, 80>::rounds == 80);
static_assert(k<512, 80>::strength == 512);
static_assert(is_same_type<k<256, 64>::constants_t, std_array<uint32_t, 64>>);
static_assert(is_same_type<k<160, 80>::constants_t, std_array<uint32_t, 80>>);
static_assert(is_same_type<k<512, 80>::constants_t, std_array<uint64_t, 80>>);

// k160
static_assert(k160::size == 256);
static_assert(k160::rounds == 80);
static_assert(k160::strength == 160);
static_assert(k160::get[0] == 0x5a827999);
static_assert(k160::get[19] == 0x5a827999);
static_assert(k160::get[20] == 0x6ed9eba1);
static_assert(k160::get[39] == 0x6ed9eba1);
static_assert(k160::get[40] == 0x8f1bbcdc);
static_assert(k160::get[59] == 0x8f1bbcdc);
static_assert(k160::get[60] == 0xca62c1d6);
static_assert(k160::get[79] == 0xca62c1d6);
static_assert(is_same_type<k160::constants_t, std_array<uint32_t, 80>>);

// k256
static_assert(k256::size == 256);
static_assert(k256::rounds == 64);
static_assert(k256::strength == 256);
static_assert(k256::get[0] == 0x428a2f98);
static_assert(k256::get[63] == 0xc67178f2);
static_assert(is_same_type<k256::constants_t, std_array<uint32_t, 64>>);

// k512
static_assert(k512::size == 512);
static_assert(k512::rounds == 80);
static_assert(k512::strength == 512);
static_assert(k512::get[0] == 0x428a2f98d728ae22);
static_assert(k512::get[79] == 0x6c44198c4a475817);
static_assert(is_same_type<k512::constants_t, std_array<uint64_t, 80>>);

// h<k160,...>
static_assert(h<k160>::size == 256);
static_assert(h<k160>::digest == 160);
static_assert(h<k160, 160>::word_bits == 32);
static_assert(h<k160, 160>::word_bytes == 4);
static_assert(h<k160, 160>::chunk_words == 8);
static_assert(h<k160, 160>::block_words == 16);
static_assert(h<k160, 160>::state_words == 5);
static_assert(is_same_type<h<k160, 160>::K, k160>);
static_assert(is_same_type<h<k160, 160>::word_t, uint32_t>);
static_assert(is_same_type<h<k160, 160>::state_t, std_array<uint32_t, 5>>);

// h<k256,...>
static_assert(h<k256, 256>::size == 256);
static_assert(h<k256, 224>::size == 256);
static_assert(h<k256, 256>::digest == 256);
static_assert(h<k256, 224>::digest == 224);
static_assert(h<k256, 256>::word_bits == 32);
static_assert(h<k256, 224>::word_bits == 32);
static_assert(h<k256, 256>::word_bytes == 4);
static_assert(h<k256, 224>::word_bytes == 4);
static_assert(h<k256, 256>::chunk_words == 8);
static_assert(h<k256, 224>::chunk_words == 8);
static_assert(h<k256, 256>::block_words == 16);
static_assert(h<k256, 224>::block_words == 16);
static_assert(h<k256, 256>::state_words == 8);
static_assert(h<k256, 224>::state_words == 8);
static_assert(is_same_type<h<k256, 256>::K, k256>);
static_assert(is_same_type<h<k256, 224>::K, k256>);
static_assert(is_same_type<h<k256, 256>::word_t, uint32_t>);
static_assert(is_same_type<h<k256, 224>::word_t, uint32_t>);
static_assert(is_same_type<h<k256, 256>::state_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<h<k256, 224>::state_t, std_array<uint32_t, 8>>);

// h<k512,...>
static_assert(h<k512, 512>::size == 512);
static_assert(h<k512, 384>::size == 512);
static_assert(h<k512, 224>::size == 512);
static_assert(h<k512, 256>::size == 512);
static_assert(h<k512, 512>::digest == 512);
static_assert(h<k512, 384>::digest == 384);
static_assert(h<k512, 224>::digest == 224);
static_assert(h<k512, 256>::digest == 256);
static_assert(h<k512, 512>::word_bits == 64);
static_assert(h<k512, 384>::word_bits == 64);
static_assert(h<k512, 224>::word_bits == 64);
static_assert(h<k512, 256>::word_bits == 64);
static_assert(h<k512, 512>::word_bytes == 8);
static_assert(h<k512, 384>::word_bytes == 8);
static_assert(h<k512, 224>::word_bytes == 8);
static_assert(h<k512, 256>::word_bytes == 8);
static_assert(h<k512, 512>::chunk_words == 8);
static_assert(h<k512, 384>::chunk_words == 8);
static_assert(h<k512, 224>::chunk_words == 8);
static_assert(h<k512, 256>::chunk_words == 8);
static_assert(h<k512, 512>::block_words == 16);
static_assert(h<k512, 384>::block_words == 16);
static_assert(h<k512, 224>::block_words == 16);
static_assert(h<k512, 256>::block_words == 16);
static_assert(h<k512, 512>::state_words == 8);
static_assert(h<k512, 384>::state_words == 8);
static_assert(h<k512, 224>::state_words == 8);
static_assert(h<k512, 256>::state_words == 8);
static_assert(is_same_type<h<k512, 512>::K, k512>);
static_assert(is_same_type<h<k512, 384>::K, k512>);
static_assert(is_same_type<h<k512, 224>::K, k512>);
static_assert(is_same_type<h<k512, 256>::K, k512>);
static_assert(is_same_type<h<k512, 512>::word_t, uint64_t>);
static_assert(is_same_type<h<k512, 384>::word_t, uint64_t>);
static_assert(is_same_type<h<k512, 224>::word_t, uint64_t>);
static_assert(is_same_type<h<k512, 256>::word_t, uint64_t>);
static_assert(is_same_type<h<k512, 512>::state_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<h<k512, 384>::state_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<h<k512, 224>::state_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<h<k512, 256>::state_t, std_array<uint64_t, 8>>);

// h160
static_assert(h160::size == 256);
static_assert(h160::digest == 160);
static_assert(h160::rounds == 80);
static_assert(h160::get[0] == 0x67452301);
static_assert(h160::get[4] == 0xc3d2e1f0);
static_assert(h160::get.size() == 5);
static_assert(h160::K::rounds == 80);
static_assert(h160::K::get[0] == 0x5a827999);
static_assert(h160::K::get[79] == 0xca62c1d6);
static_assert(h160::K::get.size() == 80);

// h256<>
static_assert(h256<>::size == 256);
static_assert(h256<>::digest == 256);
static_assert(h256<>::rounds == 64);
static_assert(h256<>::get[0] == 0x6a09e667);
static_assert(h256<>::get[7] == 0x5be0cd19);
static_assert(h256<>::get.size() == 8);
static_assert(h256<>::K::rounds == 64);
static_assert(h256<>::K::get[0] == 0x428a2f98);
static_assert(h256<>::K::get[63] == 0xc67178f2);
static_assert(h256<>::K::get.size() == 64);

// h512<>
static_assert(h512<>::size == 512);
static_assert(h512<>::digest == 512);
static_assert(h512<>::rounds == 80);
static_assert(h512<>::get[0] == 0x6a09e667f3bcc908);
static_assert(h512<>::get[7] == 0x5be0cd19137e2179);
static_assert(h512<>::get.size() == 8);
static_assert(h512<>::K::rounds == 80);
static_assert(h512<>::K::get[0] == 0x428a2f98d728ae22);
static_assert(h512<>::K::get[79] == 0x6c44198c4a475817);
static_assert(h512<>::K::get.size() == 80);

// sha160
static_assert(sha160::size == 256);
static_assert(sha160::digest == 160);
static_assert(sha160::get.size() == 5);
static_assert(sha160::get[0] == 0x67452301);
static_assert(sha160::get[4] == 0xc3d2e1f0);
static_assert(sha160::K::rounds == 80);
static_assert(sha160::K::get.size() == 80);
static_assert(sha160::K::get[0] == 0x5a827999);
static_assert(sha160::K::get[79] == 0xca62c1d6);

// sha256 (namespace conflict)
static_assert(sha::sha256::size == 256);
static_assert(sha::sha256::digest == 256);
static_assert(sha::sha256::get.size() == 8);
static_assert(sha::sha256::get[0] == 0x6a09e667);
static_assert(sha::sha256::get[7] == 0x5be0cd19);
static_assert(sha::sha256::K::rounds == 64);
static_assert(sha::sha256::K::get.size() == 64);
static_assert(sha::sha256::K::get[0] == 0x428a2f98);
static_assert(sha::sha256::K::get[63] == 0xc67178f2);

// sha256_256
static_assert(sha256_256::size == 256);
static_assert(sha256_256::digest == 256);
static_assert(sha256_256::get.size() == 8);
static_assert(sha256_256::get[0] == 0x6a09e667);
static_assert(sha256_256::get[7] == 0x5be0cd19);
static_assert(sha256_256::K::rounds == 64);
static_assert(sha256_256::K::get.size() == 64);
static_assert(sha256_256::K::get[0] == 0x428a2f98);
static_assert(sha256_256::K::get[63] == 0xc67178f2);

// sha512
static_assert(sha512::size == 512);
static_assert(sha512::digest == 512);
static_assert(sha512::get.size() == 8);
static_assert(sha512::get[0] == 0x6a09e667f3bcc908);
static_assert(sha512::get[7] == 0x5be0cd19137e2179);
static_assert(sha512::K::rounds == 80);
static_assert(sha512::K::get.size() == 80);
static_assert(sha512::K::get[0] == 0x428a2f98d728ae22);
static_assert(sha512::K::get[79] == 0x6c44198c4a475817);

// sha512_512
static_assert(sha512_512::size == 512);
static_assert(sha512_512::digest == 512);
static_assert(sha512_512::get.size() == 8);
static_assert(sha512_512::get[0] == 0x6a09e667f3bcc908);
static_assert(sha512_512::get[7] == 0x5be0cd19137e2179);
static_assert(sha512_512::K::rounds == 80);
static_assert(sha512_512::K::get.size() == 80);
static_assert(sha512_512::K::get[0] == 0x428a2f98d728ae22);
static_assert(sha512_512::K::get[79] == 0x6c44198c4a475817);

// Truncations.
static_assert(sha256_224::digest == 224);
static_assert(sha256_224::digest == 224);
static_assert(sha512_256::digest == 256);
static_assert(sha512_224::digest == 224);
static_assert(sha512_384::digest == 384);

// algorithm<sha160>
static_assert(algorithm<sha160>::big_end_count);
static_assert(algorithm<sha160>::count_bits == 64u);
static_assert(algorithm<sha160>::count_bytes == 8u);
static_assert(algorithm<sha160>::H::get.size() == 5u);
static_assert(algorithm<sha160>::K::get.size() == 80u);
static_assert(algorithm<sha160>::limit_bits == std::numeric_limits<uint64_t>::max() - 64u);
static_assert(algorithm<sha160>::limit_bytes == algorithm<sha160>::limit_bits / byte_bits);
static_assert(is_same_type<algorithm<sha160>::byte_t, uint8_t>);
static_assert(is_same_type<algorithm<sha160>::word_t, uint32_t>);
static_assert(is_same_type<algorithm<sha160>::state_t, std_array<uint32_t, 5>>);
static_assert(is_same_type<algorithm<sha160>::chunk_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<algorithm<sha160>::words_t, std_array<uint32_t, 16>>);
static_assert(is_same_type<algorithm<sha160>::buffer_t, std_array<uint32_t, 80>>);
static_assert(is_same_type<algorithm<sha160>::block_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<sha160>::half_t, std_array<uint8_t, 32>>);
static_assert(is_same_type<algorithm<sha160>::digest_t, std_array<uint8_t, 20>>);
static_assert(is_same_type<algorithm<sha160>::count_t, uint64_t>);
static_assert(is_same_type<algorithm<sha160>::digests_t, std_vector<std_array<uint8_t, 20>>>);
static_assert(is_same_type<algorithm<sha160>::set_t, std_vector<cref<std_array<uint8_t, 64>>>>);
static_assert(is_same_type<algorithm<sha160>::sets_t<42>, std_vector<cref<std_array<std_array<uint8_t, 64>, 42>>>>);
static_assert(is_same_type<decltype(algorithm<sha160>::limit_bits), const uint64_t>);
static_assert(is_same_type<decltype(algorithm<sha160>::limit_bytes), const uint64_t>);

// algorithm<sha::sha256>
static_assert(algorithm<sha::sha256>::big_end_count);
static_assert(algorithm<sha::sha256>::count_bits == 64u);
static_assert(algorithm<sha::sha256>::count_bytes == 8u);
static_assert(algorithm<sha::sha256>::H::get.size() == 8u);
static_assert(algorithm<sha::sha256>::K::get.size() == 64u);
static_assert(algorithm<sha::sha256>::limit_bits == std::numeric_limits<uint64_t>::max() - 64u);
static_assert(algorithm<sha::sha256>::limit_bytes == algorithm<sha::sha256>::limit_bits / byte_bits);
static_assert(is_same_type<algorithm<sha::sha256>::byte_t, uint8_t>);
static_assert(is_same_type<algorithm<sha::sha256>::word_t, uint32_t>);
static_assert(is_same_type<algorithm<sha::sha256>::state_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<algorithm<sha::sha256>::chunk_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<algorithm<sha::sha256>::words_t, std_array<uint32_t, 16>>);
static_assert(is_same_type<algorithm<sha::sha256>::buffer_t, std_array<uint32_t, 64>>);
static_assert(is_same_type<algorithm<sha::sha256>::block_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<sha::sha256>::half_t, std_array<uint8_t, 32>>);
static_assert(is_same_type<algorithm<sha::sha256>::digest_t, std_array<uint8_t, 32>>);
static_assert(is_same_type<algorithm<sha::sha256>::count_t, uint64_t>);
static_assert(is_same_type<algorithm<sha::sha256>::digests_t, std_vector<std_array<uint8_t, 32>>>);
static_assert(is_same_type<algorithm<sha::sha256>::set_t, std_vector<cref<std_array<uint8_t, 64>>>>);
static_assert(is_same_type<algorithm<sha::sha256>::sets_t<42>, std_vector<cref<std_array<std_array<uint8_t, 64>, 42>>>>);
static_assert(is_same_type<decltype(algorithm<sha::sha256>::limit_bits), const uint64_t>);
static_assert(is_same_type<decltype(algorithm<sha::sha256>::limit_bytes), const uint64_t>);

// algorithm<sha512>
static_assert(algorithm<sha512>::big_end_count);
static_assert(algorithm<sha512>::count_bits == 128u);
static_assert(algorithm<sha512>::count_bytes == 16u);
static_assert(algorithm<sha512>::H::get.size() == 8u);
static_assert(algorithm<sha512>::K::get.size() == 80u);
static_assert(algorithm<sha512>::limit_bits == std::numeric_limits<uint128_t>::max() - 128u);
static_assert(algorithm<sha512>::limit_bytes == algorithm<sha512>::limit_bits / byte_bits);
static_assert(is_same_type<algorithm<sha512>::byte_t, uint8_t>);
static_assert(is_same_type<algorithm<sha512>::word_t, uint64_t>);
static_assert(is_same_type<algorithm<sha512>::state_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<algorithm<sha512>::chunk_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<algorithm<sha512>::words_t, std_array<uint64_t, 16>>);
static_assert(is_same_type<algorithm<sha512>::buffer_t, std_array<uint64_t, 80>>);
static_assert(is_same_type<algorithm<sha512>::block_t, std_array<uint8_t, 128>>);
static_assert(is_same_type<algorithm<sha512>::half_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<sha512>::digest_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<sha512>::count_t, uint128_t>);
static_assert(is_same_type<algorithm<sha512>::digests_t, std_vector<std_array<uint8_t, 64>>>);
static_assert(is_same_type<algorithm<sha512>::set_t, std_vector<cref<std_array<uint8_t, 128>>>>);
static_assert(is_same_type<algorithm<sha512>::sets_t<42>, std_vector<cref<std_array<std_array<uint8_t, 128>, 42>>>>);
static_assert(is_same_type<decltype(algorithm<sha512>::limit_bits), const uint128_t>);
static_assert(is_same_type<decltype(algorithm<sha512>::limit_bytes), const uint128_t>);

BOOST_AUTO_TEST_SUITE_END()

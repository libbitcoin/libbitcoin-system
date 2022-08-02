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
#include "../../test.hpp"
    
BOOST_AUTO_TEST_SUITE(sha_algorithm_tests)

constexpr auto half160 = sha160::half_t{};
constexpr auto full160 = sha160::block_t{};
constexpr auto pair160 = std_array<uint8_t, array_count<sha160::block_t> * two>{};

constexpr auto half256 = sha256::half_t{};
constexpr auto full256 = sha256::block_t{};
constexpr auto pair256 = std_array<uint8_t, array_count<sha256::block_t> * two>{};

constexpr auto half512 = sha512::half_t{};
constexpr auto full512 = sha512::block_t{};
constexpr auto pair512 = std_array<uint8_t, array_count<sha512::block_t> * two>{};

constexpr auto expected_half160 = base16_array("de8a847bff8c343d69b853a215e6ee775ef2ef96");
constexpr auto expected_full160 = base16_array("c8d7d0ef0eedfa82d2ea1aa592845b9a6d4b02b7");
constexpr auto expected_pair160 = base16_array("0ae4f711ef5d6e9d26c611fd2c8c8ac45ecbf9e7");
constexpr auto expected_merk160 = base16_array("a8782546751527a4a1ad999058165d6af22421c8");

constexpr auto expected_half256 = base16_array("66687aadf862bd776c8fc18b8e9f8e20089714856ee233b3902a591d0d5f2925");
constexpr auto expected_full256 = base16_array("f5a5fd42d16a20302798ef6ed309979b43003d2320d9f0e8ea9831a92759fb4b");
constexpr auto expected_pair256 = base16_array("38723a2e5e8a17aa7950dc008209944e898f69a7bd10a23c839d341e935fd5ca");
constexpr auto expected_merk256 = base16_array("e2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf9");

constexpr auto expected_half512 = base16_array("7be9fda48f4179e611c698a73cff09faf72869431efee6eaad14de0cb44bbf66503f752b7a8eb17083355f3ce6eb7d2806f236b25af96a24e22b887405c20081");
constexpr auto expected_full512 = base16_array("ab942f526272e456ed68a979f50202905ca903a141ed98443567b11ef0bf25a552d639051a01be58558122c58e3de07d749ee59ded36acf0c55cd91924d6ba11");
constexpr auto expected_pair512 = base16_array("693f95d58383a6162d2aab49eb60395dcc4bb22295120caf3f21e3039003230b287c566a03c7a0ca5accaed2133c700b1cb3f82edf8adcbddc92b4f9fb9910c6");
constexpr auto expected_merk512 = base16_array("56d3e5825edf06e467e50dfeb09c1df2d9940121c05d61a162bfcb80aea3aa5fe958d917ac993d76cd3ea86240fedbb79520ce7b9c275793e3c75a82116cc320");

// SHA aliases.
static_assert(is_same_type<sha::algorithm<sha::h160>,      sha160>);
static_assert(is_same_type<sha::algorithm<sha::h256<224>>, sha256_224>);
static_assert(is_same_type<sha::algorithm<sha::h256<>>,    sha256>);
static_assert(is_same_type<sha::algorithm<sha::h512<256>>, sha512_256>);
static_assert(is_same_type<sha::algorithm<sha::h512<224>>, sha512_224>);
static_assert(is_same_type<sha::algorithm<sha::h512<384>>, sha512_384>);
static_assert(is_same_type<sha::algorithm<sha::h512<>>,    sha512>);

// constexpr for all variants of sha160/256/512!
static_assert(sha160::hash(half160)        == expected_half160);
static_assert(sha160::hash(full160)        == expected_full160);
static_assert(sha160::double_hash(full160) == expected_merk160);
static_assert(sha256::hash(half256)        == expected_half256);
static_assert(sha256::hash(full256)        == expected_full256);
static_assert(sha256::double_hash(full256) == expected_merk256);
static_assert(sha512::hash(half512)        == expected_half512);
static_assert(sha512::hash(full512)        == expected_full512);
static_assert(sha512::double_hash(full512) == expected_merk512);

// sha160
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(algorithm__hash_half160__null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha160::hash(half160), expected_half160);
    BOOST_CHECK_EQUAL(sha1_hash(half160), expected_half160);
}

BOOST_AUTO_TEST_CASE(algorithm__hash_full160__null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha160::hash(full160), expected_full160);
    BOOST_CHECK_EQUAL(sha1_hash(full160), expected_full160);
}

BOOST_AUTO_TEST_CASE(algorithm__hash_blocks160__null_hash__expected)
{
    const std_vector<cref<sha160::block_t>> pair
    {
        std::cref(full160), std::cref(full160)
    };

    BOOST_CHECK_EQUAL(sha160::hash(pair), expected_pair160);
    BOOST_CHECK_EQUAL(sha1_hash(pair160), expected_pair160);
}

BOOST_AUTO_TEST_CASE(algorithm__double_hash160__null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha160::double_hash(full160), expected_merk160);
    BOOST_CHECK_EQUAL(sha1_hash(sha1_hash(full160)), expected_merk160);
}

BOOST_AUTO_TEST_CASE(algorithm__double_hash_blocks160__null_hash__expected)
{
    const sha160::digests_t expected
    {
        expected_merk160, expected_merk160
    };

    const sha160::set_t set
    {
        std::cref(full160), std::cref(full160)
    };

    BOOST_CHECK_EQUAL(sha160::double_hash(set), expected);
}

// sha256
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(algorithm__hash_half256__null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha256::hash(half256), expected_half256);
    BOOST_CHECK_EQUAL(sha256_hash(half256), expected_half256);
}

BOOST_AUTO_TEST_CASE(algorithm__hash_full256__null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha256::hash(full256), expected_full256);
    BOOST_CHECK_EQUAL(sha256_hash(full256), expected_full256);
}

BOOST_AUTO_TEST_CASE(algorithm__hash_blocks256__null_hash__expected)
{
    const std_vector<cref<sha256::block_t>> pair
    {
        std::cref(full256), std::cref(full256)
    };

    BOOST_CHECK_EQUAL(sha256::hash(pair), expected_pair256);
    BOOST_CHECK_EQUAL(sha256_hash(pair256), expected_pair256);
}

BOOST_AUTO_TEST_CASE(algorithm__double_hash256__null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha256::double_hash(full256), expected_merk256);
    BOOST_CHECK_EQUAL(sha256_hash(sha256_hash(full256)), expected_merk256);
}

BOOST_AUTO_TEST_CASE(algorithm__double_hash_blocks256__null_hash__expected)
{
    const sha256::digests_t expected
    {
        expected_merk256, expected_merk256
    };

    const sha256::set_t set
    {
        std::cref(full256), std::cref(full256)
    };

    BOOST_CHECK_EQUAL(sha256::double_hash(set), expected);
}

// sha512
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(algorithm__hash_half512__null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha512::hash(half512), expected_half512);
    BOOST_CHECK_EQUAL(sha512_hash(half512), expected_half512);
}

BOOST_AUTO_TEST_CASE(algorithm__hash_full512__null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha512::hash(full512), expected_full512);
    BOOST_CHECK_EQUAL(sha512_hash(full512), expected_full512);
}

BOOST_AUTO_TEST_CASE(algorithm__hash_blocks512__null_hashes__expected)
{
    const std_vector<cref<sha512::block_t>> pair
    {
        std::cref(full512), std::cref(full512)
    };

    BOOST_CHECK_EQUAL(sha512::hash(pair), expected_pair512);
    BOOST_CHECK_EQUAL(sha512_hash(pair512), expected_pair512);
}

BOOST_AUTO_TEST_CASE(algorithm__double_hash512__null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha512::double_hash(full512), expected_merk512);
    BOOST_CHECK_EQUAL(sha512_hash(sha512_hash(full512)), expected_merk512);
}

BOOST_AUTO_TEST_CASE(algorithm__double_hash_blocks512__null_hashes__expected)
{
    const sha512::digests_t expected
    {
        expected_merk512, expected_merk512
    };

    const sha512::set_t set
    {
        std::cref(full512), std::cref(full512)
    };

    BOOST_CHECK_EQUAL(sha512::double_hash(set), expected);
}

// Verify types.
// ----------------------------------------------------------------------------

using namespace sha;

// k<,>
static_assert(k<160, 80>::strength == 160);
static_assert(k<160, 80>::rounds == 80);
static_assert(k<160, 80>::columns == 20);
static_assert(k<160, 80>::rows == 8);
static_assert(k<160, 80>::size == 256);
static_assert(is_same_type<k<160, 80>::constants_t, std_array<uint32_t, 80>>);
static_assert(k<256, 64>::strength == 256);
static_assert(k<256, 64>::rounds == 64);
static_assert(k<256, 80>::columns == 20);
static_assert(k<256, 80>::rows == 12);
static_assert(k<256, 64>::size == 256);
static_assert(is_same_type<k<256, 64>::constants_t, std_array<uint32_t, 64>>);
static_assert(k<512, 80>::strength == 512);
static_assert(k<512, 80>::rounds == 80);
static_assert(k<512, 80>::columns == 20);
static_assert(k<512, 80>::rows == 25);
static_assert(k<512, 80>::size == 512);
static_assert(is_same_type<k<512, 80>::constants_t, std_array<uint64_t, 80>>);

// k160
static_assert(k160::strength == 160);
static_assert(k160::rounds == 80);
static_assert(k160::columns == 20);
static_assert(k160::rows == 8);
static_assert(k160::size == 256);
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
static_assert(k256::strength == 256);
static_assert(k256::rounds == 64);
static_assert(k256::columns == 20);
static_assert(k256::rows == 12);
static_assert(k256::size == 256);
static_assert(k256::get[0] == 0x428a2f98);
static_assert(k256::get[63] == 0xc67178f2);
static_assert(is_same_type<k256::constants_t, std_array<uint32_t, 64>>);

// k512
static_assert(k512::strength == 512);
static_assert(k512::rounds == 80);
static_assert(k512::columns == 20);
static_assert(k512::rows == 25);
static_assert(k512::size == 512);
static_assert(k512::get[0] == 0x428a2f98d728ae22);
static_assert(k512::get[79] == 0x6c44198c4a475817);
static_assert(is_same_type<k512::constants_t, std_array<uint64_t, 80>>);

// h<k160,...>
////static_assert(h<rmd::k160>::size == 256);
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
////static_assert(rmd160::big_end_count);
static_assert(sha160::big_end_count);
static_assert(sha160::count_bits == 64u);
static_assert(sha160::count_bytes == 8u);
static_assert(sha160::H::get.size() == 5u);
static_assert(sha160::K::get.size() == 80u);
static_assert(sha160::limit_bits == std::numeric_limits<uint64_t>::max() - 64u);
static_assert(sha160::limit_bytes == sha160::limit_bits / byte_bits);
static_assert(is_same_type<sha160::byte_t, uint8_t>);
static_assert(is_same_type<sha160::word_t, uint32_t>);
static_assert(is_same_type<sha160::state_t, std_array<uint32_t, 5>>);
static_assert(is_same_type<sha160::chunk_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<sha160::words_t, std_array<uint32_t, 16>>);
static_assert(is_same_type<sha160::buffer_t, std_array<uint32_t, 80>>);
static_assert(is_same_type<sha160::block_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<sha160::half_t, std_array<uint8_t, 32>>);
static_assert(is_same_type<sha160::digest_t, std_array<uint8_t, 20>>);
static_assert(is_same_type<sha160::count_t, uint64_t>);
static_assert(is_same_type<sha160::digests_t, std_vector<std_array<uint8_t, 20>>>);
static_assert(is_same_type<sha160::set_t, std_vector<cref<std_array<uint8_t, 64>>>>);
static_assert(is_same_type<sha160::sets_t<42>, std_vector<cref<std_array<std_array<uint8_t, 64>, 42>>>>);
static_assert(is_same_type<decltype(sha160::limit_bits), const uint64_t>);
static_assert(is_same_type<decltype(sha160::limit_bytes), const uint64_t>);

// sha256
static_assert(sha256::big_end_count);
static_assert(sha256::count_bits == 64u);
static_assert(sha256::count_bytes == 8u);
static_assert(sha256::H::get.size() == 8u);
static_assert(sha256::K::get.size() == 64u);
static_assert(sha256::limit_bits == std::numeric_limits<uint64_t>::max() - 64u);
static_assert(sha256::limit_bytes == sha256::limit_bits / byte_bits);
static_assert(is_same_type<sha256::byte_t, uint8_t>);
static_assert(is_same_type<sha256::word_t, uint32_t>);
static_assert(is_same_type<sha256::state_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<sha256::chunk_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<sha256::words_t, std_array<uint32_t, 16>>);
static_assert(is_same_type<sha256::buffer_t, std_array<uint32_t, 64>>);
static_assert(is_same_type<sha256::block_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<sha256::half_t, std_array<uint8_t, 32>>);
static_assert(is_same_type<sha256::digest_t, std_array<uint8_t, 32>>);
static_assert(is_same_type<sha256::count_t, uint64_t>);
static_assert(is_same_type<sha256::digests_t, std_vector<std_array<uint8_t, 32>>>);
static_assert(is_same_type<sha256::set_t, std_vector<cref<std_array<uint8_t, 64>>>>);
static_assert(is_same_type<sha256::sets_t<42>, std_vector<cref<std_array<std_array<uint8_t, 64>, 42>>>>);
static_assert(is_same_type<decltype(sha256::limit_bits), const uint64_t>);
static_assert(is_same_type<decltype(sha256::limit_bytes), const uint64_t>);

// sha512
static_assert(sha512::big_end_count);
static_assert(sha512::count_bits == 128u);
static_assert(sha512::count_bytes == 16u);
static_assert(sha512::H::get.size() == 8u);
static_assert(sha512::K::get.size() == 80u);
static_assert(sha512::limit_bits == std::numeric_limits<uint128_t>::max() - 128u);
static_assert(sha512::limit_bytes == sha512::limit_bits / byte_bits);
static_assert(is_same_type<sha512::byte_t, uint8_t>);
static_assert(is_same_type<sha512::word_t, uint64_t>);
static_assert(is_same_type<sha512::state_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<sha512::chunk_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<sha512::words_t, std_array<uint64_t, 16>>);
static_assert(is_same_type<sha512::buffer_t, std_array<uint64_t, 80>>);
static_assert(is_same_type<sha512::block_t, std_array<uint8_t, 128>>);
static_assert(is_same_type<sha512::half_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<sha512::digest_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<sha512::count_t, uint128_t>);
static_assert(is_same_type<sha512::digests_t, std_vector<std_array<uint8_t, 64>>>);
static_assert(is_same_type<sha512::set_t, std_vector<cref<std_array<uint8_t, 128>>>>);
static_assert(is_same_type<sha512::sets_t<42>, std_vector<cref<std_array<std_array<uint8_t, 128>, 42>>>>);
static_assert(is_same_type<decltype(sha512::limit_bits), const uint128_t>);
static_assert(is_same_type<decltype(sha512::limit_bytes), const uint128_t>);

// Truncations.
static_assert(sha256_224::big_end_count);
static_assert(sha256_224::big_end_count);
static_assert(sha512_256::big_end_count);
static_assert(sha512_224::big_end_count);
static_assert(sha512_384::big_end_count);

BOOST_AUTO_TEST_SUITE_END()

#if defined(DISABLED)

// This is the completely unrolled preparing function.
// This makes 48 calls (16-64) to (3+ ops):
// buffer[a] =
//     buffer[b] + sigma0(buffer[c]) +
//     buffer[d] + sigma1(buffer[e]);
//
// Where sigmas(x) are the following (4^, 4rotl, 2>> ops).
// sigma0: std::rotr(x,  7) ^ std::rotr(x, 18) ^ (x >>  3);
// sigma1: std::rotr(x, 17) ^ std::rotr(x, 19) ^ (x >> 10);
//
//    1   working data word (index shifted)
//    8   working variables (indexes shifted)
// ------------------------------------------------------------
//    9   mov ops required per round
//   
//   48   rounds (prepare 3 blocks)
//   13   math ops required per round (3add, 4xor, 4ror, 2shr)
//  -16   ops, first block without k values (no +)
// ------------------------------------------------------------
//   12.7 average math ops required per round
//   
// ============================================================
//   21.7 average ops required per round
//   
//  611   math ops observed
//  425   mov  ops observed
// ------------------------------------------------------------
// 1036   total ops observed
//   
//  9.000 mov ops required per round
//  8.854 mov ops observed per round
// ------------------------------------------------------------
//101.65% move efficiency
//   
// 12.667 math ops required per round (average)
// 12.729 math ops observed per round
// ------------------------------------------------------------
// 99.51% math efficiency
//   
// 21.667 ops required per round (average)
// 21.583 ops observed per round
// ------------------------------------------------------------
//100.39% compiler net efficiency (better than manual assembly)

preparing(buffer_t& buffer) NOEXCEPT
{
    auto pbuffer = buffer.data();
00007FF697D823B0  mov         qword ptr [rsp+8],rcx  

// 8 working variables pushed onto the stack.
00007FF697D823B5  push        rbx  
00007FF697D823B6  push        rbp  
00007FF697D823B7  push        rsi  
00007FF697D823B8  push        rdi  
00007FF697D823B9  push        r12  
00007FF697D823BB  push        r13  
00007FF697D823BD  push        r14  
00007FF697D823BF  push        r15  

// Rotate 8 working variables.
00007FF697D823C1  sub         rsp,28h  
00007FF697D823C5  mov         r9d,dword ptr [rcx+4]  
00007FF697D823C9  mov         rdi,rcx  
00007FF697D823CC  mov         r12d,dword ptr [rcx+38h]  
00007FF697D823D0  mov         eax,r9d  
00007FF697D823D3  mov         r10d,dword ptr [rcx+8]  
00007FF697D823D7  mov         r8d,r9d  
00007FF697D823DA  mov         r14d,dword ptr [rcx+24h]  
00007FF697D823DE  mov         edx,r9d  
00007FF697D823E1  mov         r15d,dword ptr [rdi+3Ch]  
00007FF697D823E5  mov         r11d,r12d  
00007FF697D823E8  mov         ebp,dword ptr [rdi+28h]  
00007FF697D823EB  mov         ebx,r15d  
00007FF697D823EE  mov         esi,dword ptr [rdi+2Ch]  

// Rounds (add, xor, ror, shr, mov).
// Inline c++ comments are an approximation (last reference).
00007FF697D823F1  ror         eax,12h  
00007FF697D823F4  shr         r8d,3  
00007FF697D823F8  xor         r8d,eax  
00007FF697D823FB  ror         edx,7  
00007FF697D823FE  xor         r8d,edx  
00007FF697D82401  shr         r11d,0Ah  
00007FF697D82405  shr         ebx,0Ah  
00007FF697D82408  mov         eax,r12d  
00007FF697D8240B  ror         eax,13h  
00007FF697D8240E  mov         edx,r12d  
00007FF697D82411  xor         r11d,eax  
00007FF697D82414  ror         edx,11h  
00007FF697D82417  xor         r11d,edx  
00007FF697D8241A  mov         eax,r10d  
00007FF697D8241D  ror         eax,12h  
00007FF697D82420  add         r11d,r14d  
00007FF697D82423  add         r11d,r8d  
00007FF697D82426  mov         edx,r15d  
00007FF697D82429  add         r11d,dword ptr [rcx]  
00007FF697D8242C  mov         r8d,r10d  
00007FF697D8242F  shr         r8d,3  
00007FF697D82433  mov         r13d,r11d  
00007FF697D82436  xor         r8d,eax  
00007FF697D82439  mov         dword ptr [rcx+40h],r11d  
00007FF697D8243D  shr         r13d,0Ah  
00007FF697D82441  mov         eax,r15d  
00007FF697D82444  ror         eax,13h  
00007FF697D82447  mov         ecx,r10d  
00007FF697D8244A  xor         ebx,eax  
00007FF697D8244C  ror         ecx,7  
00007FF697D8244F  xor         r8d,ecx  
00007FF697D82452  ror         edx,11h  
00007FF697D82455  xor         ebx,edx  
    prepare<16>(pbuffer);
00007FF697D82457  mov         dword ptr [rsp+80h],r11d  
00007FF697D8245F  add         ebx,ebp  
00007FF697D82461  mov         edx,r11d  
00007FF697D82464  add         ebx,r8d  
00007FF697D82467  ror         edx,11h  
00007FF697D8246A  add         ebx,r9d  
00007FF697D8246D  mov         r9d,dword ptr [rdi+0Ch]  
00007FF697D82471  mov         eax,r9d  
00007FF697D82474  mov         dword ptr [rsp+88h],ebx 
00007FF697D8247B  ror         eax,12h  
00007FF697D8247E  mov         r8d,r9d  
00007FF697D82481  shr         r8d,3  
00007FF697D82485  mov         ecx,r9d  
00007FF697D82488  xor         r8d,eax  
00007FF697D8248B  ror         ecx,7  
00007FF697D8248E  xor         r8d,ecx  
    prepare<17>(pbuffer);
00007FF697D82491  mov         dword ptr [rdi+44h],ebx  
00007FF697D82494  mov         eax,r11d  
00007FF697D82497  mov         r11d,dword ptr [rdi+10h]  
00007FF697D8249B  ror         eax,13h  
00007FF697D8249E  mov         ecx,r11d  
00007FF697D824A1  xor         r13d,eax  
00007FF697D824A4  ror         ecx,7  
00007FF697D824A7  xor         r13d,edx  
00007FF697D824AA  mov         eax,r11d  
00007FF697D824AD  add         r13d,esi  
00007FF697D824B0  ror         eax,12h  
00007FF697D824B3  add         r13d,r8d  
00007FF697D824B6  mov         r8d,r11d  
00007FF697D824B9  add         r13d,r10d  
00007FF697D824BC  shr         r8d,3  
00007FF697D824C0  xor         r8d,eax  
00007FF697D824C3  mov         dword ptr [rsp],r13d  
00007FF697D824C7  xor         r8d,ecx  
    prepare<18>(pbuffer);
00007FF697D824CA  mov         dword ptr [rdi+48h],r13d  
00007FF697D824CE  mov         edi,dword ptr [rdi+30h]  
00007FF697D824D1  mov         eax,ebx  
00007FF697D824D3  ror         eax,13h  
00007FF697D824D6  mov         ecx,ebx  
00007FF697D824D8  shr         ecx,0Ah  
00007FF697D824DB  mov         edx,ebx  
00007FF697D824DD  xor         eax,ecx  
00007FF697D824DF  ror         edx,11h  
00007FF697D824E2  xor         eax,edx  
00007FF697D824E4  mov         edx,r13d  
00007FF697D824E7  add         eax,edi  
00007FF697D824E9  ror         edx,11h  
00007FF697D824EC  add         eax,r8d  
00007FF697D824EF  add         eax,r9d  
00007FF697D824F2  mov         r9,qword ptr [buffer]  
00007FF697D824F7  mov         dword ptr [rsp+78h],eax  
00007FF697D824FB  mov         r10d,dword ptr [r9+14h]  
00007FF697D824FF  mov         ecx,r10d  
00007FF697D82502  mov         ebx,dword ptr [r9+34h]  
00007FF697D82506  mov         r8d,r10d  
    prepare<19>(pbuffer);
00007FF697D82509  mov         dword ptr [r9+4Ch],eax  
00007FF697D8250D  mov         eax,r10d  
00007FF697D82510  ror         eax,12h  
00007FF697D82513  shr         r8d,3  
00007FF697D82517  xor         r8d,eax  
00007FF697D8251A  ror         ecx,7  
00007FF697D8251D  xor         r8d,ecx  
00007FF697D82520  mov         eax,r13d  
00007FF697D82523  shr         r13d,0Ah  
00007FF697D82527  ror         eax,13h  
00007FF697D8252A  xor         r13d,eax  
00007FF697D8252D  xor         r13d,edx  
00007FF697D82530  add         r13d,ebx  
00007FF697D82533  add         r13d,r8d  
00007FF697D82536  add         r13d,r11d  
00007FF697D82539  mov         dword ptr [r9+50h],r13d  
00007FF697D8253D  mov         r9d,dword ptr [r9+18h]  
00007FF697D82541  mov         ecx,r9d  
00007FF697D82544  ror         ecx,7  
00007FF697D82547  mov         eax,r9d  
00007FF697D8254A  ror         eax,12h  
00007FF697D8254D  mov         r8d,r9d  
00007FF697D82550  shr         r8d,3  
00007FF697D82554  xor         r8d,eax  
    prepare<20>(pbuffer);
00007FF697D82557  mov         dword ptr [rsp+4],r13d  
00007FF697D8255C  mov         rax,qword ptr [buffer]  
00007FF697D82561  xor         r8d,ecx  
00007FF697D82564  mov         ecx,dword ptr [rsp+78h]  
00007FF697D82568  mov         r11d,ecx  
00007FF697D8256B  ror         r11d,13h  
00007FF697D8256F  mov         edx,ecx  
00007FF697D82571  shr         ecx,0Ah  
00007FF697D82574  xor         r11d,ecx  
00007FF697D82577  ror         edx,11h  
00007FF697D8257A  xor         r11d,edx  
00007FF697D8257D  mov         edx,r13d  
00007FF697D82580  add         r11d,r8d  
00007FF697D82583  ror         edx,11h  
00007FF697D82586  add         r11d,r10d  
00007FF697D82589  mov         r10d,dword ptr [rax+1Ch]  
00007FF697D8258D  add         r11d,r12d  
00007FF697D82590  mov         r8d,r10d  
00007FF697D82593  mov         dword ptr [rax+54h],r11d  
00007FF697D82597  mov         ecx,r10d  
00007FF697D8259A  shr         r8d,3  
00007FF697D8259E  mov         eax,r10d  
00007FF697D825A1  ror         eax,12h  
00007FF697D825A4  xor         r8d,eax  
00007FF697D825A7  ror         ecx,7  
00007FF697D825AA  mov         eax,r13d  
    prepare<21>(pbuffer);
00007FF697D825AD  mov         dword ptr [rsp+0Ch],r11d  
    prepare<22>(pbuffer);
00007FF697D825B2  shr         r13d,0Ah  
00007FF697D825B6  xor         r8d,ecx  
00007FF697D825B9  ror         eax,13h  
00007FF697D825BC  xor         r13d,eax  
00007FF697D825BF  mov         rax,qword ptr [buffer]  
00007FF697D825C4  xor         r13d,edx  
00007FF697D825C7  add         r13d,r8d  
00007FF697D825CA  add         r13d,r9d  
00007FF697D825CD  add         r13d,r15d  
00007FF697D825D0  mov         dword ptr [rax+58h],r13d  
00007FF697D825D4  mov         r9d,dword ptr [rax+20h]  
00007FF697D825D8  mov         edx,r11d  
00007FF697D825DB  ror         edx,11h  
00007FF697D825DE  mov         eax,r9d  
00007FF697D825E1  ror         eax,12h  
00007FF697D825E4  mov         r8d,r9d  
00007FF697D825E7  shr         r8d,3  
00007FF697D825EB  mov         ecx,r9d  
00007FF697D825EE  xor         r8d,eax  
00007FF697D825F1  ror         ecx,7  
00007FF697D825F4  xor         r8d,ecx  
00007FF697D825F7  mov         eax,r11d  
00007FF697D825FA  ror         eax,13h  
00007FF697D825FD  mov         ecx,r11d  
00007FF697D82600  mov         r11,qword ptr [buffer]  
00007FF697D82605  shr         ecx,0Ah  
00007FF697D82608  xor         ecx,eax  
00007FF697D8260A  mov         eax,r14d  
00007FF697D8260D  xor         ecx,edx  
00007FF697D8260F  ror         eax,12h  
00007FF697D82612  add         ecx,r8d  
00007FF697D82615  mov         edx,r13d  
00007FF697D82618  add         r10d,ecx  
00007FF697D8261B  ror         edx,11h  
00007FF697D8261E  add         r10d,dword ptr [rsp+80h]  
00007FF697D82626  mov         r8d,r14d  
00007FF697D82629  shr         r8d,3  
00007FF697D8262D  mov         ecx,r14d  
00007FF697D82630  xor         r8d,eax  
00007FF697D82633  ror         ecx,7  
00007FF697D82636  xor         r8d,ecx  
    prepare<23>(pbuffer);
00007FF697D82639  mov         dword ptr [r11+5Ch],r10d  
00007FF697D8263D  mov         ecx,r13d  
00007FF697D82640  mov         eax,r13d  
00007FF697D82643  shr         ecx,0Ah  
00007FF697D82646  ror         eax,13h  
00007FF697D82649  xor         ecx,eax  
00007FF697D8264B  mov         eax,ebp  
00007FF697D8264D  xor         ecx,edx  
00007FF697D8264F  ror         eax,12h  
00007FF697D82652  add         ecx,r8d  
00007FF697D82655  mov         edx,r10d  
00007FF697D82658  add         r9d,ecx  
00007FF697D8265B  ror         edx,11h  
00007FF697D8265E  add         r9d,dword ptr [rsp+88h] 
00007FF697D82666  mov         r8d,ebp  
00007FF697D82669  shr         r8d,3  
00007FF697D8266D  mov         ecx,ebp  
00007FF697D8266F  xor         r8d,eax  
00007FF697D82672  ror         ecx,7  
00007FF697D82675  xor         r8d,ecx  
    prepare<24>(pbuffer);
00007FF697D82678  mov         dword ptr [r11+60h],r9d  
00007FF697D8267C  mov         r11d,r10d  
00007FF697D8267F  mov         eax,r10d  
00007FF697D82682  shr         r11d,0Ah  
00007FF697D82686  mov         ecx,esi  
00007FF697D82688  ror         ecx,7  
00007FF697D8268B  ror         eax,13h  
00007FF697D8268E  xor         r11d,eax  
00007FF697D82691  mov         eax,esi  
00007FF697D82693  xor         r11d,edx  
00007FF697D82696  ror         eax,12h  
00007FF697D82699  add         r11d,r14d  
00007FF697D8269C  mov         edx,r9d  
00007FF697D8269F  mov         r14,qword ptr [buffer]  
00007FF697D826A4  add         r11d,r8d  
00007FF697D826A7  add         r11d,dword ptr [rsp]  
00007FF697D826AB  mov         r8d,esi  
00007FF697D826AE  shr         r8d,3  
00007FF697D826B2  xor         r8d,eax  
00007FF697D826B5  ror         edx,11h  
00007FF697D826B8  xor         r8d,ecx  
    prepare<25>(pbuffer);
00007FF697D826BB  mov         dword ptr [r14+64h],r11d  
00007FF697D826BF  mov         ecx,r9d  
00007FF697D826C2  mov         eax,r9d  
00007FF697D826C5  shr         ecx,0Ah  
00007FF697D826C8  ror         eax,13h  
00007FF697D826CB  xor         ecx,eax  
00007FF697D826CD  xor         ecx,edx  
00007FF697D826CF  add         ecx,r8d  
00007FF697D826D2  add         ebp,ecx  
00007FF697D826D4  add         ebp,dword ptr [rsp+78h]  
00007FF697D826D8  mov         eax,edi  
00007FF697D826DA  ror         eax,12h  
00007FF697D826DD  mov         r8d,edi  
00007FF697D826E0  shr         r8d,3  
00007FF697D826E4  mov         ecx,edi  
00007FF697D826E6  xor         r8d,eax  
00007FF697D826E9  ror         ecx,7  
00007FF697D826EC  xor         r8d,ecx  
    prepare<26>(pbuffer);
00007FF697D826EF  mov         dword ptr [r14+68h],ebp  
00007FF697D826F3  mov         eax,r11d  
00007FF697D826F6  mov         ecx,r11d  
00007FF697D826F9  shr         ecx,0Ah  
00007FF697D826FC  mov         edx,r11d  
00007FF697D826FF  ror         eax,13h  
00007FF697D82702  xor         ecx,eax  
00007FF697D82704  ror         edx,11h  
00007FF697D82707  xor         ecx,edx  
00007FF697D82709  mov         eax,ebx  
00007FF697D8270B  add         ecx,r8d  
00007FF697D8270E  ror         eax,12h  
00007FF697D82711  add         ecx,dword ptr [rsp+4] 
00007FF697D82715  mov         r8d,ebx  
00007FF697D82718  add         esi,ecx  
00007FF697D8271A  shr         r8d,3  
00007FF697D8271E  xor         r8d,eax  
    prepare<27>(pbuffer);
00007FF697D82721  mov         dword ptr [r14+6Ch],esi 
00007FF697D82725  mov         r14d,dword ptr [rsp+0Ch]  
00007FF697D8272A  mov         ecx,ebx  
00007FF697D8272C  ror         ecx,7  
00007FF697D8272F  mov         eax,ebp  
00007FF697D82731  xor         r8d,ecx  
00007FF697D82734  ror         eax,13h  
00007FF697D82737  mov         ecx,ebp  
00007FF697D82739  mov         edx,ebp  
00007FF697D8273B  shr         ecx,0Ah  
00007FF697D8273E  xor         ecx,eax  
00007FF697D82740  ror         edx,11h  
00007FF697D82743  mov         rax,qword ptr [buffer]  
00007FF697D82748  xor         ecx,edx  
00007FF697D8274A  add         ecx,r8d  
00007FF697D8274D  mov         edx,esi  
00007FF697D8274F  add         ecx,r14d  
00007FF697D82752  ror         edx,11h  
00007FF697D82755  add         edi,ecx  
00007FF697D82757  mov         r8d,r12d  
    prepare<28>(pbuffer);
00007FF697D8275A  mov         dword ptr [rax+70h],edi  
00007FF697D8275D  mov         ecx,r12d  
00007FF697D82760  ror         ecx,7  
00007FF697D82763  mov         eax,r12d  
00007FF697D82766  ror         eax,12h  
00007FF697D82769  shr         r8d,3  
00007FF697D8276D  xor         r8d,eax  
00007FF697D82770  mov         eax,esi  
00007FF697D82772  xor         r8d,ecx  
00007FF697D82775  ror         eax,13h  
00007FF697D82778  mov         ecx,esi  
00007FF697D8277A  shr         ecx,0Ah  
00007FF697D8277D  xor         eax,ecx  
00007FF697D8277F  mov         ecx,r15d  
00007FF697D82782  xor         eax,edx  
00007FF697D82784  ror         ecx,7  
00007FF697D82787  add         eax,r8d  
00007FF697D8278A  mov         edx,edi  
00007FF697D8278C  add         eax,r13d  
00007FF697D8278F  ror         edx,11h  
00007FF697D82792  add         eax,ebx  
00007FF697D82794  mov         r8d,r15d  
00007FF697D82797  mov         rbx,qword ptr [buffer]  
00007FF697D8279C  mov         dword ptr [rsp+0Ch],eax  
00007FF697D827A0  shr         r8d,3  
    prepare<29>(pbuffer);
00007FF697D827A4  mov         dword ptr [rbx+74h],eax  
00007FF697D827A7  mov         eax,r15d  
00007FF697D827AA  ror         eax,12h  
00007FF697D827AD  xor         r8d,eax  
00007FF697D827B0  mov         eax,edi  
00007FF697D827B2  xor         r8d,ecx  
00007FF697D827B5  ror         eax,13h  
00007FF697D827B8  mov         ecx,edi  
00007FF697D827BA  shr         ecx,0Ah  
00007FF697D827BD  xor         eax,ecx  
00007FF697D827BF  xor         eax,edx  
00007FF697D827C1  mov         edx,dword ptr [rsp+80h]  
00007FF697D827C8  add         eax,r10d  
00007FF697D827CB  mov         ecx,edx  
00007FF697D827CD  add         eax,r12d  
00007FF697D827D0  ror         ecx,7  
00007FF697D827D3  mov         r12d,dword ptr [rsp+0Ch]  
00007FF697D827D8  add         eax,r8d  
00007FF697D827DB  mov         dword ptr [rbx+78h],eax  
00007FF697D827DE  mov         r8d,edx  
    prepare<30>(pbuffer);
00007FF697D827E1  mov         dword ptr [rsp+8],eax  
00007FF697D827E5  mov         ebx,r12d  
00007FF697D827E8  mov         eax,edx  
00007FF697D827EA  shr         r8d,3  
00007FF697D827EE  ror         eax,12h  
00007FF697D827F1  mov         edx,r12d  
00007FF697D827F4  xor         r8d,eax  
00007FF697D827F7  ror         edx,11h  
00007FF697D827FA  xor         r8d,ecx  
00007FF697D827FD  shr         ebx,0Ah  
00007FF697D82800  mov         eax,r12d  
00007FF697D82803  ror         eax,13h  
00007FF697D82806  xor         ebx,eax  
00007FF697D82808  mov         rax,qword ptr [buffer]  
00007FF697D8280D  xor         ebx,edx  
00007FF697D8280F  mov         edx,dword ptr [rsp+88h]  
00007FF697D82816  mov         ecx,edx  
00007FF697D82818  add         ebx,r9d  
00007FF697D8281B  ror         ecx,7  
00007FF697D8281E  add         ebx,r15d  
00007FF697D82821  mov         r15d,dword ptr [rsp+8]  
00007FF697D82826  add         ebx,r8d  
00007FF697D82829  mov         dword ptr [rax+7Ch],ebx  
00007FF697D8282C  mov         r8d,edx  
00007FF697D8282F  mov         eax,edx  
00007FF697D82831  shr         r8d,3  
00007FF697D82835  ror         eax,12h  
00007FF697D82838  mov         edx,r15d  
00007FF697D8283B  xor         r8d,eax  
00007FF697D8283E  ror         edx,11h  
00007FF697D82841  xor         r8d,ecx  
    prepare<31>(pbuffer);
00007FF697D82844  mov         dword ptr [rsp+0Ch],ebx  
00007FF697D82848  mov         ecx,r15d  
00007FF697D8284B  mov         eax,r15d  
00007FF697D8284E  shr         ecx,0Ah  
00007FF697D82851  ror         eax,13h  
00007FF697D82854  xor         ecx,eax  
00007FF697D82856  mov         eax,dword ptr [rsp+80h]  
00007FF697D8285D  xor         ecx,edx  
00007FF697D8285F  mov         edx,dword ptr [rsp]  
00007FF697D82862  add         ecx,r11d  
00007FF697D82865  add         eax,ecx  
00007FF697D82867  mov         rcx,qword ptr [buffer]  
00007FF697D8286C  add         eax,r8d  
00007FF697D8286F  mov         r8d,edx  
00007FF697D82872  mov         dword ptr [rsp+80h],eax 
00007FF697D82879  shr         r8d,3  
    prepare<32>(pbuffer);
00007FF697D8287D  mov         dword ptr [rcx+80h],eax 
00007FF697D82883  mov         eax,edx  
00007FF697D82885  ror         eax,12h  
00007FF697D82888  mov         ecx,edx  
00007FF697D8288A  ror         ecx,7  
00007FF697D8288D  xor         r8d,eax  
00007FF697D82890  xor         r8d,ecx  
00007FF697D82893  mov         eax,ebx  
00007FF697D82895  ror         eax,13h  
00007FF697D82898  mov         ecx,ebx  
00007FF697D8289A  shr         ecx,0Ah  
00007FF697D8289D  mov         edx,ebx  
00007FF697D8289F  mov         rbx,qword ptr [buffer]  
00007FF697D828A4  xor         ecx,eax  
00007FF697D828A6  mov         eax,dword ptr [rsp+88h]  
00007FF697D828AD  ror         edx,11h  
00007FF697D828B0  xor         ecx,edx  
00007FF697D828B2  mov         edx,dword ptr [rsp+78h]  
00007FF697D828B6  add         ecx,ebp  
00007FF697D828B8  add         eax,ecx  
00007FF697D828BA  mov         ecx,edx  
    prepare<33>(pbuffer);
00007FF697D828BC  add         eax,r8d  
00007FF697D828BF  mov         dword ptr [rsp+88h],eax  
00007FF697D828C6  mov         dword ptr [rbx+84h],eax  
00007FF697D828CC  ror         ecx,7  
00007FF697D828CF  mov         eax,edx  
00007FF697D828D1  ror         eax,12h  
00007FF697D828D4  mov         r8d,edx  
00007FF697D828D7  shr         r8d,3  
00007FF697D828DB  xor         r8d,eax  
00007FF697D828DE  xor         r8d,ecx  
00007FF697D828E1  mov         ecx,dword ptr [rsp+80h]  
00007FF697D828E8  mov         eax,ecx  
00007FF697D828EA  mov         edx,ecx  
00007FF697D828EC  ror         eax,13h  
00007FF697D828EF  shr         ecx,0Ah  
00007FF697D828F2  xor         ecx,eax  
00007FF697D828F4  ror         edx,11h  
00007FF697D828F7  mov         eax,dword ptr [rsp]  
00007FF697D828FA  xor         ecx,edx  
00007FF697D828FC  add         ecx,esi  
00007FF697D828FE  add         eax,ecx  
00007FF697D82900  add         eax,r8d  
00007FF697D82903  mov         dword ptr [rbx+88h],eax  
00007FF697D82909  mov         ebx,dword ptr [rsp+4]  
00007FF697D8290D  mov         ecx,ebx  
    prepare<34>(pbuffer);
00007FF697D8290F  mov         dword ptr [rsp],eax  
00007FF697D82912  mov         r8d,ebx  
00007FF697D82915  ror         ecx,7  
00007FF697D82918  mov         eax,ebx  
00007FF697D8291A  ror         eax,12h  
00007FF697D8291D  shr         r8d,3  
00007FF697D82921  xor         r8d,eax  
00007FF697D82924  xor         r8d,ecx  
00007FF697D82927  mov         ecx,dword ptr [rsp+88h]  
00007FF697D8292E  mov         eax,ecx  
00007FF697D82930  mov         edx,ecx  
00007FF697D82932  ror         eax,13h  
00007FF697D82935  shr         ecx,0Ah  
00007FF697D82938  xor         ecx,eax  
00007FF697D8293A  ror         edx,11h  
00007FF697D8293D  mov         eax,dword ptr [rsp+78h]  
00007FF697D82941  xor         ecx,edx  
00007FF697D82943  add         ecx,edi  
00007FF697D82945  add         eax,ecx  
00007FF697D82947  mov         rcx,qword ptr [buffer]  
00007FF697D8294C  add         eax,r8d  
00007FF697D8294F  mov         r8d,r14d  
00007FF697D82952  mov         dword ptr [rsp+4],eax  
00007FF697D82956  shr         r8d,3  
    prepare<35>(pbuffer);
00007FF697D8295A  mov         dword ptr [rcx+8Ch],eax  
00007FF697D82960  mov         eax,r14d  
00007FF697D82963  ror         eax,12h  
00007FF697D82966  mov         ecx,r14d  
00007FF697D82969  xor         r8d,eax  
00007FF697D8296C  ror         ecx,7  
00007FF697D8296F  xor         r8d,ecx  
00007FF697D82972  mov         ecx,dword ptr [rsp]  
00007FF697D82975  mov         eax,ecx  
00007FF697D82977  mov         edx,ecx  
00007FF697D82979  ror         eax,13h  
00007FF697D8297C  shr         ecx,0Ah  
00007FF697D8297F  xor         eax,ecx  
00007FF697D82981  ror         edx,11h  
00007FF697D82984  xor         eax,edx  
00007FF697D82986  mov         ecx,r13d  
00007FF697D82989  add         eax,ebx  
00007FF697D8298B  ror         ecx,7  
00007FF697D8298E  mov         rbx,qword ptr [buffer]  
00007FF697D82993  add         eax,r8d  
00007FF697D82996  add         eax,r12d  
00007FF697D82999  mov         r8d,r13d  
00007FF697D8299C  mov         dword ptr [rsp+78h],eax  
00007FF697D829A0  shr         r8d,3  
    prepare<36>(pbuffer);
00007FF697D829A4  mov         dword ptr [rbx+90h],eax  
00007FF697D829AA  mov         eax,r13d  
00007FF697D829AD  ror         eax,12h  
00007FF697D829B0  xor         r8d,eax  
00007FF697D829B3  xor         r8d,ecx  
00007FF697D829B6  mov         ecx,dword ptr [rsp+4]  
00007FF697D829BA  mov         edx,ecx  
00007FF697D829BC  mov         eax,ecx  
00007FF697D829BE  ror         edx,11h  
00007FF697D829C1  ror         eax,13h  
00007FF697D829C4  shr         ecx,0Ah  
00007FF697D829C7  xor         eax,ecx  
00007FF697D829C9  mov         ecx,r10d  
00007FF697D829CC  xor         eax,edx  
00007FF697D829CE  ror         ecx,7  
00007FF697D829D1  add         eax,r14d  
00007FF697D829D4  mov         r14d,dword ptr [rsp+78h]
00007FF697D829D9  add         eax,r8d  
00007FF697D829DC  mov         edx,r14d  
00007FF697D829DF  add         eax,r15d  
00007FF697D829E2  ror         edx,11h  
00007FF697D829E5  mov         dword ptr [rsp+10h],eax  
00007FF697D829E9  mov         r8d,r10d  
    prepare<37>(pbuffer);
00007FF697D829EC  mov         dword ptr [rbx+94h],eax  
00007FF697D829F2  mov         eax,r10d  
00007FF697D829F5  mov         ebx,dword ptr [rsp+0Ch]  
00007FF697D829F9  ror         eax,12h  
00007FF697D829FC  shr         r8d,3  
00007FF697D82A00  xor         r8d,eax  
00007FF697D82A03  mov         eax,r14d  
00007FF697D82A06  ror         eax,13h  
00007FF697D82A09  xor         r8d,ecx  
00007FF697D82A0C  mov         ecx,r14d  
00007FF697D82A0F  shr         ecx,0Ah  
00007FF697D82A12  xor         eax,ecx  
00007FF697D82A14  mov         rcx,qword ptr [buffer]  
00007FF697D82A19  xor         eax,edx  
00007FF697D82A1B  add         eax,r13d  
00007FF697D82A1E  mov         r13d,dword ptr [rsp+10h]  
00007FF697D82A23  add         eax,r8d  
00007FF697D82A26  mov         edx,r13d  
00007FF697D82A29  add         eax,ebx  
00007FF697D82A2B  ror         edx,11h  
00007FF697D82A2E  mov         dword ptr [rcx+98h],eax  
00007FF697D82A34  mov         r8d,r9d  
    prepare<38>(pbuffer);
00007FF697D82A37  mov         dword ptr [rsp+8],eax  
00007FF697D82A3B  mov         ecx,r9d  
00007FF697D82A3E  ror         ecx,7  
00007FF697D82A41  mov         eax,r9d  
00007FF697D82A44  ror         eax,12h  
00007FF697D82A47  shr         r8d,3  
00007FF697D82A4B  xor         r8d,eax  
00007FF697D82A4E  mov         eax,r13d  
00007FF697D82A51  ror         eax,13h  
00007FF697D82A54  xor         r8d,ecx  
00007FF697D82A57  mov         ecx,r13d  
00007FF697D82A5A  shr         ecx,0Ah  
00007FF697D82A5D  xor         eax,ecx  
00007FF697D82A5F  mov         rcx,qword ptr [buffer]  
00007FF697D82A64  xor         eax,edx  
00007FF697D82A66  add         eax,r10d  
00007FF697D82A69  mov         r10d,dword ptr [rsp+80h]  
00007FF697D82A71  add         eax,r8d  
00007FF697D82A74  mov         r8d,r11d  
00007FF697D82A77  add         eax,r10d  
00007FF697D82A7A  shr         r8d,3  
00007FF697D82A7E  mov         dword ptr [rcx+9Ch],eax  
00007FF697D82A84  mov         ecx,r11d  
    prepare<39>(pbuffer);
00007FF697D82A87  mov         dword ptr [rsp+0Ch],eax  
00007FF697D82A8B  mov         eax,r11d  
00007FF697D82A8E  ror         eax,12h  
00007FF697D82A91  xor         r8d,eax  
00007FF697D82A94  ror         ecx,7  
00007FF697D82A97  xor         r8d,ecx  
00007FF697D82A9A  mov         ecx,dword ptr [rsp+8]  
00007FF697D82A9E  mov         eax,ecx  
00007FF697D82AA0  mov         edx,ecx  
00007FF697D82AA2  ror         eax,13h  
00007FF697D82AA5  shr         ecx,0Ah  
00007FF697D82AA8  xor         eax,ecx  
00007FF697D82AAA  ror         edx,11h  
00007FF697D82AAD  mov         rcx,qword ptr [buffer]  
00007FF697D82AB2  xor         eax,edx  
00007FF697D82AB4  add         eax,r9d  
00007FF697D82AB7  mov         r9d,dword ptr [rsp+88h]  
00007FF697D82ABF  add         eax,r8d  
00007FF697D82AC2  add         eax,r9d  
00007FF697D82AC5  mov         dword ptr [rcx+0A0h],eax 
00007FF697D82ACB  mov         ecx,ebp  
    prepare<40>(pbuffer);
00007FF697D82ACD  mov         dword ptr [rsp+78h],eax 
00007FF697D82AD1  mov         eax,ebp  
00007FF697D82AD3  ror         ecx,7  
00007FF697D82AD6  ror         eax,12h  
00007FF697D82AD9  mov         r8d,ebp  
00007FF697D82ADC  shr         r8d,3  
00007FF697D82AE0  xor         r8d,eax  
00007FF697D82AE3  xor         r8d,ecx  
00007FF697D82AE6  mov         ecx,dword ptr [rsp+0Ch]  
00007FF697D82AEA  mov         eax,ecx  
00007FF697D82AEC  mov         edx,ecx  
00007FF697D82AEE  ror         eax,13h  
00007FF697D82AF1  shr         ecx,0Ah  
00007FF697D82AF4  xor         eax,ecx  
00007FF697D82AF6  ror         edx,11h  
00007FF697D82AF9  mov         rcx,qword ptr [buffer]  
00007FF697D82AFE  xor         eax,edx  
00007FF697D82B00  add         eax,r11d  
00007FF697D82B03  mov         r11d,dword ptr [rsp]  
00007FF697D82B07  add         eax,r8d  
00007FF697D82B0A  mov         r8d,esi  
00007FF697D82B0D  add         eax,r11d  
00007FF697D82B10  shr         r8d,3  
00007FF697D82B14  mov         dword ptr [rcx+0A4h],eax  
00007FF697D82B1A  mov         ecx,esi  
    prepare<41>(pbuffer);
00007FF697D82B1C  mov         dword ptr [rsp+80h],eax 
00007FF697D82B23  mov         eax,esi  
00007FF697D82B25  ror         eax,12h  
00007FF697D82B28  xor         r8d,eax  
00007FF697D82B2B  ror         ecx,7  
00007FF697D82B2E  xor         r8d,ecx  
00007FF697D82B31  mov         ecx,dword ptr [rsp+78h]  
00007FF697D82B35  mov         eax,ecx  
00007FF697D82B37  mov         edx,ecx  
00007FF697D82B39  ror         eax,13h  
00007FF697D82B3C  shr         ecx,0Ah  
00007FF697D82B3F  xor         eax,ecx  
00007FF697D82B41  ror         edx,11h  
00007FF697D82B44  mov         rcx,qword ptr [buffer]  
00007FF697D82B49  xor         eax,edx  
00007FF697D82B4B  add         eax,ebp  
00007FF697D82B4D  mov         ebp,dword ptr [rsp+4]  
00007FF697D82B51  add         eax,r8d  
00007FF697D82B54  mov         r8d,edi  
00007FF697D82B57  add         eax,ebp  
00007FF697D82B59  shr         r8d,3  
00007FF697D82B5D  mov         dword ptr [rcx+0A8h],eax 
00007FF697D82B63  mov         ecx,edi  
00007FF697D82B65  ror         ecx,7  
    prepare<42>(pbuffer);
00007FF697D82B68  mov         dword ptr [rsp+88h],eax 
00007FF697D82B6F  mov         eax,edi  
00007FF697D82B71  ror         eax,12h  
00007FF697D82B74  xor         r8d,eax  
00007FF697D82B77  xor         r8d,ecx  
00007FF697D82B7A  mov         ecx,dword ptr [rsp+80h]  
00007FF697D82B81  mov         eax,ecx  
00007FF697D82B83  mov         edx,ecx  
00007FF697D82B85  shr         ecx,0Ah  
00007FF697D82B88  ror         eax,13h  
00007FF697D82B8B  xor         ecx,eax  
00007FF697D82B8D  ror         edx,11h  
00007FF697D82B90  mov         rax,qword ptr [buffer]  
00007FF697D82B95  xor         ecx,edx  
00007FF697D82B97  add         esi,ecx  
00007FF697D82B99  mov         ecx,r12d  
00007FF697D82B9C  add         esi,r8d  
00007FF697D82B9F  ror         ecx,7  
00007FF697D82BA2  add         esi,r14d  
00007FF697D82BA5  mov         r8d,r12d  
00007FF697D82BA8  mov         dword ptr [rax+0ACh],esi  
00007FF697D82BAE  mov         eax,r12d  
00007FF697D82BB1  ror         eax,12h  
00007FF697D82BB4  shr         r8d,3  
00007FF697D82BB8  xor         r8d,eax  
    prepare<43>(pbuffer);
00007FF697D82BBB  mov         dword ptr [rsp+4],esi  
00007FF697D82BBF  xor         r8d,ecx  
00007FF697D82BC2  mov         ecx,dword ptr [rsp+88h]  
00007FF697D82BC9  mov         eax,ecx  
00007FF697D82BCB  mov         edx,ecx  
00007FF697D82BCD  ror         eax,13h  
00007FF697D82BD0  shr         ecx,0Ah  
00007FF697D82BD3  xor         ecx,eax  
00007FF697D82BD5  ror         edx,11h  
00007FF697D82BD8  xor         ecx,edx  
00007FF697D82BDA  mov         rax,qword ptr [buffer]  
00007FF697D82BDF  add         edi,ecx  
00007FF697D82BE1  add         edi,r8d  
00007FF697D82BE4  mov         ecx,r15d  
00007FF697D82BE7  ror         ecx,7  
00007FF697D82BEA  add         edi,r13d  
00007FF697D82BED  mov         r8d,r15d  
    prepare<44>(pbuffer);
00007FF697D82BF0  mov         dword ptr [rsp+10h],edi  
00007FF697D82BF4  mov         dword ptr [rax+0B0h],edi 
00007FF697D82BFA  mov         edx,esi  
00007FF697D82BFC  shr         r8d,3  
00007FF697D82C00  mov         eax,r15d  
00007FF697D82C03  ror         eax,12h  
00007FF697D82C06  xor         r8d,eax  
00007FF697D82C09  ror         edx,11h  
00007FF697D82C0C  xor         r8d,ecx  
00007FF697D82C0F  mov         eax,esi  
00007FF697D82C11  ror         eax,13h  
00007FF697D82C14  mov         ecx,esi  
00007FF697D82C16  mov         esi,dword ptr [rsp+8]  
00007FF697D82C1A  shr         ecx,0Ah  
00007FF697D82C1D  xor         eax,ecx  
00007FF697D82C1F  mov         rcx,qword ptr [buffer]  
00007FF697D82C24  xor         eax,edx  
00007FF697D82C26  mov         edx,edi  
00007FF697D82C28  add         eax,r8d  
00007FF697D82C2B  ror         edx,11h  
00007FF697D82C2E  add         eax,esi  
00007FF697D82C30  mov         r8d,ebx  
00007FF697D82C33  add         eax,r12d  
00007FF697D82C36  shr         r8d,3  
00007FF697D82C3A  mov         r12d,dword ptr [rsp+0Ch]  
00007FF697D82C3F  mov         dword ptr [rcx+0B4h],eax  
00007FF697D82C45  mov         ecx,ebx  
00007FF697D82C47  ror         ecx,7  
    prepare<45>(pbuffer);
00007FF697D82C4A  mov         dword ptr [rsp],eax  
00007FF697D82C4D  mov         eax,ebx  
00007FF697D82C4F  ror         eax,12h  
00007FF697D82C52  xor         r8d,eax  
00007FF697D82C55  mov         eax,edi  
00007FF697D82C57  xor         r8d,ecx  
00007FF697D82C5A  ror         eax,13h  
00007FF697D82C5D  mov         ecx,edi  
00007FF697D82C5F  shr         ecx,0Ah  
00007FF697D82C62  xor         ecx,eax  
00007FF697D82C64  mov         rax,qword ptr [buffer]  
00007FF697D82C69  xor         ecx,edx  
00007FF697D82C6B  add         ecx,r8d  
00007FF697D82C6E  mov         r8d,r10d
00007FF697D82C71  add         ecx,r12d  
00007FF697D82C74  shr         r8d,3  
00007FF697D82C78  add         r15d,ecx  
00007FF697D82C7B  mov         ecx,r10d  
00007FF697D82C7E  ror         ecx,7  
00007FF697D82C81  mov         dword ptr [rax+0B8h],r15d  
00007FF697D82C88  mov         eax,r10d  
00007FF697D82C8B  ror         eax,12h  
00007FF697D82C8E  xor         r8d,eax  
    prepare<46>(pbuffer);
00007FF697D82C91  mov         dword ptr [rsp+0Ch],r15d  
00007FF697D82C96  xor         r8d,ecx  
00007FF697D82C99  mov         ecx,dword ptr [rsp]  
00007FF697D82C9C  mov         eax,ecx  
00007FF697D82C9E  mov         edx,ecx  
00007FF697D82CA0  shr         ecx,0Ah  
00007FF697D82CA3  ror         eax,13h  
00007FF697D82CA6  xor         ecx,eax  
00007FF697D82CA8  ror         edx,11h  
00007FF697D82CAB  mov         rax,qword ptr [buffer]  
00007FF697D82CB0  xor         ecx,edx  
00007FF697D82CB2  add         ecx,r8d  
00007FF697D82CB5  mov         r8d,r9d  
00007FF697D82CB8  add         ecx,dword ptr [rsp+78h]  
00007FF697D82CBC  add         ebx,ecx  
00007FF697D82CBE  shr         r8d,3  
00007FF697D82CC2  mov         dword ptr [rax+0BCh],ebx
00007FF697D82CC8  mov         ecx,r9d  
00007FF697D82CCB  mov         eax,r9d  
00007FF697D82CCE  ror         ecx,7  
00007FF697D82CD1  ror         eax,12h  
00007FF697D82CD4  xor         r8d,eax  
    prepare<47>(pbuffer);
00007FF697D82CD7  mov         dword ptr [rsp+8],ebx  
00007FF697D82CDB  xor         r8d,ecx  
00007FF697D82CDE  mov         eax,r15d  
00007FF697D82CE1  ror         eax,13h  
00007FF697D82CE4  mov         ecx,r11d  
00007FF697D82CE7  ror         ecx,7  
00007FF697D82CEA  mov         edx,r15d  
00007FF697D82CED  ror         edx,11h  
00007FF697D82CF0  shr         r15d,0Ah  
00007FF697D82CF4  xor         r15d,eax  
00007FF697D82CF7  mov         eax,r11d  
00007FF697D82CFA  ror         eax,12h  
00007FF697D82CFD  xor         r15d,edx  
00007FF697D82D00  add         r15d,r8d  
00007FF697D82D03  mov         edx,ebx  
00007FF697D82D05  add         r15d,dword ptr [rsp+80h] 
00007FF697D82D0D  mov         r8d,r11d  
00007FF697D82D10  shr         r8d,3  
00007FF697D82D14  add         r15d,r10d  
00007FF697D82D17  mov         r10,qword ptr [buffer] 
00007FF697D82D1C  xor         r8d,eax  
00007FF697D82D1F  xor         r8d,ecx  
00007FF697D82D22  ror         edx,11h  
00007FF697D82D25  mov         eax,ebx  
00007FF697D82D27  mov         ecx,ebx  
00007FF697D82D29  ror         eax,13h  
00007FF697D82D2C  shr         ecx,0Ah  
00007FF697D82D2F  xor         ecx,eax  
    prepare<48>(pbuffer);
00007FF697D82D31  mov         dword ptr [r10+0C0h],r15d 
00007FF697D82D38  xor         ecx,edx  
00007FF697D82D3A  mov         eax,ebp  
00007FF697D82D3C  add         ecx,r8d  
00007FF697D82D3F  ror         eax,12h  
00007FF697D82D42  add         ecx,dword ptr [rsp+88h] 
00007FF697D82D49  mov         r8d,ebp  
00007FF697D82D4C  add         r9d,ecx  
00007FF697D82D4F  shr         r8d,3  
00007FF697D82D53  xor         r8d,eax  
    prepare<49>(pbuffer);
00007FF697D82D56  mov         dword ptr [r10+0C4h],r9d  
00007FF697D82D5D  mov         eax,r15d  
00007FF697D82D60  mov         ecx,ebp  
00007FF697D82D62  ror         eax,13h  
00007FF697D82D65  mov         r10d,r15d  
00007FF697D82D68  shr         r10d,0Ah  
00007FF697D82D6C  mov         edx,r15d  
00007FF697D82D6F  xor         r10d,eax  
00007FF697D82D72  ror         ecx,7  
00007FF697D82D75  mov         rax,qword ptr [buffer]  
00007FF697D82D7A  xor         r8d,ecx  
00007FF697D82D7D  ror         edx,11h  
00007FF697D82D80  mov         ecx,r14d  
00007FF697D82D83  xor         r10d,edx  
00007FF697D82D86  ror         ecx,7  
00007FF697D82D89  add         r10d,r8d  
00007FF697D82D8C  mov         edx,r9d  
00007FF697D82D8F  add         r10d,dword ptr [rsp+4]  
00007FF697D82D94  mov         r8d,r14d  
00007FF697D82D97  add         r10d,r11d  
00007FF697D82D9A  shr         r8d,3  
    prepare<50>(pbuffer);
00007FF697D82D9E  mov         dword ptr [rax+0C8h],r10d  
00007FF697D82DA5  mov         r11d,r9d  
00007FF697D82DA8  shr         r11d,0Ah  
00007FF697D82DAC  mov         eax,r14d  
00007FF697D82DAF  ror         eax,12h  
00007FF697D82DB2  xor         r8d,eax  
00007FF697D82DB5  ror         edx,11h  
00007FF697D82DB8  xor         r8d,ecx  
00007FF697D82DBB  mov         eax,r9d  
00007FF697D82DBE  ror         eax,13h  
00007FF697D82DC1  mov         ecx,r13d  
00007FF697D82DC4  xor         r11d,eax  
00007FF697D82DC7  ror         ecx,7  
00007FF697D82DCA  xor         r11d,edx  
00007FF697D82DCD  mov         eax,r13d  
00007FF697D82DD0  add         r11d,r8d  
00007FF697D82DD3  ror         eax,12h  
00007FF697D82DD6  add         r11d,edi  
00007FF697D82DD9  mov         r8d,r13d  
00007FF697D82DDC  add         r11d,ebp  
00007FF697D82DDF  mov         rbp,qword ptr [buffer]
00007FF697D82DE4  shr         r8d,3  
    prepare<51>(pbuffer);
00007FF697D82DE8  mov         dword ptr [rbp+0CCh],r11d  
00007FF697D82DEF  xor         r8d,eax  
00007FF697D82DF2  mov         ebx,r10d  
00007FF697D82DF5  xor         r8d,ecx  
00007FF697D82DF8  shr         ebx,0Ah  
00007FF697D82DFB  mov         ecx,esi  
00007FF697D82DFD  mov         eax,r10d  
00007FF697D82E00  ror         eax,13h  
00007FF697D82E03  mov         edx,r10d  
00007FF697D82E06  xor         ebx,eax  
00007FF697D82E08  ror         ecx,7  
00007FF697D82E0B  ror         edx,11h  
00007FF697D82E0E  mov         eax,esi  
00007FF697D82E10  ror         eax,12h  
00007FF697D82E13  xor         ebx,edx  
00007FF697D82E15  add         ebx,r8d  
00007FF697D82E18  mov         edi,r11d  
00007FF697D82E1B  add         ebx,dword ptr [rsp] 
00007FF697D82E1E  mov         r8d,esi  
00007FF697D82E21  shr         r8d,3  
00007FF697D82E25  add         ebx,r14d  
00007FF697D82E28  mov         r14d,dword ptr [rsp+78h]  
00007FF697D82E2D  xor         r8d,eax  
00007FF697D82E30  xor         r8d,ecx  
    prepare<52>(pbuffer);
00007FF697D82E33  mov         dword ptr [rbp+0D0h],ebx  
00007FF697D82E39  shr         edi,0Ah  
00007FF697D82E3C  mov         ecx,r12d  
00007FF697D82E3F  ror         ecx,7  
00007FF697D82E42  mov         eax,r11d  
00007FF697D82E45  ror         eax,13h  
00007FF697D82E48  mov         edx,r11d  
00007FF697D82E4B  xor         edi,eax  
00007FF697D82E4D  ror         edx,11h  
00007FF697D82E50  xor         edi,edx  
00007FF697D82E52  mov         eax,r12d  
00007FF697D82E55  add         edi,r8d  
00007FF697D82E58  ror         eax,12h  
00007FF697D82E5B  add         edi,dword ptr [rsp+0Ch]  
00007FF697D82E5F  mov         r8d,r12d  
00007FF697D82E62  shr         r8d,3  
00007FF697D82E66  add         edi,r13d  
00007FF697D82E69  mov         r13,qword ptr [buffer]  
00007FF697D82E6E  xor         r8d,eax  
00007FF697D82E71  xor         r8d,ecx  
    prepare<53>(pbuffer);
00007FF697D82E74  mov         dword ptr [rbp+0D4h],edi  
00007FF697D82E7A  mov         ecx,ebx  
00007FF697D82E7C  mov         eax,ebx  
00007FF697D82E7E  shr         ecx,0Ah  
00007FF697D82E81  mov         edx,ebx  
00007FF697D82E83  ror         eax,13h  
00007FF697D82E86  xor         ecx,eax  
00007FF697D82E88  ror         edx,11h  
00007FF697D82E8B  xor         ecx,edx  
00007FF697D82E8D  mov         eax,r14d  
00007FF697D82E90  add         ecx,r8d  
00007FF697D82E93  ror         eax,12h  
00007FF697D82E96  add         ecx,dword ptr [rsp+8]  
00007FF697D82E9A  mov         r8d,r14d  
00007FF697D82E9D  add         esi,ecx  
00007FF697D82E9F  shr         r8d,3  
    prepare<54>(pbuffer);
00007FF697D82EA3  mov         dword ptr [rbp+0D8h],esi  
00007FF697D82EA9  xor         r8d,eax  
00007FF697D82EAC  mov         ebp,edi  
00007FF697D82EAE  mov         eax,edi  
00007FF697D82EB0  shr         ebp,0Ah  
00007FF697D82EB3  mov         ecx,r14d  
00007FF697D82EB6  ror         ecx,7  
00007FF697D82EB9  mov         edx,edi  
00007FF697D82EBB  xor         r8d,ecx  
00007FF697D82EBE  ror         eax,13h  
00007FF697D82EC1  xor         ebp,eax  
00007FF697D82EC3  ror         edx,11h  
00007FF697D82EC6  xor         ebp,edx  
00007FF697D82EC8  add         ebp,r8d  
00007FF697D82ECB  add         ebp,r15d  
00007FF697D82ECE  add         ebp,r12d  
00007FF697D82ED1  mov         r12d,dword ptr [rsp+80h]  
00007FF697D82ED9  mov         ecx,r12d  
    prepare<55>(pbuffer);
00007FF697D82EDC  mov         dword ptr [r13+0DCh],ebp  
00007FF697D82EE3  ror         ecx,7  
00007FF697D82EE6  mov         eax,r12d  
00007FF697D82EE9  ror         eax,12h  
00007FF697D82EEC  mov         r8d,r12d  
00007FF697D82EEF  mov         edx,esi  
00007FF697D82EF1  shr         r8d,3  
00007FF697D82EF5  xor         r8d,eax  
00007FF697D82EF8  ror         edx,11h  
00007FF697D82EFB  xor         r8d,ecx  
00007FF697D82EFE  mov         eax,esi  
00007FF697D82F00  ror         eax,13h  
00007FF697D82F03  mov         ecx,esi  
00007FF697D82F05  shr         ecx,0Ah  
00007FF697D82F08  xor         ecx,eax  
00007FF697D82F0A  xor         ecx,edx  
00007FF697D82F0C  mov         edx,ebp  
00007FF697D82F0E  add         ecx,r8d  
00007FF697D82F11  ror         edx,11h  
00007FF697D82F14  add         ecx,r9d  
00007FF697D82F17  mov         r9d,ebp  
00007FF697D82F1A  add         r14d,ecx  
00007FF697D82F1D  shr         r9d,0Ah  
    prepare<56>(pbuffer);
00007FF697D82F21  mov         dword ptr [r13+0E0h],r14d  
00007FF697D82F28  mov         r13d,dword ptr [rsp+88h]  
00007FF697D82F30  mov         eax,r13d  
00007FF697D82F33  ror         eax,12h  
00007FF697D82F36  mov         r8d,r13d  
00007FF697D82F39  shr         r8d,3  
00007FF697D82F3D  mov         ecx,r13d  
00007FF697D82F40  xor         r8d,eax  
00007FF697D82F43  ror         ecx,7  
00007FF697D82F46  xor         r8d,ecx  
00007FF697D82F49  mov         eax,ebp  
00007FF697D82F4B  ror         eax,13h  
00007FF697D82F4E  xor         r9d,eax  
00007FF697D82F51  xor         r9d,edx  
00007FF697D82F54  mov         edx,r14d  
00007FF697D82F57  add         r9d,r8d  
00007FF697D82F5A  ror         edx,11h  
00007FF697D82F5D  add         r9d,r10d  
00007FF697D82F60  mov         r10d,dword ptr [rsp+4]  
00007FF697D82F65  add         r9d,r12d  
00007FF697D82F68  mov         eax,r10d  
00007FF697D82F6B  mov         r12,qword ptr [buffer]  
00007FF697D82F70  mov         r8d,r10d  
00007FF697D82F73  ror         eax,12h  
00007FF697D82F76  mov         ecx,r10d  
00007FF697D82F79  ror         ecx,7  
00007FF697D82F7C  shr         r8d,3  
00007FF697D82F80  xor         r8d,eax  
    prepare<57>(pbuffer);
00007FF697D82F83  mov         dword ptr [r12+0E4h],r9d  
00007FF697D82F8B  xor         r8d,ecx  
00007FF697D82F8E  mov         eax,r14d  
00007FF697D82F91  ror         eax,13h  
00007FF697D82F94  mov         ecx,r14d  
00007FF697D82F97  shr         ecx,0Ah  
00007FF697D82F9A  xor         ecx,eax  
00007FF697D82F9C  xor         ecx,edx  
00007FF697D82F9E  add         r8d,ecx  
00007FF697D82FA1  add         r8d,r11d  
00007FF697D82FA4  mov         r11d,dword ptr [rsp+10h]  
    prepare<58>(pbuffer);
00007FF697D82FA9  add         r8d,r13d  
00007FF697D82FAC  mov         eax,r11d  
00007FF697D82FAF  ror         eax,12h  
00007FF697D82FB2  mov         edx,r11d  
00007FF697D82FB5  shr         edx,3  
00007FF697D82FB8  mov         ecx,r11d  
00007FF697D82FBB  ror         ecx,7  
00007FF697D82FBE  xor         edx,eax  
00007FF697D82FC0  mov         eax,r9d  
00007FF697D82FC3  mov         dword ptr [r12+0E8h],r8d  
    prepare<59>(pbuffer);
00007FF697D82FCB  xor         edx,ecx  
00007FF697D82FCD  ror         eax,13h  
00007FF697D82FD0  mov         ecx,r9d  
00007FF697D82FD3  shr         r9d,0Ah  
00007FF697D82FD7  xor         r9d,eax  
00007FF697D82FDA  ror         ecx,11h  
00007FF697D82FDD  xor         r9d,ecx  
00007FF697D82FE0  add         r9d,edx  
00007FF697D82FE3  add         r9d,ebx  
00007FF697D82FE6  mov         ebx,dword ptr [rsp]  
00007FF697D82FE9  add         r10d,r9d  
    prepare<60>(pbuffer);
00007FF697D82FEC  mov         ecx,ebx  
00007FF697D82FEE  mov         dword ptr [r12+0ECh],r10d  
00007FF697D82FF6  ror         ecx,7  
00007FF697D82FF9  mov         r9d,r8d  
00007FF697D82FFC  ror         r9d,13h  
00007FF697D83000  mov         eax,ebx  
00007FF697D83002  ror         eax,12h  
00007FF697D83005  mov         edx,ebx  
00007FF697D83007  shr         edx,3  
00007FF697D8300A  xor         edx,eax  
00007FF697D8300C  xor         edx,ecx  
00007FF697D8300E  mov         ecx,r8d  
00007FF697D83011  shr         r8d,0Ah  
00007FF697D83015  xor         r9d,r8d  
00007FF697D83018  ror         ecx,11h  
00007FF697D8301B  xor         r9d,ecx  
00007FF697D8301E  mov         r8d,r10d  
00007FF697D83021  add         r9d,edx  
00007FF697D83024  ror         r8d,13h  
00007FF697D83028  add         r9d,edi  
00007FF697D8302B  add         r9d,r11d  
00007FF697D8302E  mov         r11d,dword ptr [rsp+0Ch]  
00007FF697D83033  mov         dword ptr [r12+0F0h],r9d  
    prepare<61>(pbuffer);
00007FF697D8303B  mov         eax,r11d  
00007FF697D8303E  ror         eax,12h  
00007FF697D83041  mov         edx,r11d  
00007FF697D83044  shr         edx,3  
00007FF697D83047  mov         ecx,r11d  
00007FF697D8304A  xor         edx,eax  
00007FF697D8304C  ror         ecx,7  
00007FF697D8304F  xor         edx,ecx  
00007FF697D83051  mov         ecx,r10d  
00007FF697D83054  ror         ecx,11h  
00007FF697D83057  shr         r10d,0Ah  
00007FF697D8305B  xor         r8d,r10d  
00007FF697D8305E  mov         r10d,dword ptr [rsp+8]  
00007FF697D83063  xor         r8d,ecx  
00007FF697D83066  mov         eax,r10d  
00007FF697D83069  add         r8d,edx  
00007FF697D8306C  ror         eax,12h  
00007FF697D8306F  add         r8d,esi  
00007FF697D83072  mov         edx,r10d  
00007FF697D83075  add         r8d,ebx  
00007FF697D83078  shr         edx,3  
00007FF697D8307B  xor         edx,eax  
00007FF697D8307D  mov         dword ptr [r12+0F4h],r8d  
    prepare<62>(pbuffer);
00007FF697D83085  mov         eax,r9d  
00007FF697D83088  mov         ecx,r10d  
00007FF697D8308B  ror         ecx,7  
00007FF697D8308E  xor         edx,ecx  
00007FF697D83090  ror         eax,13h  
00007FF697D83093  mov         ecx,r9d  
00007FF697D83096  shr         r9d,0Ah  
00007FF697D8309A  xor         r9d,eax  
00007FF697D8309D  ror         ecx,11h  
00007FF697D830A0  xor         r9d,ecx  
    prepare<63>(pbuffer);
00007FF697D830A3  mov         eax,r15d  
00007FF697D830A6  mov         ecx,r15d  
00007FF697D830A9  ror         eax,12h  
00007FF697D830AC  ror         ecx,7  
00007FF697D830AF  add         r9d,edx  
00007FF697D830B2  shr         r15d,3  
00007FF697D830B6  add         r9d,ebp  
00007FF697D830B9  xor         r15d,eax  
00007FF697D830BC  add         r9d,r11d  
00007FF697D830BF  mov         eax,r8d  
00007FF697D830C2  mov         dword ptr [r12+0F8h],r9d  
00007FF697D830CA  xor         r15d,ecx  
00007FF697D830CD  ror         eax,13h  
00007FF697D830D0  mov         ecx,r8d  
00007FF697D830D3  shr         r8d,0Ah  
00007FF697D830D7  xor         r8d,eax  
00007FF697D830DA  ror         ecx,11h  
00007FF697D830DD  xor         r8d,ecx  
00007FF697D830E0  add         r8d,r15d  
00007FF697D830E3  add         r8d,r14d  
00007FF697D830E6  add         r8d,r10d  
00007FF697D830E9  mov         dword ptr [r12+0FCh],r8d
00007FF697D830F1  add         rsp,28h  

// 8 Working variables popped from the stack.
00007FF697D830F5  pop         r15  
00007FF697D830F7  pop         r14  
00007FF697D830F9  pop         r13  
00007FF697D830FB  pop         r12  
00007FF697D830FD  pop         rdi  
00007FF697D830FE  pop         rsi  
00007FF697D830FF  pop         rbp  
00007FF697D83100  pop         rbx  
00007FF697D83101  ret  
}
#endif

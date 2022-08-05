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
////constexpr auto expected_pair160 = base16_array("0ae4f711ef5d6e9d26c611fd2c8c8ac45ecbf9e7");
constexpr auto expected_merk160 = base16_array("a8782546751527a4a1ad999058165d6af22421c8");

constexpr auto expected_half256 = base16_array("66687aadf862bd776c8fc18b8e9f8e20089714856ee233b3902a591d0d5f2925");
constexpr auto expected_full256 = base16_array("f5a5fd42d16a20302798ef6ed309979b43003d2320d9f0e8ea9831a92759fb4b");
////constexpr auto expected_pair256 = base16_array("38723a2e5e8a17aa7950dc008209944e898f69a7bd10a23c839d341e935fd5ca");
constexpr auto expected_merk256 = base16_array("e2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf9");

constexpr auto expected_half512 = base16_array("7be9fda48f4179e611c698a73cff09faf72869431efee6eaad14de0cb44bbf66503f752b7a8eb17083355f3ce6eb7d2806f236b25af96a24e22b887405c20081");
constexpr auto expected_full512 = base16_array("ab942f526272e456ed68a979f50202905ca903a141ed98443567b11ef0bf25a552d639051a01be58558122c58e3de07d749ee59ded36acf0c55cd91924d6ba11");
////constexpr auto expected_pair512 = base16_array("693f95d58383a6162d2aab49eb60395dcc4bb22295120caf3f21e3039003230b287c566a03c7a0ca5accaed2133c700b1cb3f82edf8adcbddc92b4f9fb9910c6");
constexpr auto expected_merk512 = base16_array("56d3e5825edf06e467e50dfeb09c1df2d9940121c05d61a162bfcb80aea3aa5fe958d917ac993d76cd3ea86240fedbb79520ce7b9c275793e3c75a82116cc320");

// RMD aliases are not concurrent.
static_assert(is_same_type<sha::algorithm<sha::h160,      false>, sha160>);
static_assert(is_same_type<sha::algorithm<sha::h256<224>, false>, sha256_224>);
static_assert(is_same_type<sha::algorithm<sha::h256<>,    false>, sha256>);
static_assert(is_same_type<sha::algorithm<sha::h512<256>, false>, sha512_256>);
static_assert(is_same_type<sha::algorithm<sha::h512<224>, false>, sha512_224>);
static_assert(is_same_type<sha::algorithm<sha::h512<384>, false>, sha512_384>);
static_assert(is_same_type<sha::algorithm<sha::h512<>,    false>, sha512>);

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

BOOST_AUTO_TEST_CASE(algorithm__double_hash160__null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha160::double_hash(full160), expected_merk160);
    BOOST_CHECK_EQUAL(sha1_hash(sha1_hash(full160)), expected_merk160);
}

// needs set expectation.
////BOOST_AUTO_TEST_CASE(algorithm__hash_blocks160__null_hash__expected)
////{
////    const std_vector<cref<sha160::block_t>> pair
////    {
////        std::cref(full160), std::cref(full160)
////    };
////
////    BOOST_CHECK_EQUAL(sha160::merkle_hash(pair), expected_pair160);
////    BOOST_CHECK_EQUAL(sha1_hash(pair160), expected_pair160);
////}

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

BOOST_AUTO_TEST_CASE(algorithm__double_hash256__null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha256::double_hash(full256), expected_merk256);
    BOOST_CHECK_EQUAL(sha256_hash(sha256_hash(full256)), expected_merk256);
}

// needs set expectation.
////BOOST_AUTO_TEST_CASE(algorithm__hash_blocks256__null_hash__expected)
////{
////    const std_vector<cref<sha256::block_t>> pair
////    {
////        std::cref(full256), std::cref(full256)
////    };
////
////    BOOST_CHECK_EQUAL(sha256::merkle_hash(pair), expected_pair256);
////    BOOST_CHECK_EQUAL(sha256_hash(pair256), expected_pair256);
////}

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

BOOST_AUTO_TEST_CASE(algorithm__double_hash512__null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha512::double_hash(full512), expected_merk512);
    BOOST_CHECK_EQUAL(sha512_hash(sha512_hash(full512)), expected_merk512);
}


// sha256 (concurrent)
// ----------------------------------------------------------------------------
using sha_256 = sha::algorithm<sha::h256<>, true>;

BOOST_AUTO_TEST_CASE(algorithm__concurrent_hash_half256__null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha_256::hash(half256), expected_half256);
    BOOST_CHECK_EQUAL(sha256_hash(half256), expected_half256);
}

BOOST_AUTO_TEST_CASE(algorithm__concurrent_hash_full256__null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha_256::hash(full256), expected_full256);
    BOOST_CHECK_EQUAL(sha256_hash(full256), expected_full256);
}

BOOST_AUTO_TEST_CASE(algorithm__concurrent_double_hash256__null_hash__expected)
{
    BOOST_CHECK_EQUAL(sha_256::double_hash(full256), expected_merk256);
    BOOST_CHECK_EQUAL(sha256_hash(sha256_hash(full256)), expected_merk256);
}

// needs set expectation.
////BOOST_AUTO_TEST_CASE(algorithm__hash_blocks512__null_hashes__expected)
////{
////    const std_vector<cref<sha512::block_t>> pair
////    {
////        std::cref(full512), std::cref(full512)
////    };
////
////    BOOST_CHECK_EQUAL(sha512::merkle_hash(pair), expected_pair512);
////    BOOST_CHECK_EQUAL(sha512_hash(pair512), expected_pair512);
////}

#if !defined(INVISIBLE)

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
static_assert(is_same_type<sha160::blocks_t, std_vector<cref<std_array<uint8_t, 64>>>>);
static_assert(is_same_type<sha160::set_t<42>, std_vector<cref<std_array<std_array<uint8_t, 64>, 42>>>>);
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
static_assert(is_same_type<sha256::blocks_t, std_vector<cref<std_array<uint8_t, 64>>>>);
static_assert(is_same_type<sha256::set_t<42>, std_vector<cref<std_array<std_array<uint8_t, 64>, 42>>>>);
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
static_assert(is_same_type<sha512::blocks_t, std_vector<cref<std_array<uint8_t, 128>>>>);
static_assert(is_same_type<sha512::set_t<42>, std_vector<cref<std_array<std_array<uint8_t, 128>, 42>>>>);
static_assert(is_same_type<decltype(sha512::limit_bits), const uint128_t>);
static_assert(is_same_type<decltype(sha512::limit_bytes), const uint128_t>);

// Truncations.
static_assert(sha256_224::big_end_count);
static_assert(sha256_224::big_end_count);
static_assert(sha512_256::big_end_count);
static_assert(sha512_224::big_end_count);
static_assert(sha512_384::big_end_count);

#endif

BOOST_AUTO_TEST_SUITE_END()

#if defined(DISABLED)

// This is the completely unrolled rounding function.
// 7add per round.
//const auto t = add_<k>(add_(add_(add_(Sigma1(e), choice(e, f, g)), h), w));
//d = /*e =*/    add_(t, d);
//h = /*a =*/    add_(add_(t, Sigma0(a)), majority(a, b, c));
// 2Signma(x), 1choice(x), 1 majority(x).
// 4xor, 4ror, 2shr; 2xor, 1and; 2or, 2and =
// 6xor, 4ror, 2shr, 3and, 2or.
// return xor_(xor_(ror_<A>(x), ror_<B>(x)), shr_<C>(x));
// return xor_(and_(x, xor_(y, z)), z);
// return or_(and_(x, or_(y, z)), and_(y, z));
// 24 math ops per round required: 7add, 6xor, 4ror, 2shr, 3and, 2or.
// Sigma, choice, majority, round imply 16 mov ops (math results).
//
//   64   rounds (one block)
//   16   mov ops required per round
//   24   math ops required per round
// ============================================================
//   40   average ops required per round
//   
// 1532   math ops observed
//  859   mov ops observed (includes lea calcs)
// ------------------------------------------------------------
// 2391   total ops observed
//   
// 16.000 mov ops required per round
// 13.422 mov ops observed per round (average)
// ------------------------------------------------------------
//119.21% move efficiency
//   
// 24.000 math ops required per round
// 23.938 math ops observed per round (average)
// ------------------------------------------------------------
// 99.51% math efficiency
//   
// 40     ops required per round
// 37.359 ops observed per round (average)
// ------------------------------------------------------------
//107.07% compiler net efficiency (better than manual assembly)

//    round< 0>(pstate, pbuffer);
//00007FF764B151C3  mov         r9d,dword ptr [rdx]  
//00007FF764B151C6  xor         r8d,r11d  
//00007FF764B151C9  mov         esi,dword ptr [r14+0Ch]  
//00007FF764B151CD  and         r8d,r12d  
//00007FF764B151D0  xor         r8d,r11d  
//00007FF764B151D3  mov         qword ptr [rsp+20h],rcx  
//00007FF764B151D8  ror         eax,0Bh  
//00007FF764B151DB  add         r9d,428A2F98h  
//00007FF764B151E2  ror         ebp,2  
//00007FF764B151E5  mov         ecx,r12d  
//00007FF764B151E8  ror         ecx,6  
//00007FF764B151EB  mov         edx,r12d  
//00007FF764B151EE  xor         ecx,eax  
//00007FF764B151F0  ror         edx,19h  
//00007FF764B151F3  xor         ecx,edx  
//00007FF764B151F5  mov         eax,edi  
//00007FF764B151F7  add         ecx,dword ptr [r14+1Ch]  
//00007FF764B151FB  and         eax,r10d  
//00007FF764B151FE  add         ecx,r8d  
//00007FF764B15201  mov         edx,r15d  
//00007FF764B15204  add         r9d,ecx  
//00007FF764B15207  ror         edx,16h  
//00007FF764B1520A  add         esi,r9d  
//00007FF764B1520D  mov         r8d,edi  
//00007FF764B15210  or          r8d,r10d  
//00007FF764B15213  mov         dword ptr [r14+0Ch],esi  
//00007FF764B15217  and         r8d,r15d  
//    round< 1>(pstate, pbuffer);
//00007FF764B1521A  mov         ecx,esi  
//00007FF764B1521C  or          r8d,eax  
//00007FF764B1521F  ror         ecx,6  
//00007FF764B15222  mov         eax,r15d  
//00007FF764B15225  ror         eax,0Dh  
//00007FF764B15228  xor         ebp,eax  
//00007FF764B1522A  mov         eax,esi  
//00007FF764B1522C  xor         ebp,edx  
//00007FF764B1522E  ror         eax,0Bh 
//00007FF764B15231  add         ebp,r8d  
//00007FF764B15234  xor         ecx,eax  
//00007FF764B15236  add         ebp,r9d  
//00007FF764B15239  mov         r8d,r12d  
//00007FF764B1523C  xor         r8d,ebx  
//00007FF764B1523F  mov         dword ptr [r14+1Ch],ebp  
//00007FF764B15243  mov         r9d,dword ptr [r13+4]  
//00007FF764B15247  and         r8d,esi  
//00007FF764B1524A  xor         r8d,ebx  
//00007FF764B1524D  add         r9d,71374491h  
//00007FF764B15254  mov         edx,esi  
//00007FF764B15256  mov         eax,r15d  
//00007FF764B15259  ror         edx,19h  
//00007FF764B1525C  and         eax,edi  
//00007FF764B1525E  xor         ecx,edx  
//00007FF764B15260  mov         edx,ebp  
//00007FF764B15262  add         ecx,r11d  
//00007FF764B15265  ror         edx,16h  
//00007FF764B15268  add         ecx,r8d  
//00007FF764B1526B  mov         r8d,r15d  
//00007FF764B1526E  or          r8d,edi  
//00007FF764B15271  add         r9d,ecx  
//00007FF764B15274  and         r8d,ebp  
//00007FF764B15277  or          r8d,eax  
//00007FF764B1527A  mov         eax,ebp  
//00007FF764B1527C  ror         eax,0Dh  
//00007FF764B1527F  lea         r11d,[r10+r9]  
//00007FF764B15283  mov         dword ptr [r14+8],r11d  
//00007FF764B15287  mov         r14d,ebp  
//00007FF764B1528A  ror         r14d,2  
//00007FF764B1528E  xor         r14d,eax  
//00007FF764B15291  xor         r14d,edx  
//    const state_t start{ state };
//00007FF764B15294  vmovups     ymmword ptr [start],ymm0  
//00007FF764B1529A  mov         rax,qword ptr [rsp+20h]  
//00007FF764B1529F  lea         r10d,[rdi-4A3F0431h]  
//00007FF764B152A6  mov         rdi,qword ptr [rsp+20h]  
//00007FF764B152AB  add         r14d,r8d  
//00007FF764B152AE  add         r14d,r9d  
//    round< 2>(pstate, pbuffer);
//00007FF764B152B1  mov         r8d,r12d  
//00007FF764B152B4  xor         r8d,esi  
//00007FF764B152B7  mov         ecx,r11d  
//00007FF764B152BA  ror         ecx,6  
//00007FF764B152BD  and         r8d,r11d  
//00007FF764B152C0  mov         dword ptr [rax+18h],r14d  
//00007FF764B152C4  xor         r8d,r12d  
//00007FF764B152C7  mov         r9d,dword ptr [r13+8]  
//00007FF764B152CB  mov         eax,r11d  
//00007FF764B152CE  ror         eax,0Bh  
//00007FF764B152D1  mov         edx,r11d  
//00007FF764B152D4  xor         ecx,eax  
//00007FF764B152D6  ror         edx,19h  
//00007FF764B152D9  xor         ecx,edx  
//00007FF764B152DB  mov         eax,ebp  
//00007FF764B152DD  add         ecx,ebx  
//00007FF764B152DF  and         eax,r15d  
//00007FF764B152E2  add         ecx,r8d  
//00007FF764B152E5  mov         edx,r14d  
//00007FF764B152E8  add         r9d,ecx  
//00007FF764B152EB  ror         edx,16h  
//00007FF764B152EE  add         r10d,r9d  
//00007FF764B152F1  mov         r8d,ebp  
//00007FF764B152F4  or          r8d,r15d  
//00007FF764B152F7  mov         dword ptr [rdi+4],r10d  
//00007FF764B152FB  and         r8d,r14d  
//00007FF764B152FE  mov         ecx,r14d  
//00007FF764B15301  or          r8d,eax  
//00007FF764B15304  ror         ecx,2  
//00007FF764B15307  lea         ebx,[r9-4A3F0431h]  
//00007FF764B1530E  mov         eax,r14d  
//00007FF764B15311  ror         eax,0Dh  
//00007FF764B15314  xor         ecx,eax  
//    round< 3>(pstate, pbuffer);
//00007FF764B15316  mov         eax,r10d  
//00007FF764B15319  xor         ecx,edx  
//00007FF764B1531B  ror         eax,0Bh  
//00007FF764B1531E  add         ecx,r8d  
//00007FF764B15321  mov         edx,r10d  
//00007FF764B15324  add         ebx,ecx  
//00007FF764B15326  ror         edx,19h  
//00007FF764B15329  mov         dword ptr [rdi+14h],ebx  
//00007FF764B1532C  mov         r8d,esi  
//00007FF764B1532F  mov         r9d,dword ptr [r13+0Ch]  
//00007FF764B15333  xor         r8d,r11d  
//00007FF764B15336  and         r8d,r10d  
//00007FF764B15339  mov         ecx,r10d  
//00007FF764B1533C  ror         ecx,6  
//00007FF764B1533F  xor         r8d,esi  
//00007FF764B15342  xor         ecx,eax  
//00007FF764B15344  mov         rsi,rdi  
//00007FF764B15347  xor         ecx,edx  
//00007FF764B15349  mov         eax,r14d  
//00007FF764B1534C  add         ecx,r12d  
//00007FF764B1534F  and         eax,ebp  
//00007FF764B15351  add         ecx,r8d  
//00007FF764B15354  lea         r12d,[r15-164A245Bh]  
//00007FF764B1535B  add         r9d,ecx  
//00007FF764B1535E  mov         r8d,r14d  
//00007FF764B15361  or          r8d,ebp  
//00007FF764B15364  add         r12d,r9d  
//00007FF764B15367  and         r8d,ebx  
//00007FF764B1536A  mov         dword ptr [rdi],r12d  
//00007FF764B1536D  or          r8d,eax  
//00007FF764B15370  mov         ecx,ebx  
//00007FF764B15372  ror         ecx,2  
//00007FF764B15375  lea         r15d,[r9-164A245Bh]  
//00007FF764B1537C  mov         eax,ebx  
//00007FF764B1537E  mov         edx,ebx  
//00007FF764B15380  ror         eax,0Dh  
//00007FF764B15383  xor         ecx,eax  
//00007FF764B15385  ror         edx,16h  
//00007FF764B15388  xor         ecx,edx  
//00007FF764B1538A  add         ecx,r8d  
//00007FF764B1538D  add         r15d,ecx  
//00007FF764B15390  mov         dword ptr [rdi+10h],r15d  
//    round< 4>(pstate, pbuffer);
//00007FF764B15394  mov         r9d,dword ptr [r13+10h]  
//00007FF764B15398  add         r9d,3956C25Bh  
//00007FF764B1539F  mov         r8d,r11d  
//00007FF764B153A2  xor         r8d,r10d  
//00007FF764B153A5  mov         eax,r12d  
//00007FF764B153A8  and         r8d,r12d  
//00007FF764B153AB  ror         eax,0Bh  
//00007FF764B153AE  xor         r8d,r11d  
//00007FF764B153B1  mov         ecx,r12d  
//00007FF764B153B4  ror         ecx,6  
//00007FF764B153B7  mov         edx,r12d  
//00007FF764B153BA  xor         ecx,eax  
//00007FF764B153BC  ror         edx,19h  
//00007FF764B153BF  xor         ecx,edx  
//00007FF764B153C1  mov         eax,ebx  
//00007FF764B153C3  add         ecx,dword ptr [rdi+0Ch]  
//00007FF764B153C6  and         eax,r14d  
//00007FF764B153C9  mov         edi,dword ptr [rdi+1Ch]  
//00007FF764B153CC  add         ecx,r8d  
//00007FF764B153CF  add         r9d,ecx  
//00007FF764B153D2  mov         r8d,ebx  
//00007FF764B153D5  or          r8d,r14d  
//00007FF764B153D8  add         edi,r9d  
//    round< 5>(pstate, pbuffer);
//00007FF764B153DB  mov         r14,qword ptr [rsp+20h]  
//00007FF764B153E0  and         r8d,r15d  
//00007FF764B153E3  or          r8d,eax  
//00007FF764B153E6  mov         dword ptr [rsi+1Ch],edi  
//00007FF764B153E9  mov         eax,r15d  
//00007FF764B153EC  mov         ebp,r15d  
//00007FF764B153EF  ror         eax,0Dh  
//00007FF764B153F2  mov         edx,r15d  
//00007FF764B153F5  ror         edx,16h  
//00007FF764B153F8  mov         ecx,edi  
//00007FF764B153FA  ror         ebp,2  
//00007FF764B153FD  xor         ebp,eax  
//00007FF764B153FF  ror         ecx,6  
//00007FF764B15402  xor         ebp,edx  
//00007FF764B15404  mov         eax,edi  
//00007FF764B15406  add         ebp,r9d  
//00007FF764B15409  ror         eax,0Bh  
//00007FF764B1540C  add         ebp,r8d  
//00007FF764B1540F  xor         ecx,eax  
//00007FF764B15411  mov         dword ptr [rsi+0Ch],ebp  
//00007FF764B15414  mov         r8d,r10d  
//00007FF764B15417  mov         r11d,dword ptr [rsi+18h]  
//00007FF764B1541B  xor         r8d,r12d  
//00007FF764B1541E  mov         r9d,dword ptr [r13+14h]  
//00007FF764B15422  and         r8d,edi  
//00007FF764B15425  xor         r8d,r10d  
//00007FF764B15428  add         r9d,59F111F1h  
//00007FF764B1542F  mov         edx,edi  
//00007FF764B15431  mov         eax,r15d  
//00007FF764B15434  ror         edx,19h  
//00007FF764B15437  and         eax,ebx  
//00007FF764B15439  xor         ecx,edx  
//00007FF764B1543B  mov         edx,ebp  
//00007FF764B1543D  add         ecx,dword ptr [rsi+8]  
//00007FF764B15440  add         ecx,r8d  
//00007FF764B15443  ror         edx,16h  
//00007FF764B15446  add         r9d,ecx  
//00007FF764B15449  mov         r8d,r15d  
//00007FF764B1544C  or          r8d,ebx  
//00007FF764B1544F  add         r11d,r9d  
//00007FF764B15452  mov         dword ptr [rsi+18h],r11d  
//00007FF764B15456  and         r8d,ebp  
//00007FF764B15459  or          r8d,eax  
//00007FF764B1545C  mov         esi,ebp  
//00007FF764B1545E  ror         esi,2  
//00007FF764B15461  mov         eax,ebp  
//00007FF764B15463  ror         eax,0Dh  
//00007FF764B15466  xor         esi,eax  
//00007FF764B15468  xor         esi,edx  
//    round< 6>(pstate, pbuffer);
//00007FF764B1546A  mov         edx,r11d  
//00007FF764B1546D  add         esi,r9d  
//00007FF764B15470  add         esi,r8d  
//00007FF764B15473  mov         r8d,r12d  
//00007FF764B15476  xor         r8d,edi  
//00007FF764B15479  mov         dword ptr [r14+8],esi  
//00007FF764B1547D  mov         r9d,dword ptr [r13+18h]  
//00007FF764B15481  and         r8d,r11d  
//00007FF764B15484  xor         r8d,r12d  
//00007FF764B15487  ror         edx,19h  
//00007FF764B1548A  mov         r10d,dword ptr [r14+14h]  
//00007FF764B1548E  mov         eax,r11d  
//00007FF764B15491  ror         eax,0Bh  
//00007FF764B15494  add         r10d,923F82A4h  
//00007FF764B1549B  mov         ecx,r11d  
//00007FF764B1549E  ror         ecx,6  
//00007FF764B154A1  xor         ecx,eax  
//00007FF764B154A3  mov         eax,ebp  
//00007FF764B154A5  xor         ecx,edx  
//00007FF764B154A7  and         eax,r15d  
//00007FF764B154AA  add         ecx,dword ptr [r14+4]  
//00007FF764B154AE  mov         edx,esi  
//00007FF764B154B0  add         ecx,r8d  
//00007FF764B154B3  ror         edx,16h  
//00007FF764B154B6  add         r9d,ecx  
//00007FF764B154B9  mov         r8d,ebp  
//00007FF764B154BC  or          r8d,r15d  
//00007FF764B154BF  add         r10d,r9d  
//00007FF764B154C2  and         r8d,esi  
//00007FF764B154C5  mov         dword ptr [r14+14h],r10d  
//00007FF764B154C9  or          r8d,eax  
//00007FF764B154CC  mov         ecx,esi  
//00007FF764B154CE  ror         ecx,2  
//00007FF764B154D1  lea         ebx,[r9-6DC07D5Ch]  
//00007FF764B154D8  mov         eax,esi  
//00007FF764B154DA  ror         eax,0Dh  
//00007FF764B154DD  xor         ecx,eax  
//    round< 7>(pstate, pbuffer);
//00007FF764B154DF  mov         eax,r10d  
//00007FF764B154E2  xor         ecx,edx  
//00007FF764B154E4  ror         eax,0Bh  
//00007FF764B154E7  add         ecx,r8d  
//00007FF764B154EA  mov         edx,r10d  
//00007FF764B154ED  add         ebx,ecx  
//00007FF764B154EF  ror         edx,19h  
//00007FF764B154F2  mov         dword ptr [r14+4],ebx  
//00007FF764B154F6  mov         r8d,edi  
//00007FF764B154F9  mov         r9d,dword ptr [r13+1Ch]  
//00007FF764B154FD  xor         r8d,r11d  
//00007FF764B15500  and         r8d,r10d  
//00007FF764B15503  mov         ecx,r10d  
//00007FF764B15506  xor         r8d,edi  
//00007FF764B15509  ror         ecx,6  
//00007FF764B1550C  mov         edi,dword ptr [r14+10h]  
//00007FF764B15510  xor         ecx,eax  
//00007FF764B15512  xor         ecx,edx  
//00007FF764B15514  add         edi,0AB1C5ED5h  
//00007FF764B1551A  add         ecx,r12d  
//00007FF764B1551D  mov         eax,esi  
//00007FF764B1551F  add         ecx,r8d  
//00007FF764B15522  and         eax,ebp  
//00007FF764B15524  add         r9d,ecx  
//00007FF764B15527  mov         r8d,esi  
//00007FF764B1552A  or          r8d,ebp  
//00007FF764B1552D  add         edi,r9d  
//00007FF764B15530  and         r8d,ebx  
//00007FF764B15533  mov         dword ptr [r14+10h],edi  
//00007FF764B15537  or          r8d,eax  
//00007FF764B1553A  mov         ecx,ebx  
//00007FF764B1553C  ror         ecx,2  
//00007FF764B1553F  lea         r15d,[r9-54E3A12Bh]  
//00007FF764B15546  mov         eax,ebx  
//00007FF764B15548  mov         edx,ebx  
//00007FF764B1554A  ror         eax,0Dh  
//00007FF764B1554D  mov         r12,r14  
//00007FF764B15550  xor         ecx,eax  
//00007FF764B15552  ror         edx,16h  
//00007FF764B15555  xor         ecx,edx  
//    round< 8>(pstate, pbuffer);
//00007FF764B15557  mov         eax,edi  
//00007FF764B15559  add         ecx,r8d  
//00007FF764B1555C  ror         eax,0Bh  
//00007FF764B1555F  add         r15d,ecx  
//00007FF764B15562  mov         r8d,r10d  
//00007FF764B15565  xor         r8d,r11d  
//00007FF764B15568  mov         dword ptr [r14],r15d  
//00007FF764B1556B  mov         r9d,dword ptr [r13+20h]  
//00007FF764B1556F  and         r8d,edi  
//00007FF764B15572  mov         edx,edi  
//00007FF764B15574  xor         r8d,r11d  
//00007FF764B15577  ror         edx,19h  
//00007FF764B1557A  mov         ecx,edi  
//00007FF764B1557C  ror         ecx,6  
//00007FF764B1557F  mov         r11d,dword ptr [r14+0Ch]  
//00007FF764B15583  xor         ecx,eax  
//00007FF764B15585  xor         ecx,edx  
//00007FF764B15587  add         r11d,0D807AA98h  
//00007FF764B1558E  add         ecx,dword ptr [r14+1Ch]  
//00007FF764B15592  mov         eax,ebx  
//00007FF764B15594  add         ecx,r8d  
//00007FF764B15597  and         eax,esi  
//00007FF764B15599  add         r9d,ecx  
//00007FF764B1559C  mov         r8d,ebx  
//00007FF764B1559F  or          r8d,esi  
//00007FF764B155A2  add         r11d,r9d  
//00007FF764B155A5  and         r8d,r15d  
//00007FF764B155A8  mov         dword ptr [r14+0Ch],r11d  
//00007FF764B155AC  or          r8d,eax  
//00007FF764B155AF  mov         ecx,r15d  
//00007FF764B155B2  ror         ecx,2  
//00007FF764B155B5  lea         esi,[r9-27F85568h]  
//00007FF764B155BC  mov         eax,r15d  
//00007FF764B155BF  mov         edx,r15d  
//00007FF764B155C2  ror         eax,0Dh  
//00007FF764B155C5  xor         ecx,eax  
//00007FF764B155C7  ror         edx,16h  
//00007FF764B155CA  xor         ecx,edx  
//    round< 9>(pstate, pbuffer);
//00007FF764B155CC  mov         eax,r11d  
//00007FF764B155CF  add         ecx,r8d  
//00007FF764B155D2  ror         eax,0Bh  
//00007FF764B155D5  add         esi,ecx  
//00007FF764B155D7  mov         r8d,edi  
//00007FF764B155DA  xor         r8d,r10d  
//00007FF764B155DD  mov         dword ptr [r14+1Ch],esi  
//00007FF764B155E1  mov         r9d,dword ptr [r13+24h]  
//00007FF764B155E5  and         r8d,r11d  
//00007FF764B155E8  xor         r8d,r10d  
//00007FF764B155EB  add         r9d,12835B01h  
//00007FF764B155F2  mov         r10d,dword ptr [r14+8]  
//00007FF764B155F6  mov         ecx,r11d  
//00007FF764B155F9  ror         ecx,6  
//00007FF764B155FC  mov         edx,r11d  
//00007FF764B155FF  xor         ecx,eax  
//00007FF764B15601  ror         edx,19h  
//00007FF764B15604  xor         ecx,edx  
//00007FF764B15606  mov         eax,r15d  
//00007FF764B15609  add         ecx,dword ptr [r14+18h]  
//00007FF764B1560D  and         eax,ebx  
//00007FF764B1560F  add         ecx,r8d  
//00007FF764B15612  mov         ebp,esi  
//00007FF764B15614  add         r9d,ecx  
//00007FF764B15617  ror         ebp,2  
//00007FF764B1561A  add         r10d,r9d  
//00007FF764B1561D  mov         r8d,r15d  
//00007FF764B15620  or          r8d,ebx  
//00007FF764B15623  mov         dword ptr [r14+8],r10d  
//00007FF764B15627  and         r8d,esi  
//00007FF764B1562A  mov         edx,esi  
//00007FF764B1562C  or          r8d,eax  
//00007FF764B1562F  ror         edx,16h  
//00007FF764B15632  mov         eax,esi  
//    round<10>(pstate, pbuffer);
//00007FF764B15634  mov         ecx,r10d  
//00007FF764B15637  ror         ecx,6  
//00007FF764B1563A  ror         eax,0Dh  
//00007FF764B1563D  xor         ebp,eax  
//00007FF764B1563F  mov         eax,r10d  
//00007FF764B15642  xor         ebp,edx  
//00007FF764B15644  ror         eax,0Bh  
//00007FF764B15647  add         ebp,r8d  
//00007FF764B1564A  xor         ecx,eax  
//00007FF764B1564C  add         ebp,r9d  
//00007FF764B1564F  mov         r8d,r11d  
//00007FF764B15652  xor         r8d,edi  
//00007FF764B15655  mov         dword ptr [r14+18h],ebp  
//00007FF764B15659  mov         r9d,dword ptr [r13+28h]  
//00007FF764B1565D  and         r8d,r10d  
//00007FF764B15660  xor         r8d,edi  
//00007FF764B15663  mov         edx,r10d  
//00007FF764B15666  ror         edx,19h  
//00007FF764B15669  add         r9d,243185BEh  
//00007FF764B15670  xor         ecx,edx  
//00007FF764B15672  add         ecx,dword ptr [r14+14h]  
//00007FF764B15676  add         ecx,r8d  
//00007FF764B15679  add         r9d,ecx  
//00007FF764B1567C  mov         edi,dword ptr [r14+4]  
//00007FF764B15680  mov         r8d,esi  
//00007FF764B15683  or          r8d,r15d  
//00007FF764B15686  add         edi,r9d  
//00007FF764B15689  and         r8d,ebp  
//00007FF764B1568C  mov         dword ptr [r14+4],edi  
//00007FF764B15690  mov         eax,esi  
//00007FF764B15692  mov         r14d,ebp  
//00007FF764B15695  and         eax,r15d  
//00007FF764B15698  ror         r14d,2  
//00007FF764B1569C  or          r8d,eax  
//00007FF764B1569F  mov         edx,ebp  
//00007FF764B156A1  ror         edx,16h  
//00007FF764B156A4  mov         eax,ebp  
//00007FF764B156A6  ror         eax,0Dh  
//    round<11>(pstate, pbuffer);
//00007FF764B156A9  mov         ecx,edi  
//00007FF764B156AB  xor         r14d,eax  
//00007FF764B156AE  ror         ecx,6  
//00007FF764B156B1  xor         r14d,edx  
//00007FF764B156B4  mov         eax,edi  
//00007FF764B156B6  add         r14d,r8d  
//00007FF764B156B9  ror         eax,0Bh  
//00007FF764B156BC  xor         ecx,eax  
//00007FF764B156BE  add         r14d,r9d  
//00007FF764B156C1  mov         dword ptr [r12+14h],r14d  
//00007FF764B156C6  mov         r8d,r10d  
//00007FF764B156C9  mov         r9d,dword ptr [r13+2Ch]  
//00007FF764B156CD  xor         r8d,r11d  
//00007FF764B156D0  and         r8d,edi  
//00007FF764B156D3  add         r9d,550C7DC3h  
//00007FF764B156DA  xor         r8d,r11d  
//00007FF764B156DD  mov         edx,edi  
//00007FF764B156DF  ror         edx,19h  
//00007FF764B156E2  mov         eax,ebp  
//00007FF764B156E4  xor         ecx,edx  
//00007FF764B156E6  and         eax,esi  
//00007FF764B156E8  add         ecx,dword ptr [r12+10h]  
//00007FF764B156ED  mov         edx,r14d  
//00007FF764B156F0  add         ecx,r8d  
//00007FF764B156F3  ror         edx,16h  
//00007FF764B156F6  add         r9d,ecx  
//00007FF764B156F9  mov         r8d,ebp  
//00007FF764B156FC  or          r8d,esi  
//00007FF764B156FF  add         r15d,r9d  
//00007FF764B15702  and         r8d,r14d  
//00007FF764B15705  mov         dword ptr [r12],r15d  
//00007FF764B15709  or          r8d,eax  
//00007FF764B1570C  mov         esi,r14d  
//00007FF764B1570F  ror         esi,2  
//00007FF764B15712  mov         eax,r14d  
//00007FF764B15715  ror         eax,0Dh  
//    round<12>(pstate, pbuffer);
//00007FF764B15718  mov         ecx,r15d  
//00007FF764B1571B  xor         esi,eax  
//00007FF764B1571D  ror         ecx,6  
//00007FF764B15720  xor         esi,edx  
//00007FF764B15722  mov         eax,r15d  
//00007FF764B15725  add         esi,r8d  
//00007FF764B15728  ror         eax,0Bh  
//00007FF764B1572B  add         esi,r9d  
//00007FF764B1572E  xor         ecx,eax  
//00007FF764B15730  mov         dword ptr [r12+10h],esi  
//00007FF764B15735  mov         r8d,r10d  
//00007FF764B15738  mov         r9d,dword ptr [r13+30h]  
//00007FF764B1573C  xor         r8d,edi  
//00007FF764B1573F  mov         ebx,dword ptr [r12+1Ch]  
//00007FF764B15744  and         r8d,r15d  
//00007FF764B15747  xor         r8d,r10d  
//00007FF764B1574A  add         r9d,72BE5D74h  
//00007FF764B15751  mov         edx,r15d  
//00007FF764B15754  mov         eax,r14d  
//00007FF764B15757  ror         edx,19h  
//00007FF764B1575A  xor         ecx,edx  
//00007FF764B1575C  add         ecx,dword ptr [r12+0Ch]  
//00007FF764B15761  add         ecx,r8d  
//00007FF764B15764  mov         r8d,r14d  
//00007FF764B15767  or          r8d,ebp  
//00007FF764B1576A  add         r9d,ecx  
//00007FF764B1576D  add         ebx,r9d  
//00007FF764B15770  and         r8d,esi  
//00007FF764B15773  and         eax,ebp  
//00007FF764B15775  mov         dword ptr [r12+1Ch],ebx  
//00007FF764B1577A  or          r8d,eax  
//00007FF764B1577D  mov         ebp,esi  
//00007FF764B1577F  ror         ebp,2  
//00007FF764B15782  mov         eax,esi  
//00007FF764B15784  ror         eax,0Dh  
//00007FF764B15787  mov         edx,esi  
//00007FF764B15789  xor         ebp,eax  
//00007FF764B1578B  ror         edx,16h  
//00007FF764B1578E  xor         ebp,edx  
//    round<13>(pstate, pbuffer);
//00007FF764B15790  mov         eax,ebx  
//00007FF764B15792  add         ebp,r8d  
//00007FF764B15795  ror         eax,0Bh  
//00007FF764B15798  add         ebp,r9d  
//00007FF764B1579B  mov         r8d,edi  
//00007FF764B1579E  xor         r8d,r15d  
//00007FF764B157A1  mov         dword ptr [r12+0Ch],ebp  
//00007FF764B157A6  mov         r9d,dword ptr [r13+34h]  
//00007FF764B157AA  and         r8d,ebx  
//00007FF764B157AD  mov         r10d,dword ptr [r12+18h]  
//00007FF764B157B2  xor         r8d,edi  
//00007FF764B157B5  add         r10d,80DEB1FEh  
//00007FF764B157BC  mov         ecx,ebx  
//00007FF764B157BE  ror         ecx,6  
//00007FF764B157C1  mov         edx,ebx  
//00007FF764B157C3  xor         ecx,eax  
//00007FF764B157C5  ror         edx,19h  
//00007FF764B157C8  xor         ecx,edx  
//00007FF764B157CA  mov         edi,esi  
//00007FF764B157CC  add         ecx,dword ptr [r12+8]  
//00007FF764B157D1  or          edi,r14d  
//00007FF764B157D4  add         ecx,r8d  
//00007FF764B157D7  and         edi,ebp  
//00007FF764B157D9  add         r9d,ecx  
//00007FF764B157DC  mov         eax,esi  
//00007FF764B157DE  and         eax,r14d  
//00007FF764B157E1  add         r10d,r9d  
//00007FF764B157E4  or          edi,eax  
//00007FF764B157E6  mov         dword ptr [r12+18h],r10d  
//00007FF764B157EB  add         r9d,80DEB1FEh  
//00007FF764B157F2  mov         eax,ebp  
//00007FF764B157F4  ror         eax,0Dh  
//00007FF764B157F7  mov         ecx,ebp  
//00007FF764B157F9  ror         ecx,2  
//00007FF764B157FC  mov         edx,ebp  
//00007FF764B157FE  xor         ecx,eax  
//00007FF764B15800  ror         edx,16h  
//00007FF764B15803  xor         ecx,edx  
//    round<14>(pstate, pbuffer);
//00007FF764B15805  mov         eax,r10d  
//00007FF764B15808  add         ecx,r9d  
//00007FF764B1580B  ror         eax,0Bh  
//00007FF764B1580E  add         edi,ecx  
//00007FF764B15810  mov         r8d,r15d  
//00007FF764B15813  xor         r8d,ebx  
//00007FF764B15816  mov         dword ptr [r12+8],edi  
//00007FF764B1581B  mov         r9d,dword ptr [r13+38h]  
//00007FF764B1581F  and         r8d,r10d  
//00007FF764B15822  mov         r11d,dword ptr [r12+14h]  
//00007FF764B15827  xor         r8d,r15d  
//00007FF764B1582A  add         r11d,9BDC06A7h  
//00007FF764B15831  mov         ecx,r10d  
//00007FF764B15834  ror         ecx,6  
//00007FF764B15837  mov         edx,r10d  
//00007FF764B1583A  xor         ecx,eax  
//00007FF764B1583C  ror         edx,19h  
//00007FF764B1583F  xor         ecx,edx  
//00007FF764B15841  mov         eax,ebp  
//00007FF764B15843  add         ecx,dword ptr [r12+4]  
//00007FF764B15848  and         eax,esi  
//00007FF764B1584A  add         ecx,r8d  
//00007FF764B1584D  mov         edx,edi  
//00007FF764B1584F  add         r9d,ecx  
//00007FF764B15852  ror         edx,16h  
//00007FF764B15855  add         r11d,r9d  
//00007FF764B15858  mov         r8d,ebp  
//00007FF764B1585B  or          r8d,esi  
//00007FF764B1585E  mov         dword ptr [r12+14h],r11d  
//00007FF764B15863  and         r8d,edi  
//00007FF764B15866  mov         ecx,edi  
//00007FF764B15868  or          r8d,eax  
//00007FF764B1586B  mov         eax,edi  
//00007FF764B1586D  ror         eax,0Dh  
//00007FF764B15870  ror         ecx,2  
//00007FF764B15873  lea         esi,[r9-6423F959h]  
//00007FF764B1587A  xor         ecx,eax  
//    round<15>(pstate, pbuffer);
//00007FF764B1587C  mov         eax,r11d  
//00007FF764B1587F  xor         ecx,edx  
//00007FF764B15881  ror         eax,0Bh  
//00007FF764B15884  add         ecx,r8d  
//00007FF764B15887  mov         edx,r11d  
//00007FF764B1588A  add         esi,ecx  
//00007FF764B1588C  ror         edx,19h  
//00007FF764B1588F  mov         dword ptr [r12+4],esi  
//00007FF764B15894  mov         r8d,ebx  
//00007FF764B15897  mov         r9d,dword ptr [r13+3Ch]  
//00007FF764B1589B  xor         r8d,r10d  
//00007FF764B1589E  and         r8d,r11d  
//00007FF764B158A1  mov         ecx,r11d  
//00007FF764B158A4  ror         ecx,6  
//00007FF764B158A7  xor         r8d,ebx  
//00007FF764B158AA  mov         ebx,dword ptr [r12+10h]  
//00007FF764B158AF  xor         ecx,eax  
//00007FF764B158B1  xor         ecx,edx  
//00007FF764B158B3  mov         eax,edi  
//00007FF764B158B5  add         ecx,r15d  
//00007FF764B158B8  and         eax,ebp  
//00007FF764B158BA  add         ecx,r8d  
//00007FF764B158BD  add         ebx,0C19BF174h  
//00007FF764B158C3  add         r9d,ecx  
//00007FF764B158C6  mov         r8d,edi  
//00007FF764B158C9  or          r8d,ebp  
//00007FF764B158CC  add         ebx,r9d  
//00007FF764B158CF  and         r8d,esi  
//00007FF764B158D2  mov         dword ptr [r12+10h],ebx  
//00007FF764B158D7  or          r8d,eax  
//00007FF764B158DA  mov         ecx,esi  
//00007FF764B158DC  ror         ecx,2  
//00007FF764B158DF  lea         r14d,[r9-3E640E8Ch]  
//00007FF764B158E6  mov         eax,esi  
//00007FF764B158E8  mov         edx,esi  
//00007FF764B158EA  ror         eax,0Dh  
//00007FF764B158ED  xor         ecx,eax  
//00007FF764B158EF  ror         edx,16h  
//00007FF764B158F2  xor         ecx,edx  
//    round<16>(pstate, pbuffer);
//00007FF764B158F4  mov         eax,ebx  
//00007FF764B158F6  add         ecx,r8d  
//00007FF764B158F9  ror         eax,0Bh  
//    round<15>(pstate, pbuffer);
//00007FF764B158FC  add         r14d,ecx  
//00007FF764B158FF  mov         r8d,r11d  
//00007FF764B15902  xor         r8d,r10d  
//00007FF764B15905  mov         dword ptr [r12],r14d  
//00007FF764B15909  mov         r9d,dword ptr [r13+40h]  
//00007FF764B1590D  and         r8d,ebx  
//00007FF764B15910  xor         r8d,r10d  
//00007FF764B15913  mov         ecx,ebx  
//00007FF764B15915  mov         r10d,dword ptr [r12+0Ch]  
//00007FF764B1591A  mov         edx,ebx  
//00007FF764B1591C  ror         ecx,6  
//00007FF764B1591F  add         r10d,0E49B69C1h  
//00007FF764B15926  xor         ecx,eax  
//00007FF764B15928  ror         edx,19h  
//00007FF764B1592B  xor         ecx,edx  
//00007FF764B1592D  mov         eax,esi  
//00007FF764B1592F  add         ecx,dword ptr [r12+1Ch]  
//00007FF764B15934  and         eax,edi  
//00007FF764B15936  add         ecx,r8d  
//00007FF764B15939  mov         edx,r14d  
//00007FF764B1593C  add         r9d,ecx  
//00007FF764B1593F  ror         edx,16h  
//00007FF764B15942  add         r10d,r9d  
//00007FF764B15945  mov         r8d,esi  
//00007FF764B15948  or          r8d,edi  
//00007FF764B1594B  mov         dword ptr [r12+0Ch],r10d  
//00007FF764B15950  and         r8d,r14d  
//00007FF764B15953  mov         ecx,r14d  
//00007FF764B15956  or          r8d,eax  
//00007FF764B15959  ror         ecx,2  
//00007FF764B1595C  mov         eax,r14d  
//00007FF764B1595F  lea         edi,[r9-1B64963Fh]  
//00007FF764B15966  ror         eax,0Dh  
//00007FF764B15969  xor         ecx,eax  
//00007FF764B1596B  xor         ecx,edx  
//00007FF764B1596D  add         ecx,r8d  
//00007FF764B15970  add         edi,ecx  
//    round<17>(pstate, pbuffer);
//00007FF764B15972  mov         r8d,r11d  
//00007FF764B15975  xor         r8d,ebx  
//00007FF764B15978  mov         dword ptr [r12+1Ch],edi  
//00007FF764B1597D  mov         r9d,dword ptr [r13+44h]  
//00007FF764B15981  and         r8d,r10d  
//00007FF764B15984  xor         r8d,r11d  
//00007FF764B15987  mov         ecx,r10d  
//00007FF764B1598A  mov         r11d,dword ptr [r12+8]  
//00007FF764B1598F  mov         eax,r10d  
//00007FF764B15992  ror         ecx,6  
//00007FF764B15995  add         r11d,0EFBE4786h  
//00007FF764B1599C  ror         eax,0Bh  
//00007FF764B1599F  mov         edx,r10d  
//00007FF764B159A2  xor         ecx,eax  
//00007FF764B159A4  ror         edx,19h  
//00007FF764B159A7  xor         ecx,edx  
//00007FF764B159A9  mov         eax,r14d  
//00007FF764B159AC  add         ecx,dword ptr [r12+18h]  
//00007FF764B159B1  and         eax,esi  
//00007FF764B159B3  add         ecx,r8d  
//00007FF764B159B6  mov         edx,edi  
//00007FF764B159B8  add         r9d,ecx  
//00007FF764B159BB  ror         edx,16h  
//00007FF764B159BE  add         r11d,r9d  
//00007FF764B159C1  mov         r8d,r14d  
//00007FF764B159C4  or          r8d,esi  
//00007FF764B159C7  mov         dword ptr [r12+8],r11d  
//00007FF764B159CC  and         r8d,edi  
//00007FF764B159CF  mov         ecx,edi  
//00007FF764B159D1  or          r8d,eax  
//00007FF764B159D4  ror         ecx,2  
//00007FF764B159D7  lea         esi,[r9-1041B87Ah]  
//00007FF764B159DE  mov         eax,edi  
//00007FF764B159E0  ror         eax,0Dh  
//00007FF764B159E3  xor         ecx,eax  
//    round<18>(pstate, pbuffer);
//00007FF764B159E5  mov         eax,r11d  
//00007FF764B159E8  xor         ecx,edx  
//00007FF764B159EA  ror         eax,0Bh  
//00007FF764B159ED  add         ecx,r8d  
//00007FF764B159F0  mov         edx,r11d  
//00007FF764B159F3  add         esi,ecx  
//00007FF764B159F5  ror         edx,19h  
//00007FF764B159F8  mov         dword ptr [r12+18h],esi  
//00007FF764B159FD  mov         r8d,ebx  
//00007FF764B15A00  mov         r9d,dword ptr [r13+48h]  
//00007FF764B15A04  xor         r8d,r10d  
//00007FF764B15A07  and         r8d,r11d  
//00007FF764B15A0A  add         r9d,0FC19DC6h  
//00007FF764B15A11  xor         r8d,ebx  
//00007FF764B15A14  mov         ecx,r11d  
//00007FF764B15A17  mov         ebx,dword ptr [r12+4]  
//00007FF764B15A1C  mov         ebp,esi  
//00007FF764B15A1E  ror         ecx,6  
//00007FF764B15A21  xor         ecx,eax  
//00007FF764B15A23  ror         ebp,2  
//00007FF764B15A26  xor         ecx,edx  
//00007FF764B15A28  mov         eax,edi  
//00007FF764B15A2A  add         ecx,dword ptr [r12+14h]  
//00007FF764B15A2F  and         eax,r14d  
//00007FF764B15A32  add         ecx,r8d  
//00007FF764B15A35  mov         edx,esi  
//00007FF764B15A37  add         r9d,ecx  
//00007FF764B15A3A  ror         edx,16h  
//00007FF764B15A3D  add         ebx,r9d  
//00007FF764B15A40  mov         r8d,edi  
//00007FF764B15A43  or          r8d,r14d  
//00007FF764B15A46  mov         dword ptr [r12+4],ebx  
//00007FF764B15A4B  and         r8d,esi  
//00007FF764B15A4E  or          r8d,eax  
//00007FF764B15A51  mov         eax,esi  
//00007FF764B15A53  ror         eax,0Dh  
//00007FF764B15A56  xor         ebp,eax  
//00007FF764B15A58  xor         ebp,edx  
//00007FF764B15A5A  add         ebp,r9d  
//00007FF764B15A5D  add         ebp,r8d  
//    round<19>(pstate, pbuffer);
//00007FF764B15A60  mov         r8d,r10d  
//00007FF764B15A63  xor         r8d,r11d  
//00007FF764B15A66  mov         dword ptr [r12+14h],ebp  
//00007FF764B15A6B  mov         r9d,dword ptr [r13+4Ch]  
//00007FF764B15A6F  and         r8d,ebx  
//00007FF764B15A72  xor         r8d,r10d  
//00007FF764B15A75  add         r9d,240CA1CCh  
//00007FF764B15A7C  mov         eax,ebx  
//00007FF764B15A7E  mov         ecx,ebx  
//00007FF764B15A80  ror         eax,0Bh  
//00007FF764B15A83  mov         edx,ebx  
//00007FF764B15A85  ror         ecx,6  
//00007FF764B15A88  xor         ecx,eax  
//00007FF764B15A8A  ror         edx,19h  
//00007FF764B15A8D  xor         ecx,edx  
//00007FF764B15A8F  mov         eax,esi  
//00007FF764B15A91  add         ecx,dword ptr [r12+10h]  
//00007FF764B15A96  and         eax,edi  
//00007FF764B15A98  add         ecx,r8d  
//00007FF764B15A9B  mov         edx,ebp  
//00007FF764B15A9D  add         r9d,ecx  
//00007FF764B15AA0  ror         edx,16h  
//00007FF764B15AA3  mov         r8d,esi  
//00007FF764B15AA6  or          r8d,edi  
//00007FF764B15AA9  and         r8d,ebp  
//00007FF764B15AAC  or          r8d,eax  
//00007FF764B15AAF  lea         r15d,[r9+r14]  
//00007FF764B15AB3  mov         dword ptr [r12],r15d  
//00007FF764B15AB7  mov         eax,ebp  
//00007FF764B15AB9  ror         eax,0Dh  
//00007FF764B15ABC  mov         r14d,ebp  
//00007FF764B15ABF  ror         r14d,2  
//    round<20>(pstate, pbuffer);
//00007FF764B15AC3  mov         ecx,r15d  
//00007FF764B15AC6  xor         r14d,eax  
//00007FF764B15AC9  ror         ecx,6  
//00007FF764B15ACC  xor         r14d,edx  
//00007FF764B15ACF  mov         eax,r15d  
//00007FF764B15AD2  add         r14d,r8d  
//00007FF764B15AD5  ror         eax,0Bh  
//00007FF764B15AD8  xor         ecx,eax  
//00007FF764B15ADA  add         r14d,r9d  
//00007FF764B15ADD  mov         dword ptr [r12+10h],r14d  
//00007FF764B15AE2  mov         r8d,ebx  
//00007FF764B15AE5  mov         r9d,dword ptr [r13+50h]  
//00007FF764B15AE9  xor         r8d,r11d  
//00007FF764B15AEC  and         r8d,r15d  
//00007FF764B15AEF  add         r9d,2DE92C6Fh  
//00007FF764B15AF6  xor         r8d,r11d  
//00007FF764B15AF9  mov         edx,r15d  
//00007FF764B15AFC  mov         r11d,dword ptr [r12+1Ch]  
//00007FF764B15B01  mov         eax,ebp  
//00007FF764B15B03  ror         edx,19h  
//00007FF764B15B06  and         eax,esi  
//00007FF764B15B08  xor         ecx,edx  
//00007FF764B15B0A  mov         edi,r14d  
//00007FF764B15B0D  add         ecx,dword ptr [r12+0Ch]  
//00007FF764B15B12  mov         edx,r14d  
//00007FF764B15B15  add         ecx,r8d  
//00007FF764B15B18  ror         edi,2  
//00007FF764B15B1B  add         r9d,ecx  
//00007FF764B15B1E  ror         edx,16h  
//00007FF764B15B21  add         r11d,r9d  
//00007FF764B15B24  mov         r8d,ebp  
//00007FF764B15B27  or          r8d,esi  
//00007FF764B15B2A  mov         dword ptr [r12+1Ch],r11d  
//00007FF764B15B2F  and         r8d,r14d  
//    round<21>(pstate, pbuffer);
//00007FF764B15B32  mov         ecx,r11d  
//00007FF764B15B35  or          r8d,eax  
//00007FF764B15B38  ror         ecx,6  
//00007FF764B15B3B  mov         eax,r14d  
//00007FF764B15B3E  ror         eax,0Dh  
//00007FF764B15B41  xor         edi,eax  
//00007FF764B15B43  mov         eax,r11d  
//00007FF764B15B46  xor         edi,edx  
//00007FF764B15B48  ror         eax,0Bh  
//00007FF764B15B4B  add         edi,r8d  
//00007FF764B15B4E  mov         edx,r11d  
//00007FF764B15B51  add         edi,r9d  
//00007FF764B15B54  ror         edx,19h  
//00007FF764B15B57  mov         r8d,r15d  
//00007FF764B15B5A  mov         dword ptr [r12+0Ch],edi  
//00007FF764B15B5F  mov         r9d,dword ptr [r13+54h]  
//00007FF764B15B63  xor         r8d,ebx  
//00007FF764B15B66  and         r8d,r11d  
//00007FF764B15B69  xor         r8d,ebx  
//00007FF764B15B6C  xor         ecx,eax  
//00007FF764B15B6E  mov         ebx,dword ptr [r12+18h]  
//00007FF764B15B73  xor         ecx,edx  
//00007FF764B15B75  add         ecx,dword ptr [r12+8]  
//00007FF764B15B7A  add         r9d,4A7484AAh  
//00007FF764B15B81  add         ecx,r8d  
//00007FF764B15B84  mov         eax,r14d  
//00007FF764B15B87  add         r9d,ecx  
//00007FF764B15B8A  and         eax,ebp  
//00007FF764B15B8C  add         ebx,r9d  
//00007FF764B15B8F  mov         r8d,r14d  
//00007FF764B15B92  or          r8d,ebp  
//00007FF764B15B95  mov         dword ptr [r12+18h],ebx  
//00007FF764B15B9A  and         r8d,edi  
//00007FF764B15B9D  mov         ebp,edi  
//00007FF764B15B9F  or          r8d,eax  
//00007FF764B15BA2  ror         ebp,2  
//00007FF764B15BA5  mov         eax,edi  
//00007FF764B15BA7  mov         edx,edi  
//00007FF764B15BA9  ror         eax,0Dh  
//    round<22>(pstate, pbuffer);
//00007FF764B15BAC  mov         ecx,ebx  
//00007FF764B15BAE  xor         ebp,eax  
//00007FF764B15BB0  ror         ecx,6  
//00007FF764B15BB3  ror         edx,16h  
//00007FF764B15BB6  mov         eax,ebx  
//00007FF764B15BB8  xor         ebp,edx  
//00007FF764B15BBA  ror         eax,0Bh  
//00007FF764B15BBD  xor         ecx,eax  
//00007FF764B15BBF  add         ebp,r8d  
//00007FF764B15BC2  add         ebp,r9d  
//00007FF764B15BC5  mov         r8d,r15d  
//00007FF764B15BC8  xor         r8d,r11d  
//00007FF764B15BCB  mov         dword ptr [r12+8],ebp 
//00007FF764B15BD0  mov         r9d,dword ptr [r13+58h]  
//00007FF764B15BD4  and         r8d,ebx  
//00007FF764B15BD7  mov         r10d,dword ptr [r12+14h]  
//00007FF764B15BDC  xor         r8d,r15d  
//00007FF764B15BDF  add         r9d,5CB0A9DCh  
//00007FF764B15BE6  mov         edx,ebx  
//00007FF764B15BE8  ror         edx,19h  
//00007FF764B15BEB  mov         eax,edi  
//00007FF764B15BED  xor         ecx,edx  
//00007FF764B15BEF  and         eax,r14d  
//00007FF764B15BF2  add         ecx,dword ptr [r12+4]  
//00007FF764B15BF7  mov         esi,ebp  
//00007FF764B15BF9  add         ecx,r8d  
//00007FF764B15BFC  ror         esi,2  
//00007FF764B15BFF  add         r9d,ecx  
//00007FF764B15C02  mov         r8d,edi  
//00007FF764B15C05  add         r10d,r9d  
//00007FF764B15C08  or          r8d,r14d  
//00007FF764B15C0B  and         r8d,ebp  
//00007FF764B15C0E  mov         dword ptr [r12+14h],r10d  
//00007FF764B15C13  or          r8d,eax  
//00007FF764B15C16  mov         edx,ebp  
//00007FF764B15C18  ror         edx,16h  
//00007FF764B15C1B  mov         eax,ebp  
//00007FF764B15C1D  ror         eax,0Dh  
//    round<23>(pstate, pbuffer);
//00007FF764B15C20  mov         ecx,r10d  
//00007FF764B15C23  xor         esi,eax  
//00007FF764B15C25  ror         ecx,6  
//00007FF764B15C28  xor         esi,edx  
//00007FF764B15C2A  mov         eax,r10d  
//00007FF764B15C2D  add         esi,r8d  
//00007FF764B15C30  ror         eax,0Bh  
//00007FF764B15C33  add         esi,r9d  
//00007FF764B15C36  xor         ecx,eax  
//00007FF764B15C38  mov         dword ptr [r12+4],esi  
//00007FF764B15C3D  mov         r8d,ebx  
//00007FF764B15C40  mov         r9d,dword ptr [r13+5Ch]  
//00007FF764B15C44  xor         r8d,r11d  
//00007FF764B15C47  and         r8d,r10d  
//00007FF764B15C4A  add         r9d,76F988DAh  
//00007FF764B15C51  xor         r8d,r11d  
//00007FF764B15C54  mov         edx,r10d  
//00007FF764B15C57  mov         r11d,dword ptr [r12+10h]  
//00007FF764B15C5C  ror         edx,19h  
//00007FF764B15C5F  xor         ecx,edx  
//00007FF764B15C61  add         ecx,r15d  
//00007FF764B15C64  add         ecx,r8d  
//00007FF764B15C67  add         r9d,ecx  
//00007FF764B15C6A  add         r11d,r9d  
//00007FF764B15C6D  mov         dword ptr [r12+10h],r11d  
//00007FF764B15C72  mov         r8d,ebp  
//00007FF764B15C75  or          r8d,edi  
//00007FF764B15C78  mov         eax,ebp  
//00007FF764B15C7A  and         eax,edi  
//00007FF764B15C7C  and         r8d,esi  
//00007FF764B15C7F  or          r8d,eax  
//    round<24>(pstate, pbuffer);
//00007FF764B15C82  mov         edi,ebp  
//00007FF764B15C84  or          edi,esi  
//00007FF764B15C86  and         ebp,esi  
//00007FF764B15C88  mov         eax,esi  
//00007FF764B15C8A  mov         r14d,esi  
//00007FF764B15C8D  ror         eax,0Dh  
//00007FF764B15C90  mov         edx,esi  
//00007FF764B15C92  ror         edx,16h  
//00007FF764B15C95  ror         r14d,2  
//00007FF764B15C99  xor         r14d,eax  
//00007FF764B15C9C  mov         eax,r11d  
//00007FF764B15C9F  xor         r14d,edx  
//00007FF764B15CA2  ror         eax,0Bh  
//00007FF764B15CA5  add         r14d,r9d  
//00007FF764B15CA8  mov         edx,r11d  
//00007FF764B15CAB  add         r14d,r8d  
//00007FF764B15CAE  ror         edx,19h  
//00007FF764B15CB1  mov         dword ptr [r12],r14d  
//00007FF764B15CB5  mov         r8d,ebx  
//00007FF764B15CB8  xor         r8d,r10d  
//00007FF764B15CBB  and         edi,r14d  
//00007FF764B15CBE  and         r8d,r11d  
//00007FF764B15CC1  or          edi,ebp  
//00007FF764B15CC3  xor         r8d,ebx  
//00007FF764B15CC6  mov         r9d,r11d  
//00007FF764B15CC9  mov         ebx,dword ptr [r12+0Ch]  
//00007FF764B15CCE  mov         ecx,r14d  
//00007FF764B15CD1  ror         ecx,2  
//00007FF764B15CD4  add         ebx,983E5152h  
//00007FF764B15CDA  ror         r9d,6  
//00007FF764B15CDE  xor         r9d,eax  
//00007FF764B15CE1  mov         eax,r14d  
//00007FF764B15CE4  xor         r9d,edx  
//00007FF764B15CE7  ror         eax,0Dh  
//00007FF764B15CEA  add         r9d,dword ptr [r13+60h]  
//00007FF764B15CEE  xor         ecx,eax  
//00007FF764B15CF0  add         r9d,dword ptr [r12+1Ch]  
//00007FF764B15CF5  mov         edx,r14d  
//00007FF764B15CF8  add         r9d,r8d  
//00007FF764B15CFB  ror         edx,16h  
//00007FF764B15CFE  xor         ecx,edx  
//00007FF764B15D00  add         ebx,r9d  
//00007FF764B15D03  mov         dword ptr [r12+0Ch],ebx  
//00007FF764B15D08  add         r9d,983E5152h  
//00007FF764B15D0F  add         ecx,r9d  
//    round<25>(pstate, pbuffer);
//00007FF764B15D12  mov         eax,ebx  
//00007FF764B15D14  add         edi,ecx  
//00007FF764B15D16  ror         eax,0Bh  
//00007FF764B15D19  mov         dword ptr [r12+1Ch],edi  
//00007FF764B15D1E  mov         ecx,ebx  
//00007FF764B15D20  mov         r9d,dword ptr [r13+64h]  
//00007FF764B15D24  mov         r8d,r11d  
//00007FF764B15D27  xor         r8d,r10d  
//00007FF764B15D2A  ror         ecx,6  
//00007FF764B15D2D  xor         ecx,eax  
//00007FF764B15D2F  and         r8d,ebx  
//00007FF764B15D32  xor         r8d,r10d  
//00007FF764B15D35  mov         edx,ebx  
//00007FF764B15D37  mov         r10d,dword ptr [r12+8]  
//00007FF764B15D3C  mov         eax,r14d  
//00007FF764B15D3F  ror         edx,19h  
//00007FF764B15D42  add         r10d,0A831C66Dh  
//00007FF764B15D49  xor         ecx,edx  
//00007FF764B15D4B  and         eax,esi  
//00007FF764B15D4D  add         ecx,dword ptr [r12+18h]  
//00007FF764B15D52  mov         edx,edi  
//00007FF764B15D54  add         ecx,r8d  
//00007FF764B15D57  mov         r8d,r14d  
//00007FF764B15D5A  or          r8d,esi  
//00007FF764B15D5D  add         r9d,ecx  
//00007FF764B15D60  add         r10d,r9d  
//00007FF764B15D63  and         r8d,edi  
//00007FF764B15D66  or          r8d,eax  
//00007FF764B15D69  mov         dword ptr [r12+8],r10d  
//00007FF764B15D6E  ror         edx,16h  
//00007FF764B15D71  lea         esi,[r9-57CE3993h]  
//00007FF764B15D78  mov         eax,edi  
//00007FF764B15D7A  mov         ecx,edi  
//00007FF764B15D7C  ror         ecx,2  
//00007FF764B15D7F  ror         eax,0Dh  
//00007FF764B15D82  xor         ecx,eax  
//    round<26>(pstate, pbuffer);
//00007FF764B15D84  mov         eax,r10d  
//00007FF764B15D87  xor         ecx,edx  
//00007FF764B15D89  ror         eax,0Bh  
//00007FF764B15D8C  add         ecx,r8d  
//00007FF764B15D8F  mov         edx,r10d  
//00007FF764B15D92  add         esi,ecx  
//00007FF764B15D94  ror         edx,19h  
//00007FF764B15D97  mov         dword ptr [r12+18h],esi 
//00007FF764B15D9C  mov         r8d,r11d  
//00007FF764B15D9F  mov         r9d,dword ptr [r13+68h]  
//00007FF764B15DA3  xor         r8d,ebx  
//00007FF764B15DA6  and         r8d,r10d  
//00007FF764B15DA9  mov         ecx,r10d  
//00007FF764B15DAC  ror         ecx,6  
//00007FF764B15DAF  xor         r8d,r11d  
//00007FF764B15DB2  mov         r11d,dword ptr [r12+4]  
//00007FF764B15DB7  xor         ecx,eax  
//00007FF764B15DB9  xor         ecx,edx  
//00007FF764B15DBB  add         r11d,0B00327C8h  
//00007FF764B15DC2  add         ecx,dword ptr [r12+14h]  
//00007FF764B15DC7  mov         eax,edi  
//00007FF764B15DC9  add         ecx,r8d  
//00007FF764B15DCC  and         eax,r14d  
//00007FF764B15DCF  add         r9d,ecx  
//00007FF764B15DD2  mov         r8d,edi  
//00007FF764B15DD5  or          r8d,r14d  
//00007FF764B15DD8  add         r11d,r9d  
//00007FF764B15DDB  and         r8d,esi  
//00007FF764B15DDE  mov         dword ptr [r12+4],r11d  
//00007FF764B15DE3  or          r8d,eax  
//    round<27>(pstate, pbuffer);
//00007FF764B15DE6  add         r14d,0BF597FC7h  
//00007FF764B15DED  lea         ebp,[r9-4FFCD838h]  
//00007FF764B15DF4  mov         eax,esi  
//00007FF764B15DF6  ror         eax,0Dh  
//00007FF764B15DF9  mov         ecx,esi  
//00007FF764B15DFB  ror         ecx,2  
//00007FF764B15DFE  mov         edx,esi  
//00007FF764B15E00  xor         ecx,eax  
//00007FF764B15E02  ror         edx,16h  
//00007FF764B15E05  xor         ecx,edx  
//00007FF764B15E07  mov         eax,r11d  
//00007FF764B15E0A  add         ecx,r8d  
//00007FF764B15E0D  ror         eax,0Bh  
//00007FF764B15E10  add         ebp,ecx  
//00007FF764B15E12  mov         r8d,ebx  
//00007FF764B15E15  xor         r8d,r10d  
//00007FF764B15E18  mov         dword ptr [r12+14h],ebp  
//00007FF764B15E1D  mov         r9d,dword ptr [r13+6Ch]  
//00007FF764B15E21  and         r8d,r11d  
//00007FF764B15E24  xor         r8d,ebx  
//00007FF764B15E27  mov         ecx,r11d  
//00007FF764B15E2A  ror         ecx,6  
//00007FF764B15E2D  mov         edx,r11d  
//00007FF764B15E30  xor         ecx,eax  
//00007FF764B15E32  ror         edx,19h  
//00007FF764B15E35  xor         ecx,edx  
//00007FF764B15E37  mov         eax,esi  
//00007FF764B15E39  add         ecx,dword ptr [r12+10h]  
//00007FF764B15E3E  and         eax,edi  
//00007FF764B15E40  add         ecx,r8d  
//00007FF764B15E43  mov         edx,ebp  
//00007FF764B15E45  add         r9d,ecx  
//00007FF764B15E48  ror         edx,16h  
//00007FF764B15E4B  add         r14d,r9d  
//00007FF764B15E4E  mov         r8d,esi  
//00007FF764B15E51  or          r8d,edi  
//00007FF764B15E54  mov         dword ptr [r12],r14d  
//00007FF764B15E58  and         r8d,ebp  
//00007FF764B15E5B  mov         ecx,ebp  
//00007FF764B15E5D  or          r8d,eax  
//00007FF764B15E60  ror         ecx,2  
//00007FF764B15E63  mov         eax,ebp  
//00007FF764B15E65  ror         eax,0Dh  
//00007FF764B15E68  xor         ecx,eax  
//00007FF764B15E6A  xor         ecx,edx  
//00007FF764B15E6C  lea         ebx,[r9-40A68039h]  
//00007FF764B15E73  add         ecx,r8d  
//    round<28>(pstate, pbuffer);
//00007FF764B15E76  mov         eax,r14d  
//00007FF764B15E79  add         ebx,ecx  
//00007FF764B15E7B  ror         eax,0Bh  
//00007FF764B15E7E  mov         dword ptr [r12+10h],ebx  
//00007FF764B15E83  mov         r8d,r10d  
//00007FF764B15E86  mov         r9d,dword ptr [r13+70h]  
//00007FF764B15E8A  xor         r8d,r11d  
//00007FF764B15E8D  and         r8d,r14d  
//00007FF764B15E90  mov         ecx,r14d  
//00007FF764B15E93  ror         ecx,6  
//00007FF764B15E96  xor         r8d,r10d  
//00007FF764B15E99  mov         r10d,dword ptr [r12+1Ch]  
//00007FF764B15E9E  xor         ecx,eax  
//00007FF764B15EA0  mov         eax,ebp  
//00007FF764B15EA2  add         r10d,0C6E00BF3h  
//00007FF764B15EA9  and         eax,esi  
//00007FF764B15EAB  mov         edx,r14d  
//00007FF764B15EAE  ror         edx,19h  
//00007FF764B15EB1  xor         ecx,edx  
//00007FF764B15EB3  mov         edx,ebx  
//00007FF764B15EB5  add         ecx,dword ptr [r12+0Ch]  
//00007FF764B15EBA  add         ecx,r8d  
//00007FF764B15EBD  ror         edx,16h  
//00007FF764B15EC0  add         r9d,ecx  
//00007FF764B15EC3  mov         r8d,ebp  
//00007FF764B15EC6  or          r8d,esi  
//00007FF764B15EC9  add         r10d,r9d  
//00007FF764B15ECC  and         r8d,ebx  
//00007FF764B15ECF  mov         dword ptr [r12+1Ch],r10d  
//00007FF764B15ED4  or          r8d,eax  
//00007FF764B15ED7  mov         ecx,ebx  
//00007FF764B15ED9  ror         ecx,2  
//00007FF764B15EDC  lea         edi,[r9-391FF40Dh]  
//00007FF764B15EE3  mov         eax,ebx  
//00007FF764B15EE5  ror         eax,0Dh  
//00007FF764B15EE8  xor         ecx,eax  
//    round<29>(pstate, pbuffer);
//00007FF764B15EEA  mov         eax,r10d  
//00007FF764B15EED  xor         ecx,edx  
//00007FF764B15EEF  ror         eax,0Bh  
//00007FF764B15EF2  add         ecx,r8d  
//00007FF764B15EF5  mov         edx,r10d  
//00007FF764B15EF8  add         edi,ecx  
//00007FF764B15EFA  ror         edx,19h  
//00007FF764B15EFD  mov         dword ptr [r12+0Ch],edi  
//00007FF764B15F02  mov         ecx,r10d  
//00007FF764B15F05  mov         r9d,dword ptr [r13+74h]  
//00007FF764B15F09  mov         r8d,r14d  
//00007FF764B15F0C  ror         ecx,6  
//00007FF764B15F0F  xor         r8d,r11d  
//00007FF764B15F12  xor         ecx,eax  
//00007FF764B15F14  and         r8d,r10d  
//00007FF764B15F17  xor         ecx,edx  
//00007FF764B15F19  xor         r8d,r11d  
//00007FF764B15F1C  add         ecx,dword ptr [r12+8]  
//00007FF764B15F21  mov         eax,ebx  
//00007FF764B15F23  mov         r11d,dword ptr [r12+18h]  
//00007FF764B15F28  add         ecx,r8d  
//00007FF764B15F2B  add         r9d,ecx  
//00007FF764B15F2E  and         eax,ebp  
//00007FF764B15F30  add         r11d,0D5A79147h  
//00007FF764B15F37  mov         r8d,ebx  
//00007FF764B15F3A  or          r8d,ebp  
//00007FF764B15F3D  add         r11d,r9d  
//00007FF764B15F40  and         r8d,edi  
//00007FF764B15F43  mov         dword ptr [r12+18h],r11d  
//00007FF764B15F48  or          r8d,eax  
//00007FF764B15F4B  lea         ebp,[r9-2A586EB9h]  
//00007FF764B15F52  mov         eax,edi  
//00007FF764B15F54  mov         ecx,edi  
//00007FF764B15F56  ror         ecx,2  
//00007FF764B15F59  mov         edx,edi  
//00007FF764B15F5B  ror         eax,0Dh  
//00007FF764B15F5E  xor         ecx,eax  
//00007FF764B15F60  ror         edx,16h  
//00007FF764B15F63  xor         ecx,edx  
//00007FF764B15F65  add         ecx,r8d  
//00007FF764B15F68  add         ebp,ecx  
//00007FF764B15F6A  mov         dword ptr [r12+8],ebp  
//    round<30>(pstate, pbuffer);
//00007FF764B15F6F  mov         esi,dword ptr [r12+14h]  
//00007FF764B15F74  mov         eax,r11d  
//00007FF764B15F77  mov         r9d,dword ptr [r13+78h]  
//00007FF764B15F7B  mov         ecx,r11d  
//00007FF764B15F7E  ror         eax,0Bh  
//00007FF764B15F81  add         r9d,6CA6351h  
//00007FF764B15F88  ror         ecx,6  
//00007FF764B15F8B  mov         r8d,r10d  
//00007FF764B15F8E  xor         ecx,eax  
//00007FF764B15F90  xor         r8d,r14d  
//00007FF764B15F93  and         r8d,r11d  
//00007FF764B15F96  mov         eax,edi  
//00007FF764B15F98  xor         r8d,r14d  
//00007FF764B15F9B  and         eax,ebx  
//00007FF764B15F9D  mov         edx,r11d  
//00007FF764B15FA0  mov         r15d,ebp  
//00007FF764B15FA3  ror         edx,19h  
//00007FF764B15FA6  xor         ecx,edx  
//00007FF764B15FA8  ror         r15d,2  
//00007FF764B15FAC  add         ecx,dword ptr [r12+4]  
//00007FF764B15FB1  mov         edx,ebp  
//00007FF764B15FB3  add         ecx,r8d  
//00007FF764B15FB6  ror         edx,16h  
//00007FF764B15FB9  add         r9d,ecx  
//00007FF764B15FBC  mov         r8d,edi  
//00007FF764B15FBF  or          r8d,ebx  
//00007FF764B15FC2  add         esi,r9d  
//00007FF764B15FC5  and         r8d,ebp  
//00007FF764B15FC8  mov         dword ptr [r12+14h],esi  
//00007FF764B15FCD  or          r8d,eax  
//    round<31>(pstate, pbuffer);
//00007FF764B15FD0  mov         ecx,esi  
//00007FF764B15FD2  ror         ecx,6  
//00007FF764B15FD5  mov         eax,ebp  
//00007FF764B15FD7  ror         eax,0Dh  
//00007FF764B15FDA  xor         r15d,eax  
//00007FF764B15FDD  mov         eax,esi  
//00007FF764B15FDF  xor         r15d,edx  
//00007FF764B15FE2  ror         eax,0Bh  
//00007FF764B15FE5  add         r15d,r8d  
//00007FF764B15FE8  xor         ecx,eax  
//00007FF764B15FEA  add         r15d,r9d  
//00007FF764B15FED  mov         eax,ebp  
//00007FF764B15FEF  mov         dword ptr [r12+4],r15d  
//00007FF764B15FF4  and         eax,edi  
//00007FF764B15FF6  mov         r9d,dword ptr [r13+7Ch]  
//00007FF764B15FFA  mov         r8d,r10d  
//00007FF764B15FFD  xor         r8d,r11d  
//00007FF764B16000  add         r9d,14292967h  
//00007FF764B16007  and         r8d,esi  
//00007FF764B1600A  mov         edx,esi  
//00007FF764B1600C  xor         r8d,r10d  
//00007FF764B1600F  ror         edx,19h  
//00007FF764B16012  xor         ecx,edx  
//00007FF764B16014  mov         edx,r15d  
//00007FF764B16017  add         ecx,r14d  
//00007FF764B1601A  ror         edx,16h  
//00007FF764B1601D  mov         r14d,dword ptr [r12+10h]  
//00007FF764B16022  add         ecx,r8d  
//00007FF764B16025  add         r9d,ecx  
//00007FF764B16028  mov         r8d,ebp  
//00007FF764B1602B  or          r8d,edi  
//00007FF764B1602E  add         r14d,r9d  
//00007FF764B16031  mov         dword ptr [r12+10h],r14d  
//00007FF764B16036  and         r8d,r15d  
//00007FF764B16039  or          r8d,eax  
//00007FF764B1603C  mov         r12d,r15d  
//00007FF764B1603F  ror         r12d,2  
//00007FF764B16043  mov         eax,r15d  
//00007FF764B16046  ror         eax,0Dh  
//00007FF764B16049  xor         r12d,eax  
//00007FF764B1604C  mov         rax,qword ptr [rsp+20h]  
//00007FF764B16051  xor         r12d,edx  
//    round<32>(pstate, pbuffer);
//00007FF764B16054  mov         edx,r14d  
//00007FF764B16057  add         r12d,r9d  
//00007FF764B1605A  add         r12d,r8d  
//00007FF764B1605D  mov         r8d,r11d  
//00007FF764B16060  xor         r8d,esi  
//00007FF764B16063  mov         dword ptr [rax],r12d  
//00007FF764B16066  mov         r9d,dword ptr [r13+80h]  
//00007FF764B1606D  and         r8d,r14d  
//00007FF764B16070  xor         r8d,r11d  
//00007FF764B16073  mov         r11,qword ptr [rsp+20h]  
//00007FF764B16078  add         r9d,27B70A85h  
//00007FF764B1607F  ror         edx,19h  
//00007FF764B16082  mov         eax,r14d  
//00007FF764B16085  ror         eax,0Bh  
//00007FF764B16088  mov         ecx,r14d  
//00007FF764B1608B  ror         ecx,6  
//00007FF764B1608E  mov         edi,r12d  
//00007FF764B16091  mov         ebx,dword ptr [r11+0Ch]  
//00007FF764B16095  xor         ecx,eax  
//00007FF764B16097  xor         ecx,edx  
//00007FF764B16099  ror         edi,2  
//00007FF764B1609C  add         ecx,dword ptr [r11+1Ch]  
//00007FF764B160A0  mov         eax,r15d  
//00007FF764B160A3  add         ecx,r8d  
//00007FF764B160A6  and         eax,ebp  
//00007FF764B160A8  add         r9d,ecx  
//00007FF764B160AB  mov         r8d,r15d  
//00007FF764B160AE  or          r8d,ebp  
//00007FF764B160B1  add         ebx,r9d  
//00007FF764B160B4  and         r8d,r12d  
//00007FF764B160B7  mov         dword ptr [r11+0Ch],ebx  
//00007FF764B160BB  or          r8d,eax  
//00007FF764B160BE  mov         edx,r12d  
//00007FF764B160C1  ror         edx,16h  
//00007FF764B160C4  mov         eax,r12d  
//00007FF764B160C7  ror         eax,0Dh  
//    round<33>(pstate, pbuffer);
//00007FF764B160CA  mov         ecx,ebx  
//00007FF764B160CC  xor         edi,eax  
//00007FF764B160CE  ror         ecx,6  
//00007FF764B160D1  xor         edi,edx  
//00007FF764B160D3  mov         eax,ebx  
//00007FF764B160D5  ror         eax,0Bh  
//00007FF764B160D8  add         edi,r9d  
//00007FF764B160DB  add         edi,r8d  
//00007FF764B160DE  xor         ecx,eax  
//00007FF764B160E0  mov         dword ptr [r11+1Ch],edi  
//00007FF764B160E4  mov         r8d,r14d  
//00007FF764B160E7  mov         r9d,dword ptr [r13+84h]  
//00007FF764B160EE  xor         r8d,esi  
//00007FF764B160F1  mov         r10d,dword ptr [r11+8]  
//00007FF764B160F5  and         r8d,ebx  
//00007FF764B160F8  xor         r8d,esi  
//00007FF764B160FB  add         r9d,2E1B2138h  
//00007FF764B16102  mov         edx,ebx  
//00007FF764B16104  mov         eax,r12d  
//00007FF764B16107  ror         edx,19h  
//00007FF764B1610A  and         eax,r15d  
//00007FF764B1610D  xor         ecx,edx  
//00007FF764B1610F  mov         esi,edi  
//00007FF764B16111  add         ecx,dword ptr [r11+18h]  
//00007FF764B16115  mov         edx,edi  
//00007FF764B16117  add         ecx,r8d  
//00007FF764B1611A  ror         esi,2  
//00007FF764B1611D  add         r9d,ecx  
//00007FF764B16120  ror         edx,16h  
//00007FF764B16123  add         r10d,r9d  
//00007FF764B16126  mov         r8d,r12d  
//00007FF764B16129  or          r8d,r15d  
//00007FF764B1612C  mov         dword ptr [r11+8],r10d  
//00007FF764B16130  and         r8d,edi  
//    round<34>(pstate, pbuffer);
//00007FF764B16133  mov         ecx,r10d  
//00007FF764B16136  or          r8d,eax  
//00007FF764B16139  ror         ecx,6  
//00007FF764B1613C  mov         eax,edi  
//00007FF764B1613E  ror         eax,0Dh  
//00007FF764B16141  xor         esi,eax  
//00007FF764B16143  mov         eax,r10d  
//00007FF764B16146  xor         esi,edx  
//00007FF764B16148  ror         eax,0Bh  
//00007FF764B1614B  add         esi,r8d  
//00007FF764B1614E  mov         edx,r10d  
//00007FF764B16151  add         esi,r9d  
//00007FF764B16154  ror         edx,19h  
//00007FF764B16157  mov         r8d,ebx  
//00007FF764B1615A  mov         dword ptr [r11+18h],esi  
//00007FF764B1615E  mov         r9d,dword ptr [r13+88h]  
//00007FF764B16165  xor         r8d,r14d  
//00007FF764B16168  and         r8d,r10d  
//00007FF764B1616B  xor         r8d,r14d  
//00007FF764B1616E  xor         ecx,eax  
//00007FF764B16170  mov         r14,qword ptr [rsp+20h]  
//00007FF764B16175  xor         ecx,edx  
//00007FF764B16177  add         ecx,dword ptr [r11+14h]  
//00007FF764B1617B  mov         eax,edi  
//00007FF764B1617D  mov         r11d,dword ptr [r11+4]  
//00007FF764B16181  add         ecx,r8d  
//00007FF764B16184  and         eax,r12d  
//00007FF764B16187  mov         r8d,edi  
//00007FF764B1618A  or          r8d,r12d  
//00007FF764B1618D  add         r9d,4D2C6DFCh  
//00007FF764B16194  add         r9d,ecx  
//00007FF764B16197  and         r8d,esi  
//00007FF764B1619A  or          r8d,eax  
//00007FF764B1619D  add         r11d,r9d  
//00007FF764B161A0  mov         dword ptr [r14+4],r11d  
//00007FF764B161A4  mov         eax,esi  
//00007FF764B161A6  ror         eax,0Dh  
//00007FF764B161A9  mov         ebp,esi  
//00007FF764B161AB  ror         ebp,2  
//00007FF764B161AE  mov         edx,esi  
//00007FF764B161B0  xor         ebp,eax  
//00007FF764B161B2  ror         edx,16h  
//00007FF764B161B5  xor         ebp,edx  
//    round<35>(pstate, pbuffer);
//00007FF764B161B7  mov         eax,r11d  
//00007FF764B161BA  add         ebp,r8d  
//00007FF764B161BD  ror         eax,0Bh  
//00007FF764B161C0  add         ebp,r9d  
//00007FF764B161C3  mov         ecx,r11d  
//00007FF764B161C6  ror         ecx,6  
//00007FF764B161C9  mov         r8d,r10d  
//00007FF764B161CC  xor         ecx,eax  
//00007FF764B161CE  mov         dword ptr [r14+14h],ebp  
//00007FF764B161D2  mov         r9d,dword ptr [r13+8Ch]  
//00007FF764B161D9  xor         r8d,ebx  
//00007FF764B161DC  and         r8d,r11d  
//00007FF764B161DF  add         r9d,53380D13h  
//00007FF764B161E6  xor         r8d,ebx  
//00007FF764B161E9  mov         edx,r11d  
//00007FF764B161EC  ror         edx,19h  
//00007FF764B161EF  mov         eax,esi  
//00007FF764B161F1  xor         ecx,edx  
//00007FF764B161F3  and         eax,edi  
//00007FF764B161F5  add         ecx,dword ptr [r14+10h]  
//00007FF764B161F9  mov         edx,ebp  
//00007FF764B161FB  add         ecx,r8d  
//00007FF764B161FE  ror         edx,16h  
//00007FF764B16201  add         r9d,ecx  
//00007FF764B16204  mov         r8d,esi  
//00007FF764B16207  or          r8d,edi  
//00007FF764B1620A  mov         edi,ebp  
//00007FF764B1620C  and         r8d,ebp  
//00007FF764B1620F  ror         edi,2  
//00007FF764B16212  or          r8d,eax  
//00007FF764B16215  mov         eax,ebp  
//00007FF764B16217  lea         r15d,[r12+r9]  
//00007FF764B1621B  ror         eax,0Dh  
//00007FF764B1621E  xor         edi,eax  
//00007FF764B16220  mov         dword ptr [r14],r15d  
//00007FF764B16223  xor         edi,edx  
//    round<36>(pstate, pbuffer);
//00007FF764B16225  mov         eax,r15d  
//00007FF764B16228  add         edi,r8d  
//00007FF764B1622B  ror         eax,0Bh  
//00007FF764B1622E  add         edi,r9d  
//00007FF764B16231  mov         ecx,r15d  
//00007FF764B16234  ror         ecx,6  
//00007FF764B16237  mov         r8d,r10d  
//00007FF764B1623A  xor         ecx,eax  
//00007FF764B1623C  mov         dword ptr [r14+10h],edi 
//00007FF764B16240  mov         r9d,dword ptr [r13+90h]  
//00007FF764B16247  xor         r8d,r11d  
//00007FF764B1624A  mov         ebx,dword ptr [r14+1Ch]  
//00007FF764B1624E  and         r8d,r15d  
//00007FF764B16251  xor         r8d,r10d  
//00007FF764B16254  mov         edx,r15d  
//00007FF764B16257  ror         edx,19h  
//00007FF764B1625A  add         r9d,650A7354h  
//00007FF764B16261  xor         ecx,edx  
//00007FF764B16263  mov         r12,r14  
//00007FF764B16266  add         ecx,dword ptr [r14+0Ch]  
//00007FF764B1626A  add         ecx,r8d  
//00007FF764B1626D  add         r9d,ecx  
//00007FF764B16270  add         ebx,r9d  
//00007FF764B16273  mov         r8d,ebp  
//00007FF764B16276  or          r8d,esi  
//00007FF764B16279  mov         dword ptr [r14+1Ch],ebx  
//00007FF764B1627D  and         r8d,edi  
//00007FF764B16280  mov         eax,ebp  
//00007FF764B16282  and         eax,esi  
//00007FF764B16284  mov         r14d,edi  
//00007FF764B16287  or          r8d,eax  
//00007FF764B1628A  ror         r14d,2  
//00007FF764B1628E  mov         eax,edi  
//00007FF764B16290  mov         edx,edi  
//00007FF764B16292  ror         eax,0Dh  
//    round<37>(pstate, pbuffer);
//00007FF764B16295  mov         ecx,ebx  
//00007FF764B16297  xor         r14d,eax  
//00007FF764B1629A  ror         ecx,6  
//00007FF764B1629D  ror         edx,16h  
//00007FF764B162A0  mov         eax,ebx  
//00007FF764B162A2  xor         r14d,edx
//00007FF764B162A5  ror         eax,0Bh  
//00007FF764B162A8  add         r14d,r8d  
//00007FF764B162AB  xor         ecx,eax  
//00007FF764B162AD  add         r14d,r9d  
//00007FF764B162B0  mov         r8d,r11d  
//00007FF764B162B3  xor         r8d,r15d 
//00007FF764B162B6  mov         dword ptr [r12+0Ch],r14d  
//00007FF764B162BB  mov         r9d,dword ptr [r13+94h]  
//00007FF764B162C2  and         r8d,ebx  
//00007FF764B162C5  xor         r8d,r11d  
//00007FF764B162C8  add         r9d,766A0ABBh  
//00007FF764B162CF  mov         r11d,dword ptr [r12+18h]  
//00007FF764B162D4  mov         edx,ebx  
//00007FF764B162D6  ror         edx,19h  
//00007FF764B162D9  mov         eax,edi  
//00007FF764B162DB  xor         ecx,edx  
//00007FF764B162DD  and         eax,ebp  
//00007FF764B162DF  add         ecx,dword ptr [r12+8]  
//00007FF764B162E4  mov         esi,r14d  
//00007FF764B162E7  add         ecx,r8d  
//00007FF764B162EA  ror         esi,2  
//00007FF764B162ED  add         r9d,ecx  
//00007FF764B162F0  mov         r8d,edi  
//00007FF764B162F3  or          r8d,ebp  
//00007FF764B162F6  add         r11d,r9d  
//00007FF764B162F9  and         r8d,r14d  
//00007FF764B162FC  mov         dword ptr [r12+18h],r11d  
//00007FF764B16301  or          r8d,eax  
//00007FF764B16304  mov         edx,r14d  
//00007FF764B16307  ror         edx,16h  
//00007FF764B1630A  mov         eax,r14d  
//00007FF764B1630D  ror         eax,0Dh  
//    round<38>(pstate, pbuffer);
//00007FF764B16310  mov         ecx,r11d  
//00007FF764B16313  xor         esi,eax  
//00007FF764B16315  ror         ecx,6  
//00007FF764B16318  xor         esi,edx  
//00007FF764B1631A  mov         eax,r11d  
//00007FF764B1631D  add         esi,r9d  
//00007FF764B16320  ror         eax,0Bh  
//00007FF764B16323  add         esi,r8d  
//00007FF764B16326  xor         ecx,eax  
//00007FF764B16328  mov         dword ptr [r12+8],esi  
//00007FF764B1632D  mov         r8d,ebx  
//00007FF764B16330  mov         r9d,dword ptr [r13+98h]  
//00007FF764B16337  xor         r8d,r15d  
//00007FF764B1633A  mov         r10d,dword ptr [r12+14h]  
//00007FF764B1633F  and         r8d,r11d  
//00007FF764B16342  xor         r8d,r15d  
//00007FF764B16345  add         r10d,81C2C92Eh  
//00007FF764B1634C  mov         edx,r11d  
//00007FF764B1634F  mov         eax,r14d  
//00007FF764B16352  ror         edx,19h  
//00007FF764B16355  xor         ecx,edx  
//00007FF764B16357  add         ecx,dword ptr [r12+4]  
//00007FF764B1635C  add         ecx,r8d  
//00007FF764B1635F  mov         r8d,r14d  
//00007FF764B16362  or          r8d,edi  
//00007FF764B16365  add         r9d,ecx  
//00007FF764B16368  add         r10d,r9d  
//00007FF764B1636B  and         r8d,esi  
//00007FF764B1636E  and         eax,edi  
//00007FF764B16370  mov         dword ptr [r12+14h],r10d  
//00007FF764B16375  or          r8d,eax  
//00007FF764B16378  lea         ebp,[r9-7E3D36D2h]  
//00007FF764B1637F  mov         eax,esi  
//00007FF764B16381  mov         ecx,esi  
//00007FF764B16383  ror         ecx,2  
//00007FF764B16386  mov         edx,esi  
//00007FF764B16388  ror         eax,0Dh  
//00007FF764B1638B  xor         ecx,eax  
//00007FF764B1638D  ror         edx,16h  
//00007FF764B16390  xor         ecx,edx  
//    round<39>(pstate, pbuffer);
//00007FF764B16392  mov         eax,r10d 
//00007FF764B16395  add         ecx,r8d  
//00007FF764B16398  ror         eax,0Bh  
//00007FF764B1639B  add         ebp,ecx 
//00007FF764B1639D  mov         r8d,ebx  
//00007FF764B163A0  xor         r8d,r11d  
//00007FF764B163A3  mov         dword ptr [r12+4],ebp  
//00007FF764B163A8  mov         r9d,dword ptr [r13+9Ch]  
//00007FF764B163AF  and         r8d,r10d  
//00007FF764B163B2  mov         edi,dword ptr [r12+10h]  
//00007FF764B163B7  xor         r8d,ebx  
//00007FF764B163BA  add         edi,92722C85h  
//00007FF764B163C0  mov         ecx,r10d  
//00007FF764B163C3  ror         ecx,6  
//00007FF764B163C6  mov         edx,r10d  
//00007FF764B163C9  xor         ecx,eax  
//00007FF764B163CB  ror         edx,19h  
//00007FF764B163CE  xor         ecx,edx  
//00007FF764B163D0  mov         eax,esi  
//00007FF764B163D2  add         ecx,r15d  
//00007FF764B163D5  and         eax,r14d  
//00007FF764B163D8  add         ecx,r8d  
//00007FF764B163DB  mov         edx,ebp  
//00007FF764B163DD  add         r9d,ecx  
//00007FF764B163E0  ror         edx,16h  
//00007FF764B163E3  add         edi,r9d  
//00007FF764B163E6  mov         r8d,esi  
//00007FF764B163E9  or          r8d,r14d  
//00007FF764B163EC  mov         dword ptr [r12+10h],edi  
//00007FF764B163F1  and         r8d,ebp  
//00007FF764B163F4  mov         ecx,ebp  
//00007FF764B163F6  or          r8d,eax  
//00007FF764B163F9  ror         ecx,2  
//00007FF764B163FC  lea         r14d,[r9-6D8DD37Bh]  
//00007FF764B16403  mov         eax,ebp  
//00007FF764B16405  ror         eax,0Dh  
//00007FF764B16408  xor         ecx,eax  
//    round<40>(pstate, pbuffer);
//00007FF764B1640A  mov         eax,edi  
//00007FF764B1640C  xor         ecx,edx  
//00007FF764B1640E  ror         eax,0Bh  
//00007FF764B16411  add         ecx,r8d  
//00007FF764B16414  mov         edx,edi  
//00007FF764B16416  add         r14d,ecx  
//00007FF764B16419  ror         edx,19h  
//00007FF764B1641C  mov         dword ptr [r12],r14d 
//00007FF764B16420  mov         ecx,edi  
//00007FF764B16422  mov         r9d,dword ptr [r13+0A0h]  
//00007FF764B16429  mov         r8d,r11d  
//00007FF764B1642C  mov         ebx,dword ptr [r12+0Ch]  
//00007FF764B16431  xor         r8d,r10d  
//00007FF764B16434  and         r8d,edi  
//00007FF764B16437  ror         ecx,6  
//00007FF764B1643A  xor         ecx,eax  
//00007FF764B1643C  xor         r8d,r11d  
//00007FF764B1643F  xor         ecx,edx  
//00007FF764B16441  add         ebx,0A2BFE8A1h  
//00007FF764B16447  add         ecx,dword ptr [r12+1Ch]  
//00007FF764B1644C  mov         eax,ebp  
//00007FF764B1644E  add         ecx,r8d  
//00007FF764B16451  and         eax,esi  
//00007FF764B16453  add         r9d,ecx  
//00007FF764B16456  mov         r8d,ebp  
//00007FF764B16459  or          r8d,esi  
//00007FF764B1645C  add         ebx,r9d  
//00007FF764B1645F  and         r8d,r14d  
//00007FF764B16462  mov         dword ptr [r12+0Ch],ebx  
//00007FF764B16467  or          r8d,eax  
//00007FF764B1646A  mov         edx,r14d  
//00007FF764B1646D  mov         eax,r14d  
//00007FF764B16470  ror         edx,16h  
//00007FF764B16473  ror         eax,0Dh  
//00007FF764B16476  lea         esi,[r9-5D40175Fh]  
//00007FF764B1647D  mov         ecx,r14d  
//00007FF764B16480  ror         ecx,2  
//00007FF764B16483  xor         ecx,eax  
//    round<41>(pstate, pbuffer);
//00007FF764B16485  mov         eax,ebx  
//00007FF764B16487  xor         ecx,edx  
//00007FF764B16489  ror         eax,0Bh  
//00007FF764B1648C  add         ecx,r8d  
//00007FF764B1648F  mov         edx,ebx  
//00007FF764B16491  add         esi,ecx  
//00007FF764B16493  ror         edx,19h  
//00007FF764B16496  mov         dword ptr [r12+1Ch],esi  
//00007FF764B1649B  mov         ecx,ebx  
//00007FF764B1649D  mov         r9d,dword ptr [r13+0A4h]  
//00007FF764B164A4  mov         r8d,edi  
//00007FF764B164A7  ror         ecx,6  
//00007FF764B164AA  xor         r8d,r10d  
//00007FF764B164AD  xor         ecx,eax  
//00007FF764B164AF  and         r8d,ebx  
//00007FF764B164B2  xor         ecx,edx  
//00007FF764B164B4  xor         r8d,r10d  
//00007FF764B164B7  add         ecx,dword ptr [r12+18h]  
//00007FF764B164BC  mov         eax,r14d  
//00007FF764B164BF  mov         r10d,dword ptr [r12+8]  
//00007FF764B164C4  add         ecx,r8d  
//00007FF764B164C7  add         r9d,ecx  
//00007FF764B164CA  and         eax,ebp  
//00007FF764B164CC  add         r10d,0A81A664Bh  
//00007FF764B164D3  mov         r8d,r14d  
//00007FF764B164D6  or          r8d,ebp  
//00007FF764B164D9  add         r10d,r9d  
//00007FF764B164DC  and         r8d,esi  
//00007FF764B164DF  mov         dword ptr [r12+8],r10d  
//00007FF764B164E4  or          r8d,eax  
//00007FF764B164E7  lea         ebp,[r9-57E599B5h]  
//00007FF764B164EE  mov         eax,esi  
//00007FF764B164F0  mov         ecx,esi  
//00007FF764B164F2  ror         ecx,2  
//00007FF764B164F5  mov         edx,esi  
//00007FF764B164F7  ror         eax,0Dh  
//00007FF764B164FA  xor         ecx,eax  
//00007FF764B164FC  ror         edx,16h  
//00007FF764B164FF  xor         ecx,edx  
//    round<42>(pstate, pbuffer);
//00007FF764B16501  mov         eax,r10d  
//00007FF764B16504  add         ecx,r8d  
//00007FF764B16507  ror         eax,0Bh  
//00007FF764B1650A  add         ebp,ecx  
//00007FF764B1650C  mov         r8d,ebx  
//00007FF764B1650F  xor         r8d,edi  
//00007FF764B16512  mov         dword ptr [r12+18h],ebp  
//00007FF764B16517  mov         r9d,dword ptr [r13+0A8h]  
//00007FF764B1651E  and         r8d,r10d  
//00007FF764B16521  mov         r11d,dword ptr [r12+4]  
//00007FF764B16526  xor         r8d,edi  
//00007FF764B16529  add         r11d,0C24B8B70h  
//00007FF764B16530  mov         ecx,r10d  
//00007FF764B16533  ror         ecx,6  
//00007FF764B16536  mov         edx,r10d  
//00007FF764B16539  xor         ecx,eax  
//00007FF764B1653B  ror         edx,19h  
//00007FF764B1653E  xor         ecx,edx  
//00007FF764B16540  mov         eax,esi  
//00007FF764B16542  add         ecx,dword ptr [r12+14h]  
//00007FF764B16547  and         eax,r14d  
//00007FF764B1654A  add         ecx,r8d  
//00007FF764B1654D  mov         edx,ebp  
//00007FF764B1654F  add         r9d,ecx  
//00007FF764B16552  ror         edx,16h  
//00007FF764B16555  add         r11d,r9d  
//00007FF764B16558  mov         r8d,esi  
//00007FF764B1655B  or          r8d,r14d  
//00007FF764B1655E  mov         dword ptr [r12+4],r11d  
//00007FF764B16563  and         r8d,ebp  
//00007FF764B16566  mov         ecx,ebp  
//00007FF764B16568  or          r8d,eax  
//00007FF764B1656B  ror         ecx,2  
//00007FF764B1656E  mov         eax,ebp  
//00007FF764B16570  ror         eax,0Dh  
//00007FF764B16573  xor         ecx,eax  
//00007FF764B16575  xor         ecx,edx  
//00007FF764B16577  add         ecx,r8d  
//00007FF764B1657A  lea         edi,[r9-3DB47490h]  
//    round<43>(pstate, pbuffer);
//00007FF764B16581  mov         r8d,r10d  
//00007FF764B16584  add         edi,ecx  
//00007FF764B16586  xor         r8d,ebx  
//00007FF764B16589  and         r8d,r11d  
//00007FF764B1658C  mov         dword ptr [r12+14h],edi  
//00007FF764B16591  mov         r9d,dword ptr [r13+0ACh]  
//00007FF764B16598  xor         r8d,ebx  
//00007FF764B1659B  add         r14d,0C76C51A3h  
//00007FF764B165A2  mov         ecx,r11d  
//00007FF764B165A5  ror         ecx,6  
//00007FF764B165A8  mov         eax,r11d  
//00007FF764B165AB  ror         eax,0Bh  
//00007FF764B165AE  mov         edx,r11d  
//00007FF764B165B1  xor         ecx,eax  
//00007FF764B165B3  ror         edx,19h  
//00007FF764B165B6  xor         ecx,edx  
//00007FF764B165B8  mov         eax,ebp  
//00007FF764B165BA  add         ecx,dword ptr [r12+10h]  
//00007FF764B165BF  and         eax,esi  
//00007FF764B165C1  add         ecx,r8d  
//00007FF764B165C4  mov         edx,edi  
//00007FF764B165C6  add         r9d,ecx  
//00007FF764B165C9  ror         edx,16h  
//00007FF764B165CC  add         r14d,r9d  
//00007FF764B165CF  mov         r8d,ebp  
//00007FF764B165D2  or          r8d,esi  
//00007FF764B165D5  mov         dword ptr [r12],r14d  
//00007FF764B165D9  and         r8d,edi  
//00007FF764B165DC  mov         ecx,edi  
//00007FF764B165DE  or          r8d,eax  
//00007FF764B165E1  ror         ecx,2  
//00007FF764B165E4  lea         esi,[r9-3893AE5Dh]  
//00007FF764B165EB  mov         eax,edi  
//00007FF764B165ED  ror         eax,0Dh  
//00007FF764B165F0  xor         ecx,eax  
//    round<44>(pstate, pbuffer);
//00007FF764B165F2  mov         eax,r14d  
//00007FF764B165F5  xor         ecx,edx  
//00007FF764B165F7  ror         eax,0Bh  
//00007FF764B165FA  add         ecx,r8d  
//00007FF764B165FD  mov         edx,r14d  
//00007FF764B16600  add         esi,ecx  
//00007FF764B16602  ror         edx,19h  
//00007FF764B16605  mov         dword ptr [r12+10h],esi  
//00007FF764B1660A  mov         ecx,r14d  
//00007FF764B1660D  mov         r9d,dword ptr [r13+0B0h]  
//00007FF764B16614  mov         r8d,r11d  
//00007FF764B16617  xor         r8d,r10d  
//00007FF764B1661A  ror         ecx,6  
//00007FF764B1661D  xor         ecx,eax  
//00007FF764B1661F  and         r8d,r14d  
//00007FF764B16622  xor         ecx,edx  
//00007FF764B16624  xor         r8d,r10d  
//00007FF764B16627  add         ecx,dword ptr [r12+0Ch]  
//00007FF764B1662C  mov         eax,edi  
//00007FF764B1662E  mov         r10d,dword ptr [r12+1Ch]  
//00007FF764B16633  add         ecx,r8d  
//00007FF764B16636  add         r9d,ecx  
//00007FF764B16639  and         eax,ebp  
//00007FF764B1663B  add         r10d,0D192E819h  
//00007FF764B16642  mov         r8d,edi  
//00007FF764B16645  or          r8d,ebp  
//00007FF764B16648  add         r10d,r9d  
//00007FF764B1664B  and         r8d,esi  
//00007FF764B1664E  mov         dword ptr [r12+1Ch],r10d  
//00007FF764B16653  or          r8d,eax  
//00007FF764B16656  lea         ebx,[r9-2E6D17E7h]  
//00007FF764B1665D  mov         eax,esi  
//00007FF764B1665F  mov         ecx,esi  
//00007FF764B16661  ror         ecx,2  
//00007FF764B16664  mov         edx,esi  
//00007FF764B16666  ror         eax,0Dh  
//00007FF764B16669  xor         ecx,eax  
//00007FF764B1666B  ror         edx,16h  
//00007FF764B1666E  xor         ecx,edx  
//00007FF764B16670  add         ecx,r8d  
//    round<45>(pstate, pbuffer);
//00007FF764B16673  mov         r8d,r14d  
//00007FF764B16676  add         ebx,ecx  
//00007FF764B16678  mov         dword ptr [r12+0Ch],ebx  
//00007FF764B1667D  xor         r8d,r11d  
//00007FF764B16680  mov         r9d,dword ptr [r13+0B4h]  
//00007FF764B16687  and         r8d,r10d  
//00007FF764B1668A  mov         eax,r10d  
//00007FF764B1668D  xor         r8d,r11d  
//00007FF764B16690  ror         eax,0Bh  
//00007FF764B16693  mov         r11d,dword ptr [r12+18h]  
//00007FF764B16698  mov         ecx,r10d  
//00007FF764B1669B  ror         ecx,6  
//00007FF764B1669E  add         r11d,0D6990624h  
//00007FF764B166A5  xor         ecx,eax  
//00007FF764B166A7  mov         edx,r10d  
//00007FF764B166AA  ror         edx,19h  
//00007FF764B166AD  mov         eax,esi  
//00007FF764B166AF  xor         ecx,edx  
//00007FF764B166B1  and         eax,edi  
//00007FF764B166B3  add         ecx,dword ptr [r12+8]  
//00007FF764B166B8  mov         edx,ebx  
//00007FF764B166BA  add         ecx,r8d  
//00007FF764B166BD  ror         edx,16h  
//00007FF764B166C0  add         r9d,ecx  
//00007FF764B166C3  mov         r8d,esi  
//00007FF764B166C6  or          r8d,edi  
//00007FF764B166C9  add         r11d,r9d  
//00007FF764B166CC  and         r8d,ebx  
//00007FF764B166CF  mov         dword ptr [r12+18h],r11d  
//00007FF764B166D4  or          r8d,eax  
//00007FF764B166D7  mov         ecx,ebx  
//00007FF764B166D9  ror         ecx,2  
//00007FF764B166DC  lea         ebp,[r9-2966F9DCh]  
//00007FF764B166E3  mov         eax,ebx  
//00007FF764B166E5  ror         eax,0Dh  
//00007FF764B166E8  xor         ecx,eax  
//    round<46>(pstate, pbuffer);
//00007FF764B166EA  mov         eax,r11d  
//00007FF764B166ED  xor         ecx,edx  
//00007FF764B166EF  ror         eax,0Bh  
//00007FF764B166F2  add         ecx,r8d  
//00007FF764B166F5  mov         edx,r11d  
//00007FF764B166F8  add         ebp,ecx  
//00007FF764B166FA  ror         edx,19h  
//00007FF764B166FD  mov         dword ptr [r12+8],ebp  
//00007FF764B16702  mov         ecx,r11d  
//00007FF764B16705  mov         r9d,dword ptr [r13+0B8h]  
//00007FF764B1670C  mov         r8d,r10d  
//00007FF764B1670F  mov         edi,dword ptr [r12+14h]  
//00007FF764B16714  xor         r8d,r14d  
//00007FF764B16717  ror         ecx,6  
//00007FF764B1671A  and         r8d,r11d  
//00007FF764B1671D  xor         ecx,eax  
//00007FF764B1671F  xor         r8d,r14d  
//00007FF764B16722  xor         ecx,edx  
//00007FF764B16724  add         edi,0F40E3585h  
//00007FF764B1672A  add         ecx,dword ptr [r12+4]  
//00007FF764B1672F  mov         eax,ebx  
//00007FF764B16731  add         ecx,r8d  
//00007FF764B16734  and         eax,esi  
//00007FF764B16736  add         r9d,ecx  
//00007FF764B16739  mov         r8d,ebx  
//00007FF764B1673C  or          r8d,esi  
//00007FF764B1673F  add         edi,r9d  
//00007FF764B16742  and         r8d,ebp  
//00007FF764B16745  mov         dword ptr [r12+14h],edi  
//00007FF764B1674A  or          r8d,eax  
//00007FF764B1674D  mov         ecx,ebp  
//00007FF764B1674F  ror         ecx,2  
//00007FF764B16752  lea         esi,[r9-0BF1CA7Bh]  
//00007FF764B16759  mov         eax,ebp  
//00007FF764B1675B  mov         edx,ebp  
//00007FF764B1675D  ror         eax,0Dh  
//00007FF764B16760  xor         ecx,eax  
//00007FF764B16762  ror         edx,16h  
//00007FF764B16765  xor         ecx,edx  
//    round<47>(pstate, pbuffer);
//00007FF764B16767  mov         eax,edi  
//00007FF764B16769  add         ecx,r8d  
//00007FF764B1676C  mov         edx,edi  
//00007FF764B1676E  add         esi,ecx  
//00007FF764B16770  mov         r8d,r11d  
//00007FF764B16773  xor         r8d,r10d  
//00007FF764B16776  mov         dword ptr [r12+4],esi  
//00007FF764B1677B  mov         r9d,dword ptr [r13+0BCh]  
//00007FF764B16782  and         r8d,edi  
//00007FF764B16785  xor         r8d,r10d  
//00007FF764B16788  ror         edx,19h  
//00007FF764B1678B  ror         eax,0Bh  
//00007FF764B1678E  add         r9d,106AA070h  
//00007FF764B16795  mov         ecx,edi  
//00007FF764B16797  mov         r15d,esi  
//00007FF764B1679A  ror         ecx,6  
//00007FF764B1679D  xor         ecx,eax  
//00007FF764B1679F  ror         r15d,16h  
//00007FF764B167A3  xor         ecx,edx  
//00007FF764B167A5  mov         eax,ebp  
//00007FF764B167A7  add         ecx,r14d  
//00007FF764B167AA  and         eax,ebx  
//00007FF764B167AC  mov         r14d,dword ptr [r12+10h]  
//00007FF764B167B1  add         ecx,r8d  
//00007FF764B167B4  add         r9d,ecx  
//00007FF764B167B7  mov         r8d,ebp  
//00007FF764B167BA  or          r8d,ebx  
//00007FF764B167BD  add         r14d,r9d  
//00007FF764B167C0  and         r8d,esi  
//00007FF764B167C3  mov         dword ptr [r12+10h],r14d  
//00007FF764B167C8  or          r8d,eax  
//00007FF764B167CB  mov         ecx,esi  
//00007FF764B167CD  ror         ecx,0Dh  
//00007FF764B167D0  mov         eax,esi  
//00007FF764B167D2  ror         eax,2  
//    round<48>(pstate, pbuffer);
//00007FF764B167D5  mov         edx,r14d  
//00007FF764B167D8  xor         r15d,eax  
//00007FF764B167DB  ror         edx,19h  
//00007FF764B167DE  xor         r15d,ecx  
//00007FF764B167E1  mov         eax,r14d  
//00007FF764B167E4  add         r15d,r8d  
//00007FF764B167E7  ror         eax,0Bh  
//00007FF764B167EA  add         r15d,r9d  
//00007FF764B167ED  mov         ecx,r14d  
//00007FF764B167F0  ror         ecx,6  
//00007FF764B167F3  mov         r8d,r11d  
//00007FF764B167F6  xor         ecx,eax  
//00007FF764B167F8  mov         dword ptr [r12],r15d  
//00007FF764B167FC  mov         r9d,dword ptr [r13+0C0h]  
//00007FF764B16803  xor         ecx,edx  
//00007FF764B16805  add         ecx,dword ptr [r12+1Ch]  
//00007FF764B1680A  xor         r8d,edi  
//00007FF764B1680D  mov         ebx,dword ptr [r12+0Ch]  
//00007FF764B16812  and         r8d,r14d  
//00007FF764B16815  xor         r8d,r11d  
//00007FF764B16818  add         r9d,19A4C116h  
//00007FF764B1681F  add         ecx,r8d  
//00007FF764B16822  mov         eax,esi  
//00007FF764B16824  and         eax,ebp  
//00007FF764B16826  add         r9d,ecx  
//00007FF764B16829  add         ebx,r9d  
//00007FF764B1682C  mov         r8d,esi  
//00007FF764B1682F  or          r8d,ebp  
//00007FF764B16832  mov         dword ptr [r12+0Ch],ebx  
//00007FF764B16837  and         r8d,r15d  
//00007FF764B1683A  mov         ebp,r15d  
//00007FF764B1683D  or          r8d,eax  
//00007FF764B16840  ror         ebp,2  
//00007FF764B16843  mov         eax,r15d  
//00007FF764B16846  mov         edx,r15d  
//00007FF764B16849  ror         eax,0Dh  
//    round<49>(pstate, pbuffer);
//00007FF764B1684C  mov         ecx,ebx  
//00007FF764B1684E  xor         ebp,eax  
//00007FF764B16850  ror         edx,16h  
//00007FF764B16853  xor         ebp,edx  
//00007FF764B16855  ror         ecx,6  
//00007FF764B16858  add         ebp,r9d  
//00007FF764B1685B  mov         eax,ebx  
//00007FF764B1685D  add         ebp,r8d  
//00007FF764B16860  ror         eax,0Bh  
//00007FF764B16863  xor         ecx,eax  
//00007FF764B16865  mov         dword ptr [r12+1Ch],ebp 
//00007FF764B1686A  mov         r9d,dword ptr [r13+0C4h]  
//00007FF764B16871  mov         r8d,r14d  
//00007FF764B16874  xor         r8d,edi  
//00007FF764B16877  mov         edx,ebx  
//00007FF764B16879  ror         edx,19h  
//00007FF764B1687C  and         r8d,ebx  
//00007FF764B1687F  xor         ecx,edx  
//00007FF764B16881  xor         r8d,edi  
//00007FF764B16884  add         ecx,dword ptr [r12+18h]  
//00007FF764B16889  add         ecx,r8d  
//00007FF764B1688C  mov         r10d,dword ptr [r12+8]  
//00007FF764B16891  add         r9d,1E376C08h  
//00007FF764B16898  add         r9d,ecx  
//00007FF764B1689B  mov         r8d,r15d  
//00007FF764B1689E  or          r8d,esi  
//00007FF764B168A1  add         r10d,r9d  
//00007FF764B168A4  and         r8d,ebp  
//00007FF764B168A7  mov         dword ptr [r12+8],r10d  
//00007FF764B168AC  mov         eax,r15d  
//00007FF764B168AF  mov         edi,ebp  
//00007FF764B168B1  and         eax,esi  
//00007FF764B168B3  ror         edi,2  
//00007FF764B168B6  or          r8d,eax  
//00007FF764B168B9  mov         edx,ebp  
//00007FF764B168BB  ror         edx,16h  
//00007FF764B168BE  mov         eax,ebp  
//00007FF764B168C0  ror         eax,0Dh  
//    round<50>(pstate, pbuffer);
//00007FF764B168C3  mov         ecx,r10d  
//00007FF764B168C6  xor         edi,eax  
//00007FF764B168C8  ror         ecx,6  
//00007FF764B168CB  xor         edi,edx  
//00007FF764B168CD  mov         eax,r10d  
//00007FF764B168D0  add         edi,r8d  
//00007FF764B168D3  ror         eax,0Bh  
//00007FF764B168D6  xor         ecx,eax  
//00007FF764B168D8  add         edi,r9d  
//00007FF764B168DB  mov         dword ptr [r12+18h],edi  
//00007FF764B168E0  mov         r8d,r14d  
//00007FF764B168E3  mov         r9d,dword ptr [r13+0C8h]  
//00007FF764B168EA  xor         r8d,ebx  
//00007FF764B168ED  mov         r11d,dword ptr [r12+4]  
//00007FF764B168F2  and         r8d,r10d  
//00007FF764B168F5  xor         r8d,r14d  
//00007FF764B168F8  add         r9d,2748774Ch  
//00007FF764B168FF  mov         edx,r10d  
//00007FF764B16902  mov         eax,ebp  
//00007FF764B16904  ror         edx,19h  
//00007FF764B16907  and         eax,r15d  
//00007FF764B1690A  xor         ecx,edx  
//00007FF764B1690C  mov         esi,edi  
//00007FF764B1690E  add         ecx,dword ptr [r12+14h]  
//00007FF764B16913  mov         edx,edi  
//00007FF764B16915  add         ecx,r8d  
//00007FF764B16918  ror         esi,2  
//00007FF764B1691B  add         r9d,ecx  
//00007FF764B1691E  ror         edx,16h  
//00007FF764B16921  add         r11d,r9d  
//00007FF764B16924  mov         r8d,ebp  
//00007FF764B16927  or          r8d,r15d  
//00007FF764B1692A  mov         dword ptr [r12+4],r11d  
//00007FF764B1692F  and         r8d,edi  
//    round<51>(pstate, pbuffer);
//00007FF764B16932  mov         ecx,r11d  
//00007FF764B16935  or          r8d,eax  
//00007FF764B16938  ror         ecx,6  
//00007FF764B1693B  mov         eax,edi  
//00007FF764B1693D  ror         eax,0Dh  
//00007FF764B16940  xor         esi,eax 
//00007FF764B16942  mov         eax,r11d  
//00007FF764B16945  xor         esi,edx  
//00007FF764B16947  ror         eax,0Bh 
//00007FF764B1694A  add         esi,r8d 
//00007FF764B1694D  xor         ecx,eax  
//00007FF764B1694F  add         esi,r9d  
//00007FF764B16952  mov         r8d,r10d  
//00007FF764B16955  xor         r8d,ebx  
//00007FF764B16958  mov         dword ptr [r12+14h],esi  
//00007FF764B1695D  mov         r9d,dword ptr [r13+0CCh]  
//00007FF764B16964  and         r8d,r11d  
//00007FF764B16967  xor         r8d,ebx  
//00007FF764B1696A  add         r9d,34B0BCB5h  
//00007FF764B16971  mov         edx,r11d  
//00007FF764B16974  ror         edx,19h  
//00007FF764B16977  xor         ecx,edx  
//00007FF764B16979  add         ecx,dword ptr [r12+10h]  
//00007FF764B1697E  add         ecx,r8d  
//00007FF764B16981  mov         r8d,edi  
//00007FF764B16984  add         r9d,ecx  
//00007FF764B16987  or          r8d,ebp  
//00007FF764B1698A  and         r8d,esi  
//00007FF764B1698D  lea         r14d,[r15+r9]  
//00007FF764B16991  mov         dword ptr [r12],r14d  
//00007FF764B16995  mov         eax,edi  
//00007FF764B16997  mov         edx,esi  
//00007FF764B16999  and         eax,ebp  
//00007FF764B1699B  ror         edx,16h  
//00007FF764B1699E  or          r8d,eax  
//00007FF764B169A1  mov         ebp,esi  
//00007FF764B169A3  ror         ebp,2  
//00007FF764B169A6  mov         eax,esi  
//00007FF764B169A8  ror         eax,0Dh  
//    round<52>(pstate, pbuffer);
//00007FF764B169AB  mov         ecx,r14d  
//00007FF764B169AE  xor         ebp,eax  
//00007FF764B169B0  ror         ecx,6  
//00007FF764B169B3  xor         ebp,edx 
//00007FF764B169B5  mov         eax,r14d  
//00007FF764B169B8  add         ebp,r8d  
//00007FF764B169BB  ror         eax,0Bh  
//00007FF764B169BE  xor         ecx,eax 
//00007FF764B169C0  add         ebp,r9d  
//00007FF764B169C3  mov         dword ptr [r12+10h],ebp 
//00007FF764B169C8  mov         r8d,r11d  
//00007FF764B169CB  mov         r9d,dword ptr [r13+0D0h]  
//00007FF764B169D2  xor         r8d,r10d  
//00007FF764B169D5  and         r8d,r14d  
//00007FF764B169D8  add         r9d,391C0CB3h  
//00007FF764B169DF  xor         r8d,r10d  
//00007FF764B169E2  mov         edx,r14d  
//00007FF764B169E5  mov         r10d,dword ptr [r12+1Ch]  
//00007FF764B169EA  mov         eax,esi  
//00007FF764B169EC  and         eax,edi  
//00007FF764B169EE  ror         edx,19h  
//00007FF764B169F1  xor         ecx,edx  
//00007FF764B169F3  mov         ebx,ebp  
//00007FF764B169F5  add         ecx,dword ptr [r12+0Ch]  
//00007FF764B169FA  mov         edx,ebp  
//00007FF764B169FC  add         ecx,r8d  
//00007FF764B169FF  ror         edx,16h  
//00007FF764B16A02  add         r9d,ecx  
//00007FF764B16A05  ror         ebx,2  
//00007FF764B16A08  add         r10d,r9d  
//00007FF764B16A0B  mov         r8d,esi  
//00007FF764B16A0E  or          r8d,edi  
//00007FF764B16A11  mov         dword ptr [r12+1Ch],r10d  
//00007FF764B16A16  and         r8d,ebp  
//    round<53>(pstate, pbuffer);
//00007FF764B16A19  mov         ecx,r10d  
//00007FF764B16A1C  or          r8d,eax 
//00007FF764B16A1F  ror         ecx,6  
//00007FF764B16A22  mov         eax,ebp  
//00007FF764B16A24  ror         eax,0Dh  
//00007FF764B16A27  xor         ebx,eax 
//00007FF764B16A29  mov         eax,r10d  
//00007FF764B16A2C  xor         ebx,edx  
//00007FF764B16A2E  ror         eax,0Bh  
//00007FF764B16A31  add         ebx,r8d  
//00007FF764B16A34  xor         ecx,eax  
//00007FF764B16A36  add         ebx,r9d  
//00007FF764B16A39  mov         r8d,r11d  
//00007FF764B16A3C  xor         r8d,r14d  
//00007FF764B16A3F  mov         dword ptr [r12+0Ch],ebx  
//00007FF764B16A44  mov         r9d,dword ptr [r13+0D4h]  
//00007FF764B16A4B  and         r8d,r10d  
//00007FF764B16A4E  xor         r8d,r11d  
//00007FF764B16A51  add         r9d,4ED8AA4Ah  
//00007FF764B16A58  mov         r11d,dword ptr [r12+18h]  
//00007FF764B16A5D  mov         edx,r10d  
//00007FF764B16A60  ror         edx,19h  
//00007FF764B16A63  mov         eax,ebp  
//00007FF764B16A65  xor         ecx,edx  
//00007FF764B16A67  and         eax,esi  
//00007FF764B16A69  add         ecx,dword ptr [r12+8]  
//00007FF764B16A6E  mov         edx,ebx  
//00007FF764B16A70  add         ecx,r8d  
//00007FF764B16A73  ror         edx,16h  
//00007FF764B16A76  add         r9d,ecx  
//00007FF764B16A79  mov         r8d,ebp  
//00007FF764B16A7C  or          r8d,esi  
//00007FF764B16A7F  add         r11d,r9d  
//00007FF764B16A82  and         r8d,ebx  
//00007FF764B16A85  mov         dword ptr [r12+18h],r11d  
//00007FF764B16A8A  or          r8d,eax  
//00007FF764B16A8D  mov         eax,ebx  
//00007FF764B16A8F  ror         eax,0Dh  
//00007FF764B16A92  mov         esi,ebx  
//    round<54>(pstate, pbuffer);
//00007FF764B16A94  mov         ecx,r11d  
//00007FF764B16A97  ror         ecx,6  
//00007FF764B16A9A  ror         esi,2  
//00007FF764B16A9D  xor         esi,eax  
//00007FF764B16A9F  mov         eax,r11d  
//00007FF764B16AA2  ror         eax,0Bh  
//00007FF764B16AA5  xor         esi,edx  
//00007FF764B16AA7  xor         ecx,eax  
//00007FF764B16AA9  add         esi,r9d  
//00007FF764B16AAC  add         esi,r8d  
//00007FF764B16AAF  mov         edx,r11d  
//00007FF764B16AB2  mov         dword ptr [r12+8],esi  
//00007FF764B16AB7  mov         r8d,r14d  
//00007FF764B16ABA  mov         r9d,dword ptr [r13+0D8h]  
//00007FF764B16AC1  xor         r8d,r10d  
//00007FF764B16AC4  mov         edi,dword ptr [r12+14h]  
//00007FF764B16AC9  and         r8d,r11d  
//00007FF764B16ACC  xor         r8d,r14d  
//00007FF764B16ACF  ror         edx,19h  
//00007FF764B16AD2  xor         ecx,edx  
//00007FF764B16AD4  add         r9d,5B9CCA4Fh  
//00007FF764B16ADB  add         ecx,dword ptr [r12+4]  
//00007FF764B16AE0  mov         eax,ebx  
//00007FF764B16AE2  add         ecx,r8d  
//00007FF764B16AE5  and         eax,ebp  
//00007FF764B16AE7  add         r9d,ecx  
//00007FF764B16AEA  mov         r8d,ebx  
//00007FF764B16AED  or          r8d,ebp  
//00007FF764B16AF0  add         edi,r9d  
//00007FF764B16AF3  and         r8d,esi  
//00007FF764B16AF6  mov         dword ptr [r12+14h],edi  
//00007FF764B16AFB  or          r8d,eax  
//00007FF764B16AFE  mov         ebp,esi  
//00007FF764B16B00  ror         ebp,2  
//00007FF764B16B03  mov         eax,esi  
//00007FF764B16B05  ror         eax,0Dh  
//00007FF764B16B08  mov         edx,esi  
//00007FF764B16B0A  xor         ebp,eax  
//00007FF764B16B0C  ror         edx,16h  
//00007FF764B16B0F  xor         ebp,edx  
//    round<55>(pstate, pbuffer);
//00007FF764B16B11  mov         eax,edi  
//00007FF764B16B13  ror         eax,0Bh  
//00007FF764B16B16  add         ebp,r9d  
//00007FF764B16B19  add         ebp,r8d  
//00007FF764B16B1C  mov         ecx,edi  
//00007FF764B16B1E  ror         ecx,6  
//00007FF764B16B21  mov         r8d,r10d  
//00007FF764B16B24  xor         ecx,eax  
//00007FF764B16B26  mov         dword ptr [r12+4],ebp  
//00007FF764B16B2B  mov         r9d,dword ptr [r13+0DCh]  
//00007FF764B16B32  xor         r8d,r11d  
//00007FF764B16B35  and         r8d,edi  
//00007FF764B16B38  add         r9d,682E6FF3h  
//00007FF764B16B3F  xor         r8d,r10d  
//00007FF764B16B42  mov         edx,edi  
//00007FF764B16B44  mov         r10d,dword ptr [r12+10h]  
//00007FF764B16B49  mov         eax,esi  
//00007FF764B16B4B  ror         edx,19h  
//00007FF764B16B4E  and         eax,ebx  
//00007FF764B16B50  xor         ecx,edx  
//00007FF764B16B52  mov         edx,ebp  
//00007FF764B16B54  add         ecx,r14d  
//00007FF764B16B57  ror         edx,16h  
//00007FF764B16B5A  add         ecx,r8d  
//00007FF764B16B5D  mov         r14d,ebp  
//00007FF764B16B60  ror         r14d,2  
//00007FF764B16B64  add         r9d,ecx  
//00007FF764B16B67  add         r10d,r9d  
//00007FF764B16B6A  mov         r8d,esi  
//00007FF764B16B6D  or          r8d,ebx  
//00007FF764B16B70  mov         dword ptr [r12+10h],r10d  
//00007FF764B16B75  and         r8d,ebp  
//00007FF764B16B78  or          r8d,eax  
//00007FF764B16B7B  mov         eax,ebp  
//00007FF764B16B7D  ror         eax,0Dh  
//00007FF764B16B80  xor         r14d,eax  
//00007FF764B16B83  xor         r14d,edx  
//00007FF764B16B86  add         r14d,r9d  
//00007FF764B16B89  add         r14d,r8d  
//00007FF764B16B8C  mov         dword ptr [r12],r14d  
//    round<56>(pstate, pbuffer);
//00007FF764B16B90  mov         r9d,dword ptr [r13+0E0h]  
//00007FF764B16B97  mov         r8d,r11d  
//00007FF764B16B9A  mov         ebx,dword ptr [r12+0Ch]  
//00007FF764B16B9F  xor         r8d,edi  
//00007FF764B16BA2  and         r8d,r10d  
//00007FF764B16BA5  add         r9d,748F82EEh  
//00007FF764B16BAC  xor         r8d,r11d  
//00007FF764B16BAF  mov         eax,r10d  
//00007FF764B16BB2  ror         eax,0Bh  
//00007FF764B16BB5  mov         ecx,r10d  
//00007FF764B16BB8  ror         ecx,6  
//00007FF764B16BBB  mov         edx,r10d  
//00007FF764B16BBE  xor         ecx,eax  
//00007FF764B16BC0  ror         edx,19h  
//00007FF764B16BC3  xor         ecx,edx  
//00007FF764B16BC5  mov         eax,ebp  
//00007FF764B16BC7  add         ecx,dword ptr [r12+1Ch]  
//00007FF764B16BCC  and         eax,esi  
//00007FF764B16BCE  add         ecx,r8d  
//00007FF764B16BD1  mov         edx,r14d  
//00007FF764B16BD4  add         r9d,ecx  
//00007FF764B16BD7  ror         edx,16h  
//00007FF764B16BDA  add         ebx,r9d  
//00007FF764B16BDD  mov         r8d,ebp  
//00007FF764B16BE0  or          r8d,esi  
//00007FF764B16BE3  mov         dword ptr [r12+0Ch],ebx  
//00007FF764B16BE8  and         r8d,r14d  
//00007FF764B16BEB  mov         esi,r14d  
//00007FF764B16BEE  or          r8d,eax  
//00007FF764B16BF1  ror         esi,2  
//00007FF764B16BF4  mov         eax,r14d  
//    round<57>(pstate, pbuffer);
//00007FF764B16BF7  mov         ecx,ebx  
//00007FF764B16BF9  ror         eax,0Dh  
//00007FF764B16BFC  xor         esi,eax  
//00007FF764B16BFE  ror         ecx,6  
//00007FF764B16C01  xor         esi,edx  
//00007FF764B16C03  mov         eax,ebx  
//00007FF764B16C05  add         esi,r9d  
//00007FF764B16C08  ror         eax,0Bh  
//00007FF764B16C0B  add         esi,r8d  
//00007FF764B16C0E  xor         ecx,eax  
//00007FF764B16C10  mov         dword ptr [r12+1Ch],esi  
//00007FF764B16C15  mov         r8d,edi  
//00007FF764B16C18  mov         r9d,dword ptr [r13+0E4h]  
//00007FF764B16C1F  xor         r8d,r10d  
//00007FF764B16C22  mov         r11d,dword ptr [r12+8]  
//00007FF764B16C27  and         r8d,ebx  
//00007FF764B16C2A  xor         r8d,edi  
//00007FF764B16C2D  add         r9d,78A5636Fh  
//00007FF764B16C34  mov         edx,ebx  
//00007FF764B16C36  mov         eax,r14d  
//00007FF764B16C39  and         eax,ebp  
//00007FF764B16C3B  ror         edx,19h  
//00007FF764B16C3E  xor         ecx,edx  
//00007FF764B16C40  mov         edx,esi  
//00007FF764B16C42  add         ecx,dword ptr [r12+18h]  
//00007FF764B16C47  add         ecx,r8d  
//00007FF764B16C4A  ror         edx,16h  
//00007FF764B16C4D  add         r9d,ecx  
//00007FF764B16C50  mov         r8d,r14d  
//00007FF764B16C53  or          r8d,ebp  
//00007FF764B16C56  add         r11d,r9d  
//00007FF764B16C59  and         r8d,esi  
//00007FF764B16C5C  mov         dword ptr [r12+8],r11d  
//00007FF764B16C61  or          r8d,eax  
//00007FF764B16C64  mov         ebp,esi  
//00007FF764B16C66  ror         ebp,2  
//00007FF764B16C69  mov         eax,esi  
//00007FF764B16C6B  ror         eax,0Dh  
//00007FF764B16C6E  xor         ebp,eax  
//00007FF764B16C70  xor         ebp,edx  
//    round<58>(pstate, pbuffer);
//00007FF764B16C72  mov         edx,r11d  
//00007FF764B16C75  add         ebp,r9d  
//00007FF764B16C78  add         ebp,r8d  
//00007FF764B16C7B  mov         r8d,ebx  
//00007FF764B16C7E  xor         r8d,r10d  
//00007FF764B16C81  mov         dword ptr [r12+18h],ebp  
//00007FF764B16C86  mov         r9d,dword ptr [r13+0E8h]  
//00007FF764B16C8D  and         r8d,r11d  
//00007FF764B16C90  xor         r8d,r10d  
//00007FF764B16C93  ror         edx,19h  
//00007FF764B16C96  mov         r10d,dword ptr [r12+4]  
//    round<59>(pstate, pbuffer);
//00007FF764B16C9B  lea         r15d,[r14-7338FDF8h]  
//00007FF764B16CA2  add         r10d,84C87814h  
//00007FF764B16CA9  mov         eax,r11d  
//00007FF764B16CAC  ror         eax,0Bh  
//00007FF764B16CAF  mov         ecx,r11d  
//00007FF764B16CB2  ror         ecx,6  
//00007FF764B16CB5  mov         edi,esi  
//00007FF764B16CB7  xor         ecx,eax  
//00007FF764B16CB9  or          edi,r14d  
//00007FF764B16CBC  xor         ecx,edx  
//00007FF764B16CBE  and         edi,ebp  
//00007FF764B16CC0  add         ecx,dword ptr [r12+14h]  
//00007FF764B16CC5  mov         eax,esi  
//00007FF764B16CC7  add         ecx,r8d  
//00007FF764B16CCA  and         eax,r14d  
//00007FF764B16CCD  add         r9d,ecx  
//00007FF764B16CD0  or          edi,eax  
//00007FF764B16CD2  add         r10d,r9d  
//00007FF764B16CD5  mov         ecx,ebp  
//00007FF764B16CD7  ror         ecx,2  
//00007FF764B16CDA  add         r9d,84C87814h  
//00007FF764B16CE1  mov         dword ptr [r12+4],r10d  
//00007FF764B16CE6  mov         eax,ebp  
//00007FF764B16CE8  ror         eax,0Dh  
//00007FF764B16CEB  mov         edx,ebp  
//00007FF764B16CED  xor         ecx,eax  
//00007FF764B16CEF  ror         edx,16h  
//00007FF764B16CF2  xor         ecx,edx  
//00007FF764B16CF4  mov         eax,r10d  
//00007FF764B16CF7  add         ecx,r9d  
//00007FF764B16CFA  ror         eax,0Bh  
//00007FF764B16CFD  add         edi,ecx  
//00007FF764B16CFF  mov         r8d,r11d  
//00007FF764B16D02  xor         r8d,ebx  
//00007FF764B16D05  mov         dword ptr [r12+14h],edi  
//    round<59>(pstate, pbuffer); 
//00007FF764B16D11  and         r8d,r10d  
//00007FF764B16D14  xor         r8d,ebx  
//00007FF764B16D17  mov         ecx,r10d  
//00007FF764B16D1A  ror         ecx,6  
//00007FF764B16D1D  mov         edx,r10d  
//00007FF764B16D20  xor         ecx,eax  
//00007FF764B16D22  ror         edx,19h  
//00007FF764B16D25  xor         ecx,edx  
//00007FF764B16D27  mov         eax,ebp  
//00007FF764B16D29  add         ecx,dword ptr [r12+10h]  
//00007FF764B16D2E  and         eax,esi  
//00007FF764B16D30  add         ecx,r8d  
//00007FF764B16D33  mov         edx,edi  
//00007FF764B16D35  add         r9d,ecx  
//00007FF764B16D38  ror         edx,16h  
//00007FF764B16D3B  add         r15d,r9d  
//00007FF764B16D3E  mov         r8d,ebp  
//00007FF764B16D41  or          r8d,esi  
//00007FF764B16D44  mov         dword ptr [r12],r15d  
//00007FF764B16D48  and         r8d,edi  
//00007FF764B16D4B  mov         ecx,edi  
//00007FF764B16D4D  or          r8d,eax  
//00007FF764B16D50  ror         ecx,2  
//00007FF764B16D53  lea         r14d,[r9-7338FDF8h]  
//00007FF764B16D5A  mov         eax,edi  
//00007FF764B16D5C  ror         eax,0Dh  
//00007FF764B16D5F  xor         ecx,eax  
//    round<60>(pstate, pbuffer);
//00007FF764B16D61  mov         eax,r15d  
//00007FF764B16D64  xor         ecx,edx  
//00007FF764B16D66  ror         eax,0Bh  
//00007FF764B16D69  add         ecx,r8d  
//00007FF764B16D6C  mov         edx,r15d  
//00007FF764B16D6F  add         r14d,ecx  
//00007FF764B16D72  ror         edx,19h  
//00007FF764B16D75  mov         r8d,r10d  
//00007FF764B16D78  mov         dword ptr [r12+10h],r14d  
//00007FF764B16D7D  mov         r9d,dword ptr [r13+0F0h]  
//00007FF764B16D84  xor         r8d,r11d  
//00007FF764B16D87  and         r8d,r15d  
//00007FF764B16D8A  mov         ecx,r15d  
//00007FF764B16D8D  ror         ecx,6  
//00007FF764B16D90  xor         r8d,r11d  
//00007FF764B16D93  xor         ecx,eax  
//00007FF764B16D95  xor         ecx,edx  
//00007FF764B16D97  add         ecx,dword ptr [r12+0Ch]  
//00007FF764B16D9C  mov         eax,edi  
//00007FF764B16D9E  add         ecx,r8d  
//00007FF764B16DA1  mov         esi,dword ptr [r12+1Ch]  
//00007FF764B16DA6  add         r9d,ecx  
//00007FF764B16DA9  and         eax,ebp  
//00007FF764B16DAB  mov         r8d,edi  
//00007FF764B16DAE  add         esi,90BEFFFAh  
//00007FF764B16DB4  or          r8d,ebp  
//00007FF764B16DB7  add         esi,r9d  
//00007FF764B16DBA  and         r8d,r14d  
//00007FF764B16DBD  mov         dword ptr [r12+1Ch],esi  
//00007FF764B16DC2  or          r8d,eax  
//00007FF764B16DC5  lea         ebp,[r9-6F410006h]  
//00007FF764B16DCC  mov         eax,r14d  
//00007FF764B16DCF  mov         ecx,r14d  
//00007FF764B16DD2  ror         ecx,2  
//00007FF764B16DD5  mov         edx,r14d  
//00007FF764B16DD8  ror         eax,0Dh  
//00007FF764B16DDB  xor         ecx,eax  
//00007FF764B16DDD  ror         edx,16h  
//00007FF764B16DE0  xor         ecx,edx  
//    round<61>(pstate, pbuffer);
//00007FF764B16DE2  mov         eax,esi  
//00007FF764B16DE4  add         ecx,r8d  
//00007FF764B16DE7  ror         eax,0Bh  
//00007FF764B16DEA  add         ebp,ecx  
//00007FF764B16DEC  mov         r8d,r15d  
//00007FF764B16DEF  xor         r8d,r10d  
//00007FF764B16DF2  mov         dword ptr [r12+0Ch],ebp  
//00007FF764B16DF7  mov         r9d,dword ptr [r13+0F4h]  
//00007FF764B16DFE  and         r8d,esi  
//00007FF764B16E01  mov         ebx,dword ptr [r12+18h]  
//00007FF764B16E06  xor         r8d,r10d  
//00007FF764B16E09  add         ebx,0A4506CEBh  
//00007FF764B16E0F  mov         ecx,esi  
//00007FF764B16E11  ror         ecx,6  
//00007FF764B16E14  mov         edx,esi  
//00007FF764B16E16  xor         ecx,eax  
//00007FF764B16E18  ror         edx,19h  
//00007FF764B16E1B  xor         ecx,edx  
//00007FF764B16E1D  mov         eax,r14d  
//00007FF764B16E20  add         ecx,dword ptr [r12+8]  
//00007FF764B16E25  and         eax,edi  
//00007FF764B16E27  add         ecx,r8d  
//00007FF764B16E2A  mov         edx,ebp  
//00007FF764B16E2C  add         r9d,ecx  
//00007FF764B16E2F  ror         edx,16h  
//00007FF764B16E32  add         ebx,r9d  
//00007FF764B16E35  mov         r8d,r14d  
//00007FF764B16E38  or          r8d,edi  
//00007FF764B16E3B  mov         dword ptr [r12+18h],ebx  
//00007FF764B16E40  and         r8d,ebp  
//00007FF764B16E43  mov         ecx,ebp  
//00007FF764B16E45  or          r8d,eax  
//00007FF764B16E48  ror         ecx,2  
//00007FF764B16E4B  lea         edi,[r9-5BAF9315h]  
//00007FF764B16E52  mov         eax,ebp  
//00007FF764B16E54  ror         eax,0Dh  
//00007FF764B16E57  xor         ecx,eax  
//    round<62>(pstate, pbuffer);
//00007FF764B16E59  mov         eax,ebx  
//00007FF764B16E5B  xor         ecx,edx  
//00007FF764B16E5D  ror         eax,0Bh  
//00007FF764B16E60  add         ecx,r8d  
//00007FF764B16E63  mov         edx,ebx  
//00007FF764B16E65  add         edi,ecx  
//00007FF764B16E67  ror         edx,19h  
//00007FF764B16E6A  mov         dword ptr [r12+8],edi  
//00007FF764B16E6F  mov         ecx,ebx  
//00007FF764B16E71  mov         r9d,dword ptr [r13+0F8h]  
//00007FF764B16E78  mov         r8d,esi  
//00007FF764B16E7B  ror         ecx,6  
//00007FF764B16E7E  xor         r8d,r15d  
//00007FF764B16E81  xor         ecx,eax  
//00007FF764B16E83  and         r8d,ebx  
//00007FF764B16E86  xor         ecx,edx  
//00007FF764B16E88  xor         r8d,r15d  
//00007FF764B16E8B  add         ecx,dword ptr [r12+4]  
//00007FF764B16E90  add         ecx,r8d  
//00007FF764B16E93  add         r9d,ecx  
//00007FF764B16E96  lea         eax,[r9-41065C09h]  
//00007FF764B16E9D  add         dword ptr [r12+14h],eax  
//00007FF764B16EA2  mov         r10d,dword ptr [r12+14h]  
//00007FF764B16EA7  lea         r11d,[r9-41065C09h]  
//00007FF764B16EAE  mov         ecx,edi  
//00007FF764B16EB0  mov         edx,edi  
//00007FF764B16EB2  ror         edx,16h  
//    round<63>(pstate, pbuffer);
//00007FF764B16EB5  mov         r9d,r10d  
//00007FF764B16EB8  ror         ecx,2  
//00007FF764B16EBB  mov         r8d,ebp  
//00007FF764B16EBE  or          r8d,r14d  
//00007FF764B16EC1  ror         r9d,0Bh  
//00007FF764B16EC5  and         r8d,edi  
//00007FF764B16EC8  mov         eax,ebp  
//00007FF764B16ECA  and         eax,r14d  
//00007FF764B16ECD  or          r8d,eax  
//00007FF764B16ED0  mov         eax,edi  
//00007FF764B16ED2  ror         eax,0Dh  
//00007FF764B16ED5  xor         ecx,eax  
//00007FF764B16ED7  xor         ecx,edx  
//00007FF764B16ED9  mov         edx,esi  
//00007FF764B16EDB  add         ecx,r8d  
//00007FF764B16EDE  xor         edx,ebx  
//00007FF764B16EE0  add         r11d,ecx  
//00007FF764B16EE3  and         edx,r10d  
//00007FF764B16EE6  mov         dword ptr [r12+4],r11d  
//00007FF764B16EEB  xor         edx,esi  
//00007FF764B16EED  mov         ecx,r10d  
//00007FF764B16EF0  mov         r8d,edi  
//00007FF764B16EF3  ror         ecx,19h  
//00007FF764B16EF6  or          r8d,ebp  
//00007FF764B16EF9  and         r8d,r11d  
//00007FF764B16EFC  ror         r10d,6  
//00007FF764B16F00  xor         r9d,r10d  
//00007FF764B16F03  and         edi,ebp  
//00007FF764B16F05  xor         r9d,ecx  
//00007FF764B16F08  or          r8d,edi  
//00007FF764B16F0B  add         r9d,r15d  
//00007FF764B16F0E  mov         rcx,r12  
//00007FF764B16F11  add         r9d,edx  
//00007FF764B16F14  mov         edx,r11d  
//00007FF764B16F17  add         r9d,dword ptr [r13+0FCh]  
//00007FF764B16F1E  ror         edx,16h  
//00007FF764B16F21  lea         eax,[r9-398E870Eh]  
//00007FF764B16F28  add         dword ptr [r12+10h],eax  
//00007FF764B16F2D  mov         eax,r11d  
//00007FF764B16F30  ror         eax,0Dh  
//00007FF764B16F33  ror         r11d,2  
//00007FF764B16F37  xor         r11d,eax  
//00007FF764B16F3A  lea         eax,[r9-398E870Eh]  
//00007FF764B16F41  xor         r11d,edx  
//00007FF764B16F44  lea         rdx,[start]  
//00007FF764B16F49  add         r11d,r8d  
//00007FF764B16F4C  add         eax,r11d  
//00007FF764B16F4F  mov         dword ptr [r12],eax 

#endif

#if defined(DISABLED)

// The SHA-1 hash computation method described in Sec. 6.1.2 assumes that the message schedule
// W0, W1,…, W79 is implemented as an array of eighty 32-bit words. This is efficient from the
// standpoint of the minimization of execution time, since the addresses of Wt-3,…, Wt-16 in step (2)
// of Sec. 6.1.2 are easily computed.
// However, if memory is limited, an alternative is to regard {Wt} as a circular queue that may be
// implemented using an array of sixteen 32-bit words, W0, W1,…, W15. The alternate method that is
// described in this section yields the same message digest as the SHA-1 computation method
// described in Sec. 6.1.2. Although this alternate method saves sixty-four 32-bit words of storage,
// it is likely to lengthen the execution time due to the increased complexity of the address
// computations for the {Wt} in step (3).
//
// This is the completely unrolled preparing function.
// This makes 48 calls (16-64) to (3add ops):
// buffer[Round] = add_(
//     add_(buffer[Round - 16], sigma0(buffer[Round - 15])),
//     add_(buffer[Round -  7], sigma1(buffer[Round -  2])));
//
// Where 2 sigmas(x) are the following (4xor, 4ror, 2shr).
// return xor_(xor_(ror_<A>(x), ror_<B>(x)), shr_<C>(x));
//
//    9   mov ops implied per round (prepare() math moves)
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

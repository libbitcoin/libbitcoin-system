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

using sha_256 = sha::algorithm<sha::sha256_256>;

constexpr auto half = base16_array("0000000000000000000000000000000000000000000000000000000000000000");
constexpr auto full = base16_array("00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");

BOOST_AUTO_TEST_CASE(sha_algorithm__constexpr_half_block__null_hash__expected)
{
    constexpr auto expected = base16_array("66687aadf862bd776c8fc18b8e9f8e20089714856ee233b3902a591d0d5f2925");
    static_assert(sha_256::hash(half) == expected);
    BOOST_REQUIRE_EQUAL(sha_256::hash(half), expected);
    BOOST_REQUIRE_EQUAL(sha256_hash(half), expected);
}

BOOST_AUTO_TEST_CASE(sha_algorithm__constexpr_full_block__null_hash__expected)
{
    constexpr auto expected = base16_array("f5a5fd42d16a20302798ef6ed309979b43003d2320d9f0e8ea9831a92759fb4b");
    static_assert(sha_256::hash(full) == expected);
    BOOST_REQUIRE_EQUAL(sha_256::hash(full), expected);
    BOOST_REQUIRE_EQUAL(sha256_hash(full), expected);
}

BOOST_AUTO_TEST_CASE(sha_algorithm__const_twin_block__null_hash__unexpected)
{
    constexpr auto expected = base16_array("38723a2e5e8a17aa7950dc008209944e898f69a7bd10a23c839d341e935fd5ca");
    const std_vector<std::reference_wrapper<const sha::block>> twin{ std::cref(full), std::cref(full) };

    // sha::hash(twin): f6b8e1b204d06f15cfda035de7e224d9bdfda16cbd17966c92c22b8268e66862
    BOOST_CHECK_NE(sha_256::hash(twin), expected);
    BOOST_REQUIRE_EQUAL(sha256_hash(full, full), expected);
}

// merkle(full)
// merkle(twin)

using namespace sha;

// K

static_assert(k<uint32_t, 64>::rounds == 64);
static_assert(k<uint32_t, 80>::rounds == 80);
static_assert(k<uint64_t, 80>::rounds == 80);
static_assert(is_same_type<k<uint32_t, 64>::constants_t, std_array<uint32_t, 64>>);
static_assert(is_same_type<k<uint32_t, 80>::constants_t, std_array<uint32_t, 80>>);
static_assert(is_same_type<k<uint64_t, 80>::constants_t, std_array<uint64_t, 80>>);

static_assert(k160::rounds == 80);
static_assert(k160::get[0] == 0x5a827999);
static_assert(k160::get[19] == 0x5a827999);
static_assert(k160::get[20] == 0x6ed9eba1);
static_assert(k160::get[39] == 0x6ed9eba1);
static_assert(k160::get[40] == 0x8f1bbcdc);
static_assert(k160::get[59] == 0x8f1bbcdc);
static_assert(k160::get[60] == 0xca62c1d6);
static_assert(k160::get[79] == 0xca62c1d6);
static_assert(is_same_type<k160::constants_t, std_array<uint32_t, 80>>);

static_assert(k256::rounds == 64);
static_assert(k256::get[0] == 0x428a2f98);
static_assert(k256::get[63] == 0xc67178f2);
static_assert(is_same_type<k256::constants_t, std_array<uint32_t, 64>>);

static_assert(k512::rounds == 80);
static_assert(k512::get[0] == 0x428a2f98d728ae22);
static_assert(k512::get[79] == 0x6c44198c4a475817);
static_assert(is_same_type<k512::constants_t, std_array<uint64_t, 80>>);

// H

static_assert(h<256, 256>::digest == 256);
static_assert(h<256, 224>::digest == 224);
static_assert(h<512, 512>::digest == 512);
static_assert(h<512, 384>::digest == 384);
static_assert(h<512, 224>::digest == 224);
static_assert(h<512, 256>::digest == 256);
static_assert(h<256, 256>::strength == 256);
static_assert(h<256, 224>::strength == 256);
static_assert(h<512, 512>::strength == 512);
static_assert(h<512, 384>::strength == 512);
static_assert(h<512, 224>::strength == 512);
static_assert(h<512, 256>::strength == 512);
static_assert(is_same_type<h<256, 256>::K, k256>);
static_assert(is_same_type<h<256, 224>::K, k256>);
static_assert(is_same_type<h<512, 512>::K, k512>);
static_assert(is_same_type<h<512, 384>::K, k512>);
static_assert(is_same_type<h<512, 224>::K, k512>);
static_assert(is_same_type<h<512, 256>::K, k512>);
static_assert(is_same_type<h<256, 256>::word_t, uint32_t>);
static_assert(is_same_type<h<256, 224>::word_t, uint32_t>);
static_assert(is_same_type<h<512, 512>::word_t, uint64_t>);
static_assert(is_same_type<h<512, 384>::word_t, uint64_t>);
static_assert(is_same_type<h<512, 224>::word_t, uint64_t>);
static_assert(is_same_type<h<512, 256>::word_t, uint64_t>);
static_assert(is_same_type<h<256, 256>::state_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<h<256, 256>::chunk_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<h<256, 256>::words_t, std_array<uint32_t, 16>>);
static_assert(is_same_type<h<256, 256>::buffer_t, std_array<uint32_t, 64>>);
static_assert(is_same_type<h<512, 512>::state_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<h<512, 512>::chunk_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<h<512, 512>::words_t, std_array<uint64_t, 16>>);
static_assert(is_same_type<h<512, 512>::buffer_t, std_array<uint64_t, 64>>);

static_assert(h160::digest == 160);
static_assert(h160::strength == 256);
static_assert(h160::K::rounds == 80);
static_assert(h160::K::get[0] == 0x5a827999);
static_assert(h160::K::get[79] == 0xca62c1d6);
static_assert(h160::H::get.size() == 5);
static_assert(h160::H::get[0] == 0x67452301);
static_assert(h160::H::get[4] == 0xc3d2e1f0);

static_assert(h256<256>::digest == 256);
static_assert(h256<256>::strength == 256);
static_assert(h256<256>::K::rounds == 64);
static_assert(h256<256>::K::get[0] == 0x428a2f98);
static_assert(h256<256>::K::get[63] == 0xc67178f2);
static_assert(h256<256>::H::get.size() == 8);
static_assert(h256<256>::H::get[0] == 0x6a09e667);
static_assert(h256<256>::H::get[7] == 0x5be0cd19);
static_assert(h256<256>::pad::chunk.size() == 8);
static_assert(h256<256>::pad::chunk[0] == 0x80000000);
static_assert(h256<256>::pad::chunk[7] == 0x00000100);
static_assert(h256<256>::pad::block.size() == 16);
static_assert(h256<256>::pad::block[0] == 0x80000000);
static_assert(h256<256>::pad::block[15] == 0x00000200);
static_assert(h256<256>::pad::buffer.size() == 64);
static_assert(h256<256>::pad::buffer[0] == 0x80000000);
static_assert(h256<256>::pad::buffer[63] == 0x85a7a484);
static_assert(h256<256>::pad::stream.size() == 16);
static_assert(h256<256>::pad::stream[0] == 0x80000000);
static_assert(h256<256>::pad::stream[15] == 0x00000000);

static_assert(h512<512>::digest == 512);
static_assert(h512<512>::strength == 512);
static_assert(h512<512>::K::rounds == 80);
static_assert(h512<512>::K::get[0] == 0x428a2f98d728ae22);
static_assert(h512<512>::K::get[79] == 0x6c44198c4a475817);
static_assert(h512<512>::H::get.size() == 8);
static_assert(h512<512>::H::get[0] == 0x6a09e667f3bcc908);
static_assert(h512<512>::H::get[7] == 0x5be0cd19137e2179);

// Algorithms

static_assert(sha160::digest == 160);
static_assert(sha160::strength == 256);
static_assert(sha160::K::rounds == 80);
static_assert(sha160::K::get.size() == 80);
static_assert(sha160::K::get[0] == 0x5a827999);
static_assert(sha160::K::get[79] == 0xca62c1d6);
static_assert(sha160::H::get.size() == 5);
static_assert(sha160::H::get[0] == 0x67452301);
static_assert(sha160::H::get[4] == 0xc3d2e1f0);

static_assert(sha256_256::digest == 256);
static_assert(sha256_256::strength == 256);
static_assert(sha256_256::K::rounds == 64);
static_assert(sha256_256::K::get.size() == 64);
static_assert(sha256_256::K::get[0] == 0x428a2f98);
static_assert(sha256_256::K::get[63] == 0xc67178f2);
static_assert(sha256_256::H::get.size() == 8);
static_assert(sha256_256::H::get[0] == 0x6a09e667);
static_assert(sha256_256::H::get[7] == 0x5be0cd19);

static_assert(sha512_512::digest == 512);
static_assert(sha512_512::strength == 512);
static_assert(sha512_512::K::rounds == 80);
static_assert(sha512_512::K::get.size() == 80);
static_assert(sha512_512::K::get[0] == 0x428a2f98d728ae22);
static_assert(sha512_512::K::get[79] == 0x6c44198c4a475817);
static_assert(sha512_512::H::get.size() == 8);
static_assert(sha512_512::H::get[0] == 0x6a09e667f3bcc908);
static_assert(sha512_512::H::get[7] == 0x5be0cd19137e2179);

// algorithm<sha256_256>

static_assert(algorithm<sha256_256>::H::get.size() == 8u);
static_assert(algorithm<sha256_256>::K::get.size() == 64u);
static_assert(algorithm<sha256_256>::pad::chunk.size() == 8u);
static_assert(algorithm<sha256_256>::digest_words == 8u);
static_assert(algorithm<sha256_256>::digest_bytes == 32u);
static_assert(algorithm<sha256_256>::block_words == 16u);
static_assert(algorithm<sha256_256>::block_bytes == 64u);
static_assert(algorithm<sha256_256>::chunk_bytes == 32u);
static_assert(algorithm<sha256_256>::buffer_words == 64u);
static_assert(algorithm<sha256_256>::buffer_bytes == 256u);
static_assert(algorithm<sha256_256>::count_bits == 64u);
static_assert(algorithm<sha256_256>::count_bytes == 8u);
static_assert(algorithm<sha256_256>::limit_bits == std::numeric_limits<uint64_t>::max() - 64u);
static_assert(algorithm<sha256_256>::limit_bytes == algorithm<sha256_256>::limit_bits / byte_bits);
static_assert(is_same_type<algorithm<sha256_256>::byte_t, uint8_t>);
static_assert(is_same_type<algorithm<sha256_256>::word_t, uint32_t>);
static_assert(is_same_type<algorithm<sha256_256>::count_t, uint64_t>);
static_assert(is_same_type<algorithm<sha256_256>::digest_t, std_array<uint8_t, 32>>);
static_assert(is_same_type<algorithm<sha256_256>::half_t, std_array<uint8_t, 32>>);
static_assert(is_same_type<algorithm<sha256_256>::block_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<sha256_256>::words_t, std_array<uint32_t, 16>>);
static_assert(is_same_type<algorithm<sha512_512>::buffer_t, std_array<uint64_t, 64>>);
static_assert(is_same_type<algorithm<sha256_256>::chunk_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<decltype(algorithm<sha256_256>::limit_bits), const uint64_t>);
static_assert(is_same_type<decltype(algorithm<sha256_256>::limit_bytes), const uint64_t>);

// algorithm<sha512_512>

static_assert(algorithm<sha512_512>::H::get.size() == 8u);
static_assert(algorithm<sha512_512>::K::get.size() == 80u);
static_assert(algorithm<sha512_512>::pad::chunk.size() == 8u);
static_assert(algorithm<sha512_512>::digest_words == 8u);
static_assert(algorithm<sha512_512>::digest_bytes == 64u);
static_assert(algorithm<sha512_512>::block_words == 16u);
static_assert(algorithm<sha512_512>::block_bytes == 128u);
static_assert(algorithm<sha512_512>::chunk_bytes == 64u);
static_assert(algorithm<sha512_512>::buffer_words == 64u);
static_assert(algorithm<sha512_512>::buffer_bytes == 512u);
static_assert(algorithm<sha512_512>::count_bits == 128u);
static_assert(algorithm<sha512_512>::count_bytes == 16u);
static_assert(algorithm<sha512_512>::limit_bits == std::numeric_limits<uint128_t>::max() - 128u);
static_assert(algorithm<sha512_512>::limit_bytes == algorithm<sha512_512>::limit_bits / byte_bits);
static_assert(is_same_type<algorithm<sha512_512>::byte_t, uint8_t>);
static_assert(is_same_type<algorithm<sha512_512>::word_t, uint64_t>);
static_assert(is_same_type<algorithm<sha512_512>::count_t, uint128_t>);
static_assert(is_same_type<algorithm<sha512_512>::digest_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<sha512_512>::half_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<sha512_512>::block_t, std_array<uint8_t, 128>>);
static_assert(is_same_type<algorithm<sha512_512>::words_t, std_array<uint64_t, 16>>);
static_assert(is_same_type<algorithm<sha512_512>::buffer_t, std_array<uint64_t, 64>>);
static_assert(is_same_type<algorithm<sha256_256>::chunk_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<decltype(algorithm<sha512_512>::limit_bits), const uint128_t>);
static_assert(is_same_type<decltype(algorithm<sha512_512>::limit_bytes), const uint128_t>);

BOOST_AUTO_TEST_SUITE_END()

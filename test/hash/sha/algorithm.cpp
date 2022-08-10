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
#include "../hash.hpp"
    
BOOST_AUTO_TEST_SUITE(sha_algorithm_tests)

// Other test vectors are dependant upon the correctness of these.
static_assert(sha160::hash(sha160::half_t{})  == sha_half160);
static_assert(sha160::hash(sha160::block_t{}) == sha_full160);
static_assert(sha256::hash(sha256::half_t{})  == sha_half256);
static_assert(sha256::hash(sha256::block_t{}) == sha_full256);
static_assert(sha512::hash(sha512::half_t{})  == sha_half512);
static_assert(sha512::hash(sha512::block_t{}) == sha_full512);

// sha160 (cached)
// ----------------------------------------------------------------------------
static_assert(sha160::cached);

// sha160::hash (cached)
BOOST_AUTO_TEST_CASE(sha__sha160_hash__null_hash__expected)
{
    // Correlate non-const-evaluated to const-evaluated.
    BOOST_REQUIRE_EQUAL(sha160::hash(sha160::half_t{}), sha_half160);
    BOOST_REQUIRE_EQUAL(sha160::hash(sha160::block_t{}), sha_full160);
}

// accumulator<sha160>::hash (cached)
BOOST_AUTO_TEST_CASE(sha__accumulator_sha160_hash__test_vectors__expected)
{
    // Verify non-const-evaluated to against public vectors.
    for (const auto& test: sha160_tests)
    {
        const auto hash = accumulator<sha160>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha160>::hash (uncached)
BOOST_AUTO_TEST_CASE(sha__uncached_accumulator_sha160_hash__test_vectors__expected)
{
    using sha_160 = sha::algorithm<sha::h160, true, true, false>;
    static_assert(!sha_160::cached);

    // Verify non-const-evaluated to against public vectors.
    for (const auto& test: sha160_tests)
    {
        const auto hash = accumulator<sha_160>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// sha256
// ----------------------------------------------------------------------------
static_assert(sha256::cached);

// sha256::hash (cached)
BOOST_AUTO_TEST_CASE(sha__sha256_hash__null_hash__expected)
{
    // Correlate non-const-evaluated to const-evaluated.
    BOOST_REQUIRE_EQUAL(sha256::hash(sha256::half_t{}), sha_half256);
    BOOST_REQUIRE_EQUAL(sha256::hash(sha256::block_t{}), sha_full256);
}

// accumulator<sha256>::hash (cached)
BOOST_AUTO_TEST_CASE(sha__accumulator_sha256_hash__test_vectors__expected)
{
    // Verify non-const-evaluated to against public vectors.
    for (const auto& test: sha256_tests)
    {
        const auto hash = accumulator<sha256>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha256>::hash (uncached)
BOOST_AUTO_TEST_CASE(sha__uncached_accumulator_sha256_hash__test_vectors__expected)
{
    using sha_256 = sha::algorithm<sha::h256<>, true, true, false>;
    static_assert(!sha_256::cached);

    // Verify non-const-evaluated to against public vectors.
    for (const auto& test: sha256_tests)
    {
        const auto hash = accumulator<sha_256>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// sha256::hash
BOOST_AUTO_TEST_CASE(algorithm__sha256_hash__one_block__expected)
{
    static_assert(sha256::hash(std_array<sha256::block_t, 1>({ sha256::block_t{ 0 } })) == sha_full256);
    BOOST_CHECK_EQUAL(sha256::hash(sha256::block_t{ 0 }), sha_full256);
}

BOOST_AUTO_TEST_CASE(algorithm__sha256_hash__full_block__expected)
{
    static_assert(sha256::hash(sha256::block_t{ 0 }) == sha_full256);
    BOOST_CHECK_EQUAL(sha256::hash(sha256::block_t{ 0 }), sha_full256);
}

BOOST_AUTO_TEST_CASE(algorithm__sha256_hash__state__equals_byteswapped_half)
{
    constexpr auto expected = sha256::hash(sha256::half_t{ 42 });
    static_assert(sha256::hash(sha256::state_t{ byteswap(42_u32) }) == expected);
    BOOST_CHECK_EQUAL(sha256::hash(sha256::state_t{ byteswap(42_u32) }), expected);
}

BOOST_AUTO_TEST_CASE(algorithm__sha256_hash__half_block__expected)
{
    static_assert(sha256::hash(sha256::half_t{ 0 }) == sha_half256);
    BOOST_CHECK_EQUAL(sha256::hash(sha256::half_t{ 0 }), sha_half256);
}

BOOST_AUTO_TEST_CASE(algorithm__sha256_hash__half_blocks__expected)
{
    constexpr auto expected = sha256::hash(sha256::half_t{ 0 }, sha256::half_t{ 0 });
    static_assert(sha256::hash(sha256::block_t{ 0 }) == expected);
    BOOST_CHECK_EQUAL(sha256::hash({ 0 }, { 0 }), expected);
}

// sha256::double_hash
BOOST_AUTO_TEST_CASE(algorithm__sha256_double_hash__full_block__expected)
{
    constexpr auto expected = sha256::hash(sha256::hash(sha256::block_t{ 42 }));
    static_assert(sha256::double_hash(sha256::block_t{ 42 }) == expected);
    BOOST_CHECK_EQUAL(sha256::double_hash(sha256::block_t{ 42 }), expected);
}

BOOST_AUTO_TEST_CASE(algorithm__sha256_double_hash__half_block__expected)
{
    constexpr auto expected = sha256::hash(sha256::hash(sha256::half_t{ 42 }));
    static_assert(sha256::double_hash(sha256::half_t{ 42 }) == expected);
    BOOST_CHECK_EQUAL(sha256::double_hash(sha256::half_t{ 42 }), expected);
}

BOOST_AUTO_TEST_CASE(algorithm__sha256_double_hash__half_blocks__expected)
{
    constexpr auto expected = sha256::hash(sha256::hash(block<sha256>(0, 1)));
    static_assert(sha256::double_hash({ 0 }, { 1 }) == expected);
    BOOST_CHECK_EQUAL(sha256::double_hash({ 0 }, { 1 }), expected);
}

// sha256::merkle_hash
BOOST_AUTO_TEST_CASE(algorithm__sha256_merkle_hash__two__expected)
{
    constexpr auto expected = sha256::double_hash({ 0 }, { 1 });
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(merkle_hash<sha256, 0, 1>(expected));
#endif
    sha256::digests_t digests{ { 0 }, { 1 } };
    sha256::merkle_hash(digests);
    BOOST_CHECK_EQUAL(digests.size(), one);
    BOOST_CHECK_EQUAL(digests.front(), expected);
}

BOOST_AUTO_TEST_CASE(algorithm__sha256_merkle_hash__four__expected)
{
    constexpr auto expected1 = sha256::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha256::double_hash({ 2 }, { 3 });
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(merkle_hash<sha256, 0, 1, 2, 3>(expected1, expected2));
#endif
    sha256::digests_t digests{ { 0 }, { 1 }, { 2 }, { 3 } };
    sha256::merkle_hash(digests);
    BOOST_CHECK_EQUAL(digests.size(), two);
    BOOST_CHECK_EQUAL(digests.front(), expected1);
    BOOST_CHECK_EQUAL(digests.back(), expected2);
}

// sha256::merkle_root
BOOST_AUTO_TEST_CASE(algorithm__sha256_merkle_root__empty__null_long_hash)
{
    constexpr auto expected = null_hash;
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(sha256::merkle_root({}) == expected);
#endif
    BOOST_CHECK_EQUAL(sha256::merkle_root({}), expected);
}

BOOST_AUTO_TEST_CASE(algorithm__sha256_merkle_root__one__same)
{
    constexpr auto expected = sha256::digest_t{ 42 };
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(sha256::merkle_root({ { 42 } }) == expected);
#endif
    BOOST_CHECK_EQUAL(sha256::merkle_root({ { 42 } }), expected);
}

BOOST_AUTO_TEST_CASE(algorithm__sha256_merkle_root__two__expected)
{
    constexpr auto expected = sha256::double_hash({ 0 }, { 1 });
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(sha256::merkle_root({ { 0 }, { 1 } }) == expected);
#endif
    BOOST_CHECK_EQUAL(sha256::merkle_root({ { 0 }, { 1 } }), expected);
}

BOOST_AUTO_TEST_CASE(algorithm__sha256_merkle_root__three__expected)
{
    constexpr auto expected1 = sha256::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha256::double_hash({ 2 }, { 2 });
    constexpr auto expected = sha256::double_hash(expected1, expected2);
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(sha256::merkle_root({ { 0 }, { 1 }, { 2 } }) == expected);
#endif
    BOOST_CHECK_EQUAL(sha256::merkle_root({ { 0 }, { 1 }, { 2 } }), expected);
}

BOOST_AUTO_TEST_CASE(algorithm__sha256_merkle_root__four__expected)
{
    constexpr auto expected1 = sha256::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha256::double_hash({ 2 }, { 3 });
    constexpr auto expected = sha256::double_hash(expected1, expected2);
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(sha256::merkle_root({ { 0 }, { 1 }, { 2 }, { 3 } }) == expected);
#endif
    BOOST_CHECK_EQUAL(sha256::merkle_root({ { 0 }, { 1 }, { 2 }, { 3 } }), expected);
}

// sha512
// ----------------------------------------------------------------------------
static_assert(sha512::cached);

// sha512::hash (cached)
BOOST_AUTO_TEST_CASE(sha__sha512_hash__null_hash__expected)
{
    // Correlate non-const-evaluated to const-evaluated.
    BOOST_REQUIRE_EQUAL(sha512::hash(sha512::half_t{}), sha_half512);
    BOOST_REQUIRE_EQUAL(sha512::hash(sha512::block_t{}), sha_full512);
}

// accumulator<sha512>::hash (cached)
BOOST_AUTO_TEST_CASE(sha__accumulator_sha512_hash__test_vectors__expected)
{
    // Verify non-const-evaluated to against public vectors.
    for (const auto& test: sha512_tests)
    {
        const auto hash = accumulator<sha512>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha512>::hash (uncached)
BOOST_AUTO_TEST_CASE(sha__uncached_accumulator_sha512_hash__test_vectors__expected)
{
    using sha_512 = sha::algorithm<sha::h512<>, true, true, false>;
    static_assert(!sha_512::cached);

    // Verify non-const-evaluated to against public vectors.
    for (const auto& test: sha512_tests)
    {
        const auto hash = accumulator<sha_512>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// sha512::hash
BOOST_AUTO_TEST_CASE(algorithm__sha512_hash__one_block__expected)
{
    static_assert(sha512::hash(std_array<sha512::block_t, 1>({ sha512::block_t{ 0 } })) == sha_full512);
    BOOST_CHECK_EQUAL(sha512::hash(sha512::block_t{ 0 }), sha_full512);
}

BOOST_AUTO_TEST_CASE(algorithm__sha512_hash__full_block__expected)
{
    static_assert(sha512::hash(sha512::block_t{ 0 }) == sha_full512);
    BOOST_CHECK_EQUAL(sha512::hash(sha512::block_t{ 0 }), sha_full512);
}

BOOST_AUTO_TEST_CASE(algorithm__sha512_hash__state__equals_byteswapped_half)
{
    constexpr auto expected = sha512::hash(sha512::half_t{ 42 });
    static_assert(sha512::hash(sha512::state_t{ byteswap(42_u64) }) == expected);
    BOOST_CHECK_EQUAL(sha512::hash(sha512::state_t{ byteswap(42_u64) }), expected);
}

BOOST_AUTO_TEST_CASE(algorithm__sha512_hash__half_block__expected)
{
    static_assert(sha512::hash(sha512::half_t{ 0 }) == sha_half512);
    BOOST_CHECK_EQUAL(sha512::hash(sha512::half_t{ 0 }), sha_half512);
}

BOOST_AUTO_TEST_CASE(algorithm__sha512_hash__half_blocks__expected)
{
    constexpr auto expected = sha512::hash(sha512::half_t{ 0 }, sha512::half_t{ 0 });
    static_assert(sha512::hash(sha512::block_t{ 0 }) == expected);
    BOOST_CHECK_EQUAL(sha512::hash({ 0 }, { 0 }), expected);
}

// sha512::double_hash
BOOST_AUTO_TEST_CASE(algorithm__sha512_double_hash__full_block__expected)
{
    constexpr auto expected = sha512::hash(sha512::hash(sha512::block_t{ 42 }));
    static_assert(sha512::double_hash(sha512::block_t{ 42 }) == expected);
    BOOST_CHECK_EQUAL(sha512::double_hash(sha512::block_t{ 42 }), expected);
}

BOOST_AUTO_TEST_CASE(algorithm__sha512_double_hash__half_block__expected)
{
    constexpr auto expected = sha512::hash(sha512::hash(sha512::half_t{ 42 }));
    static_assert(sha512::double_hash(sha512::half_t{ 42 }) == expected);
    BOOST_CHECK_EQUAL(sha512::double_hash(sha512::half_t{ 42 }), expected);
}

BOOST_AUTO_TEST_CASE(algorithm__sha512_double_hash__half_blocks__expected)
{
    constexpr auto expected = sha512::hash(sha512::hash(block<sha512>(0, 1)));
    static_assert(sha512::double_hash({ 0 }, { 1 }) == expected);
    BOOST_CHECK_EQUAL(sha512::double_hash({ 0 }, { 1 }), expected);
}

// sha512::merkle_hash
BOOST_AUTO_TEST_CASE(algorithm__sha512_merkle_hash__two__expected)
{
    constexpr auto expected = sha512::double_hash({ 0 }, { 1 });
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(merkle_hash<sha512, 0, 1>(expected));
#endif
    sha512::digests_t digests{ { 0 }, { 1 } };
    sha512::merkle_hash(digests);
    BOOST_CHECK_EQUAL(digests.size(), one);
    BOOST_CHECK_EQUAL(digests.front(), expected);
}

BOOST_AUTO_TEST_CASE(algorithm__sha512_merkle_hash__four__expected)
{
    constexpr auto expected1 = sha512::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha512::double_hash({ 2 }, { 3 });
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(merkle_hash<sha512, 0, 1, 2, 3>(expected1, expected2));
#endif
    sha512::digests_t digests{ { 0 }, { 1 }, { 2 }, { 3 } };
    sha512::merkle_hash(digests);
    BOOST_CHECK_EQUAL(digests.size(), two);
    BOOST_CHECK_EQUAL(digests.front(), expected1);
    BOOST_CHECK_EQUAL(digests.back(), expected2);
}

// sha512::merkle_root
BOOST_AUTO_TEST_CASE(algorithm__sha512_merkle_root__empty__null_long_hash)
{
    constexpr auto expected = null_long_hash;
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(sha512::merkle_root({}) == expected);
#endif
    BOOST_CHECK_EQUAL(sha512::merkle_root({}), expected);
}

BOOST_AUTO_TEST_CASE(algorithm__sha512_merkle_root__one__same)
{
    constexpr auto expected = sha512::digest_t{ 42 };
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(sha512::merkle_root({ { 42 } }) == expected);
#endif
    BOOST_CHECK_EQUAL(sha512::merkle_root({ { 42 } }), expected);
}

BOOST_AUTO_TEST_CASE(algorithm__sha512_merkle_root__two__expected)
{
    constexpr auto expected = sha512::double_hash({ 0 }, { 1 });
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(sha512::merkle_root({ { 0 }, { 1 } }) == expected);
#endif
    BOOST_CHECK_EQUAL(sha512::merkle_root({ { 0 }, { 1 } }), expected);
}

BOOST_AUTO_TEST_CASE(algorithm__sha512_merkle_root__three__expected)
{
    constexpr auto expected1 = sha512::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha512::double_hash({ 2 }, { 2 });
    constexpr auto expected = sha512::double_hash(expected1, expected2);
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(sha512::merkle_root({ { 0 }, { 1 }, { 2 } }) == expected);
#endif
    BOOST_CHECK_EQUAL(sha512::merkle_root({ { 0 }, { 1 }, { 2 } }), expected);
}

BOOST_AUTO_TEST_CASE(algorithm__sha512_merkle_root__four__expected)
{
    constexpr auto expected1 = sha512::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha512::double_hash({ 2 }, { 3 });
    constexpr auto expected = sha512::double_hash(expected1, expected2);
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(sha512::merkle_root({ { 0 }, { 1 }, { 2 }, { 3 } }) == expected);
#endif
    BOOST_CHECK_EQUAL(sha512::merkle_root({ { 0 }, { 1 }, { 2 }, { 3 } }), expected);
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
static_assert(h<k160>::strength == 160);
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
static_assert(h<k256, 256>::strength == 256);
static_assert(h<k256, 224>::strength == 256);
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
static_assert(h<k512, 512>::strength == 512);
static_assert(h<k512, 384>::strength == 512);
static_assert(h<k512, 224>::strength == 512);
static_assert(h<k512, 256>::strength == 512);
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
static_assert(h160::rounds == 80);
static_assert(h160::digest == 160);
static_assert(h160::strength == 160);
static_assert(h160::get[0] == 0x67452301);
static_assert(h160::get[4] == 0xc3d2e1f0);
static_assert(h160::get.size() == 5);
static_assert(h160::K::rounds == 80);
static_assert(h160::K::get[0] == 0x5a827999);
static_assert(h160::K::get[79] == 0xca62c1d6);
static_assert(h160::K::get.size() == 80);

// h256<>
static_assert(h256<>::size == 256);
static_assert(h256<>::rounds == 64);
static_assert(h256<>::digest == 256);
static_assert(h256<>::strength == 256);
static_assert(h256<>::get[0] == 0x6a09e667);
static_assert(h256<>::get[7] == 0x5be0cd19);
static_assert(h256<>::get.size() == 8);
static_assert(h256<>::K::rounds == 64);
static_assert(h256<>::K::get[0] == 0x428a2f98);
static_assert(h256<>::K::get[63] == 0xc67178f2);
static_assert(h256<>::K::get.size() == 64);

// h512<>
static_assert(h512<>::size == 512);
static_assert(h512<>::rounds == 80);
static_assert(h512<>::digest == 512);
static_assert(h512<>::strength == 512);
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
static_assert(is_same_type<decltype(sha512::limit_bits), const uint128_t>);
static_assert(is_same_type<decltype(sha512::limit_bytes), const uint128_t>);

// Truncations.
static_assert(sha256_224::big_end_count);
static_assert(sha256_224::big_end_count);
static_assert(sha512_256::big_end_count);
static_assert(sha512_224::big_end_count);
static_assert(sha512_384::big_end_count);

BOOST_AUTO_TEST_SUITE_END()


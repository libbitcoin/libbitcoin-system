/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
    
BOOST_AUTO_TEST_SUITE(sha512_tests_)

constexpr auto vectorized = (with_sse41 || with_avx2 || with_avx512) && !build_x32;
constexpr auto compressed = with_shani || with_neon;

BOOST_AUTO_TEST_CASE(sha512__hash__null_hash__expected)
{
    // Correlate non-const-evaluated to const-evaluated.
    BOOST_REQUIRE_EQUAL(sha512::hash(sha512::half_t{}), sha_half512);
    BOOST_REQUIRE_EQUAL(sha512::hash(sha512::block_t{}), sha_full512);
}

// accumulator<sha512>::hash (false, false, false)
BOOST_AUTO_TEST_CASE(sha512__fff_accumulator_hash__test_vectors__expected)
{
    using sha_512 = sha::algorithm<sha::h512<>, false, false, false>;
    static_assert(!sha_512::compression);
    static_assert(!sha_512::vectorization);
    static_assert(!sha_512::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_tests)
    {
        const auto hash = accumulator<sha_512>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha512>::hash (true, false, false)
BOOST_AUTO_TEST_CASE(sha512__tff_accumulator_hash__test_vectors__expected)
{
    using sha_512 = sha::algorithm<sha::h512<>, true, false, false>;
    static_assert(sha_512::compression == compressed);
    static_assert(!sha_512::vectorization);
    static_assert(!sha_512::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_tests)
    {
        const auto hash = accumulator<sha_512>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha512>::hash (true, false, true)
BOOST_AUTO_TEST_CASE(sha512__tft_accumulator_hash__test_vectors__expected)
{
    using sha_512 = sha::algorithm<sha::h512<>, true, false, true>;
    static_assert(sha_512::compression == compressed);
    static_assert(!sha_512::vectorization);
    static_assert(sha_512::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_tests)
    {
        const auto hash = accumulator<sha_512>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha512>::hash (true, true, false)
BOOST_AUTO_TEST_CASE(sha512__ttf_accumulator_hash__test_vectors__expected)
{
    using sha_512 = sha::algorithm<sha::h512<>, true, true, false>;
    static_assert(sha_512::compression == compressed);
    static_assert(sha_512::vectorization == vectorized);
    static_assert(!sha_512::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_tests)
    {
        const auto hash = accumulator<sha_512>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha512>::hash (true, true, true)
BOOST_AUTO_TEST_CASE(sha512__ttt_accumulator_hash__test_vectors__expected)
{
    using sha_512 = sha::algorithm<sha::h512<>, true, true, true>;
    static_assert(sha_512::compression == compressed);
    static_assert(sha_512::vectorization == vectorized);
    static_assert(sha_512::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_tests)
    {
        const auto hash = accumulator<sha_512>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha512>::hash (false, true, false)
BOOST_AUTO_TEST_CASE(sha512__ftf_accumulator_hash__test_vectors__expected)
{
    using sha_512 = sha::algorithm<sha::h512<>, false, true, false>;
    static_assert(!sha_512::compression);
    static_assert(sha_512::vectorization == vectorized);
    static_assert(!sha_512::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_tests)
    {
        const auto hash = accumulator<sha_512>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// sha512::hash
BOOST_AUTO_TEST_CASE(sha512__hash__one_block__expected)
{
    static_assert(sha512::hash(std_array<sha512::block_t, 1>({ sha512::block_t{ 0 } })) == sha_full512);
    BOOST_CHECK_EQUAL(sha512::hash(sha512::block_t{ 0 }), sha_full512);
}

BOOST_AUTO_TEST_CASE(sha512__hash__full_block__expected)
{
    static_assert(sha512::hash(sha512::block_t{ 0 }) == sha_full512);
    BOOST_CHECK_EQUAL(sha512::hash(sha512::block_t{ 0 }), sha_full512);
}

BOOST_AUTO_TEST_CASE(sha512__hash__state__equals_byteswapped_half)
{
    constexpr auto expected = sha512::hash(sha512::half_t{ 42 });
    static_assert(sha512::hash(sha512::state_t{ byteswap(42_u64) }) == expected);
    BOOST_CHECK_EQUAL(sha512::hash(sha512::state_t{ byteswap(42_u64) }), expected);
}

BOOST_AUTO_TEST_CASE(sha512__hash__half_block__expected)
{
    static_assert(sha512::hash(sha512::half_t{ 0 }) == sha_half512);
    BOOST_CHECK_EQUAL(sha512::hash(sha512::half_t{ 0 }), sha_half512);
}

BOOST_AUTO_TEST_CASE(sha512__hash__half_blocks__expected)
{
    constexpr auto expected = sha512::hash(sha512::half_t{ 0 }, sha512::half_t{ 0 });
    static_assert(sha512::hash(sha512::block_t{ 0 }) == expected);
    BOOST_CHECK_EQUAL(sha512::hash(sha512::half_t{ 0 }, sha512::half_t{ 0 }), expected);
}

// sha512::double_hash
BOOST_AUTO_TEST_CASE(sha512__double_hash__full_block__expected)
{
    constexpr auto expected = sha512::hash(sha512::hash(sha512::block_t{ 42 }));
    static_assert(sha512::double_hash(sha512::block_t{ 42 }) == expected);
    BOOST_CHECK_EQUAL(sha512::double_hash(sha512::block_t{ 42 }), expected);
}

BOOST_AUTO_TEST_CASE(sha512__double_hash__half_block__expected)
{
    constexpr auto expected = sha512::hash(sha512::hash(sha512::half_t{ 42 }));
    static_assert(sha512::double_hash(sha512::half_t{ 42 }) == expected);
    BOOST_CHECK_EQUAL(sha512::double_hash(sha512::half_t{ 42 }), expected);
}

BOOST_AUTO_TEST_CASE(sha512__double_hash__half_blocks__expected)
{
    constexpr auto expected = sha512::hash(sha512::hash(block<sha512>(0, 1)));
    static_assert(sha512::double_hash({ 0 }, { 1 }) == expected);
    BOOST_CHECK_EQUAL(sha512::double_hash({ 0 }, { 1 }), expected);
}

// sha512::merkle_hash
BOOST_AUTO_TEST_CASE(sha512__merkle_hash__two__expected)
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

BOOST_AUTO_TEST_CASE(sha512__merkle_hash__four__expected)
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
BOOST_AUTO_TEST_CASE(sha512__merkle_root__empty__null_long_hash)
{
    constexpr auto expected = null_long_hash;
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(sha512::merkle_root({}) == expected);
#endif
    BOOST_CHECK_EQUAL(sha512::merkle_root({}), expected);
}

BOOST_AUTO_TEST_CASE(sha512__merkle_root__one__same)
{
    constexpr auto expected = sha512::digest_t{ 42 };
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(sha512::merkle_root({ { 42 } }) == expected);
#endif
    BOOST_CHECK_EQUAL(sha512::merkle_root({ { 42 } }), expected);
}

BOOST_AUTO_TEST_CASE(sha512__merkle_root__two__expected)
{
    constexpr auto expected = sha512::double_hash({ 0 }, { 1 });
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(sha512::merkle_root({ { 0 }, { 1 } }) == expected);
#endif
    BOOST_CHECK_EQUAL(sha512::merkle_root({ { 0 }, { 1 } }), expected);
}

BOOST_AUTO_TEST_CASE(sha512__merkle_root__three__expected)
{
    constexpr auto expected1 = sha512::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha512::double_hash({ 2 }, { 2 });
    constexpr auto expected = sha512::double_hash(expected1, expected2);
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(sha512::merkle_root({ { 0 }, { 1 }, { 2 } }) == expected);
#endif
    BOOST_CHECK_EQUAL(sha512::merkle_root({ { 0 }, { 1 }, { 2 } }), expected);
}

BOOST_AUTO_TEST_CASE(sha512__merkle_root__four__expected)
{
    constexpr auto expected1 = sha512::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha512::double_hash({ 2 }, { 3 });
    constexpr auto expected = sha512::double_hash(expected1, expected2);
#if defined (HAVE_VECTOR_CONSTEXPR)
    static_assert(sha512::merkle_root({ { 0 }, { 1 }, { 2 }, { 3 } }) == expected);
#endif
    BOOST_CHECK_EQUAL(sha512::merkle_root({ { 0 }, { 1 }, { 2 }, { 3 } }), expected);
}

BOOST_AUTO_TEST_SUITE_END()

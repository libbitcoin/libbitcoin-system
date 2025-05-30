/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

constexpr auto vector = have_128 || have_256 || have_512;
constexpr auto native = /*have_sha*/ false;

// Other test vectors are dependent upon the correctness of these.
static_assert(sha512::hash(sha512::byte_t{}) == sha_byte512);
static_assert(sha512::hash(sha512::half_t{}) == sha_half512);
static_assert(sha512::hash(sha512::block_t{}) == sha_full512);

BOOST_AUTO_TEST_CASE(sha512__hash__null_hash__expected)
{
    // Correlate non-const-evaluated to const-evaluated.
    BOOST_REQUIRE_EQUAL(sha512::hash(sha512::byte_t{}), sha_byte512);
    BOOST_REQUIRE_EQUAL(sha512::hash(sha512::half_t{}), sha_half512);
    BOOST_REQUIRE_EQUAL(sha512::hash(sha512::block_t{}), sha_full512);
}

// accumulator<sha512>::hash (false, false, false)
BOOST_AUTO_TEST_CASE(sha512_512__fff_accumulator_hash__test_vectors__expected)
{
    using sha_512 = sha::algorithm<sha::h512<>, false, false, false>;
    static_assert(!sha_512::native);
    static_assert(!sha_512::vector);
    static_assert(!sha_512::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_512_tests)
    {
        const auto hash = accumulator<sha_512>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha512>::hash (true, false, false)
BOOST_AUTO_TEST_CASE(sha512_512__tff_accumulator_hash__test_vectors__expected)
{
    using sha_512 = sha::algorithm<sha::h512<>, true, false, false>;
    static_assert(sha_512::native == native);
    static_assert(!sha_512::vector);
    static_assert(!sha_512::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_512_tests)
    {
        const auto hash = accumulator<sha_512>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha512>::hash (true, false, true)
BOOST_AUTO_TEST_CASE(sha512_512__tft_accumulator_hash__test_vectors__expected)
{
    using sha_512 = sha::algorithm<sha::h512<>, true, false, true>;
    static_assert(sha_512::native == native);
    static_assert(!sha_512::vector);
    static_assert(sha_512::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_512_tests)
    {
        const auto hash = accumulator<sha_512>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha512>::hash (true, true, false)
BOOST_AUTO_TEST_CASE(sha512_512__ttf_accumulator_hash__test_vectors__expected)
{
    using sha_512 = sha::algorithm<sha::h512<>, true, true, false>;
    static_assert(sha_512::native == native);
    static_assert(sha_512::vector == vector);
    static_assert(!sha_512::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_512_tests)
    {
        const auto hash = accumulator<sha_512>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha512>::hash (true, true, true)
BOOST_AUTO_TEST_CASE(sha512_512__ttt_accumulator_hash__test_vectors__expected)
{
    using sha_512 = sha::algorithm<sha::h512<>, true, true, true>;
    static_assert(sha_512::native == native);
    static_assert(sha_512::vector == vector);
    static_assert(sha_512::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_512_tests)
    {
        const auto hash = accumulator<sha_512>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha512>::hash (false, true, false)
BOOST_AUTO_TEST_CASE(sha512_512__ftf_accumulator_hash__test_vectors__expected)
{
    using sha_512 = sha::algorithm<sha::h512<512>, false, true, false>;
    static_assert(!sha_512::native);
    static_assert(sha_512::vector == vector);
    static_assert(!sha_512::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_512_tests)
    {
        const auto hash = accumulator<sha_512>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha512_384>::hash (true, true, true)
BOOST_AUTO_TEST_CASE(sha512_384__ttt_accumulator_hash__test_vectors__expected)
{
    using sha_512_384 = sha::algorithm<sha::h512<384>, true, true, true>;
    static_assert(sha_512_384::native == native);
    static_assert(sha_512_384::vector == vector);
    static_assert(sha_512_384::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_384_tests)
    {
        const auto hash = accumulator<sha_512_384>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha512_384>::hash (false, false, false)
BOOST_AUTO_TEST_CASE(sha512_384__fff_accumulator_hash__test_vectors__expected)
{
    using sha_512_384 = sha::algorithm<sha::h512<384>, false, false, false>;
    static_assert(!sha_512_384::native);
    static_assert(!sha_512_384::vector);
    static_assert(!sha_512_384::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_384_tests)
    {
        const auto hash = accumulator<sha_512_384>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha512_256>::hash (true, true, true)
BOOST_AUTO_TEST_CASE(sha512_256__ttt_accumulator_hash__test_vectors__expected)
{
    using sha_512_256 = sha::algorithm<sha::h512<256>, true, true, true>;
    static_assert(sha_512_256::native == native);
    static_assert(sha_512_256::vector == vector);
    static_assert(sha_512_256::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_256_tests)
    {
        const auto hash = accumulator<sha_512_256>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha512_256>::hash (false, false, false)
BOOST_AUTO_TEST_CASE(sha512_256__fff_accumulator_hash__test_vectors__expected)
{
    using sha_512_256 = sha::algorithm<sha::h512<256>, false, false, false>;
    static_assert(!sha_512_256::native);
    static_assert(!sha_512_256::vector);
    static_assert(!sha_512_256::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_256_tests)
    {
        const auto hash = accumulator<sha_512_256>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha512_224>::hash (true, true, true)
BOOST_AUTO_TEST_CASE(sha512_224__ttt_accumulator_hash__test_vectors__expected)
{
    using sha_512_224 = sha::algorithm<sha::h512<224>, true, true, true>;
    static_assert(sha_512_224::native == native);
    static_assert(sha_512_224::vector == vector);
    static_assert(sha_512_224::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_224_tests)
    {
        const auto hash = accumulator<sha_512_224>::hash(test.data);
        BOOST_REQUIRE_EQUAL(hash, test.expected);
    }
}

// accumulator<sha512_224>::hash (false, false, false)
BOOST_AUTO_TEST_CASE(sha512_224__fff_accumulator_hash__test_vectors__expected)
{
    using sha_512_224 = sha::algorithm<sha::h512<224>, false, false, false>;
    static_assert(!sha_512_224::native);
    static_assert(!sha_512_224::vector);
    static_assert(!sha_512_224::caching);

    // Verify non-const-evaluated against public vectors.
    for (const auto& test: sha512_224_tests)
    {
        const auto hash = accumulator<sha_512_224>::hash(test.data);
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
    static_assert(sha512::finalize_second(sha512::state_t{ byteswap(42_u64) }) == expected);
    BOOST_CHECK_EQUAL(sha512::finalize_second(sha512::state_t{ byteswap(42_u64) }), expected);
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

BOOST_AUTO_TEST_CASE(sha512__hash__quart_blocks__expected)
{
    constexpr auto expected = sha512::hash(sha512::quart_t{ 0 }, sha512::quart_t{ 0 });
    static_assert(sha512::hash(sha512::half_t{ 0 }) == expected);
    BOOST_CHECK_EQUAL(sha512::hash(sha512::quart_t{ 0 }, sha512::quart_t{ 0 }), expected);
}

// sha512::midstate
BOOST_AUTO_TEST_CASE(sha512__midstate__half_blocks__expected)
{
    constexpr sha512::state_t expected
    {
        0xcf7881d5774acbe8,
        0x533362e0fbc78070,
        0x0267639d87460eda,
        0x3086cb40e85931b0,
        0x717dc95288a023a3,
        0x96bab2c14ce0b5e0,
        0x6fc4fe04eae33e0b,
        0x91f4d80cbd668bee
    };

    auto copy = expected;

    static_assert(sha512::midstate(sha512::half_t{ 0 }, sha512::half_t{ 0 }) == expected);
    BOOST_CHECK_EQUAL(sha512::midstate(sha512::half_t{ 0 }, sha512::half_t{ 0 }), expected);
    BOOST_CHECK_EQUAL(sha512::finalize(copy, one), sha512::hash(sha512::half_t{ 0 }, sha512::half_t{ 0 }));
}

// sha512::simple_hash
BOOST_AUTO_TEST_CASE(sha512__simple_hash__minimal__expected)
{
    BOOST_CHECK_EQUAL(sha512::simple_hash(data_array<zero>{}), accumulator<sha512>::hash(data_array<zero>{}));
}

BOOST_AUTO_TEST_CASE(sha512__simple_hash__byte__expected)
{
    static_assert(sha512::simple_hash(data_array<one>{ 0 }) == sha_byte512);
    BOOST_CHECK_EQUAL(sha512::simple_hash(data_array<one>{ 0 }), accumulator<sha512>::hash(data_array<one>{ 0 }));
}

BOOST_AUTO_TEST_CASE(sha512__simple_hash__maximal__expected)
{
    BOOST_CHECK_EQUAL(sha512::simple_hash(data_array<sha512::space>{ 0 }), accumulator<sha512>::hash(data_array<sha512::space>{ 0 }));
}

BOOST_AUTO_TEST_CASE(sha512__simple_hash__quart_block__expected)
{
    BOOST_CHECK_EQUAL(sha512::simple_hash(sha512::quart_t{ 0 }), accumulator<sha512>::hash(sha512::quart_t{ 0 }));
}

BOOST_AUTO_TEST_CASE(sha512__simple_hash__half_block__expected)
{
    static_assert(sha512::simple_hash(sha512::half_t{ 0 }) == sha_half512);
    BOOST_CHECK_EQUAL(sha512::simple_hash(sha512::half_t{ 0 }), accumulator<sha512>::hash(sha512::half_t{ 0 }));
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
    static_assert(merkle_hash<sha512, 0, 1>(expected));
    sha512::digests_t digests{ { 0 }, { 1 } };
    sha512::merkle_hash(digests);
    BOOST_CHECK_EQUAL(digests.size(), one);
    BOOST_CHECK_EQUAL(digests.front(), expected);
}

BOOST_AUTO_TEST_CASE(sha512__merkle_hash__four__expected)
{
    constexpr auto expected1 = sha512::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha512::double_hash({ 2 }, { 3 });
    static_assert(merkle_hash<sha512, 0, 1, 2, 3>(expected1, expected2));
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
    static_assert(sha512::merkle_root({}) == expected);
    BOOST_CHECK_EQUAL(sha512::merkle_root({}), expected);
}

BOOST_AUTO_TEST_CASE(sha512__merkle_root__one__same)
{
    constexpr auto expected = sha512::digest_t{ 42 };
    static_assert(sha512::merkle_root({ { 42 } }) == expected);
    BOOST_CHECK_EQUAL(sha512::merkle_root({ { 42 } }), expected);
}

BOOST_AUTO_TEST_CASE(sha512__merkle_root__two__expected)
{
    constexpr auto expected = sha512::double_hash({ 0 }, { 1 });
    static_assert(sha512::merkle_root({ { 0 }, { 1 } }) == expected);
    BOOST_CHECK_EQUAL(sha512::merkle_root({ { 0 }, { 1 } }), expected);
}

BOOST_AUTO_TEST_CASE(sha512__merkle_root__three__expected)
{
    constexpr auto expected1 = sha512::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha512::double_hash({ 2 }, { 2 });
    constexpr auto expected = sha512::double_hash(expected1, expected2);
    static_assert(sha512::merkle_root({ { 0 }, { 1 }, { 2 } }) == expected);
    BOOST_CHECK_EQUAL(sha512::merkle_root({ { 0 }, { 1 }, { 2 } }), expected);
}

BOOST_AUTO_TEST_CASE(sha512__merkle_root__four__expected)
{
    constexpr auto expected1 = sha512::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha512::double_hash({ 2 }, { 3 });
    constexpr auto expected = sha512::double_hash(expected1, expected2);
    static_assert(sha512::merkle_root({ { 0 }, { 1 }, { 2 }, { 3 } }) == expected);
    BOOST_CHECK_EQUAL(sha512::merkle_root({ { 0 }, { 1 }, { 2 }, { 3 } }), expected);
}

BOOST_AUTO_TEST_SUITE_END()

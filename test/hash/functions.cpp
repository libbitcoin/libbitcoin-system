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
#include "../test.hpp"
#include "hash.hpp"

BOOST_AUTO_TEST_SUITE(functions_tests)

constexpr auto text = "foobar";
const std::string string{ text };
const data_chunk data{ 'f', 'o', 'o', 'b', 'a', 'r' };

// rmd128_hash/rmd128_chunk
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(functions__rmd128__text__expected)
{
    const auto expected = rmd128_chunk(string);
    BOOST_CHECK_EQUAL(rmd128_chunk(data), expected);
    BOOST_CHECK_EQUAL(rmd128_chunk("foobar"), expected);
    BOOST_CHECK_EQUAL(rmd128_chunk(std::string{ "foobar" }), expected);
}

BOOST_AUTO_TEST_CASE(functions__rmd128__empty__expected)
{
    constexpr auto expected = base16_array("cdf26213a150dc3ecb610f18f6b38b46");
    BOOST_CHECK_EQUAL(accumulator<rmd128>::hash(zero, nullptr), expected);
}

BOOST_AUTO_TEST_CASE(functions__rmd128__null_half__expected)
{
    BOOST_CHECK_EQUAL(accumulator<rmd128>::hash(rmd128::half_t{}), rmd_half128);
    BOOST_CHECK_EQUAL(rmd128_hash(rmd128::half_t{}), rmd_half128);
    BOOST_CHECK_EQUAL(rmd128_hash(to_chunk(rmd128::half_t{})), rmd_half128);
    BOOST_CHECK_EQUAL(rmd128_chunk(rmd128::half_t{}), to_chunk(rmd_half128));
    BOOST_CHECK_EQUAL(rmd128_chunk(to_chunk(rmd128::half_t{})), to_chunk(rmd_half128));
}

BOOST_AUTO_TEST_CASE(functions__rmd128__null_one__expected)
{
    BOOST_CHECK_EQUAL(accumulator<rmd128>::hash(rmd128::block_t{}), rmd_full128);
    BOOST_CHECK_EQUAL(rmd128_hash(rmd128::block_t{}), rmd_full128);
    BOOST_CHECK_EQUAL(rmd128_hash(to_chunk(rmd128::block_t{})), rmd_full128);
    BOOST_CHECK_EQUAL(rmd128_chunk(rmd128::block_t{}), to_chunk(rmd_full128));
    BOOST_CHECK_EQUAL(rmd128_chunk(to_chunk(rmd128::block_t{})), to_chunk(rmd_full128));
}

BOOST_AUTO_TEST_CASE(functions__rmd128__null_two__expected)
{
    constexpr auto pair = std_array<uint8_t, array_count<rmd128::block_t> * two>{};
    constexpr auto expected = base16_array("1b94bc163383151a53fe49dadb7a4f0e");
    BOOST_CHECK_EQUAL(accumulated<rmd128>(rmd128::block_t{}, rmd128::block_t{}), expected);
    BOOST_CHECK_EQUAL(rmd128_hash(pair), expected);
    BOOST_CHECK_EQUAL(rmd128_hash(to_chunk(pair)), expected);
    BOOST_CHECK_EQUAL(rmd128_chunk(pair), to_chunk(expected));
    BOOST_CHECK_EQUAL(rmd128_chunk(to_chunk(pair)), to_chunk(expected));
}

// rmd160_hash/rmd160_chunk
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(functions__rmd160__text__expected)
{
    const auto expected = rmd160_chunk(string);
    BOOST_CHECK_EQUAL(rmd160_chunk(data), expected);
    BOOST_CHECK_EQUAL(rmd160_chunk("foobar"), expected);
    BOOST_CHECK_EQUAL(rmd160_chunk(std::string{ "foobar" }), expected);
}

BOOST_AUTO_TEST_CASE(functions__rmd160__empty__expected)
{
    constexpr auto expected = base16_array("9c1185a5c5e9fc54612808977ee8f548b2258d31");
    BOOST_CHECK_EQUAL(accumulator<rmd160>::hash(zero, nullptr), expected);
}

BOOST_AUTO_TEST_CASE(functions__rmd160__null_half__expected)
{
    BOOST_CHECK_EQUAL(accumulator<rmd160>::hash(rmd160::half_t{}), rmd_half160);
    BOOST_CHECK_EQUAL(rmd160_hash(rmd160::half_t{}), rmd_half160);
    BOOST_CHECK_EQUAL(rmd160_hash(to_chunk(rmd160::half_t{})), rmd_half160);
    BOOST_CHECK_EQUAL(rmd160_chunk(rmd160::half_t{}), to_chunk(rmd_half160));
    BOOST_CHECK_EQUAL(rmd160_chunk(to_chunk(rmd160::half_t{})), to_chunk(rmd_half160));
}

BOOST_AUTO_TEST_CASE(functions__rmd160__null_one__expected)
{
    BOOST_CHECK_EQUAL(accumulator<rmd160>::hash(rmd160::block_t{}), rmd_full160);
    BOOST_CHECK_EQUAL(rmd160_hash(rmd160::block_t{}), rmd_full160);
    BOOST_CHECK_EQUAL(rmd160_hash(to_chunk(rmd160::block_t{})), rmd_full160);
    BOOST_CHECK_EQUAL(rmd160_chunk(rmd160::block_t{}), to_chunk(rmd_full160));
    BOOST_CHECK_EQUAL(rmd160_chunk(to_chunk(rmd160::block_t{})), to_chunk(rmd_full160));
}

BOOST_AUTO_TEST_CASE(functions__rmd160__null_two__expected)
{
    constexpr auto pair = std_array<uint8_t, array_count<rmd160::block_t> * two>{};
    constexpr auto expected = base16_array("4300a157335cb7c9fc9423e011d7dd51090d093f");
    BOOST_CHECK_EQUAL(accumulated<rmd160>(rmd160::block_t{}, rmd160::block_t{}), expected);
    BOOST_CHECK_EQUAL(rmd160_hash(pair), expected);
    BOOST_CHECK_EQUAL(rmd160_hash(to_chunk(pair)), expected);
    BOOST_CHECK_EQUAL(rmd160_chunk(pair), to_chunk(expected));
    BOOST_CHECK_EQUAL(rmd160_chunk(to_chunk(pair)), to_chunk(expected));
}

// sha1_hash/sha1_chunk
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(functions__sha1__text__expected)
{
    const auto expected = sha1_chunk(string);
    BOOST_CHECK_EQUAL(sha1_chunk(data), expected);
    BOOST_CHECK_EQUAL(sha1_chunk("foobar"), expected);
    BOOST_CHECK_EQUAL(sha1_chunk(std::string{ "foobar" }), expected);
}

BOOST_AUTO_TEST_CASE(functions__sha1__empty__expected)
{
    constexpr auto expected = base16_array("da39a3ee5e6b4b0d3255bfef95601890afd80709");
    BOOST_CHECK_EQUAL(accumulator<sha160>::hash(zero, nullptr), expected);
}

BOOST_AUTO_TEST_CASE(functions__sha1__null_half__expected)
{
    BOOST_CHECK_EQUAL(accumulator<sha160>::hash(sha160::half_t{}), sha_half160);
    BOOST_CHECK_EQUAL(sha1_hash(sha160::half_t{}), sha_half160);
    BOOST_CHECK_EQUAL(sha1_hash(to_chunk(sha160::half_t{})), sha_half160);
    BOOST_CHECK_EQUAL(sha1_chunk(sha160::half_t{}), to_chunk(sha_half160));
    BOOST_CHECK_EQUAL(sha1_chunk(to_chunk(sha160::half_t{})), to_chunk(sha_half160));
}

BOOST_AUTO_TEST_CASE(functions__sha1__null_one__expected)
{
    BOOST_CHECK_EQUAL(accumulator<sha160>::hash(sha160::block_t{}), sha_full160);
    BOOST_CHECK_EQUAL(sha1_hash(sha160::block_t{}), sha_full160);
    BOOST_CHECK_EQUAL(sha1_hash(to_chunk(sha160::block_t{})), sha_full160);
    BOOST_CHECK_EQUAL(sha1_chunk(sha160::block_t{}), to_chunk(sha_full160));
    BOOST_CHECK_EQUAL(sha1_chunk(to_chunk(sha160::block_t{})), to_chunk(sha_full160));
}

BOOST_AUTO_TEST_CASE(functions__sha1__null_two__expected)
{
    constexpr auto pair = std_array<uint8_t, array_count<sha160::block_t> * two>{};
    constexpr auto expected = base16_array("0ae4f711ef5d6e9d26c611fd2c8c8ac45ecbf9e7");
    BOOST_CHECK_EQUAL(accumulated<sha160>(sha160::block_t{}, sha160::block_t{}), expected);
    BOOST_CHECK_EQUAL(sha1_hash(pair), expected);
    BOOST_CHECK_EQUAL(sha1_hash(to_chunk(pair)), expected);
    BOOST_CHECK_EQUAL(sha1_chunk(pair), to_chunk(expected));
    BOOST_CHECK_EQUAL(sha1_chunk(to_chunk(pair)), to_chunk(expected));
}

// sha256_hash/sha256_hash2/sha256_chunk
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(functions__sha256__text__expected)
{
    const auto expected = sha256_chunk(string);
    BOOST_CHECK_EQUAL(sha256_chunk(data), expected);
    BOOST_CHECK_EQUAL(sha256_chunk("foobar"), expected);
    BOOST_CHECK_EQUAL(sha256_chunk(std::string{ "foobar" }), expected);
}

BOOST_AUTO_TEST_CASE(functions__sha256__empty__expected)
{
    constexpr auto expected = base16_array("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    BOOST_CHECK_EQUAL(accumulator<sha256>::hash(zero, nullptr), expected);
}

BOOST_AUTO_TEST_CASE(functions__sha256__empty_double__expected)
{
    constexpr auto expected = base16_array("5df6e0e2761359d30a8275058e299fcc0381534545f55cf43e41983f5d4c9456");
    BOOST_CHECK_EQUAL(accumulator<sha256>::double_hash(zero, nullptr), expected);
}

BOOST_AUTO_TEST_CASE(functions__sha256__null_half__expected)
{
    BOOST_CHECK_EQUAL(accumulator<sha256>::hash(sha256::half_t{}), sha_half256);
    BOOST_CHECK_EQUAL(sha256_hash(sha256::half_t{}), sha_half256);
    BOOST_CHECK_EQUAL(sha256_hash(to_chunk(sha256::half_t{})), sha_half256);
    BOOST_CHECK_EQUAL(sha256_chunk(sha256::half_t{}), to_chunk(sha_half256));
    BOOST_CHECK_EQUAL(sha256_chunk(to_chunk(sha256::half_t{})), to_chunk(sha_half256));
}

BOOST_AUTO_TEST_CASE(functions__sha256__null_one__expected)
{
    BOOST_CHECK_EQUAL(accumulator<sha256>::hash(sha256::block_t{}), sha_full256);
    BOOST_CHECK_EQUAL(sha256_hash(sha256::block_t{}), sha_full256);
    BOOST_CHECK_EQUAL(sha256_hash(sha256::half_t{}, sha256::half_t{}), sha_full256);
    BOOST_CHECK_EQUAL(sha256_chunk(sha256::block_t{}), to_chunk(sha_full256));
    BOOST_CHECK_EQUAL(sha256_chunk(to_chunk(sha256::block_t{})), to_chunk(sha_full256));
}

BOOST_AUTO_TEST_CASE(functions__sha256__null_two__expected)
{
    constexpr auto pair = std_array<uint8_t, array_count<sha256::block_t> * two>{};
    constexpr auto expected = base16_array("38723a2e5e8a17aa7950dc008209944e898f69a7bd10a23c839d341e935fd5ca");
    BOOST_CHECK_EQUAL(accumulated<sha256>(sha256::block_t{}, sha256::block_t{}), expected);
    BOOST_CHECK_EQUAL(sha256_hash(pair), expected);
    BOOST_CHECK_EQUAL(sha256_hash(to_chunk(pair)), expected);
    BOOST_CHECK_EQUAL(sha256_chunk(pair), to_chunk(expected));
    BOOST_CHECK_EQUAL(sha256_chunk(to_chunk(pair)), to_chunk(expected));
}

BOOST_AUTO_TEST_CASE(functions__sha256_hash2__nulls__expected)
{
    constexpr auto expected = base16_array("38723a2e5e8a17aa7950dc008209944e898f69a7bd10a23c839d341e935fd5ca");
    BOOST_CHECK_EQUAL(sha256_hash(sha256::half_t{}, sha256::half_t{}), sha_full256);
    BOOST_CHECK_EQUAL(sha256_hash2(sha256::block_t{}, sha256::block_t{}), expected);
    BOOST_CHECK_EQUAL(sha256_hash2(to_chunk(sha256::half_t{}), to_chunk(sha256::half_t{})), sha_full256);
}

// sha512_hash/sha512_chunk
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(functions__sha512__text__expected)
{
    const auto expected = sha512_chunk(string);
    BOOST_CHECK_EQUAL(sha512_chunk(data), expected);
    BOOST_CHECK_EQUAL(sha512_chunk("foobar"), expected);
    BOOST_CHECK_EQUAL(sha512_chunk(std::string{ "foobar" }), expected);
}

BOOST_AUTO_TEST_CASE(functions__sha512__empty__expected)
{
    constexpr auto expected = base16_array("cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
    BOOST_CHECK_EQUAL(accumulator<sha512>::hash(zero, nullptr), expected);
}

BOOST_AUTO_TEST_CASE(functions__sha512__empty_double__expected)
{
    constexpr auto expected = base16_array("826df068457df5dd195b437ab7e7739ff75d2672183f02bb8e1089fabcf97bd9dc80110cf42dbc7cff41c78ecb68d8ba78abe6b5178dea3984df8c55541bf949");
    BOOST_CHECK_EQUAL(accumulator<sha512>::double_hash(zero, nullptr), expected);
}

BOOST_AUTO_TEST_CASE(functions__sha512__null_half__expected)
{
    BOOST_CHECK_EQUAL(accumulator<sha512>::hash(sha512::half_t{}), sha_half512);
    BOOST_CHECK_EQUAL(sha512_hash(sha512::half_t{}), sha_half512);
    BOOST_CHECK_EQUAL(sha512_hash(to_chunk(sha512::half_t{})), sha_half512);
    BOOST_CHECK_EQUAL(sha512_chunk(sha512::half_t{}), to_chunk(sha_half512));
    BOOST_CHECK_EQUAL(sha512_chunk(to_chunk(sha512::half_t{})), to_chunk(sha_half512));
}

BOOST_AUTO_TEST_CASE(functions__sha512__null_one__expected)
{
    BOOST_CHECK_EQUAL(accumulator<sha512>::hash(sha512::block_t{}), sha_full512);
    BOOST_CHECK_EQUAL(sha512_hash(sha512::block_t{}), sha_full512);
    BOOST_CHECK_EQUAL(sha512_hash(to_chunk(sha512::block_t{})), sha_full512);
    BOOST_CHECK_EQUAL(sha512_chunk(sha512::block_t{}), to_chunk(sha_full512));
    BOOST_CHECK_EQUAL(sha512_chunk(to_chunk(sha512::block_t{})), to_chunk(sha_full512));
}

BOOST_AUTO_TEST_CASE(functions__sha512__null_two__expected)
{
    constexpr auto pair = std_array<uint8_t, array_count<sha512::block_t> * two>{};
    constexpr auto expected = base16_array("693f95d58383a6162d2aab49eb60395dcc4bb22295120caf3f21e3039003230b287c566a03c7a0ca5accaed2133c700b1cb3f82edf8adcbddc92b4f9fb9910c6");
    BOOST_CHECK_EQUAL(accumulated<sha512>(sha512::block_t{}, sha512::block_t{}), expected);
    BOOST_CHECK_EQUAL(sha512_hash(pair), expected);
    BOOST_CHECK_EQUAL(sha512_hash(to_chunk(pair)), expected);
    BOOST_CHECK_EQUAL(sha512_chunk(pair), to_chunk(expected));
    BOOST_CHECK_EQUAL(sha512_chunk(to_chunk(pair)), to_chunk(expected));
}

// bitcoin_short_hash
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(functions__bitcoin_short__empty_expected)
{
    constexpr auto expected = base16_array("b472a266d0bd89c13706a4132ccfb16f7c3b9fcb");
    BOOST_CHECK_EQUAL(bitcoin_short_hash(data_array<zero>{}), expected);
    BOOST_CHECK_EQUAL(bitcoin_short_chunk(data_array<zero>{}), to_chunk(expected));
}

BOOST_AUTO_TEST_CASE(functions__bitcoin_short__null_one__expected)
{
    constexpr auto expected = base16_array("b8bcb07f6344b42ab04250c86a6e8b75d3fdbbc6");
    BOOST_CHECK_EQUAL(rmd160_hash(sha256_hash(null_hash)), expected);
    BOOST_CHECK_EQUAL(bitcoin_short_hash(null_hash), expected);
    BOOST_CHECK_EQUAL(bitcoin_short_hash(to_chunk(null_hash)), expected);
    BOOST_CHECK_EQUAL(bitcoin_short_chunk(null_hash), to_chunk(expected));
    BOOST_CHECK_EQUAL(bitcoin_short_chunk(to_chunk(null_hash)), to_chunk(expected));
}

// bitcoin_hash
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(functions__bitcoin__empty__expected)
{
    // 0xe2e0f65d is Bitcoin empty message checksum, the first 4 bytes of expected.
    constexpr auto expected = base16_array("5df6e0e2761359d30a8275058e299fcc0381534545f55cf43e41983f5d4c9456");
    BOOST_CHECK_EQUAL(bitcoin_hash(data_array<zero>{}), expected);
    BOOST_CHECK_EQUAL(bitcoin_chunk(data_array<zero>{}), to_chunk(expected));
}

BOOST_AUTO_TEST_CASE(functions__bitcoin__null_one__expected)
{
    constexpr auto expected = base16_array("2b32db6c2c0a6235fb1397e8225ea85e0f0e6e8c7b126d0016ccbde0e667151e");
    BOOST_CHECK_EQUAL(sha256_hash(sha256_hash(null_hash)), expected);
    BOOST_CHECK_EQUAL(bitcoin_hash(null_hash), expected);
    BOOST_CHECK_EQUAL(bitcoin_hash(null_hash.size(), null_hash.data()), expected);
    BOOST_CHECK_EQUAL(bitcoin_hash(to_chunk(null_hash)), expected);
    BOOST_CHECK_EQUAL(bitcoin_chunk(null_hash), to_chunk(expected));
    BOOST_CHECK_EQUAL(bitcoin_chunk(to_chunk(null_hash)), to_chunk(expected));
}

// merkle_root
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(functions__merkle_root__empty__null_hash)
{
    constexpr auto expected = null_hash;
    BOOST_REQUIRE_EQUAL(merkle_root({}), expected);
}

BOOST_AUTO_TEST_CASE(functions__merkle_root__one__same)
{
    constexpr auto expected = sha256::digest_t{ 42 };
    BOOST_REQUIRE_EQUAL(merkle_root({ { 42 } }), expected);
}

BOOST_AUTO_TEST_CASE(functions__merkle_root__two__expected)
{
    constexpr auto expected = sha256::double_hash({ 0 }, { 1 });
    BOOST_REQUIRE_EQUAL(merkle_root({ { 0 }, { 1 } }), expected);
}

BOOST_AUTO_TEST_CASE(functions__merkle_root__three__expected)
{
    constexpr auto expected1 = sha256::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha256::double_hash({ 2 }, { 2 });
    constexpr auto expected = sha256::double_hash(expected1, expected2);
    BOOST_REQUIRE_EQUAL(merkle_root({ { 0 }, { 1 }, { 2 } }), expected);
}

BOOST_AUTO_TEST_CASE(functions__merkle_root__four__expected)
{
    constexpr auto expected1 = sha256::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha256::double_hash({ 2 }, { 3 });
    constexpr auto expected = sha256::double_hash(expected1, expected2);
    BOOST_REQUIRE_EQUAL(merkle_root({ { 0 }, { 1 }, { 2 }, { 3 } }), expected);
}

BOOST_AUTO_TEST_CASE(functions__merkle_root__six__expected)
{
    constexpr auto expected1 = sha256::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha256::double_hash({ 2 }, { 3 });
    constexpr auto expected3 = sha256::double_hash({ 4 }, { 5 });
    constexpr auto expected4 = sha256::double_hash(expected1, expected2);
    constexpr auto expected5 = sha256::double_hash(expected3, expected3);
    constexpr auto expected = sha256::double_hash(expected4, expected5);
    BOOST_REQUIRE_EQUAL(merkle_root({ { 0 }, { 1 }, { 2 }, { 3 }, { 4 }, { 5 } }), expected);
}

BOOST_AUTO_TEST_CASE(functions__merkle_root__eight_expected)
{
    constexpr auto expected1 = sha256::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha256::double_hash({ 2 }, { 3 });
    constexpr auto expected = sha256::double_hash(expected1, expected2);
    BOOST_REQUIRE_EQUAL(merkle_root({ { 0 }, { 1 }, { 2 }, { 3 } }), expected);
}

// scrypt_hash
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(functions__scrypt__empty__expected)
{
    const auto expected = scrypt<1024, 1, 1, true>::hash<hash_size>("", "");
    BOOST_REQUIRE_EQUAL(scrypt_hash(""), expected);
}

// non-cryptographic hash functions
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(functions__unique_hash__some_hash__expected)
{
    constexpr auto key = base16_hash("4242424242000000006636626660989953");
    const auto hash = unique_hash(key);

    if constexpr (sizeof(size_t) == sizeof(uint32_t))
    {
        BOOST_REQUIRE_EQUAL(hash, 0x60989953_u32);
    }
    else
    {
        BOOST_REQUIRE_EQUAL(hash, 0x6636626660989953_u64);
    }
}

BOOST_AUTO_TEST_CASE(functions__djb2_hash__alphanumeric__expected)
{
    const std::string alpha{ "01234567890abcdefghijklmnopqrstuvwxyz" };
    const auto hash = djb2_hash(alpha);

    if constexpr (sizeof(size_t) == sizeof(uint32_t))
    {
        BOOST_REQUIRE_EQUAL(hash, 3781598209_u32);
    }
    else
    {
        BOOST_REQUIRE_EQUAL(hash, 9646636626660989953_u64);
    }
}

BOOST_AUTO_TEST_CASE(functions__hash_combine__same_values__expected)
{
    constexpr auto hash = hash_combine(3781598209_size, 3781598209_size);

    if constexpr (sizeof(size_t) == sizeof(uint32_t))
    {
        BOOST_REQUIRE_EQUAL(hash, 598451203_u32);
    }
    else
    {
        BOOST_REQUIRE_EQUAL(hash, 4893418499_u64);
    }
}

BOOST_AUTO_TEST_SUITE_END()

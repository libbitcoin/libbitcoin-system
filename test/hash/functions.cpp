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
#include "hash.hpp"

BOOST_AUTO_TEST_SUITE(functions_tests)

// rmd128_hash/rmd128_chunk
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(accumulator__rmd128_hash__null_half__expected)
{
    BOOST_REQUIRE_EQUAL(accumulator<rmd128>::hash(rmd128::half_t{}), rmd_half128);
    BOOST_REQUIRE_EQUAL(rmd128_hash(rmd128::half_t{}), rmd_half128);
    BOOST_REQUIRE_EQUAL(rmd128_hash(to_chunk(rmd128::half_t{})), rmd_half128);
    BOOST_REQUIRE_EQUAL(rmd128_chunk(rmd128::half_t{}), to_chunk(rmd_half128));
    BOOST_REQUIRE_EQUAL(rmd128_chunk(to_chunk(rmd128::half_t{})), to_chunk(rmd_half128));
}

BOOST_AUTO_TEST_CASE(accumulator__rmd128_hash__null_one__expected)
{
    BOOST_REQUIRE_EQUAL(accumulator<rmd128>::hash(rmd128::block_t{}), rmd_full128);
    BOOST_REQUIRE_EQUAL(rmd128_hash(rmd128::block_t{}), rmd_full128);
    BOOST_REQUIRE_EQUAL(rmd128_hash(to_chunk(rmd128::block_t{})), rmd_full128);
    BOOST_REQUIRE_EQUAL(rmd128_chunk(rmd128::block_t{}), to_chunk(rmd_full128));
    BOOST_REQUIRE_EQUAL(rmd128_chunk(to_chunk(rmd128::block_t{})), to_chunk(rmd_full128));
}

BOOST_AUTO_TEST_CASE(accumulator__rmd128_hash__null_two__expected)
{
    constexpr auto pair = std_array<uint8_t, array_count<rmd128::block_t> * two>{};
    constexpr auto expected = base16_array("1b94bc163383151a53fe49dadb7a4f0e");
    BOOST_REQUIRE_EQUAL(accumulated<rmd128>(rmd128::block_t{}, rmd128::block_t{}), expected);
    BOOST_REQUIRE_EQUAL(rmd128_hash(pair), expected);
    BOOST_REQUIRE_EQUAL(rmd128_hash(to_chunk(pair)), expected);
    BOOST_REQUIRE_EQUAL(rmd128_chunk(pair), to_chunk(expected));
    BOOST_REQUIRE_EQUAL(rmd128_chunk(to_chunk(pair)), to_chunk(expected));
}

// rmd160_hash/rmd160_chunk
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(accumulator__rmd160_hash__null_half__expected)
{
    BOOST_REQUIRE_EQUAL(accumulator<rmd160>::hash(rmd160::half_t{}), rmd_half160);
    BOOST_REQUIRE_EQUAL(rmd160_hash(rmd160::half_t{}), rmd_half160);
    BOOST_REQUIRE_EQUAL(rmd160_hash(to_chunk(rmd160::half_t{})), rmd_half160);
    BOOST_REQUIRE_EQUAL(rmd160_chunk(rmd160::half_t{}), to_chunk(rmd_half160));
    BOOST_REQUIRE_EQUAL(rmd160_chunk(to_chunk(rmd160::half_t{})), to_chunk(rmd_half160));
}

BOOST_AUTO_TEST_CASE(accumulator__rmd160_hash__null_one__expected)
{
    BOOST_REQUIRE_EQUAL(accumulator<rmd160>::hash(rmd160::block_t{}), rmd_full160);
    BOOST_REQUIRE_EQUAL(rmd160_hash(rmd160::block_t{}), rmd_full160);
    BOOST_REQUIRE_EQUAL(rmd160_hash(to_chunk(rmd160::block_t{})), rmd_full160);
    BOOST_REQUIRE_EQUAL(rmd160_chunk(rmd160::block_t{}), to_chunk(rmd_full160));
    BOOST_REQUIRE_EQUAL(rmd160_chunk(to_chunk(rmd160::block_t{})), to_chunk(rmd_full160));
}

BOOST_AUTO_TEST_CASE(accumulator__rmd160_hash__null_two__expected)
{
    constexpr auto pair = std_array<uint8_t, array_count<rmd160::block_t> * two>{};
    constexpr auto expected = base16_array("4300a157335cb7c9fc9423e011d7dd51090d093f");
    BOOST_REQUIRE_EQUAL(accumulated<rmd160>(rmd160::block_t{}, rmd160::block_t{}), expected);
    BOOST_REQUIRE_EQUAL(rmd160_hash(pair), expected);
    BOOST_REQUIRE_EQUAL(rmd160_hash(to_chunk(pair)), expected);
    BOOST_REQUIRE_EQUAL(rmd160_chunk(pair), to_chunk(expected));
    BOOST_REQUIRE_EQUAL(rmd160_chunk(to_chunk(pair)), to_chunk(expected));
}

// sha1_hash
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(accumulator__sha1_hash__null_half__expected)
{
    BOOST_REQUIRE_EQUAL(accumulator<sha160>::hash(sha160::half_t{}), sha_half160);
    BOOST_REQUIRE_EQUAL(sha1_hash(sha160::half_t{}), sha_half160);
    BOOST_REQUIRE_EQUAL(sha1_hash(to_chunk(sha160::half_t{})), sha_half160);
    BOOST_REQUIRE_EQUAL(sha1_chunk(sha160::half_t{}), to_chunk(sha_half160));
    BOOST_REQUIRE_EQUAL(sha1_chunk(to_chunk(sha160::half_t{})), to_chunk(sha_half160));
}

BOOST_AUTO_TEST_CASE(accumulator__sha1_hash__null_one__expected)
{
    BOOST_REQUIRE_EQUAL(accumulator<sha160>::hash(sha160::block_t{}), sha_full160);
    BOOST_REQUIRE_EQUAL(sha1_hash(sha160::block_t{}), sha_full160);
    BOOST_REQUIRE_EQUAL(sha1_hash(to_chunk(sha160::block_t{})), sha_full160);
    BOOST_REQUIRE_EQUAL(sha1_chunk(sha160::block_t{}), to_chunk(sha_full160));
    BOOST_REQUIRE_EQUAL(sha1_chunk(to_chunk(sha160::block_t{})), to_chunk(sha_full160));
}

BOOST_AUTO_TEST_CASE(accumulator__sha1_hash__null_two__expected)
{
    constexpr auto pair = std_array<uint8_t, array_count<sha160::block_t> * two>{};
    constexpr auto expected = base16_array("0ae4f711ef5d6e9d26c611fd2c8c8ac45ecbf9e7");
    BOOST_REQUIRE_EQUAL(accumulated<sha160>(sha160::block_t{}, sha160::block_t{}), expected);
    BOOST_REQUIRE_EQUAL(sha1_hash(pair), expected);
    BOOST_REQUIRE_EQUAL(sha1_hash(to_chunk(pair)), expected);
    BOOST_REQUIRE_EQUAL(sha1_chunk(pair), to_chunk(expected));
    BOOST_REQUIRE_EQUAL(sha1_chunk(to_chunk(pair)), to_chunk(expected));
}

// sha256_hash/sha256_hash2/sha256_chunk
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(accumulator__sha256_hash__null_half__expected)
{
    BOOST_REQUIRE_EQUAL(accumulator<sha256>::hash(sha256::half_t{}), sha_half256);
    BOOST_REQUIRE_EQUAL(sha256_hash(sha256::half_t{}), sha_half256);
    BOOST_REQUIRE_EQUAL(sha256_hash(to_chunk(sha256::half_t{})), sha_half256);
    BOOST_REQUIRE_EQUAL(sha256_chunk(sha256::half_t{}), to_chunk(sha_half256));
    BOOST_REQUIRE_EQUAL(sha256_chunk(to_chunk(sha256::half_t{})), to_chunk(sha_half256));
}

BOOST_AUTO_TEST_CASE(accumulator__sha256_hash__null_one__expected)
{
    BOOST_REQUIRE_EQUAL(accumulator<sha256>::hash(sha256::block_t{}), sha_full256);
    BOOST_REQUIRE_EQUAL(sha256_hash(sha256::block_t{}), sha_full256);
    BOOST_REQUIRE_EQUAL(sha256_hash(sha256::half_t{}, sha256::half_t{}), sha_full256);
    BOOST_REQUIRE_EQUAL(sha256_chunk(sha256::block_t{}), to_chunk(sha_full256));
    BOOST_REQUIRE_EQUAL(sha256_chunk(to_chunk(sha256::block_t{})), to_chunk(sha_full256));
}

BOOST_AUTO_TEST_CASE(accumulator__sha256_hash__null_two__expected)
{
    constexpr auto pair = std_array<uint8_t, array_count<sha256::block_t> * two>{};
    constexpr auto expected = base16_array("38723a2e5e8a17aa7950dc008209944e898f69a7bd10a23c839d341e935fd5ca");
    BOOST_REQUIRE_EQUAL(accumulated<sha256>(sha256::block_t{}, sha256::block_t{}), expected);
    BOOST_REQUIRE_EQUAL(sha256_hash(pair), expected);
    BOOST_REQUIRE_EQUAL(sha256_hash(to_chunk(pair)), expected);
    BOOST_REQUIRE_EQUAL(sha256_chunk(pair), to_chunk(expected));
    BOOST_REQUIRE_EQUAL(sha256_chunk(to_chunk(pair)), to_chunk(expected));
}

BOOST_AUTO_TEST_CASE(accumulator__sha256_hash2__nulls__expected)
{
    constexpr auto expected = base16_array("38723a2e5e8a17aa7950dc008209944e898f69a7bd10a23c839d341e935fd5ca");
    BOOST_REQUIRE_EQUAL(sha256_hash(sha256::half_t{}, sha256::half_t{}), sha_full256);
    BOOST_REQUIRE_EQUAL(sha256_hash2(sha256::block_t{}, sha256::block_t{}), expected);
    BOOST_REQUIRE_EQUAL(sha256_hash2(to_chunk(sha256::half_t{}), to_chunk(sha256::half_t{})), sha_full256);
}

// sha512_hash
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(accumulator__sha512_hash__null_half__expected)
{
    BOOST_REQUIRE_EQUAL(accumulator<sha512>::hash(sha512::half_t{}), sha_half512);
    BOOST_REQUIRE_EQUAL(sha512_hash(sha512::half_t{}), sha_half512);
    BOOST_REQUIRE_EQUAL(sha512_hash(to_chunk(sha512::half_t{})), sha_half512);
    BOOST_REQUIRE_EQUAL(sha512_chunk(sha512::half_t{}), to_chunk(sha_half512));
    BOOST_REQUIRE_EQUAL(sha512_chunk(to_chunk(sha512::half_t{})), to_chunk(sha_half512));
}

BOOST_AUTO_TEST_CASE(accumulator__sha512_hash__null_one__expected)
{
    BOOST_REQUIRE_EQUAL(accumulator<sha512>::hash(sha512::block_t{}), sha_full512);
    BOOST_REQUIRE_EQUAL(sha512_hash(sha512::block_t{}), sha_full512);
    BOOST_REQUIRE_EQUAL(sha512_hash(to_chunk(sha512::block_t{})), sha_full512);
    BOOST_REQUIRE_EQUAL(sha512_chunk(sha512::block_t{}), to_chunk(sha_full512));
    BOOST_REQUIRE_EQUAL(sha512_chunk(to_chunk(sha512::block_t{})), to_chunk(sha_full512));
}

BOOST_AUTO_TEST_CASE(accumulator__sha512_hash__null_two__expected)
{
    constexpr auto pair = std_array<uint8_t, array_count<sha512::block_t> * two>{};
    constexpr auto expected = base16_array("693f95d58383a6162d2aab49eb60395dcc4bb22295120caf3f21e3039003230b287c566a03c7a0ca5accaed2133c700b1cb3f82edf8adcbddc92b4f9fb9910c6");
    BOOST_REQUIRE_EQUAL(accumulated<sha512>(sha512::block_t{}, sha512::block_t{}), expected);
    BOOST_REQUIRE_EQUAL(sha512_hash(pair), expected);
    BOOST_REQUIRE_EQUAL(sha512_hash(to_chunk(pair)), expected);
    BOOST_REQUIRE_EQUAL(sha512_chunk(pair), to_chunk(expected));
    BOOST_REQUIRE_EQUAL(sha512_chunk(to_chunk(pair)), to_chunk(expected));
}

// bitcoin_short_hash
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(accumulator__bitcoin_short_hash__null_one__expected)
{
    constexpr auto expected = base16_array("b8bcb07f6344b42ab04250c86a6e8b75d3fdbbc6");
    BOOST_REQUIRE_EQUAL(rmd160_hash(sha256_hash(null_hash)), expected);
    BOOST_REQUIRE_EQUAL(bitcoin_short_hash(null_hash), expected);
    BOOST_REQUIRE_EQUAL(bitcoin_short_hash(to_chunk(null_hash)), expected);
    BOOST_REQUIRE_EQUAL(bitcoin_short_chunk(null_hash), to_chunk(expected));
    BOOST_REQUIRE_EQUAL(bitcoin_short_chunk(to_chunk(null_hash)), to_chunk(expected));
}

// bitcoin_hash
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(accumulator__bitcoin_hash__null_one__expected)
{
    constexpr auto expected = base16_array("2b32db6c2c0a6235fb1397e8225ea85e0f0e6e8c7b126d0016ccbde0e667151e");
    BOOST_REQUIRE_EQUAL(sha256_hash(sha256_hash(null_hash)), expected);
    BOOST_REQUIRE_EQUAL(bitcoin_hash(null_hash), expected);
    BOOST_REQUIRE_EQUAL(bitcoin_hash(to_chunk(null_hash)), expected);
    BOOST_REQUIRE_EQUAL(bitcoin_chunk(null_hash), to_chunk(expected));
    BOOST_REQUIRE_EQUAL(bitcoin_chunk(to_chunk(null_hash)), to_chunk(expected));
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

// scrypt_hash
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(functions__scrypt_hash__empty__expected)
{
    const auto expected = scrypt<1024, 1, 1, true>::hash<hash_size>("", "");
    BOOST_REQUIRE_EQUAL(scrypt_hash(""), expected);
}

// non-cryptographic hash functions
// ----------------------------------------------------------------------------

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

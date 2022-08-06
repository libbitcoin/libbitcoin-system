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
    const auto hash = hash_combine(3781598209_size, 3781598209_size);

    if constexpr (sizeof(size_t) == sizeof(uint32_t))
    {
        BOOST_REQUIRE_EQUAL(hash, 598451203_u32);
    }
    else
    {
        BOOST_REQUIRE_EQUAL(hash, 4893418499_u64);
    }
}

template <typename Algorithm>
typename accumulator<Algorithm>::digest_t accumulated(
    const data_slice& left, const data_slice& right) NOEXCEPT
{
    accumulator<Algorithm> accumulator{};
    accumulator.write(left.size(), left.data());
    accumulator.write(right.size(), right.data());
    return accumulator.flush();
}

// rmd128
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(accumulator__rmd_concurrent_hash_half128__null_hashes__expected)
{
    BOOST_CHECK_EQUAL(accumulator<rmd128>::hash(rmd128::half_t{}), rmd_half128);
    BOOST_CHECK_EQUAL(ripemd128_hash(rmd128::half_t{}), rmd_half128);
}

BOOST_AUTO_TEST_CASE(accumulator__rmd_concurrent_hash_one128__null_hashes__expected)
{
    BOOST_CHECK_EQUAL(accumulator<rmd128>::hash(rmd128::block_t{}), rmd_full128);
    BOOST_CHECK_EQUAL(ripemd128_hash(rmd128::block_t{}), rmd_full128);
}

BOOST_AUTO_TEST_CASE(accumulator__rmd_concurrent_hash_two128__null_hashes__expected)
{
    constexpr auto pair = std_array<uint8_t, array_count<rmd128::block_t> * two>{};
    constexpr auto sha_pair128 = base16_array("1b94bc163383151a53fe49dadb7a4f0e");
    BOOST_CHECK_EQUAL(accumulated<rmd128>(rmd128::block_t{}, rmd128::block_t{}), sha_pair128);
    BOOST_CHECK_EQUAL(ripemd128_hash(pair), sha_pair128);
}

// rmd160
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(accumulator__rmd_concurrent_hash_half160__null_hashes__expected)
{
    BOOST_CHECK_EQUAL(accumulator<rmd160>::hash(rmd160::half_t{}), rmd_half160);
    BOOST_CHECK_EQUAL(ripemd160_hash(rmd160::half_t{}), rmd_half160);
}

BOOST_AUTO_TEST_CASE(accumulator__rmd_concurrent_hash_one160__null_hashes__expected)
{
    BOOST_CHECK_EQUAL(accumulator<rmd160>::hash(rmd160::block_t{}), rmd_full160);
    BOOST_CHECK_EQUAL(ripemd160_hash(rmd160::block_t{}), rmd_full160);
}

BOOST_AUTO_TEST_CASE(accumulator__rmd_concurrent_hash_two160__null_hashes__expected)
{
    constexpr auto pair = std_array<uint8_t, array_count<rmd160::block_t> * two>{};
    constexpr auto sha_pair160 = base16_array("4300a157335cb7c9fc9423e011d7dd51090d093f");
    BOOST_CHECK_EQUAL(accumulated<rmd160>(rmd160::block_t{}, rmd160::block_t{}), sha_pair160);
    BOOST_CHECK_EQUAL(ripemd160_hash(pair), sha_pair160);
}

// sha160
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(accumulator__sha_hash_half160__null_hashes__expected)
{
    BOOST_CHECK_EQUAL(accumulator<sha160>::hash(sha160::half_t{}), sha_half160);
    BOOST_CHECK_EQUAL(sha1_hash(sha160::half_t{}), sha_half160);
}

BOOST_AUTO_TEST_CASE(accumulator__sha_hash_one160__null_hashes__expected)
{
    BOOST_CHECK_EQUAL(accumulator<sha160>::hash(sha160::block_t{}), sha_full160);
    BOOST_CHECK_EQUAL(sha1_hash(sha160::block_t{}), sha_full160);
}

BOOST_AUTO_TEST_CASE(accumulator__sha_hash_two160__null_hashes__expected)
{
    constexpr auto pair = std_array<uint8_t, array_count<sha160::block_t> * two>{};
    constexpr auto sha_pair160 = base16_array("0ae4f711ef5d6e9d26c611fd2c8c8ac45ecbf9e7");
    BOOST_CHECK_EQUAL(accumulated<sha160>(sha160::block_t{}, sha160::block_t{}), sha_pair160);
    BOOST_CHECK_EQUAL(sha1_hash(pair), sha_pair160);
}

// sha256
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(accumulator__sha_hash_half256__null_hashes__expected)
{
    BOOST_CHECK_EQUAL(accumulator<sha256>::hash(sha256::half_t{}), sha_half256);
    BOOST_CHECK_EQUAL(sha256_hash(sha256::half_t{}), sha_half256);
}

BOOST_AUTO_TEST_CASE(accumulator__sha_hash_one256__null_hashes__expected)
{
    BOOST_CHECK_EQUAL(accumulator<sha256>::hash(sha256::block_t{}), sha_full256);
    BOOST_CHECK_EQUAL(sha256_hash(sha256::block_t{}), sha_full256);
}

BOOST_AUTO_TEST_CASE(accumulator__sha_hash_two256__null_hashes__expected)
{
    constexpr auto pair = std_array<uint8_t, array_count<sha256::block_t> * two>{};
    constexpr auto sha_pair256 = base16_array("38723a2e5e8a17aa7950dc008209944e898f69a7bd10a23c839d341e935fd5ca");
    BOOST_CHECK_EQUAL(accumulated<sha256>(sha256::block_t{}, sha256::block_t{}), sha_pair256);
    BOOST_CHECK_EQUAL(sha256_hash(pair), sha_pair256);
    BOOST_CHECK_EQUAL(sha256_hash_slice(sha256::block_t{}, sha256::block_t{}), sha256_hash(pair));
}

// sha512
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(accumulator__sha_hash__half512__null_hash__expected)
{
    BOOST_CHECK_EQUAL(accumulator<sha512>::hash(sha512::half_t{}), sha_half512);
    BOOST_CHECK_EQUAL(sha512_hash(sha512::half_t{}), sha_half512);
}

BOOST_AUTO_TEST_CASE(accumulator__sha_hash__one512__null_hash__expected)
{
    BOOST_CHECK_EQUAL(accumulator<sha512>::hash(sha512::block_t{}), sha_full512);
    BOOST_CHECK_EQUAL(sha512_hash(sha512::block_t{}), sha_full512);
}

BOOST_AUTO_TEST_CASE(accumulator__sha_hash_two512__null_hashes__expected)
{
    constexpr auto pair = std_array<uint8_t, array_count<sha512::block_t> * two>{};
    constexpr auto sha_pair512 = base16_array("693f95d58383a6162d2aab49eb60395dcc4bb22295120caf3f21e3039003230b287c566a03c7a0ca5accaed2133c700b1cb3f82edf8adcbddc92b4f9fb9910c6");
    BOOST_CHECK_EQUAL(accumulated<sha512>(sha512::block_t{}, sha512::block_t{}), sha_pair512);
    BOOST_CHECK_EQUAL(sha512_hash(pair), sha_pair512);
}

BOOST_AUTO_TEST_SUITE_END()

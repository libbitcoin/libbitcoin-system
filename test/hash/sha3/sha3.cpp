/**
 * Copyright (c) 2011-2026 libbitcoin developers
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

BOOST_AUTO_TEST_SUITE(sha3_tests)

using sha3_224 = sha3::algorithm<sha3::h224<>>;
using sha3_384 = sha3::algorithm<sha3::h384<>>;
using sha3_512 = sha3::algorithm<sha3::h512<>>;

// FIPS 202 examples (csrc.nist.gov/projects/cryptographic-standards-and-guidelines/example-values).

constexpr auto empty224 = base16_array("6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7");
constexpr auto empty256 = base16_array("a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a");
constexpr auto empty384 = base16_array("0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac3713831264adb47fb6bd1e058d5f004");
constexpr auto empty512 = base16_array("a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a615b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26");
constexpr auto abc224 = base16_array("e642824c3f8cf24ad09234ee7d3c766fc9a3a5168d0c94ad73b46fdf");
constexpr auto abc256 = base16_array("3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532");
constexpr auto abc384 = base16_array("ec01498288516fc926459f58e2c6ad8df9b473cb0fc08c2596da7cf0e49be4b298d88cea927ac7f539f1edf228376d25");
constexpr auto abc512 = base16_array("b751850b1a57168a5693cd924b6b096e08f621827444f70d884f5d0240d2712e10e116e9192af3c91a7ec57647e3934057340b4cf408d5a56592f8274eec53f0");
constexpr auto long256 = base16_array("41c0dba2a9d6240849100376a8235e2c82e1b9998a999e21db32dd97496d3376");
constexpr auto million256 = base16_array("5c8875ae474a3634ba4fd55ec85bffd661f32aca75c6d699d0cdcb6c115891c1");
constexpr auto keccak_empty256 = base16_array("c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470");

constexpr std_array<uint8_t, 3> abc{ 'a', 'b', 'c' };
const std::string long_message{ "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" };

// Other test vectors are dependent upon the correctness of these.
static_assert(sha3_256::simple_hash(sha3_256::bytes_t<0>{}) == empty256);
static_assert(sha3_256::simple_hash(abc) == abc256);
static_assert(keccak_256::simple_hash(sha3_256::bytes_t<0>{}) == keccak_empty256);

// simple_hash (const-evaluated)

BOOST_AUTO_TEST_CASE(sha3__simple_hash__empty__expected)
{
    BOOST_REQUIRE_EQUAL(sha3_224::simple_hash(sha3_224::bytes_t<0>{}), empty224);
    BOOST_REQUIRE_EQUAL(sha3_256::simple_hash(sha3_256::bytes_t<0>{}), empty256);
    BOOST_REQUIRE_EQUAL(sha3_384::simple_hash(sha3_384::bytes_t<0>{}), empty384);
    BOOST_REQUIRE_EQUAL(sha3_512::simple_hash(sha3_512::bytes_t<0>{}), empty512);
}

BOOST_AUTO_TEST_CASE(sha3__simple_hash__abc__expected)
{
    BOOST_REQUIRE_EQUAL(sha3_224::simple_hash(abc), abc224);
    BOOST_REQUIRE_EQUAL(sha3_256::simple_hash(abc), abc256);
    BOOST_REQUIRE_EQUAL(sha3_384::simple_hash(abc), abc384);
    BOOST_REQUIRE_EQUAL(sha3_512::simple_hash(abc), abc512);
}

// hash (data_slice)

BOOST_AUTO_TEST_CASE(sha3__hash__empty__expected)
{
    BOOST_REQUIRE_EQUAL(sha3_224::hash(data_chunk{}), empty224);
    BOOST_REQUIRE_EQUAL(sha3_256::hash(data_chunk{}), empty256);
    BOOST_REQUIRE_EQUAL(sha3_384::hash(data_chunk{}), empty384);
    BOOST_REQUIRE_EQUAL(sha3_512::hash(data_chunk{}), empty512);
}

BOOST_AUTO_TEST_CASE(sha3__hash__abc__expected)
{
    const auto data = to_chunk("abc");
    BOOST_REQUIRE_EQUAL(sha3_224::hash(data), abc224);
    BOOST_REQUIRE_EQUAL(sha3_256::hash(data), abc256);
    BOOST_REQUIRE_EQUAL(sha3_384::hash(data), abc384);
    BOOST_REQUIRE_EQUAL(sha3_512::hash(data), abc512);
}

BOOST_AUTO_TEST_CASE(sha3__hash__448_bit_message__expected)
{
    BOOST_REQUIRE_EQUAL(sha3_256::hash(to_chunk(long_message)), long256);
}

BOOST_AUTO_TEST_CASE(sha3__hash__one_million_a__expected)
{
    // Spans many rate blocks (136 bytes) with a partial trailing block.
    const data_chunk data(1'000'000, 'a');
    BOOST_REQUIRE_EQUAL(sha3_256::hash(data), million256);
}

// hash (block/half/byte)

BOOST_AUTO_TEST_CASE(sha3__hash__block_forms__consistent)
{
    // Correlate the typed overloads to the data_slice overload.
    constexpr sha3_256::block_t block{};
    constexpr sha3_256::half_t half{};
    const auto block_chunk = to_chunk(block);
    const auto half_chunk = to_chunk(half);
    BOOST_REQUIRE_EQUAL(sha3_256::hash(block), sha3_256::hash(block_chunk));
    BOOST_REQUIRE_EQUAL(sha3_256::hash(half), sha3_256::hash(half_chunk));
    BOOST_REQUIRE_EQUAL(sha3_256::hash(0x61_u8), sha3_256::hash(to_chunk("a")));
}

// accumulate/finalize

BOOST_AUTO_TEST_CASE(sha3__accumulate_finalize__blocks_and_tail__expected)
{
    // Two full blocks then a three byte tail, streamed vs. single hash.
    constexpr sha3_256::block_t block{ 0x42 };
    constexpr std_array<uint8_t, 3> tail{ 'a', 'b', 'c' };
    auto data = to_chunk(block);
    extend(data, to_chunk(block));
    extend(data, to_chunk(tail));

    auto state = sha3_256::H::get;
    sha3_256::accumulate(state, block);
    sha3_256::accumulate(state, block);
    const auto streamed = sha3_256::finalize(state, tail);
    BOOST_REQUIRE_EQUAL(streamed, sha3_256::hash(data));
}

// keccak

BOOST_AUTO_TEST_CASE(keccak__hash__empty__expected)
{
    BOOST_REQUIRE_EQUAL(keccak_256::hash(data_chunk{}), keccak_empty256);
}

BOOST_AUTO_TEST_CASE(keccak__hash__abc__differs_from_sha3)
{
    // Domain separation alone distinguishes the two.
    const auto data = to_chunk("abc");
    BOOST_REQUIRE_NE(keccak_256::hash(data), sha3_256::hash(data));
}

BOOST_AUTO_TEST_SUITE_END()

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

BOOST_AUTO_TEST_SUITE(sha3_algorithm_tests)

using sha3_224 = sha3::algorithm<sha3::h224<>>;
using sha3_384 = sha3::algorithm<sha3::h384<>>;
using sha3_512 = sha3::algorithm<sha3::h512<>>;

// Rate (block) and digest sizes per FIPS 202 Table 3.
static_assert(sha3_224::block_bytes == 144);
static_assert(sha3_256::block_bytes == 136);
static_assert(sha3_384::block_bytes == 104);
static_assert(sha3_512::block_bytes == 72);
static_assert(keccak_256::block_bytes == 136);

static_assert(array_count<sha3_224::digest_t> == 28);
static_assert(array_count<sha3_256::digest_t> == 32);
static_assert(array_count<sha3_384::digest_t> == 48);
static_assert(array_count<sha3_512::digest_t> == 64);

static_assert(array_count<sha3_256::state_t> == 25);
static_assert(std::is_same_v<sha3_256::word_t, uint64_t>);
static_assert(sha3_256::H::domain == sha3::sha3_domain);
static_assert(keccak_256::H::domain == sha3::keccak_domain);
static_assert(sha3_256::space == 135);
static_assert(!sha3_256::vector);

// The initial state is zero (sponge).
static_assert(sha3_256::H::get == sha3_256::state_t{});

BOOST_AUTO_TEST_CASE(sha3_algorithm__hash__digest_truncation__prefix)
{
    // A truncated digest is the prefix of the full digest (same rate).
    using sha3_256_224 = sha3::algorithm<sha3::h256<224>>;
    const auto full = sha3_256::hash(to_chunk("abc"));
    const auto truncated = sha3_256_224::hash(to_chunk("abc"));
    BOOST_REQUIRE(std::equal(truncated.begin(), truncated.end(), full.begin()));
}

BOOST_AUTO_TEST_SUITE_END()

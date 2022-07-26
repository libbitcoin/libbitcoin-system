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

BOOST_AUTO_TEST_SUITE(rmd_tests)

using byte_t = uint8_t;
using word_t = uint32_t;
constexpr size_t word_bytes = sizeof(word_t);
constexpr size_t state_words = 5;
constexpr size_t block_words = 16;
constexpr size_t rows = 10;
constexpr size_t columns = 16;
constexpr size_t rounds = rows * columns;
constexpr size_t block_bytes = block_words * word_bytes;
constexpr size_t digest_bytes = state_words * word_bytes;

static constexpr auto count_bits = block_bytes;
static constexpr auto count_bytes = bytes<count_bits>;
using count_t = unsigned_exact_type<count_bytes>;

using state_t = std_array<word_t, state_words>;
using words_t = std_array<word_t, block_words>;
using constants_t = std_array<word_t, rounds>;

using block_t = std_array<byte_t, block_bytes>;
using digest_t = std_array<byte_t, digest_bytes>;
////using blocks_t = std_vector<cref<block_t>>;

/// Finalized single hash (one/multiple full blocks).
digest_t hash(const block_t& block) NOEXCEPT;
///digest_t hash(const blocks_t& blocks) NOEXCEPT;

/// Streaming (single hash, one/multiple full blocks).
void accumulate(state_t& state, const block_t& block) NOEXCEPT;
////void accumulate(state_t& state, const blocks_t& blocks) NOEXCEPT;
digest_t finalize(const state_t& state) NOEXCEPT;

/////// Write data to the context.
////bool write(size_t size, const byte_t* in) NOEXCEPT;
////
/////// Flush context to out32 (must be at least 32 bytes), resets context.
////void flush(byte_t* out32) NOEXCEPT;

// TODO: write class for injection to hash accumulator.
// TODO: may also need count endianness or serialzier.
// Hash::state_t
// Hash::byte_t
// Hash::digest_t
// Hash::digest_bytes
// Hash::count_t
// Hash::count_bytes
// Hash::block_t
// Hash::block_bytes
// Hash::accumulate
// Hash::H::get
// Hash::pad::stream
// Hash::big_end_count

BOOST_AUTO_TEST_SUITE_END()

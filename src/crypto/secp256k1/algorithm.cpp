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
#include <bitcoin/system/crypto/secp256k1/algorithm.hpp>

#include <bitcoin/system/define.hpp>
#include "generator/generator.hpp"

namespace libbitcoin {
namespace system {
namespace secp256k1 {

// The precomputed table shape matches the algorithm.
struct shape
  : algorithm
{
    static_assert(precompute::slice_size == slice_size);
    static_assert(precompute::block_size == block_size);
    static_assert(precompute::table_words == table_words);
    static_assert(precompute::comb_bits == comb_bits);
    static_assert(precompute::comb_size == comb_size);
    static_assert(precompute::comb_windows == comb_windows);
    static_assert(precompute::comb_part_windows == comb_part_windows);
    static_assert(precompute::comb_words == comb_words);
};

constinit const std_array<const uint64_t*, 16> generator_slices
{
    generator_slice_00.data(), generator_slice_01.data(),
    generator_slice_02.data(), generator_slice_03.data(),
    generator_slice_04.data(), generator_slice_05.data(),
    generator_slice_06.data(), generator_slice_07.data(),
    generator_slice_08.data(), generator_slice_09.data(),
    generator_slice_10.data(), generator_slice_11.data(),
    generator_slice_12.data(), generator_slice_13.data(),
    generator_slice_14.data(), generator_slice_15.data()
};

constinit const std_array<const uint64_t*, 3> comb_parts
{
    comb_part_00.data(), comb_part_01.data(), comb_part_02.data()
};

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

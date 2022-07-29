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
#include "test.hpp"

constexpr void foo()
{
    // It is not more optimal to emit the hashes in contiguous blocks, as they
    // are nevertheless copied into the hash buffer. This can be done with near
    // efficiency by simply copying from two locations. It saves allocation to
    // store the hash result into the original block buffer, which would imply
    // left-aligned assignment of the digest in each block. This requires the
    // buffer is non-const, which is expected here. But iterating in parallel
    // over halved results without copying hashes presents a difficulty.
    // We could iterator over a set of indexes into the blocks, reducing the
    // index on each iteration.

    // indexes[0]: 0, 1, 2, 3, 4, 5, 6, 7 (size)
    // indexes[1]: 0,    2,    4,    6    (size/2)     (/2 skip 2, shrink 2-1)
    // indexes[2]: 0,          4          (size/2/2)   (/4 skip 4, shrink 4-1)
    // indexes[3]: 0,                     (size/2/2/2) (/8 skip 8, shrink 8-1)

    // Initial index vector is [0, 2, 4, 6] and offset i = 1 = 2^0:
    // 0|1, 2|3, 4|5, 6|7
    // Then reduce to [0, 4] with offset i << 1 = 2^1:
    // 0|2, 4|6
    // Then reduce to [0] with offset i << 1 = 2^2:
    // 0|4 then done (set is odd).
    // The cost is in two 32 byte copies vs. 1 64 byte copy,
    // and construction/reduction of the index vector,
    // and a multiply for the offset in each element (own index << i).
    // The index can be reconstructed by iterating by i until size is reached.
    // And the vector can be resized to that point until odd.
    // With index, offset and data reference, computation is trivial.
    // Output is array-casted into own block. Final result is left half block 1.
}

using hasher = sha::algorithm<sha::sha256>;

constexpr auto merkle(const hasher::set_t& blocks)
{
    hasher::digests_t digests(blocks.size());
    using token = std::tuple<decltype(&foo), size_t, size_t>;
    const auto size = blocks.size();

    std::transform(blocks.begin(), blocks.end(), digests.begin(),
        [&](const hasher::block_t& block)
        {
            return hasher::double_hash(block);
        });

    return digests;
}
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
#ifndef LIBBITCOIN_SYSTEM_HASH_HASH_IPP
#define LIBBITCOIN_SYSTEM_HASH_HASH_IPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// Value will vary with sizeof(size_t).
// A DJB variation uses [^ byte] vs. [+ byte].
// Objectives: deterministic, uniform distribution, efficient computation.
constexpr size_t djb2_hash(const data_slice& data) NOEXCEPT
{
    // Nothing special here except that it tested well against collisions.
    auto hash = 5381_size;

    // Efficient sum of ((hash * 33) + byte) for all bytes.
    for (const auto byte: data)
        hash = (shift_left(hash, 5_size) + hash) + byte;

    return hash;
}

// Combine hash values, such as a pair of djb2_hash outputs.
constexpr size_t hash_combine(size_t left, size_t right) NOEXCEPT
{
    return left ^ shift_left(right, one);
}

} // namespace system
} // namespace libbitcoin

#endif

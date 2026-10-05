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

// Sponsored in part by Digital Contract Design, LLC

#ifndef LIBBITCOIN_SYSTEM_HASH_SIPHASH
#define LIBBITCOIN_SYSTEM_HASH_SIPHASH

#include <span>
#include <tuple>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/hash/functions.hpp>

namespace libbitcoin {
namespace system {

typedef std::tuple<uint64_t, uint64_t> siphash_key;
typedef std_array<uint64_t, 4> siphash_words;
typedef std_array<std::span<const uint64_t>, 4> siphash_columns;

BC_API uint64_t siphash(const siphash_key& key,
    const data_slice& message) NOEXCEPT;
BC_API uint64_t siphash(const half_hash& hash,
    const data_slice& message) NOEXCEPT;

/// A 32 byte message as four little-endian words (e.g. a hash).
BC_API uint64_t siphash(const siphash_key& key,
    const siphash_words& message) NOEXCEPT;

/// Each row of four little-endian word columns as a 32 byte message, one hash
/// per row of out (columns at least as long as out), across vector lanes.
BC_API void siphash(const std::span<uint64_t>& out, const siphash_key& key,
    const siphash_columns& columns) NOEXCEPT;

constexpr siphash_key to_siphash_key(const half_hash& hash) NOEXCEPT
{
    const auto part = split(hash);
    const auto hi = from_little_endian(part.first);
    const auto lo = from_little_endian(part.second);
    return std::make_tuple(hi, lo);
}

} // namespace system
} // namespace libbitcoin

#endif

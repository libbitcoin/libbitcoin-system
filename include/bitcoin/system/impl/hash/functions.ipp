/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

#include <utility>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/accumulator.hpp>
#include <bitcoin/system/hash/algorithms.hpp>
#include <bitcoin/system/hash/scrypt.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// General cryptographic hash functions.
// ----------------------------------------------------------------------------
// These functions optimize for half/N rmd160/sha1/sha256/512 block sizes.
// They bypass accumulation, using precomputed padding and avoiding unnecessary
// endianness conversions between double hashes. Array types are most efficient
// as size is determined at compile time and vector casting is unnecessary.
// For N full blocks of array data, the padding message schedule is computed at
// compile time, saving apprimately 30% of the hash cost for the padding block.
// For up to 1024 full blocks of non-array sha256 data, the same is applied.
// data_chunk overloads write digest to the chunk buffer, avoiding copies.
// Accumulator.write optimizes for writes of n > 0 blocks against an empty
// buffer, forwarding the set of blocks (data modulo block size) to algorithm.
// exclusive_slice is a specialization of data_slice which does not accept
// array/vector. This allows overloading for all three data scenarios, where
// optimizations are maximized for each independently.

// TODO: provide unconstrained pass-through to accumulator, which can collpase
// TODO: the number of functions to one per type.

// rmd128 [historical].
template <typename Type>
INLINE half_hash rmd128_hash(const Type& data) NOEXCEPT
{
    return accumulator<rmd128>::hash(data);
}

template <typename Type>
INLINE data_chunk rmd128_chunk(const Type& data) NOEXCEPT
{
    return accumulator<rmd128>::hash_chunk(data);
}

// rmd160 [script].
template <typename Type>
INLINE short_hash rmd160_hash(const Type& data) NOEXCEPT
{
    return accumulator<rmd160>::hash(data);
}

template <typename Type>
INLINE data_chunk rmd160_chunk(const Type& data) NOEXCEPT
{
    return accumulator<rmd160>::hash_chunk(data);
}

// sha1 (sha160) [script].
template <typename Type>
INLINE short_hash sha1_hash(const Type& data) NOEXCEPT
{
    return accumulator<sha160>::hash(data);
}

template <typename Type>
INLINE data_chunk sha1_chunk(const Type& data) NOEXCEPT
{
    return accumulator<sha160>::hash_chunk(data);
}

// sha256 [script, chain, wallet].
template <typename Type>
INLINE hash_digest sha256_hash(const Type& data) NOEXCEPT
{
    return accumulator<sha256>::hash(data);
}

INLINE hash_digest sha256_hash(const hash_digest& left,
    const hash_digest& right) NOEXCEPT
{
    return sha256::hash(left, right);
}

INLINE hash_digest sha256_hash2(const data_slice& left,
    const data_slice& right) NOEXCEPT
{
    // Used by wallet functions on mixed left/right sizes.
    // TODO: provide implicit cast from data_slice to exclusive_slice.
    accumulator<sha256> context{};
    context.write(left.size(), left.data());
    context.write(right.size(), right.data());
    return context.flush();
}

template <typename Type>
INLINE data_chunk sha256_chunk(const Type& data) NOEXCEPT
{
    return accumulator<sha256>::hash_chunk(data);
}

// sha512 [wallet].
template <typename Type>
INLINE long_hash sha512_hash(const Type& data) NOEXCEPT
{
    return accumulator<sha512>::hash(data);
}

template <typename Type>
INLINE data_chunk sha512_chunk(const Type& data) NOEXCEPT
{
    return accumulator<sha512>::hash_chunk(data);
}

// Specialzied bitcoin cryptographic hash functions.
// ----------------------------------------------------------------------------

// Bitcoin short hash (rmd160(sha256)) [script].
template <typename Type>
INLINE short_hash bitcoin_short_hash(const Type& data) NOEXCEPT
{
    return rmd160::hash(accumulator<sha256>::hash(data));
}

template <typename Type>
INLINE data_chunk bitcoin_short_chunk(const Type& data) NOEXCEPT
{
    return accumulator<rmd160>::hash_chunk(accumulator<sha256>::hash(data));
}

// Bitcoin hash (sha256(sha256)) [script, chain, wallet].
template <typename Type>
INLINE hash_digest bitcoin_hash(const Type& data) NOEXCEPT
{
    return accumulator<sha256>::double_hash(data);
}

INLINE hash_digest bitcoin_hash(size_t size, const uint8_t* data) NOEXCEPT
{
    return accumulator<sha256>::double_hash(size, data);
}

INLINE hash_digest bitcoin_hash(const hash_digest& left,
    const hash_digest& right) NOEXCEPT
{
    return sha256::double_hash(left, right);
}

INLINE hash_digest bitcoin_hash2(const data_slice& left,
    const data_slice& right) NOEXCEPT
{
    // Used by wallet functions on mixed left/right sizes.
    // double_flush performs second hash, avoiding endian conversion of state.
    // TODO: provide implicit cast from data_slice to exclusive_slice.
    accumulator<sha256> context{};
    context.write(left.size(), left.data());
    context.write(right.size(), right.data());
    return context.double_flush();
}

template <typename Type>
INLINE data_chunk bitcoin_chunk(const Type& data) NOEXCEPT
{
    return accumulator<sha256>::double_hash_chunk(data);
}

/// Merkle root from a bitcoin_hash set [chain].
INLINE hash_digest merkle_root(hashes&& set) NOEXCEPT
{
    return sha256::merkle_root(std::move(set));
}

// Litecoin scrypt hash [chain].
INLINE hash_digest scrypt_hash(const data_slice& data) NOEXCEPT
{
    // Litecoin parameters, concurrency enabled (data is password and salt).
    return scrypt<1024, 1, 1, true>::hash<hash_size>(data, data);
}

// Hash table keying.
// ----------------------------------------------------------------------------

/// For explicit use with std library containers.
template <size_t Size>
struct unique_hash_t
{
    size_t operator()(const data_array<Size>& key) const NOEXCEPT
    {
        return unique_hash(key);
    }
};

template <size_t Size>
INLINE constexpr size_t unique_hash(const data_array<Size>& key) NOEXCEPT
{
    constexpr auto size = std::min(Size, sizeof(size_t));

    // This optimization breaks data portability (by endianness).
    // Could be modified to use an endian conversion vs. simple copy.
    size_t value{};
    std::copy_n(key.begin(), size, byte_cast(value).begin());
    return value;
}

// Value will vary with sizeof(size_t).
// A DJB variation uses [^ byte] vs. [+ byte].
// Objectives: deterministic, uniform distribution, efficient computation.
INLINE constexpr size_t djb2_hash(const data_slice& data) NOEXCEPT
{
    // Nothing special here except that it tested well against collisions.
    auto hash = 5381_size;

    // Efficient sum of ((hash * 33) + byte) for all bytes.
    for (const auto byte: data)
        hash = (shift_left(hash, 5_size) + hash) + byte;

    return hash;
}

// Combine hash values, such as djb2_hash or unique_hash outputs.
// Right should always be the lower entropy value (e.g. point index).
INLINE constexpr size_t hash_combine(size_t left, size_t right) NOEXCEPT
{
    // Shift ensures left == right does not always return zero.
    return bit_xor(left, shift_left(right));
}

} // namespace system
} // namespace libbitcoin

#endif

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
#ifndef LIBBITCOIN_SYSTEM_HASH_FUNCTIONS_HPP
#define LIBBITCOIN_SYSTEM_HASH_FUNCTIONS_HPP

#include <algorithm>
#include <memory>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Common bitcoin hash container sizes by bit length.
static constexpr size_t long_hash_size = bytes<512>;
static constexpr size_t hash_size = bytes<256>;
static constexpr size_t short_hash_size = bytes<160>;
static constexpr size_t half_hash_size = bytes<128>;
static constexpr size_t quarter_hash_size = bytes<64>;
static constexpr size_t mini_hash_size = bytes<48>;

/// Common bitcoin hash containers.
typedef data_array<long_hash_size> long_hash;
typedef data_array<hash_size> hash_digest;
typedef data_array<short_hash_size> short_hash;
typedef data_array<quarter_hash_size> quarter_hash;
typedef data_array<half_hash_size> half_hash;
typedef data_array<mini_hash_size> mini_hash;

/// Shared pointers to common bitcoin hash containers.
typedef std::shared_ptr<long_hash> long_hash_ptr;
typedef std::shared_ptr<hash_digest> hash_ptr;
typedef std::shared_ptr<short_hash> short_hash_ptr;
typedef std::shared_ptr<quarter_hash> quarter_hash_ptr;
typedef std::shared_ptr<half_hash> half_hash_ptr;
typedef std::shared_ptr<mini_hash> mini_hash_ptr;

/// Shared pointers to const common bitcoin hash containers.
typedef std::shared_ptr<const long_hash> long_hash_cptr;
typedef std::shared_ptr<const hash_digest> hash_cptr;
typedef std::shared_ptr<const short_hash> short_hash_cptr;
typedef std::shared_ptr<const quarter_hash> quarter_hash_cptr;
typedef std::shared_ptr<const half_hash> half_hash_cptr;
typedef std::shared_ptr<const mini_hash> mini_hash_cptr;

/// Hashes are not pmr types, preserves constexpr.
typedef std::vector<hash_digest> hashes;

/// Null-valued common hashes.
constexpr long_hash null_long_hash{};
constexpr hash_digest null_hash{};
constexpr short_hash null_short_hash{};
constexpr half_hash null_half_hash{};
constexpr quarter_hash null_quarter_hash{};
constexpr mini_hash null_mini_hash{};

/// Consensus sentinel hash [signature hashing].
constexpr hash_digest one_hash = from_uintx(uint256_t(one));

/// Generalized cryptographic hash functions.
/// ---------------------------------------------------------------------------

/// rmd128 [historical].
template <typename Type>
INLINE half_hash  rmd128_hash(const Type& data) NOEXCEPT;
template <typename Type>
INLINE data_chunk rmd128_chunk(const Type& data) NOEXCEPT;

/// rmd160 [script].
template <typename Type>
INLINE short_hash rmd160_hash(const Type& data) NOEXCEPT;
template <typename Type>
INLINE data_chunk rmd160_chunk(const Type& data) NOEXCEPT;

/// sha1 (sha160) [script].
template <typename Type>
INLINE short_hash sha1_hash(const Type& data) NOEXCEPT;
template <typename Type>
INLINE data_chunk sha1_chunk(const Type& data) NOEXCEPT;

/// sha256 [script, wallet].
template <typename Type>
INLINE hash_digest sha256_hash(const Type& data) NOEXCEPT;
INLINE hash_digest sha256_hash(const hash_digest& left,
    const hash_digest& right) NOEXCEPT;
INLINE hash_digest sha256_hash2(const data_slice& left,
    const data_slice& right) NOEXCEPT;
template <typename Type>
INLINE data_chunk sha256_chunk(const Type& data) NOEXCEPT;

/// sha512 hash [wallet].
template <typename Type>
INLINE long_hash  sha512_hash(const Type& data) NOEXCEPT;
template <typename Type>
INLINE data_chunk sha512_chunk(const Type& data) NOEXCEPT;

/// Specialized cryptographic hash functions.
/// ---------------------------------------------------------------------------

/// Bitcoin short hash (rmd160(sha256)) [script].
template <typename Type>
INLINE short_hash bitcoin_short_hash(const Type& data) NOEXCEPT;
template <typename Type>
INLINE data_chunk bitcoin_short_chunk(const Type& data) NOEXCEPT;

/// Bitcoin hash (sha256(sha256)) [script, chain, wallet].
template <typename Type>
INLINE hash_digest bitcoin_hash(const Type& data) NOEXCEPT;
INLINE hash_digest bitcoin_hash(size_t size, const uint8_t* data) NOEXCEPT;
INLINE hash_digest bitcoin_hash(const hash_digest& left,
    const hash_digest& right) NOEXCEPT;
INLINE hash_digest bitcoin_hash2(const data_slice& left,
    const data_slice& right) NOEXCEPT;
template <typename Type>
INLINE data_chunk bitcoin_chunk(const Type& data) NOEXCEPT;

/// Merkle root from a bitcoin_hash set [chain].
INLINE hash_digest merkle_root(hashes&& set) NOEXCEPT;

/// Litecoin scrypt hash [chain].
INLINE hash_digest scrypt_hash(const data_slice& data) NOEXCEPT;

/// Hash table keying.
/// ---------------------------------------------------------------------------

/// For explicit use with std library containers.
template <size_t Size = hash_size>
struct unique_hash_t;

/// This is used in critical database indexation, modify with great care.
/// Assumes high degree of uniqueness in low order sizeof(size_t) bytes of key.
template <size_t Size>
INLINE constexpr size_t unique_hash(const data_array<Size>& key) NOEXCEPT;

/// DJB2 hash key algorithm [Daniel J. Bernstein] [hash tables].
INLINE constexpr size_t djb2_hash(const data_slice& data) NOEXCEPT;

/// Combine hash values, such as a pair of djb2_hash outputs [hash tables].
INLINE constexpr size_t hash_combine(size_t left, size_t right) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

/// Extend std and boost namespaces with djb2_hash.
/// ---------------------------------------------------------------------------
/// This allows data_array/chunk to be incorporated into std/boost hash tables.

namespace std
{
template <>
struct hash<bc::system::data_chunk>
{
    size_t operator()(const bc::system::data_chunk& data) const NOEXCEPT
    {
        return bc::system::djb2_hash(data);
    }
};

template <size_t Size>
struct hash<bc::system::data_array<Size>>
{
    size_t operator()(const bc::system::data_array<Size>& data) const NOEXCEPT
    {
        return bc::system::djb2_hash(data);
    }
};
} // namespace std

namespace boost
{
template <>
struct hash<bc::system::data_chunk>
{
    size_t operator()(const bc::system::data_chunk& data) const NOEXCEPT
    {
        return bc::system::djb2_hash(data);
    }
};

template <size_t Size>
struct hash<bc::system::data_array<Size>>
{
    size_t operator()(const bc::system::data_array<Size>& data) const NOEXCEPT
    {
        return bc::system::djb2_hash(data);
    }
};
} // namespace boost

#include <bitcoin/system/impl/hash/functions.ipp>

#endif

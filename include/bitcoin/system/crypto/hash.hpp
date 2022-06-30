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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_HASH_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_HASH_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Common bitcoin hash container sizes.
static constexpr size_t hash_size = 32;
static constexpr size_t half_hash_size = hash_size / 2;
static constexpr size_t quarter_hash_size = half_hash_size / 2;
static constexpr size_t long_hash_size = 2 * hash_size;
static constexpr size_t short_hash_size = 20;
static constexpr size_t mini_hash_size = 6;

/// Common bitcoin hash containers.
typedef data_array<mini_hash_size> mini_hash;
typedef data_array<quarter_hash_size> quarter_hash;
typedef data_array<half_hash_size> half_hash;
typedef data_array<short_hash_size> short_hash;
typedef data_array<hash_size> hash_digest;
typedef data_array<long_hash_size> long_hash;
typedef std::shared_ptr<hash_digest> hash_ptr;

/// Lists of common bitcoin hashes.
typedef std::vector<mini_hash> mini_hash_list;
typedef std::vector<quarter_hash> quarter_hash_list;
typedef std::vector<half_hash> half_hash_list;
typedef std::vector<short_hash> short_hash_list;
typedef std::vector<hash_digest> hash_list;
typedef std::vector<long_hash> long_hash_list;

/// Null-valued common hashes.

constexpr mini_hash null_mini_hash
{
    {
        0, 0, 0, 0, 0, 0
    }
};

constexpr quarter_hash null_quarter_hash
{
    {
        0, 0, 0, 0, 0, 0, 0, 0
    }
};

constexpr half_hash null_half_hash
{
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
};

constexpr short_hash null_short_hash
{
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
};

constexpr hash_digest null_hash
{
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
};

// Consensus sentinel.
constexpr hash_digest one_hash
{
    {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
};

constexpr long_hash null_long_hash
{
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
};

/// Hash conversions of corresponding integers.
template <uint32_t Bits, size_t Bytes = to_ceilinged_bytes(Bits)>
constexpr data_array<Bytes> to_hash(const uintx_t<Bits>& value) NOEXCEPT;

/// Integer conversions of corresponding hashes.
template <size_t Bytes, uint32_t Bits = to_bits<uint32_t>(Bytes)>
constexpr uintx_t<Bits> to_uintx(const data_array<Bytes>& hash) NOEXCEPT;

/// Generate a scrypt hash to fill a byte array.
/// Memory required (bytes) = 2 * 64 * work * resources.
template <size_t Size>
inline data_array<Size> scrypt(const data_slice& data, const data_slice& salt,
    uint64_t work, uint32_t resources, uint32_t parallelism) NOEXCEPT;

/// Generate a scrypt hash.
BC_API hash_digest scrypt_hash(const data_slice& data) NOEXCEPT;

/// Generate a bitcoin hash.
BC_API hash_digest bitcoin_hash(const data_slice& data) NOEXCEPT;

/// Generate a bitcoin hash of first + second concatenation.
/// This hash function is used in merkle root generation.
BC_API hash_digest bitcoin_hash(const data_slice& first,
    const data_slice& second) NOEXCEPT;

/// Reduce a set of bitcoin hashes by bitcoin-hashing pairs in place.
BC_API bool hash_reduce(std::vector<hash_digest>& hashes) NOEXCEPT;

/// Generate a bitcoin short hash.
BC_API short_hash bitcoin_short_hash(const data_slice& data) NOEXCEPT;

/// Generate a ripemd160 hash.
BC_API short_hash ripemd160_hash(const data_slice& data) NOEXCEPT;
BC_API data_chunk ripemd160_hash_chunk(const data_slice& data) NOEXCEPT;

/// Generate a sha1 hash.
BC_API short_hash sha1_hash(const data_slice& data) NOEXCEPT;
BC_API data_chunk sha1_hash_chunk(const data_slice& data) NOEXCEPT;

/// Generate a sha256 hash.
BC_API hash_digest sha256_hash(const data_slice& data) NOEXCEPT;
BC_API data_chunk sha256_hash_chunk(const data_slice& data) NOEXCEPT;

/// Generate a sha256 hash of first + second concatenation.
/// This hash function is used in electrum seed stretching.
BC_API hash_digest sha256_hash(const data_slice& first,
    const data_slice& second) NOEXCEPT;
// Generate a hmac sha256 hash.
BC_API hash_digest hmac_sha256_hash(const data_slice& data,
    const data_slice& key) NOEXCEPT;

/// Generate a pbkdf2 hmac sha256 hash.
BC_API data_chunk pbkdf2_hmac_sha256_chunk(const data_slice& passphrase,
    const data_slice& salt, size_t iterations, size_t length) NOEXCEPT;

/// Generate a sha512 hash.
BC_API long_hash sha512_hash(const data_slice& data) NOEXCEPT;

/// Generate a hmac sha512 hash.
BC_API long_hash hmac_sha512_hash(const data_slice& data,
    const data_slice& key) NOEXCEPT;

/// Generate a pkcs5 pbkdf2 hmac sha512 hash.
BC_API long_hash pkcs5_pbkdf2_hmac_sha512(const data_slice& passphrase,
    const data_slice& salt, size_t iterations) NOEXCEPT;

/// Generate a scrypt hash of specified length.
BC_API data_chunk scrypt_chunk(const data_slice& data, const data_slice& salt,
    uint64_t work, uint32_t resources, uint32_t parallelism,
    size_t length) NOEXCEPT;

/// DJB2 hash key algorithm by Dan Bernstein.
BC_API size_t djb2_hash(const data_slice& data) NOEXCEPT;

/// Combine hash values, such as a pair of djb2_hash outputs.
constexpr size_t hash_combine(size_t left, size_t right) NOEXCEPT
{
    return left ^ shift_left(right, one);
}

} // namespace system
} // namespace libbitcoin

// Extend std and boost namespaces with djb2_hash.
// ----------------------------------------------------------------------------
// This allows data_array/chunk to be incorporated into std/boost hash tables.

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
    size_t operator()(const bc::system::data_chunk& data) const  NOEXCEPT
    {
        return bc::system::djb2_hash(data);
    }
};

template <size_t Size>
struct hash<bc::system::data_array<Size>>
{
    size_t operator()(const bc::system::data_array<Size>& data) const  NOEXCEPT
    {
        return bc::system::djb2_hash(data);
    }
};
} // namespace boost

#include <bitcoin/system/impl/crypto/hash.ipp>

#endif

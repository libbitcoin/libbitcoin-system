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

#include <memory>
#include <vector>
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
typedef std::shared_ptr<hash_digest> hash_ptr;

/// Lists of common bitcoin hashes.
typedef std::vector<long_hash> long_hashes;
typedef std::vector<hash_digest> hashes;
typedef std::vector<short_hash> short_hashes;
typedef std::vector<half_hash> half_hashes;
typedef std::vector<quarter_hash> quarter_hashes;
typedef std::vector<mini_hash> mini_hashes;

/// Null-valued common hashes.
constexpr long_hash null_long_hash{};
constexpr hash_digest null_hash{};
constexpr short_hash null_short_hash{};
constexpr half_hash null_half_hash{};
constexpr quarter_hash null_quarter_hash{};
constexpr mini_hash null_mini_hash{};

// Consensus sentinel hash.
constexpr hash_digest one_hash = from_uintx(uint256_t(one));

/// Generate a bitcoin hash (sha256(sha256)).
BC_API hash_digest bitcoin_hash(const data_slice& data) NOEXCEPT;

/// Generate a bitcoin hash (sha256(sha256)) of left + right concatenation.
/// Used for generation of witness commitment.
BC_API hash_digest bitcoin_hash(const hash_digest& left,
    const hash_digest& right) NOEXCEPT;

/// Generate a bitcoin short hash (ripemd160(sha256)).
BC_API short_hash bitcoin_short_hash(const data_slice& data) NOEXCEPT;

/// Generate a bitcoin merkle root from an ordered set of hashes.
BC_API hash_digest merkle_root(hashes&& set) NOEXCEPT;

/// Generate a ripemd160 hash.
BC_API short_hash ripemd160_hash(const data_slice& data) NOEXCEPT;
BC_API data_chunk ripemd160_chunk(const data_slice& data) NOEXCEPT;

/// Generate a sha1 (160 bit) hash.
BC_API short_hash sha1_hash(const data_slice& data) NOEXCEPT;
BC_API data_chunk sha1_chunk(const data_slice& data) NOEXCEPT;

/// Generate a sha256 hash.
BC_API hash_digest sha256_hash(const data_slice& data) NOEXCEPT;
BC_API data_chunk sha256_chunk(const data_slice& data) NOEXCEPT;

/// Generate a sha256 hash of left + right concatenation.
/// This hash function is used in electrum seed stretching.
BC_API hash_digest sha256_hash(const data_slice& left,
    const data_slice& right) NOEXCEPT;

// Generate a sha256 hmac.
BC_API hash_digest hmac_sha256(const data_slice& data,
    const data_slice& key) NOEXCEPT;

/// Generate a pkcs5 pbkdf2 hmac sha256 chunk.
/// Used only by scrypt for generation of variably-sized buffer [P * R * 2 * 64].
BC_API data_chunk pbkd_sha256(const data_slice& passphrase,
    const data_slice& salt, size_t iterations, size_t length) NOEXCEPT;

/// Generate a sha512 hash.
BC_API long_hash sha512_hash(const data_slice& data) NOEXCEPT;

/// Generate a sha512 hmac.
BC_API long_hash hmac_sha512(const data_slice& data,
    const data_slice& key) NOEXCEPT;

/// Generate a pkcs5 pbkdf2 hmac sha512 chunk.
BC_API data_chunk pbkd_sha512(const data_slice& passphrase,
    const data_slice& salt, size_t iterations, size_t length) NOEXCEPT;

/// Generate a scrypt hash.
BC_API hash_digest scrypt_hash(const data_slice& data) NOEXCEPT;

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

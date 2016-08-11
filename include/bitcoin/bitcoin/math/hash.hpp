/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_HASH_HPP
#define LIBBITCOIN_HASH_HPP

#include <cstddef>
#include <string>
#include <vector>
#include <boost/functional/hash_fwd.hpp>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {

// Common bitcoin hash container sizes.
static BC_CONSTEXPR size_t hash_size = 32;
static BC_CONSTEXPR size_t half_hash_size = hash_size / 2;
static BC_CONSTEXPR size_t quarter_hash_size = half_hash_size / 2;
static BC_CONSTEXPR size_t long_hash_size = 2 * hash_size;
static BC_CONSTEXPR size_t short_hash_size = 20;
static BC_CONSTEXPR size_t mini_hash_size = 6;

// Common bitcoin hash containers.
typedef byte_array<hash_size> hash_digest;
typedef byte_array<half_hash_size> half_hash;
typedef byte_array<quarter_hash_size> quarter_hash;
typedef byte_array<long_hash_size> long_hash;
typedef byte_array<short_hash_size> short_hash;
typedef byte_array<mini_hash_size> mini_hash;

// Lists of common bitcoin hashes.
typedef std::vector<hash_digest> hash_list;
typedef std::vector<half_hash> half_hash_list;
typedef std::vector<quarter_hash> quarter_hash_list;
typedef std::vector<long_hash> long_hash_list;
typedef std::vector<short_hash> short_hash_list;
typedef std::vector<mini_hash> mini_hash_list;

// Null-valued common bitcoin hashes.

BC_CONSTEXPR hash_digest null_hash
{
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
};

BC_CONSTEXPR half_hash null_half_hash
{
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
};

BC_CONSTEXPR quarter_hash null_quarter_hash
{
    {
        0, 0, 0, 0, 0, 0, 0, 0
    }
};

BC_CONSTEXPR long_hash null_long_hash
{
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
};

BC_CONSTEXPR short_hash null_short_hash
{
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0
    }
};

BC_CONSTEXPR mini_hash null_mini_hash
{
    {
        0, 0, 0, 0, 0, 0
    }
};

/**
 * Generate a scrypt hash to fill a byte array.
 *
 * scrypt(data, salt, params)
 */
template <size_t Size>
byte_array<Size> scrypt(data_slice data, data_slice salt, uint64_t N,
    uint32_t p, uint32_t r);

/**
 * Generate a ripemd160 hash. This hash function is used in script for
 * op_ripemd160.

 * ripemd160(data)
 */
BC_API short_hash ripemd160_hash(data_slice data);

/**
 * Generate a sha1 hash. This hash function is used in script for op_sha1.
 *
 * sha1(data)
 */
BC_API short_hash sha1_hash(data_slice data);

/**
 * Generate a sha256 hash. This hash function is used in mini keys.
 *
 * sha256(data)
 */
BC_API hash_digest sha256_hash(data_slice data);

/**
 * Generate a sha256 hash. This hash function is used in electrum seed
 * stretching (deprecated).
 *
 * sha256(data)
 */
BC_API hash_digest sha256_hash(data_slice first, data_slice second);

/**
 * Generate a hmac sha256 hash. This hash function is used in deterministic
 * signing.
 *
 * hmac-sha256(data, key)
 */
BC_API hash_digest hmac_sha256_hash(data_slice data, data_slice key);

/**
 * Generate a sha512 hash. This hash function is used in bip32 keys.
 *
 * sha512(data)
 */
BC_API long_hash sha512_hash(data_slice data);

/**
 * Generate a hmac sha512 hash. This hash function is used in bip32 keys.
 *
 * hmac-sha512(data, key)
 */
BC_API long_hash hmac_sha512_hash(data_slice data, data_slice key);

/**
 * Generate a pkcs5 pbkdf2 hmac sha512 hash. This hash function is used in
 * bip39 mnemonics.
 *
 * pkcs5_pbkdf2_hmac_sha512(passphrase, salt, iterations)
 */
BC_API long_hash pkcs5_pbkdf2_hmac_sha512(data_slice passphrase,
    data_slice salt, size_t iterations);

/**
 * Generate a typical bitcoin hash. This is the most widely used
 * hash function in Bitcoin.
 *
 * sha256(sha256(data))
 */
BC_API hash_digest bitcoin_hash(data_slice data);

/**
 * Generate a bitcoin short hash. This hash function is used in a
 * few specific cases where short hashes are desired.
 *
 * ripemd160(sha256(data))
 */
BC_API short_hash bitcoin_short_hash(data_slice data);

/**
 * Generate a scrypt hash of specified length.
 *
 * scrypt(data, salt, params)
 */
BC_API data_chunk scrypt(data_slice data, data_slice salt, uint64_t N,
    uint32_t p, uint32_t r, size_t length);

} // namespace libbitcoin

// Extend std and boost namespaces with our hash wrappers.

namespace std
{
template <size_t Size>
struct hash<bc::byte_array<Size>>
{
    size_t operator()(const bc::byte_array<Size>& hash) const
    {
        return boost::hash_range(hash.begin(), hash.end());
    }
};
} // namespace std

namespace boost
{
template <size_t Size>
struct hash<bc::byte_array<Size>>
{
    size_t operator()(const bc::byte_array<Size>& hash) const
    {
        return boost::hash_range(hash.begin(), hash.end());
    }
};
} // namespace boost

#include <bitcoin/bitcoin/impl/math/hash.ipp>

#endif

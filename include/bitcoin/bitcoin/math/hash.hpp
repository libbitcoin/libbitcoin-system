/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {

constexpr size_t short_hash_size = 20;
constexpr size_t hash_size = 32;
constexpr size_t long_hash_size = 64;

// Standard hash containers.
typedef byte_array<short_hash_size> short_hash;
typedef byte_array<hash_size> hash_digest;
typedef byte_array<long_hash_size> long_hash;

// List of hashes. Useful primitive.
typedef std::vector<short_hash> short_hash_list;
typedef std::vector<hash_digest> hash_list;
typedef std::vector<long_hash> long_hash_list;

/**
 * Generate a ripemd160 hash. This hash function is used in script for
 * op_ripemd160.
 *
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

// Make hash_digest and short_hash hashable for std::*map variants
template <typename HashType>
struct BC_API std_hash_wrapper
{
    size_t operator()(const HashType& hash) const
    {
        std::hash<std::string> functor;
        return functor(std::string(std::begin(hash), std::end(hash)));
    }
};

} // namespace libbitcoin

// Extend std namespace with our hash wrappers
namespace std
{
    using libbitcoin::std_hash_wrapper;
    using libbitcoin::short_hash;
    using libbitcoin::hash_digest;
    using libbitcoin::long_hash;

    template <>
    struct BC_API hash<short_hash>
      : public std_hash_wrapper<short_hash>
    {
    };

    template <>
    struct BC_API hash<hash_digest>
      : public std_hash_wrapper<hash_digest>
    {
    };

    template <>
    struct BC_API hash<long_hash>
      : public std_hash_wrapper<long_hash>
    {
    };
} // namespace std

#endif


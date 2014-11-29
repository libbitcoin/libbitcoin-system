/*
 * Copyright (c) 2011-2014 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_EC_KEYS_HPP
#define LIBBITCOIN_EC_KEYS_HPP

#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/hash.hpp>

namespace libbitcoin {

// Private keys:
constexpr size_t ec_secret_size = 32;
typedef byte_array<ec_secret_size> ec_secret;

// Public keys:
constexpr size_t ec_compressed_size = 33;
constexpr size_t ec_uncompressed_size = 65;
typedef data_chunk ec_point;

// Compact signature for message signing:
constexpr size_t compact_signature_size = 64;
struct compact_signature
{
    byte_array<compact_signature_size> signature;
    int recid;
};

/**
 * Converts a secret parameter to a public point.
 */
BC_API ec_point secret_to_public_key(const ec_secret& secret,
    bool compressed=true);

/**
 * Verifies that a data chunk represents a valid EC point.
 */
BC_API bool verify_public_key(const ec_point& public_key);

/**
 * Verifies that a data chunk looks like a valid EC point.
 * Avoid allocations and point arithmetic.
 */
BC_API bool verify_public_key_fast(const ec_point& public_key);

/**
 * Verifies that a private key is within the valid range.
 */
BC_API bool verify_private_key(const ec_secret& private_key);

/**
 * Create a deterministic signature nonce according to rfc6979.
 */
BC_API ec_secret create_nonce(ec_secret secret, hash_digest hash);

/**
 * Create an EC signature using a private key.
 * The nonce must be a cryptographically-secure random number,
 * or the signature will leak information about the private key.
 * Consider using the `create_nonce` function here.
 * @return an EC signature, or a zero-length chunk if something goes wrong.
 * Try another nonce if this happens.
 */
BC_API data_chunk sign(ec_secret secret, hash_digest hash, ec_secret nonce);

/**
 * Create an compact EC signature for use in message signing.
 * The nonce must be a cryptographically-secure random number,
 * or the signature will leak information about the private key.
 * Consider using the `create_nonce` function here.
 * @return a compact signature. This will be all-zero if something goes wrong.
 * Try another nonce if this happens.
 */
BC_API compact_signature sign_compact(ec_secret secret, hash_digest hash,
    ec_secret nonce);

/**
 * Verifies an EC signature using a public key.
 */
BC_API bool verify_signature(const ec_point& public_key, hash_digest hash,
    const data_chunk& signature);

/**
 * Recovers the public key from a compact message signature.
 * @return a public point, or a zero-length chunk if something goes wrong.
 */
BC_API ec_point recover_compact(compact_signature signature,
    hash_digest hash, bool compressed=true);

/**
 * Computes the sum a += G*b, where G is the curve's generator point.
 * @return false on failure (such as infinity or zero).
 */
BC_API bool ec_add(ec_point& a, const ec_secret& b);

/**
 * Computes the sum a = (a + b) % n, where n is the curve order.
 * @return false on failure (such as a zero result).
 */
BC_API bool ec_add(ec_secret& a, const ec_secret& b);

/**
 * Computes the product a *= b.
 * @return false on failure (such as infinity or zero).
 */
BC_API bool ec_multiply(ec_point& a, const ec_secret& b);

/**
 * Computes the product a = (a * b) % n, where n is the curve order.
 * @return false on failure (such as a zero result).
 */
BC_API bool ec_multiply(ec_secret& a, const ec_secret& b);

} // namespace libbitcoin

#endif


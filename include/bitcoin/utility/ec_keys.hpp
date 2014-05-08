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

#include <bitcoin/define.hpp>
#include <bitcoin/types.hpp>

namespace libbitcoin {

constexpr size_t ec_secret_size = 32;
constexpr size_t ec_compressed_size = 33;
constexpr size_t ec_uncompressed_size = 65;

typedef byte_array<ec_secret_size> ec_secret;
typedef data_chunk ec_point;

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
 * Verifies that a private key is within the valid range.
 */
BC_API bool verify_private_key(const ec_secret& private_key);

/**
 * Create an EC signature using a private key. The nonce must be a
 * cryptographically-secure random number, or the signature will leak
 * information about the private key.
 * @return an EC signature, or a zero-length chunk if something goes wrong.
 * Try another nonce if this happens.
 */
BC_API data_chunk sign(ec_secret secret, hash_digest hash, ec_secret nonce);

/**
 * Verifies an EC signature using a public key.
 */
BC_API bool verify_signature(const ec_point& public_key, hash_digest hash,
    const data_chunk& signature);

/**
 * Computes the sum A += G*b, where G is the curve's generator point.
 * @return false on failure (such as infinity or zero).
 */
BC_API bool ec_tweak_add(ec_point& a, const ec_secret& b);

/**
 * Computes the product A *= b.
 * @return false on failure (such as infinity or zero).
 */
BC_API bool ec_multiply(ec_point& a, const ec_secret& b);

/**
 * Computes the sum a = (a + b) % n, where n is the curve order.
 * @return false on failure (such as a zero result).
 */
BC_API bool ec_add(ec_secret& a, const ec_secret& b);

/**
 * Computes the sum a = (a * b) % n, where n is the curve order.
 * @return false on failure (such as a zero result).
 */
BC_API bool ec_multiply(ec_secret& a, const ec_secret& b);

} // namespace libbitcoin

#endif


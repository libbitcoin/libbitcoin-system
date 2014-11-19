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

#include <cstdint>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/types.hpp>

namespace libbitcoin {

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

} // namespace libbitcoin

#endif


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
#ifndef LIBBITCOIN_SHA_HPP
#define LIBBITCOIN_SHA_HPP

#include <cstdint>
#include <bitcoin/define.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/utility/external/ripemd160.h>
#include <bitcoin/utility/external/sha1.h>
#include <bitcoin/utility/external/sha256.h>

namespace libbitcoin {

/**
 * Generate a short_hash. This hash function is used in a
 * few specific cases where short hashes are desired.
 *
 * ripemd(sha256(data))
 */
BC_API short_hash generate_short_hash(const data_chunk& chunk);

/**
 * Generate a normal hash. This is the most widely used
 * hash function in Bitcoin.
 *
 * sha256(sha256(data))
 */
BC_API hash_digest generate_hash(const data_chunk& chunk);

/**
 * Generate a checksum. Last 4 bytes of sha256(sha256(data))
 *
 * int(sha256(sha256(data))[-4:])
 */
BC_API uint32_t generate_checksum(const data_chunk& chunk);

} // namespace libbitcoin

#endif


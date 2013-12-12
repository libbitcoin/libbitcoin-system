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
#include <bitcoin/utility/ripemd.hpp>

#include <boost/detail/endian.hpp>
#include <openssl/sha.h>

#include <bitcoin/utility/logger.hpp>

namespace libbitcoin {

short_hash generate_ripemd_hash(const data_chunk& chunk)
{
    hash_digest sha_hash;
    SHA256_CTX sha_ctx;
    SHA256_Init(&sha_ctx);
    SHA256_Update(&sha_ctx, &chunk[0], chunk.size());
    SHA256_Final(sha_hash.data(), &sha_ctx);

    short_hash ripemd_hash;
    RIPEMD160_CTX ripemd_ctx;
    RIPEMD160_Init(&ripemd_ctx);
    RIPEMD160_Update(&ripemd_ctx, sha_hash.data(), SHA256_DIGEST_LENGTH);
    RIPEMD160_Final(ripemd_hash.data(), &ripemd_ctx);

    return ripemd_hash;
}

} // namespace libbitcoin


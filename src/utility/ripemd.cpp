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
#include <bitcoin/utility/sha256.hpp>

#include <boost/detail/endian.hpp>
#include <openssl/sha.h>

namespace libbitcoin {

void RIPEMD160(const uint8_t* data, const uint32_t size, uint8_t* hash)
{
    RMD160_CTX ripemd_ctx;
    RMD160Init(&ripemd_ctx);
    RMD160Update(&ripemd_ctx, data, size);
    RMD160Final(hash, &ripemd_ctx);
}

short_hash generate_ripemd_hash(const data_chunk& chunk)
{
    hash_digest sha_hash;
    SHA256_CTX sha_ctx;
    SHA256_Init(&sha_ctx);
    SHA256_Update(&sha_ctx, &chunk[0], chunk.size());
    SHA256_Final(sha_hash.data(), &sha_ctx);

    short_hash ripemd_hash;
    RIPEMD160(sha_hash.data(), static_cast<uint32_t>(ripemd_hash.size()),
        ripemd_hash.data());

    return ripemd_hash;
}

} // namespace libbitcoin
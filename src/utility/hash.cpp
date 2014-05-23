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
#include <bitcoin/utility/hash.hpp>

#include <bitcoin/format.hpp>
#include <bitcoin/utility/external/hmac_sha512.h>
#include <bitcoin/utility/external/ripemd160.h>
#include <bitcoin/utility/external/sha1.h>
#include <bitcoin/utility/external/sha256.h>
#include <bitcoin/utility/external/sha512.h>

namespace libbitcoin {

short_hash ripemd160_hash(const data_chunk& chunk)
{
    short_hash hash;
    RMD160(chunk.data(), chunk.size(), hash.data());
    return hash;
}

short_hash sha1_hash(const data_chunk& chunk)
{
    short_hash hash;
    SHA1(chunk.data(), chunk.size(), hash.data());
    return hash;
}

hash_digest sha256_hash(const data_chunk& chunk)
{
    hash_digest hash;
    SHA256__(chunk.data(), chunk.size(), hash.data());
    return hash;
}

hash_digest sha256_hash(const data_chunk& first_chunk,
    const data_chunk& second_chunk)
{
    hash_digest hash;

    SHA256CTX context;
    SHA256Init(&context);
    SHA256Update(&context, first_chunk.data(), first_chunk.size());
    SHA256Update(&context, second_chunk.data(), second_chunk.size());
    SHA256Final(&context, hash.data());

    return hash;
}

long_hash sha512_hash(const data_chunk& chunk)
{
    long_hash hash;
    SHA512__(chunk.data(), chunk.size(), hash.data());
    return hash;
}

long_hash hmac_sha512_hash(const data_chunk& chunk, 
    const data_chunk& key)
{
    long_hash hash;
    HMACSHA512(chunk.data(), chunk.size(), key.data(),
        key.size(), hash.data());
    return hash;
}

hash_digest bitcoin_hash(const data_chunk& chunk)
{
    hash_digest first_hash;
    SHA256__(chunk.data(), chunk.size(), first_hash.data());

    hash_digest second_hash;
    SHA256__(first_hash.data(), first_hash.size(), second_hash.data());

    // The hash is in the reverse of the expected order.
    std::reverse(second_hash.begin(), second_hash.end());
    return second_hash;
}

short_hash bitcoin_short_hash(const data_chunk& chunk)
{
    hash_digest sha_hash;
    SHA256__(chunk.data(), chunk.size(), sha_hash.data());

    short_hash ripemd_hash;
    RMD160(sha_hash.data(), sha_hash.size(), ripemd_hash.data());

    return ripemd_hash;
}


uint32_t bitcoin_checksum(const data_chunk& chunk)
{
    hash_digest hash = bitcoin_hash(chunk);
    return from_little_endian<uint32_t>(hash.rbegin());
}

} // namespace libbitcoin


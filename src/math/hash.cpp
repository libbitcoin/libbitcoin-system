/*
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/math/hash.hpp>

#include <algorithm>
#include <bitcoin/bitcoin/math/external/hmac_sha256.h>
#include <bitcoin/bitcoin/math/external/hmac_sha512.h>
#include <bitcoin/bitcoin/math/external/pkcs5_pbkdf2.h>
#include <bitcoin/bitcoin/math/external/ripemd160.h>
#include <bitcoin/bitcoin/math/external/sha1.h>
#include <bitcoin/bitcoin/math/external/sha256.h>
#include <bitcoin/bitcoin/math/external/sha512.h>

namespace libbitcoin {

short_hash ripemd160_hash(data_slice data)
{
    short_hash hash;
    RMD160(data.data(), data.size(), hash.data());
    return hash;
}

short_hash sha1_hash(data_slice data)
{
    short_hash hash;
    SHA1(data.data(), data.size(), hash.data());
    return hash;
}

hash_digest sha256_hash(data_slice data)
{
    hash_digest hash;
    SHA256(data.data(), data.size(), hash.data());
    return hash;
}

hash_digest sha256_hash(data_slice first, data_slice second)
{
    hash_digest hash;

    SHA256CTX context;
    SHA256Init(&context);
    SHA256Update(&context, first.data(), first.size());
    SHA256Update(&context, second.data(), second.size());
    SHA256Final(&context, hash.data());

    return hash;
}

hash_digest hmac_sha256_hash(data_slice data, data_slice key)
{
    hash_digest hash;
    HMACSHA256(data.data(), data.size(), key.data(),
        key.size(), hash.data());
    return hash;
}

long_hash sha512_hash(data_slice data)
{
    long_hash hash;
    SHA512(data.data(), data.size(), hash.data());
    return hash;
}

long_hash hmac_sha512_hash(data_slice data, data_slice key)
{
    long_hash hash;
    HMACSHA512(data.data(), data.size(), key.data(),
        key.size(), hash.data());
    return hash;
}

bool pkcs5_pbkdf2_hmac_sha512(const std::string& passphrase, 
    data_slice salt, size_t iterations, long_hash& long_hash)
{
    const auto passphrase_bytes = to_data_chunk(passphrase);
    return pkcs5_pbkdf2(&passphrase_bytes[0], passphrase_bytes.size(),
        salt.data(), salt.size(), long_hash.data(), sizeof(long_hash),
        iterations) == 0;
}

hash_digest bitcoin_hash(data_slice data)
{
    return sha256_hash(sha256_hash(data));
}

short_hash bitcoin_short_hash(data_slice data)
{
    return ripemd160_hash(sha256_hash(data));
}

} // namespace libbitcoin


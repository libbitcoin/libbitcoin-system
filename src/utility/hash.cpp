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

#include <bitcoin/utility/external/ripemd160.h>
#include <bitcoin/utility/external/sha256.h>

#include <boost/detail/endian.hpp>
#include <bitcoin/format.hpp>

namespace libbitcoin {

//short_hash generate_ripemd160_hash(const data_chunk& chunk)
//{
//    short_hash hash;
//    RMD160(chunk.data(), static_cast<uint32_t>(hash.size()), hash.data());
//    return hash;
//}
//
//short_hash generate_sha1_hash(const data_chunk& chunk)
//{
//    short_hash hash;
//    SHA1(chunk.data(), static_cast<uint32_t>(chunk.size()), hash.data());
//    return hash;
//}
//
//hash_digest generate_sha256_hash(const data_chunk& chunk)
//{
//    hash_digest hash;
//    SHA256(chunk.data(), static_cast<uint32_t>(chunk.size()), hash.data());
//    return hash;
//}

short_hash generate_short_hash(const data_chunk& chunk)
{
    hash_digest sha_hash;
    SHA256__(chunk.data(), static_cast<uint32_t>(chunk.size()),
        sha_hash.data());

    short_hash ripemd_hash;
    RMD160(sha_hash.data(), static_cast<uint32_t>(ripemd_hash.size()), 
        ripemd_hash.data());

    return ripemd_hash;
}

hash_digest generate_hash(const data_chunk& chunk)
{
    hash_digest first_hash;
    SHA256__(chunk.data(), static_cast<uint32_t>(chunk.size()),
        first_hash.data());

    hash_digest second_hash;
    SHA256__(first_hash.data(), static_cast<uint32_t>(first_hash.size()),
        second_hash.data());

    // *** TODO: VERIFY - this is not OpenSSL ***
    // SSL gives us the hash backwards
    std::reverse(second_hash.begin(), second_hash.end());
    return second_hash;
}

uint32_t generate_checksum(const data_chunk& chunk)
{
    hash_digest hash = generate_hash(chunk);
    data_chunk begin_bytes(hash.rbegin(), hash.rbegin() + 4);
    return cast_chunk<uint32_t>(begin_bytes);
}

} // namespace libbitcoin
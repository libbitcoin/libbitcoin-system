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
#include <bitcoin/utility/sha256.hpp>

#include <boost/detail/endian.hpp>
#include <bitcoin/format.hpp>

namespace libbitcoin {

hash_digest generate_sha256_hash(const data_chunk& data)
{
    SHA256_CTX ctx;
    hash_digest digest;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, &data[0], data.size());
    SHA256_Final(digest.data(), &ctx);
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, digest.data(), sha256_digest_size);
    SHA256_Final(digest.data(), &ctx);
    // SSL gives us the hash backwards
    std::reverse(digest.begin(), digest.end());
    return digest;
}

uint32_t generate_sha256_checksum(const data_chunk& data)
{
    hash_digest hash = generate_sha256_hash(data);
    data_chunk begin_bytes(hash.rbegin(), hash.rbegin() + 4);
    return cast_chunk<uint32_t>(begin_bytes);
}

} // namespace libbitcoin


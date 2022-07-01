/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/system/crypto/hash.hpp>

#include <cstddef>
#include <cstdint>
#include <vector>
// DELETEMENOW
#include <bitcoin/system/crypto/external/crypto_scrypt.h>
#include <bitcoin/system/crypto/external/hmac_sha256.h>
#include <bitcoin/system/crypto/external/hmac_sha512.h>
#include <bitcoin/system/crypto/external/pbkdf2_sha256.h>
#include <bitcoin/system/crypto/external/pkcs5_pbkdf2.h>
#include <bitcoin/system/crypto/external/ripemd160.h>
#include <bitcoin/system/crypto/external/sha1.h>
////#include <bitcoin/system/crypto/external/sha256.h>
#include <bitcoin/system/crypto/external/sha512.h>
#include <bitcoin/system/crypto/intrinsics/intrinsics.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// Hash generators.
// ----------------------------------------------------------------------------

hash_digest scrypt_hash(const data_slice& data) NOEXCEPT
{
    return scrypt<hash_size>(data, data, 1024u, 1u, 1u);
}

hash_digest bitcoin_hash(const data_slice& data) NOEXCEPT
{
    return sha256_hash(sha256_hash(data));
}

hash_digest bitcoin_hash(const data_slice& first,
    const data_slice& second) NOEXCEPT
{
    return sha256_hash(sha256_hash(first, second));
}

bool hash_reduce(std::vector<hash_digest>& hashes) NOEXCEPT
{
    const auto size = hashes.size();

    // Buffer must be a multiple of 64 (two * hash_size).
    if (is_odd(size))
        return false;

    // std::vector<std::array<uint8_t, N>>.data() is size * N contiguous bytes.
    auto buffer = hashes.front().data();

    // Buffer is transformed in place.
    intrinsics::sha256_paired_double(buffer, buffer, to_half(size));
    hashes.resize(to_half(size));
    return true;
}

short_hash bitcoin_short_hash(const data_slice& data) NOEXCEPT
{
    return ripemd160_hash(sha256_hash(data));
}

short_hash ripemd160_hash(const data_slice& data) NOEXCEPT
{
    short_hash hash;
    RMD160(data.data(), data.size(), hash.data());
    return hash;
}

data_chunk ripemd160_hash_chunk(const data_slice& data) NOEXCEPT
{
    data_chunk hash(short_hash_size, no_fill_byte_allocator);
    RMD160(data.data(), data.size(), hash.data());
    return hash;
}

short_hash sha1_hash(const data_slice& data) NOEXCEPT
{
    short_hash hash;
    SHA1(data.data(), data.size(), hash.data());
    return hash;
}

data_chunk sha1_hash_chunk(const data_slice& data) NOEXCEPT
{
    data_chunk hash(short_hash_size, no_fill_byte_allocator);
    SHA1(data.data(), data.size(), hash.data());
    return hash;
}

hash_digest sha256_hash(const data_slice& data) NOEXCEPT
{
    hash_digest hash;
    intrinsics::sha256(data.data(), data.size(), hash.data());
    return hash;
}

data_chunk sha256_hash_chunk(const data_slice& data) NOEXCEPT
{
    data_chunk hash(hash_size, no_fill_byte_allocator);
    intrinsics::sha256(data.data(), data.size(), hash.data());
    return hash;
}

hash_digest sha256_hash(const data_slice& first,
    const data_slice& second) NOEXCEPT
{
    using namespace intrinsics;

    hash_digest hash;
    sha256_context context{ sha256_initial };
    sha256_update(context, first.data(), first.size());
    sha256_update(context, second.data(), second.size());
    sha256_finalize(context, hash.data());
    return hash;
}

hash_digest hmac_sha256_hash(const data_slice& data,
    const data_slice& key) NOEXCEPT
{
    hash_digest hash;
    HMACSHA256(data.data(), data.size(), key.data(), key.size(), hash.data());
    return hash;
}

data_chunk pbkdf2_hmac_sha256_chunk(const data_slice& passphrase,
    const data_slice& salt, size_t iterations, size_t length)noexcept
{
    data_chunk hash(length, no_fill_byte_allocator);
    pbkdf2_sha256(passphrase.data(), passphrase.size(), salt.data(),
        salt.size(), iterations, hash.data(), length);
    return hash;
}

long_hash sha512_hash(const data_slice& data) NOEXCEPT
{
    long_hash hash;
    SHA512(data.data(), data.size(), hash.data());
    return hash;
}

long_hash hmac_sha512_hash(const data_slice& data,
    const data_slice& key) NOEXCEPT
{
    long_hash hash;
    HMACSHA512(data.data(), data.size(), key.data(), key.size(), hash.data());
    return hash;
}

long_hash pkcs5_pbkdf2_hmac_sha512(const data_slice& passphrase,
    const data_slice& salt, size_t iterations) NOEXCEPT
{
    auto hash = null_long_hash;
    pkcs5_pbkdf2(passphrase.data(), passphrase.size(),
        salt.data(), salt.size(), hash.data(), hash.size(), iterations);

    // If pkcs5_pbkdf2 returns != 0 then hash will be zeroized.
    // This can only be caused by out-of-memory or invalid parameterization.
    return hash;
}

data_chunk scrypt_chunk(const data_slice& data, const data_slice& salt,
    uint64_t work, uint32_t resources, uint32_t parallelism,
    size_t length) NOEXCEPT
{
    data_chunk hash(no_fill_byte_allocator);
    data_chunk out(length, 0x00);
    crypto_scrypt(data.data(), data.size(), salt.data(), salt.size(), work,
        resources, parallelism, out.data(), out.size());

    // If crypto_scrypt returns != 0 then out will be zeroized.
    // This can only be caused by out-of-memory or invalid parameterization.
    return out;
}

// Objectives: deterministic, uniform distribution, efficient computation.
size_t djb2_hash(const data_slice& data) NOEXCEPT
{
    // Nothing special here except that it tested well against collisions.
    size_t hash = 5381;

    // Efficient sum of ((hash * 33) + byte) for all bytes.
    for (const auto byte: data)
        hash = ((hash << 5) + hash) + byte;

    return hash;
}

} // namespace system
} // namespace libbitcoin

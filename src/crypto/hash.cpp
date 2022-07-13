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

#include <vector>
#include <bitcoin/system/crypto/external/external.hpp>
#include <bitcoin/system/crypto/sha256.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// Hash generators.
// ----------------------------------------------------------------------------

hash_digest scrypt_hash(const data_slice& data) NOEXCEPT
{
    return scrypt<hash_size>(data, data, 1024_u64, 1_u32, 1_u32);
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

    // buffer is [64 * size/2] contiguous bytes, transformed in place.
    auto buffer = hashes.front().data();
    sha256::sha256_double(buffer, to_half(size), buffer);
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
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    RMD160(data.data(), data.size(), hash.data());
    BC_POP_WARNING()
    return hash;
}

data_chunk ripemd160_hash_chunk(const data_slice& data) NOEXCEPT
{
    data_chunk hash(short_hash_size, no_fill_byte_allocator);
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    RMD160(data.data(), data.size(), hash.data());
    BC_POP_WARNING()
    return hash;
}

short_hash sha1_hash(const data_slice& data) NOEXCEPT
{
    short_hash hash;
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    SHA1(data.data(), data.size(), hash.data());
    BC_POP_WARNING()
    return hash;
}

data_chunk sha1_hash_chunk(const data_slice& data) NOEXCEPT
{
    data_chunk hash(short_hash_size, no_fill_byte_allocator);
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    SHA1(data.data(), data.size(), hash.data());
    BC_POP_WARNING()
    return hash;
}

hash_digest sha256_hash(const data_slice& data) NOEXCEPT
{
    hash_digest hash;
    sha256::sha256_single(hash.data(), data.size(), data.data());
    return hash;
}

data_chunk sha256_hash_chunk(const data_slice& data) NOEXCEPT
{
    data_chunk hash(hash_size, no_fill_byte_allocator);
    sha256::sha256_single(hash.data(), data.size(), data.data());
    return hash;
}

// This overload precludes the need to concatenate first + second.
hash_digest sha256_hash(const data_slice& first,
    const data_slice& second) NOEXCEPT
{
    hash_digest hash;
    sha256::context context{};
    sha256::update(context, first.size(), first.data());
    sha256::update(context, second.size(), second.data());
    sha256::finalize(context, hash.data());
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
    const data_slice& salt, size_t iterations, size_t length) NOEXCEPT
{
    data_chunk hash(length, no_fill_byte_allocator);
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    pbkdf2_sha256(passphrase.data(), passphrase.size(), salt.data(),
        salt.size(), iterations, hash.data(), length);
    BC_POP_WARNING()
    return hash;
}

long_hash sha512_hash(const data_slice& data) NOEXCEPT
{
    long_hash hash;
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    SHA512(data.data(), data.size(), hash.data());
    BC_POP_WARNING()
    return hash;
}

long_hash hmac_sha512_hash(const data_slice& data,
    const data_slice& key) NOEXCEPT
{
    long_hash hash;
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    HMACSHA512(data.data(), data.size(), key.data(), key.size(), hash.data());
    BC_POP_WARNING()
    return hash;
}

long_hash pkcs5_pbkdf2_hmac_sha512(const data_slice& passphrase,
    const data_slice& salt, size_t iterations) NOEXCEPT
{
    // If pkcs5_pbkdf2 returns != 0 then hash will be zeroized.
    // This can only be caused by out-of-memory or invalid parameterization.
    long_hash hash{};

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    pkcs5_pbkdf2(passphrase.data(), passphrase.size(),
        salt.data(), salt.size(), hash.data(), hash.size(), iterations);
    BC_POP_WARNING()
    return hash;
}

data_chunk scrypt_chunk(const data_slice& data, const data_slice& salt,
    uint64_t work, uint32_t resources, uint32_t parallelism,
    size_t length) NOEXCEPT
{
    // If crypto_scrypt returns != 0 then out will be zeroized.
    // This can only be caused by out-of-memory or invalid parameterization.
    data_chunk hash(length, 0_u8);

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    crypto_scrypt(data.data(), data.size(), salt.data(), salt.size(), work,
        resources, parallelism, hash.data(), hash.size());
    BC_POP_WARNING()
    return hash;
}

// Objectives: deterministic, uniform distribution, efficient computation.
size_t djb2_hash(const data_slice& data) NOEXCEPT
{
    // Nothing special here except that it tested well against collisions.
    auto hash = 5381_size;

    // Efficient sum of ((hash * 33) + byte) for all bytes.
    for (const auto byte: data)
        hash = (shift_left(hash, 5_size) + hash) + byte;

    return hash;
}

} // namespace system
} // namespace libbitcoin

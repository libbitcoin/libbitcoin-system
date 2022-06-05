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
#include <bitcoin/system/constants.hpp>
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
#include <bitcoin/system/serial/serial.hpp>

namespace libbitcoin {
namespace system {

// Hash conversions of corresponding integers.
// ----------------------------------------------------------------------------

mini_hash to_hash(const uint48_t& value) noexcept
{
    return to_little_endian<to_bytes(48)>(value);
}

quarter_hash to_hash(const uint64_t& value) noexcept
{
    // Use integral uint64_t.
    return to_little_endian(value);
}

half_hash to_hash(const uint128_t& value) noexcept
{
    return to_little_endian<to_bytes(128)>(value);
}

short_hash to_hash(const uint160_t& value) noexcept
{
    return to_little_endian<to_bytes(160)>(value);
}

hash_digest to_hash(const uint256_t& value) noexcept
{
    return to_little_endian<to_bytes(256)>(value);
}

long_hash to_hash(const uint512_t& value) noexcept
{
    return to_little_endian<to_bytes(512)>(value);
}

// Integer conversions of corresponding hashes.
// ----------------------------------------------------------------------------

uint48_t to_uint48(const mini_hash& hash) noexcept
{
    return from_little_endian<to_bytes(48)>(hash);
}

uint64_t to_uint64(const quarter_hash& hash) noexcept
{
    // Use integral uint64_t.
    return from_little_endian<uint64_t>(hash);
}

uint128_t to_uint128(const half_hash& hash) noexcept
{
    return from_little_endian<to_bytes(128)>(hash);
}

uint160_t to_uint160(const short_hash& hash) noexcept
{
    return from_little_endian<to_bytes(160)>(hash);
}

uint256_t to_uint256(const hash_digest& hash) noexcept
{
    return from_little_endian<to_bytes(256)>(hash);
}

uint512_t to_uint512(const long_hash& hash) noexcept
{
    return from_little_endian<to_bytes(512)>(hash);
}

// Hash generators.
// ----------------------------------------------------------------------------

hash_digest scrypt_hash(const data_slice& data) noexcept
{
    return scrypt<hash_size>(data, data, 1024u, 1u, 1u);
}

hash_digest bitcoin_hash(const data_slice& data) noexcept
{
    return sha256_hash(sha256_hash(data));
}

hash_digest bitcoin_hash(const data_slice& first,
    const data_slice& second) noexcept
{
    return sha256_hash(sha256_hash(first, second));
}

bool hash_reduce(std::vector<hash_digest>& hashes) noexcept
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

short_hash bitcoin_short_hash(const data_slice& data) noexcept
{
    return ripemd160_hash(sha256_hash(data));
}

short_hash ripemd160_hash(const data_slice& data) noexcept
{
    short_hash hash;
    RMD160(data.data(), data.size(), hash.data());
    return hash;
}

data_chunk ripemd160_hash_chunk(const data_slice& data) noexcept
{
    data_chunk hash(no_fill_byte_allocator);
    hash.resize(short_hash_size);
    RMD160(data.data(), data.size(), hash.data());
    return hash;
}

short_hash sha1_hash(const data_slice& data) noexcept
{
    short_hash hash;
    SHA1(data.data(), data.size(), hash.data());
    return hash;
}

data_chunk sha1_hash_chunk(const data_slice& data) noexcept
{
    data_chunk hash(no_fill_byte_allocator);
    hash.resize(short_hash_size);
    SHA1(data.data(), data.size(), hash.data());
    return hash;
}

hash_digest sha256_hash(const data_slice& data) noexcept
{
    hash_digest hash;
    intrinsics::sha256(data.data(), data.size(), hash.data());
    return hash;
}

data_chunk sha256_hash_chunk(const data_slice& data) noexcept
{
    data_chunk hash(no_fill_byte_allocator);
    hash.resize(hash_size);
    intrinsics::sha256(data.data(), data.size(), hash.data());
    return hash;
}

hash_digest sha256_hash(const data_slice& first,
    const data_slice& second) noexcept
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
    const data_slice& key) noexcept
{
    hash_digest hash;
    HMACSHA256(data.data(), data.size(), key.data(), key.size(), hash.data());
    return hash;
}

data_chunk pbkdf2_hmac_sha256_chunk(const data_slice& passphrase,
    const data_slice& salt, size_t iterations, size_t length)noexcept
{
    data_chunk hash(no_fill_byte_allocator);
    hash.resize(length);
    pbkdf2_sha256(passphrase.data(), passphrase.size(), salt.data(),
        salt.size(), iterations, hash.data(), length);
    return hash;
}

long_hash sha512_hash(const data_slice& data) noexcept
{
    long_hash hash;
    SHA512(data.data(), data.size(), hash.data());
    return hash;
}

long_hash hmac_sha512_hash(const data_slice& data,
    const data_slice& key) noexcept
{
    long_hash hash;
    HMACSHA512(data.data(), data.size(), key.data(), key.size(), hash.data());
    return hash;
}

long_hash pkcs5_pbkdf2_hmac_sha512(const data_slice& passphrase,
    const data_slice& salt, size_t iterations) noexcept
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
    size_t length) noexcept
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
size_t djb2_hash(const data_slice& data) noexcept
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

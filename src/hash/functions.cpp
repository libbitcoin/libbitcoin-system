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
#include <bitcoin/system/hash/functions.hpp>

#include <vector>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/hash/accumulator.hpp>
#include <bitcoin/system/hash/hmac_sha256.hpp>
#include <bitcoin/system/hash/hmac_sha512.hpp>
#include <bitcoin/system/hash/pbkd_sha256.hpp>
#include <bitcoin/system/hash/pbkd_sha512.hpp>
#include <bitcoin/system/hash/ripemd160.hpp>
#include <bitcoin/system/hash/scrypt.hpp>
#include <bitcoin/system/hash/sha_algorithm.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// Bitcoin doubled-hashes.
// ----------------------------------------------------------------------------

hash_digest bitcoin_hash(const data_slice& data) NOEXCEPT
{
    return sha256_hash(sha256_hash(data));
}

hash_digest bitcoin_hash(const hash_digest& left,
    const hash_digest& right) NOEXCEPT
{
    // TODO: replace inner with single block optimization.
    return sha256_hash(sha256_hash(left, right));
}

short_hash bitcoin_short_hash(const data_slice& data) NOEXCEPT
{
    return ripemd160_hash(sha256_hash(data));
}

// TODO: move to sha::algorithm and optimize.
inline void merkle_hash(uint8_t* out, size_t blocks,
    const uint8_t* in) NOEXCEPT
{
    using hasher = sha::algorithm<sha::sha256>;
    constexpr auto digest_size = array_count<hasher::digest_t>;
    constexpr auto block_size = array_count<hasher::block_t>;

    while (to_bool(blocks--))
    {
        auto& to = unsafe_array_cast<uint8_t, digest_size>(out);
        auto& from = unsafe_array_cast<uint8_t, block_size>(in);
        to = hasher::double_hash(from);
        std::advance(out, hash_size);
        std::advance(in, long_hash_size);
    }
}

// TODO: move to sha::algorithm and optimize.
hash_digest merkle_root(hashes&& set) NOEXCEPT
{
    if (set.empty())
        return {};

    if (is_one(set.size()))
        return std::move(set.front());

    if (is_odd(set.size()))
        set.push_back(set.back());

    while (is_even(set.size()))
    {
        // N blocks in, N/2 hashes out.
        const auto half = to_half(set.size());

        merkle_hash(
            pointer_cast<uint8_t>(set.data()), half,
            pointer_cast<const uint8_t>(set.data()));
        set.resize(half);
    }

    return std::move(set.front());
}

// Hash generators.
// ----------------------------------------------------------------------------

short_hash ripemd160_hash(const data_slice& data) NOEXCEPT
{
    short_hash hash{};
    ripemd160::hash(data.data(), data.size(), hash.data());
    return hash;
}

data_chunk ripemd160_chunk(const data_slice& data) NOEXCEPT
{
    data_chunk hash(short_hash_size, no_fill_byte_allocator);
    ripemd160::hash(data.data(), data.size(), hash.data());
    return hash;
}

short_hash sha1_hash(const data_slice& data) NOEXCEPT
{
    system::accumulator<sha::algorithm<sha::sha160>> context{};
    context.write(data);
    return context.flush();
}

data_chunk sha1_chunk(const data_slice& data) NOEXCEPT
{
    data_chunk hash(short_hash_size, no_fill_byte_allocator);
    system::accumulator<sha::algorithm<sha::sha160>> context{};
    context.write(data);
    context.flush(hash.data());
    return hash;
}

hash_digest sha256_hash(const data_slice& data) NOEXCEPT
{
    system::accumulator<sha::algorithm<sha::sha256>> context{};
    context.write(data);
    return context.flush();
}

data_chunk sha256_chunk(const data_slice& data) NOEXCEPT
{
    data_chunk hash(hash_size, no_fill_byte_allocator);
    system::accumulator<sha::algorithm<sha::sha256>> context{};
    context.write(data);
    context.flush(hash.data());
    return hash;
}

// This overload precludes the need to concatenate left + right.
hash_digest sha256_hash(const data_slice& left,
    const data_slice& right) NOEXCEPT
{
    system::accumulator<sha::algorithm<sha::sha256>> context{};
    context.write(left.size(), left.data());
    context.write(right.size(), right.data());
    return context.flush();
}

hash_digest hmac_sha256(const data_slice& data,
    const data_slice& key) NOEXCEPT
{
    hash_digest hash{};
    hmac::sha256::hash(data.data(), data.size(), key.data(), key.size(),
        hash.data());
    return hash;
}

data_chunk pbkd_sha256(const data_slice& passphrase,
    const data_slice& salt, size_t iterations, size_t length) NOEXCEPT
{
    data_chunk chunk(length, no_fill_byte_allocator);
    pbkd::sha256::hash(passphrase.data(), passphrase.size(), salt.data(),
        salt.size(), iterations, chunk.data(), chunk.size());
    return chunk;
}

long_hash sha512_hash(const data_slice& data) NOEXCEPT
{
    system::accumulator<sha::algorithm<sha::sha512>> context{};
    context.write(data);
    return context.flush();
}

long_hash hmac_sha512(const data_slice& data,
    const data_slice& key) NOEXCEPT
{
    long_hash hash{};
    hmac::sha512::hash(data.data(), data.size(), key.data(), key.size(),
        hash.data());
    return hash;
}

data_chunk pbkd_sha512(const data_slice& passphrase,
    const data_slice& salt, size_t iterations, size_t length) NOEXCEPT
{
    data_chunk chunk(length, no_fill_byte_allocator);
    pbkd::sha512::hash(passphrase.data(), passphrase.size(),
        salt.data(), salt.size(), iterations, chunk.data(), chunk.size());
    return chunk;
}

hash_digest scrypt_hash(const data_slice& data) NOEXCEPT
{
    // Litecoin parameters, with concurrency.
    return scrypt<1024, 1, 1, true>::hash<hash_size>(data, data);
}

// Value will vary with sizeof(size_t).
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

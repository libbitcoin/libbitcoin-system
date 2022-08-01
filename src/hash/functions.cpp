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
#include <bitcoin/system/hash/rmd/ripemd_.hpp>
#include <bitcoin/system/hash/rmd/algorithm.hpp>
#include <bitcoin/system/hash/scrypt.hpp>
#include <bitcoin/system/hash/sha/algorithm.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// Specialized Bitcoin cryptographic hash functions.
// ----------------------------------------------------------------------------
// TODO: optimize through specialization for array/chunk/slice inputs.
// TODO: chunk is more efficient than slice and accumulator can optimize for
// TODO: finalized hashes of fixed (array) sizes.

using sha256_final = accumulator<sha256>;

short_hash bitcoin_short_hash(const data_slice& data) NOEXCEPT
{
    return ripemd160_hash(sha256_final::hash_slice(data));
}

// moved to interpreter stack xptr.
data_chunk bitcoin_short_chunk(const data_slice& data) NOEXCEPT
{
    return ripemd160_chunk(sha256_final::hash_slice(data));
}

hash_digest bitcoin_hash(const data_slice& data) NOEXCEPT
{
    return sha256_final::hash(sha256_final::hash_slice(data));
}

// moved to interpreter stack xptr.
data_chunk bitcoin_chunk(const data_slice& data) NOEXCEPT
{
    return sha256_chunk(sha256_final::hash_slice(data));
}

hash_digest bitcoin_hash(const hash_digest& left,
    const hash_digest& right) NOEXCEPT
{
    return sha256_final::hash(sha256_hash(left, right));
}

// TODO: create std_vector<uint8_t*> parse_block(data_chunk&).
// TODO: requires specialized skip-parsing block reader to isolate txs.
// TODO: merkle_root(bitcoin_hash(parse_block(data_chunk&))).
hashes bitcoin_hash(std_vector<uint8_t*>&&) NOEXCEPT
{
    // TODO: implement in sha::algorithm.
    // TODO: each element represents set of independent sha blocks/remainders.
    // TODO: concurrently hash blocks/remainders across set.
    // TODO: thread parallelize upon vectorize/sha-ni (as in merkle hashing).
    // TODO: individual tx hashing performed concurrently with tx parse.
    // TODO: block merkle root performed concurrently with block.txs parse.
    // TODO: these are two independent jobs both requiring the raw tx/s data.
    return {};
}

// TODO: move to sha::algorithm and optimize.
inline void merkle_hash(uint8_t* out, size_t blocks,
    const uint8_t* in) NOEXCEPT
{
    constexpr auto digest_size = array_count<sha256::digest_t>;
    constexpr auto block_size = array_count<sha256::block_t>;

    // TODO: this loop is inherently unordered (independent hash sets).
    while (to_bool(blocks--))
    {
        auto& to = unsafe_array_cast<uint8_t, digest_size>(out);
        auto& from = unsafe_array_cast<uint8_t, block_size>(in);
        to = sha256::double_hash(from);
        std::advance(out, hash_size);
        std::advance(in, long_hash_size);
    }
}

hash_digest merkle_root(hashes&& set) NOEXCEPT
{
    if (set.empty())
        return {};

    if (is_one(set.size()))
        return std::move(set.front());

    if (is_odd(set.size()))
        set.push_back(set.back());

    // TODO: move to sha::algorithm and optimize.
    // TODO: this loop is inherently ordered (dependent hash sets).
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

hash_digest sha256_hash(const data_slice& left,
    const data_slice& right) NOEXCEPT
{
    accumulator<sha256> context{};
    context.write(left.size(), left.data());
    context.write(right.size(), right.data());
    return context.flush();
}

// Generalized cryptographic hash functions.
// ----------------------------------------------------------------------------
// TODO: optimize through specialization for array/chunk/slice inputs.
// TODO: non-template (array/chunk) should all be inlined.

half_hash ripemd128_hash(const data_slice& data) NOEXCEPT
{
    return accumulator<rmd128>::hash(data);
}

data_chunk ripemd128_chunk(const data_slice& data) NOEXCEPT
{
    data_chunk chunk(half_hash_size, no_fill_byte_allocator);
    return accumulator<rmd128>::hash(chunk, data);
}

short_hash ripemd160_hash(const data_slice& data) NOEXCEPT
{
    short_hash hash{};
    ripemd160::hash(data.data(), data.size(), hash.data());
    return hash;
    ////return accumulator<rmd160>::hash(data);
}

// moved to interpreter stack xptr.
data_chunk ripemd160_chunk(const data_slice& data) NOEXCEPT
{
    data_chunk chunk(short_hash_size, no_fill_byte_allocator);
    ripemd160::hash(data.data(), data.size(), chunk.data());
    return chunk;
    ////return accumulator<rmd160>::hash(chunk, data);
}

short_hash sha1_hash(const data_slice& data) NOEXCEPT
{
    return accumulator<sha160>::hash(data);
}

// moved to interpreter stack xptr.
data_chunk sha1_chunk(const data_slice& data) NOEXCEPT
{
    data_chunk chunk(short_hash_size, no_fill_byte_allocator);
    return accumulator<sha160>::hash(chunk, data);
}

hash_digest sha256_hash(const data_slice& data) NOEXCEPT
{
    return accumulator<sha256>::hash(data);
}

// moved to interpreter stack xptr.
data_chunk sha256_chunk(const data_slice& data) NOEXCEPT
{
    data_chunk chunk(hash_size, no_fill_byte_allocator);
    return accumulator<sha256>::hash(chunk, data);
}

long_hash sha512_hash(const data_slice& data) NOEXCEPT
{
    return accumulator<sha512>::hash(data);
}

data_chunk sha512_chunk(const data_slice& data) NOEXCEPT
{
    data_chunk chunk(long_hash_size, no_fill_byte_allocator);
    return accumulator<sha512>::hash(chunk, data);
}

// Specialized Litecoin cryptographic hash functions.
// ----------------------------------------------------------------------------

hash_digest scrypt_hash(const data_slice& data) NOEXCEPT
{
    // Litecoin parameters, with concurrency enabled.
    return scrypt<1024, 1, 1, true>::hash<hash_size>(data, data);
}

} // namespace system
} // namespace libbitcoin

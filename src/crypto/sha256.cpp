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
#include <bitcoin/system/crypto/sha256.hpp>

#include <algorithm>
#include <array>
#include <iterator>
#include <utility>
#include <bitcoin/system/crypto/sha256_context.hpp>
#include <bitcoin/system/crypto/hash.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace sha256 { 

// Forward declarations for each transform implementation.
void hash_shani(state& state, const block1& block) NOEXCEPT;
void hash_sse4(state& state, const block1& block) NOEXCEPT;
void hash_neon(state& state, const block1& block) NOEXCEPT;
void hash_native(state& state, const block1& block) NOEXCEPT;

// Forward declarations for each double sha56 implementation.
void merkle_shani(digest2& out, const block2& block) NOEXCEPT;
void merkle_shani(digest1& out, const block1& block) NOEXCEPT;
void merkle_avx2(digest8& out, const block8& block) NOEXCEPT;
void merkle_sse41(digest4& out, const block4& block) NOEXCEPT;
void merkle_sse4(digest1& out, const block1& block) NOEXCEPT;
void merkle_neon(digest1& out, const block1& block) NOEXCEPT;
void merkle_hash(digest1& out, const block1& block) NOEXCEPT;

// merkle (finalized merkle)
// ----------------------------------------------------------------------------

void merkle_hash(uint8_t* out, size_t blocks, const uint8_t* in) NOEXCEPT
{
    // sha256 acceleration (optimum).
    if (have_shani())
    {
        while (blocks >= 2u)
        {
            merkle_shani(
                unsafe_array_cast<digest, 2>(out),
                unsafe_array_cast<block, 2>(in));
            std::advance(out, digest_size * 2);
            std::advance(in, block_size * 2);
            blocks -= 2;
        }

        while (to_bool(blocks--))
        {
            merkle_shani(
                unsafe_array_cast<digest>(out),
                unsafe_array_cast<block>(in));
            std::advance(out, digest_size);
            std::advance(in, block_size);
        }

        return;
    }

    if (have_avx2())
    {
        while (blocks >= 8u)
        {
            merkle_avx2(
                unsafe_array_cast<digest, 8>(out),
                unsafe_array_cast<block, 8>(in));
            std::advance(out, digest_size * 8);
            std::advance(in, block_size * 8);
            blocks -= 8;
        }
    }

    if (have_sse41())
    {
        while (blocks >= 4u)
        {
            merkle_sse41(
                unsafe_array_cast<digest, 4>(out),
                unsafe_array_cast<block, 4>(in));
            std::advance(out, digest_size * 4);
            std::advance(in, block_size * 4);
            blocks -= 4;
        }
    }

    if (have_sse4())
    {
        while (to_bool(blocks--))
        {
            merkle_sse4(unsafe_array_cast<digest>(out),
                unsafe_array_cast<block>(in));
            std::advance(out, digest_size);
            std::advance(in, block_size);
        }

        return;
    }

    if (have_neon())
    {
        while (to_bool(blocks--))
        {
            merkle_neon(
                unsafe_array_cast<digest>(out),
                unsafe_array_cast<block>(in));
            std::advance(out, digest_size);
            std::advance(in, block_size);
        }

        return;
    }

    while (to_bool(blocks--))
    {
        merkle_hash(
            unsafe_array_cast<digest>(out),
            unsafe_array_cast<block>(in));
        std::advance(out, digest_size);
        std::advance(in, block_size);
    }
}

digest merkle_root(digests&& hashes) NOEXCEPT
{
    if (hashes.empty())
        return {};

    if (is_one(hashes.size()))
        return std::move(hashes.front());

    if (is_odd(hashes.size()))
        hashes.push_back(hashes.back());

    while (is_even(hashes.size()))
    {
        // N blocks in, N/2 hashes out.
        const auto half = to_half(hashes.size());

        merkle_hash(
            pointer_cast<uint8_t>(hashes.data()), half,
            pointer_cast<const uint8_t>(hashes.data()));
        hashes.resize(half);
    }

    return std::move(hashes.front());
}

// hash (unfinalized blocks)
// ----------------------------------------------------------------------------

// Transformation is unfinalized (counter not incorporated, result in state).
void update(state& state, size_t blocks, const uint8_t* in) NOEXCEPT
{
    // sha256 acceleration (optimum).
    if (have_shani())
    {
        while (to_bool(blocks--))
        {
            // TODO: use hash_shani(state, blocks{ in }) for all.
            hash_shani(state, unsafe_array_cast<block>(in));
            std::advance(in, block_size);
        }

        return;
    }

    if (have_sse4())
    {
        while (to_bool(blocks--))
        {
            // TODO: use hash_sse4(state, blocks{ in }) for all.
            hash_sse4(state, unsafe_array_cast<block>(in));
            std::advance(in, block_size);
        }

        return;
    }

    if (have_neon())
    {
        while (to_bool(blocks--))
        {
            // TODO: use hash_neon(state, blocks{ in }) for all.
            hash_neon(state, unsafe_array_cast<block>(in));
            std::advance(in, block_size);
        }

        return;
    }

    while (to_bool(blocks--))
    {
        // TODO: use single_hash(state, blocks{ in }) for all [generalize first].
        hash_native(state, unsafe_array_cast<block>(in));
        std::advance(in, block_size);
    }
}

// TODO: implemented for all in native (for NORMAL_FORM localization).
void hash_finalize(digest& out, const state& state) NOEXCEPT;
void finalize(const state& state, uint8_t* out32) NOEXCEPT
{
    // TODO: This is called for all, but currently hash endianness is only
    // TODO: unreversed for native. So cannot be use for shani/sse4/neon yet.
    hash_finalize(unsafe_array_cast<uint8_t, digest_size>(out32), state);
}

// False only on hash function overflow: (2^64-8)/8 bytes written.
bool hash(uint8_t* out32, size_t size, const uint8_t* in) NOEXCEPT
{
    sha256::context context;
    if (!context.write(size, in))
        return false;

    context.flush(out32);
    return true;
}

} // namespace sha256
} // namespace system
} // namespace libbitcoin

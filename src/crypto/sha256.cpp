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
#include <bitcoin/system/crypto/sha256_context.hpp>
#include <bitcoin/system/crypto/hash.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

// TODO: The fill transform could be consolidated with full blocks.
// TODO: This would allow it to be parallelized to the maximum extent.
// TODO: But because bytes have been copied to buffer they are not
// TODO: contiguous with input. This also implies that a streaming
// TODO: hash will be denied the optimization of multi-lane hashing.
// TODO: This can be mitigated in the stream writer using an accumulator
// TODO: of 1/2/4/8 * 64 bytes. It would be suboptimal in other contexts.
// TODO: The writer can be initialized with a buffer multiple, which can be
// TODO: informed by the available lane optimizations.

// TODO: Change transform to pass blocks and create blocks overloads.
// TODO: This implies a template implementation of each for up to lane blocks.
// TODO: Transform should otherwise iterate over all of the blocks, handling
// TODO: 1, 2, 4, or 8 at a time as applicable. This is the same approach as
// TODO: used in the double hash dispatch.

namespace libbitcoin {
namespace system {
namespace sha256 {

// Forward declarations for each double sha56 implementation.
////void double_avx2(hash4& out, const block4& block) NOEXCEPT;
////void double_avx2(hash2& out, const block2& block) NOEXCEPT;
////void double_avx2(hash1& out, const block1& block) NOEXCEPT;
////void double_sse41(hash2& out, const block2& block) NOEXCEPT;
////void double_sse41(hash1& out, const block1& block) NOEXCEPT;
////void double_neon(hash4& out, const block4& block) NOEXCEPT;
////void double_neon(hash2& out, const block2& block) NOEXCEPT;
////void double_sse4(hash4& out, const block4& block) NOEXCEPT;
////void double_sse4(hash2& out, const block2& block) NOEXCEPT;
void double_avx2(hash8& out, const block8& block) NOEXCEPT;
void double_sse41(hash4& out, const block4& block) NOEXCEPT;
void double_shani(hash2& out, const block2& block) NOEXCEPT;
void double_shani(hash1& out, const block1& block) NOEXCEPT;
void double_neon(hash1& out, const block1& block) NOEXCEPT;
void double_sse4(hash1& out, const block1& block) NOEXCEPT;
void double_hash(hash1& out, const block1& block) NOEXCEPT;

// Specialized for independently hashing each block and rehashing each result.
// Finalization is performed within each implementation.
void sha256_double(uint8_t* out, size_t blocks, const uint8_t* in) NOEXCEPT
{
#if defined(HAVE_PORTABLE)

    if (have_avx2())
    {
        while (blocks >= 8u)
        {
            double_avx2(
                unsafe_array_cast<hash, 8>(out),
                unsafe_array_cast<block, 8>(in));
            std::advance(out, hash_size * 8);
            std::advance(in, block_size * 8);
            blocks -= 8;
        }
    }

    if (have_sse41())
    {
        while (blocks >= 4u)
        {
            // BUGBUG: throws on 32 bit builds.
            double_sse41(
                unsafe_array_cast<hash, 4>(out),
                unsafe_array_cast<block, 4>(in));
            std::advance(out, hash_size * 4);
            std::advance(in, block_size * 4);
            blocks -= 4;
        }
    }

    if (have_shani())
    {
        while (blocks >= 2u)
        {
            double_shani(
                unsafe_array_cast<hash, 2>(out),
                unsafe_array_cast<block, 2>(in));
            std::advance(out, hash_size * 2);
            std::advance(in, block_size * 2);
            blocks -= 2;
        }

        while (!is_zero(blocks--))
        {
            double_shani(
                unsafe_array_cast<hash>(out),
                unsafe_array_cast<block>(in));
            std::advance(out, hash_size);
            std::advance(in, block_size);
        }

        return;
    }

    if (have_neon())
    {
        while (!is_zero(blocks--))
        {
            double_neon(
                unsafe_array_cast<hash>(out),
                unsafe_array_cast<block>(in));
            std::advance(out, hash_size);
            std::advance(in, block_size);
        }

        return;
    }

    if (have_sse4())
    {
        while (!is_zero(blocks--))
        {
            double_sse4(unsafe_array_cast<hash>(out),
                unsafe_array_cast<block>(in));
            std::advance(out, hash_size);
            std::advance(in, block_size);
        }

        return;
    }

#endif

    while (!is_zero(blocks--))
    {
        double_hash(
            unsafe_array_cast<hash>(out),
            unsafe_array_cast<block>(in));
        std::advance(out, hash_size);
        std::advance(in, block_size);
    }
}

// Forward declarations for each transform implementation.
////void single_avx2(state& state, const block8& block) NOEXCEPT;
////void single_avx2(state& state, const block4& block) NOEXCEPT;
////void single_avx2(state& state, const block2& block) NOEXCEPT;
////void single_avx2(state& state, const block1& block) NOEXCEPT;
////void single_sse41(state& state, const block4& block) NOEXCEPT;
////void single_sse41(state& state, const block2& block) NOEXCEPT;
////void single_sse41(state& state, const block1& block) NOEXCEPT;
////void single_sse4(state& state, const block4& block) NOEXCEPT;
////void single_sse4(state& state, const block2& block) NOEXCEPT;
////void single_neon(state& state, const block4& block) NOEXCEPT;
////void single_neon(state& state, const block2& block) NOEXCEPT;
////void single_shani(state& state, const block2& block) NOEXCEPT;
void single_neon(state& state, const block1& block) NOEXCEPT;
void single_shani(state& state, const block1& block) NOEXCEPT;
void single_sse4(state& state, const block1& block) NOEXCEPT;
void single_hash(state& state, const block1& block) NOEXCEPT;

void transform(state& state, size_t blocks, const uint8_t* in) NOEXCEPT
{
#if defined(HAVE_PORTABLE)

    if (have_neon())
    {
        while (!is_zero(blocks--))
        {
            single_neon(state, unsafe_array_cast<block>(in));
            std::advance(in, block_size);
        }

        return;
    }

    if (have_shani())
    {
        while (!is_zero(blocks--))
        {
            single_shani(state, unsafe_array_cast<block>(in));
            std::advance(in, block_size);
        }

        return;
    }

    if (have_sse4())
    {
        while (!is_zero(blocks--))
        {
            single_sse4(state, unsafe_array_cast<block>(in));
            std::advance(in, block_size);
        }

        return;
    }

#endif

    while (!is_zero(blocks--))
    {
        single_hash(state, unsafe_array_cast<block>(in));
        std::advance(in, block_size);
    }
}

void update(context& context, size_t size, const uint8_t* in) NOEXCEPT
{
    // Avoid retransforming the buffer in the case where gap is also zero.
    if (is_zero(size))
        return;

    // Count of bytes unfilled in the block buffer before set_data.
    const auto gap = context.gap_size();

    // Update counter and fill gap if possible.
    context.set_data(size, in);

    // Block buffer is still unfilled, so no transformation on this update.
    if (size < gap)
        return;

    // Cannot add this to iteration because non-contiguous.
    transform(context.state, one, context.buffer.data());
    std::advance(in, gap);
    size -= gap;

    // Iterate over remaining whole blocks.
    transform(context.state, size / block_size, in);

    // Add the remaining partial block to the empty block buffer.
    context.add_data(size % block_size, in);
}

void finalize(context& context, uint8_t* out) NOEXCEPT
{
    update(context, context.pad_size(), pad.data());
    update(context, context::counter_size, context.serialize_counter().data());
    context.serialize_state(unsafe_array_cast<uint8_t, hash_size>(out));
}

void sha256_single(uint8_t* out, size_t size, const uint8_t* in) NOEXCEPT
{
    sha256::context context{};
    update(context, size, in);
    finalize(context, out);
}

} // namespace sha256
} // namespace system
} // namespace libbitcoin

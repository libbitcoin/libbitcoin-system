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

namespace libbitcoin {
namespace system {
namespace sha256 { 

// Forward declarations for each transform implementation.
void single_shani(state& state, const block1& block) NOEXCEPT;
void single_sse4(state& state, const block1& block) NOEXCEPT;
void single_neon(state& state, const block1& block) NOEXCEPT;
void single_hash(state& state, const block1& block) NOEXCEPT;

// Forward declarations for each double sha56 implementation.
void double_shani(digest2& out, const block2& block) NOEXCEPT;
void double_shani(digest1& out, const block1& block) NOEXCEPT;
void double_avx2(digest8& out, const block8& block) NOEXCEPT;
void double_sse41(digest4& out, const block4& block) NOEXCEPT;
void double_sse4(digest1& out, const block1& block) NOEXCEPT;
void double_neon(digest1& out, const block1& block) NOEXCEPT;
void double_hash(digest1& out, const block1& block) NOEXCEPT;

// merkle_root (transform exposed for test).
// ----------------------------------------------------------------------------

// Transformation is finalized (counter is incorporated).
void transform(uint8_t* out, size_t blocks, const uint8_t* in) NOEXCEPT
{
    // sha256 acceleration (optimum).
    if (have_shani())
    {
        while (blocks >= 2u)
        {
            double_shani(
                unsafe_array_cast<digest, 2>(out),
                unsafe_array_cast<block, 2>(in));
            std::advance(out, digest_size * 2);
            std::advance(in, block_size * 2);
            blocks -= 2;
        }

        while (to_bool(blocks--))
        {
            double_shani(
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
            double_avx2(
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
            double_sse41(
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
            double_sse4(unsafe_array_cast<digest>(out),
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
            double_neon(
                unsafe_array_cast<digest>(out),
                unsafe_array_cast<block>(in));
            std::advance(out, digest_size);
            std::advance(in, block_size);
        }

        return;
    }

    while (to_bool(blocks--))
    {
        double_hash(
            unsafe_array_cast<digest>(out),
            unsafe_array_cast<block>(in));
        std::advance(out, digest_size);
        std::advance(in, block_size);
    }
}

void merkle_root(digests& hashes) NOEXCEPT
{
    if (hashes.empty())
    {
        hashes.push_back({});
        return;
    }

    if (is_one(hashes.size()))
        return;

    if (is_odd(hashes.size()))
        hashes.push_back(hashes.back());

    while (is_even(hashes.size()))
    {
        const auto half = to_half(hashes.size());
        transform(
            pointer_cast<uint8_t>(hashes.data()), half,
            pointer_cast<const uint8_t>(hashes.data()));
        hashes.resize(half);
    }
}

// sha256_context.hpp declares hash/update/finalize (transform for test).
// ----------------------------------------------------------------------------

// TODO: move all iteration into implementations.
// Transformation is unfinalized (counter not incorporated, result in state).
void transform(state& state, size_t blocks, const uint8_t* in) NOEXCEPT
{
    // sha256 acceleration (optimum).
    if (have_shani())
    {
        while (to_bool(blocks--))
        {
            // TODO: use single_shani(state, blocks{ in }) for all.
            single_shani(state, unsafe_array_cast<block>(in));
            std::advance(in, block_size);
        }

        return;
    }

    if (have_sse4())
    {
        while (to_bool(blocks--))
        {
            // TODO: use single_sse4(state, blocks{ in }) for all.
            single_sse4(state, unsafe_array_cast<block>(in));
            std::advance(in, block_size);
        }

        return;
    }

    if (have_neon())
    {
        while (to_bool(blocks--))
        {
            // TODO: use single_neon(state, blocks{ in }) for all.
            single_neon(state, unsafe_array_cast<block>(in));
            std::advance(in, block_size);
        }

        return;
    }

    while (to_bool(blocks--))
    {
        // TODO: use single_hash(state, blocks{ in }) for all [generalize first].
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

void finalize(context& context, uint8_t* out32) NOEXCEPT
{
    // Save the counter serialization before calling update(pad).
    const auto counter = context.serialize_counter();

    update(context, context.pad_size(), pad_any.data());
    update(context, context::counter_size, counter.data());
    context.serialize_state(unsafe_array_cast<uint8_t, digest_size>(out32));
}

void hash(uint8_t* out32, size_t size, const uint8_t* in) NOEXCEPT
{
    sha256::context context{};
    update(context, size, in);
    finalize(context, out32);
}

#ifdef DISABLED

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

////void double_avx2(digest4& out, const block4& block) NOEXCEPT;
////void double_avx2(digest2& out, const block2& block) NOEXCEPT;
////void double_avx2(digest1& out, const block1& block) NOEXCEPT;
////void double_sse41(digest2& out, const block2& block) NOEXCEPT;
////void double_sse41(digest1& out, const block1& block) NOEXCEPT;
////void double_neon(digest4& out, const block4& block) NOEXCEPT;
////void double_neon(digest2& out, const block2& block) NOEXCEPT;
////void double_sse4(digest4& out, const block4& block) NOEXCEPT;
////void double_sse4(digest2& out, const block2& block) NOEXCEPT;

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

// This is the implementation above, but cannot be generalized in the same
// manner because avx2 and sse41 incorporate padding into the hash function.
// As such portable, shani, neon, and sse4 doubling externally incorporate
// padding as wrapper functions, which generalizes the implementation.
////void sha256_double(uint8_t* out, size_t blocks, const uint8_t* in) NOEXCEPT;

// TODO: non-streaming hashes:
// TODO: Make generalized template for hash of any size < block_size - 8.
// TODO: Generate pad from size, where size +1 is sentinel and pad-8 is count.
// TODO: Copy data into pad, transform, and emit big endian state as bytes.
// TODO: Generalize to any size by transforming size/block_size blocks and then
// TODO: applying above to the remaining bytes. size % block_size = 0 implies a
// TODO: full block of pad/count. Can also apply double variant of this model.

// 64 byte hashes
// ----------------------------------------------------------------------------

// This is equivalent to sha256_single(*, 1, *) and more efficient.
// It avoids allocation of context buffer and computation/serialize of counter.
void sha256_single(digest& out, const block& in) NOEXCEPT
{
    // Transform into state.
    auto state = sha256::initial;
    transform(state, one, in.data());

    // Transform pad/count and emit state to out.
    transform(state, one, sha256::pad_64.data());
    to_big_endians(array_cast<uint32_t>(out), state);
}

// This is equivalent to sha256_double(*, 1, *) and trivially more efficient.
void sha256_double(digest& out, const block& in) NOEXCEPT
{
    // Emit sha256_single(block) into pad/count buffer.
    auto buffer = sha256::pad_32;
    sha256_single(narrowing_array_cast<uint8_t, digest_size>(buffer), in);

    // Transform result and emit state to out.
    auto state = sha256::initial;
    transform(state, one, buffer.data());
    to_big_endians(array_cast<uint32_t>(out), state);
}

// 32 byte hashes
// ----------------------------------------------------------------------------

// This is equivalent to sha256_single(*, 1, *) and more efficient.
// It avoids allocation of context buffer and computation/serialize of counter.
void sha256_single(digest& out, const digest& in) NOEXCEPT
{
    // Copy hash into pad/count buffer for transform.
    auto buffer = sha256::pad_32;
    std::copy(in.begin(), in.end(), buffer.begin());

    // Transform and emit state to out.
    auto state = sha256::initial;
    transform(state, one, buffer.data());
    to_big_endians(array_cast<uint32_t>(out), state);
}

// This is not supported by sha256_double(*, n, *) as it is blocks only.
void sha256_double(digest& out, const digest& in) NOEXCEPT
{
    // Emit sha256_single(hash) into pad/count buffer.
    auto buffer = sha256::pad_32;
    sha256_single(narrowing_array_cast<uint8_t, digest_size>(buffer), in);

    // Transform result and emit state to out.
    auto state = sha256::initial;
    transform(state, one, buffer.data());
    to_big_endians(array_cast<uint32_t>(out), state);
}

#endif // VISUAL

} // namespace sha256
} // namespace system
} // namespace libbitcoin

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
#include "../../test.hpp"

BOOST_AUTO_TEST_SUITE(merkle_tests)

// hash (see also hash.cpp)

////BOOST_AUTO_TEST_CASE(merkle__hash__fractional_block__expected)
////{
////    constexpr auto expected = base16_array("3a6eb0790f39ac87c94f3856b2dd2c5d110e6811602261a9a923d3bb23adc8b7");
////    hash_digest hash{};
////    const data_chunk chunk{ 'd', 'a', 't', 'a' };
////    sha256::hash(hash.data(), chunk.size(), chunk.data());
////    BOOST_REQUIRE_EQUAL(hash, expected);
////}
////
////// merkle_root
////
////BOOST_AUTO_TEST_CASE(merkle__merkle_root__empty__null_hash)
////{
////    BOOST_REQUIRE_EQUAL(sha256::merkle_root(sha256::digests{}), null_hash);
////}
////
////BOOST_AUTO_TEST_CASE(merkle__merkle_root__one_hash__one_hash)
////{
////    BOOST_REQUIRE_EQUAL(sha256::merkle_root(sha256::digests{ one_hash }), one_hash);
////}
////
////BC_PUSH_WARNING(NO_USE_OF_MOVED_OBJECT)
////
////// shani1, neon, sse4, native  (1 block iteration)
////hash_digest merkle_root_expectation(const hashes& hashes)
////{
////    if (hashes.empty())
////        return {};
////
////    system::hashes update;
////    system::hashes merkle{ hashes };
////
////    while (merkle.size() > one)
////    {
////        if (is_odd(merkle.size()))
////            merkle.push_back(merkle.back());
////
////        for (auto it = merkle.begin(); it != merkle.end(); it += two)
////            update.push_back(bitcoin_hash(it[0], it[1]));
////
////        std::swap(merkle, update);
////        update.clear();
////    }
////
////    return merkle.front();
////}
////
////BOOST_AUTO_TEST_CASE(hash__to_merkle_root__one_block__expected)
////{
////    hashes hashes
////    {
////        { 0 },
////        { 1 }
////    };
////
////    const auto root = merkle_root_expectation(hashes);
////
////    // shani1, neon, sse4, native  (1 block)
////    BOOST_REQUIRE_EQUAL(sha256::merkle_root(std::move(hashes)), root);
////}
////
////BOOST_AUTO_TEST_CASE(merkle__to_merkle_root__two_blocks__expected)
////{
////    hashes hashes
////    {
////        { 0 },
////        { 1 },
////        { 2 },
////        { 3 }
////    };
////
////    const auto root = merkle_root_expectation(hashes);
////
////    // shani2 (2 blocks)
////    // shani1, neon, sse4, native  (1 block)
////    BOOST_REQUIRE_EQUAL(sha256::merkle_root(std::move(hashes)), root);
////}
////
////BOOST_AUTO_TEST_CASE(merkle__to_merkle_root__four_blocks__expected)
////{
////    hashes hashes
////    {
////        { 0 },
////        { 1 },
////        { 2 },
////        { 3 },
////        { 4 },
////        { 5 },
////        { 6 },
////        { 7 }
////    };
////
////    const auto root = merkle_root_expectation(hashes);
////
////    // sse41 (4 blocks)
////    // shani2 (2 blocks)
////    // shani1, neon, sse4, native  (1 block)
////    BOOST_REQUIRE_EQUAL(sha256::merkle_root(std::move(hashes)), root);
////}
////
////BOOST_AUTO_TEST_CASE(merkle__to_merkle_root__eight_blocks__expected)
////{
////    hashes hashes
////    {
////        { 0 },
////        { 1 },
////        { 2 },
////        { 3 },
////        { 4 },
////        { 5 },
////        { 6 },
////        { 7 },
////        { 8 },
////        { 9 },
////        { 10 },
////        { 11 },
////        { 12 },
////        { 13 },
////        { 14 },
////        { 15 }
////    };
////
////    const auto root = merkle_root_expectation(hashes);
////
////    // avx2 (8 blocks)
////    // sse41 (4 blocks)
////    // shani2 (2 blocks)
////    // shani1, neon, sse4, native  (1 block)
////    BOOST_REQUIRE_EQUAL(sha256::merkle_root(std::move(hashes)), root);
////}
////
////BC_POP_WARNING()

// merkle_hash (now private to merkle_root implementation)

////// Compares sha256::transform to single via sha256_hash (one block).
////BOOST_AUTO_TEST_CASE(merkle__merkle_hash__vs_bitcoin_hash_one_block__same)
////{
////    const hashes hashes
////    {
////        { 0 },
////        hash_digest{ 1 }
////    };
////
////    hash_digest once{};
////    hashes doubled{ hashes };
////
////    // Hashed single 2 x 0/1 hash.
////    sha256::context context;
////    context.write(hashes.size() * hash_size, hashes[0].data());
////    context.flush(once.data());
////    const auto two = sha256_hash(once);
////
////    // Hashed single 0/1 hash x 2.
////    context.write(hash_size, hashes[0].data());
////    context.write(hash_size, hashes[1].data());
////    context.flush(once.data());
////    const auto twice = sha256_hash(once);
////    BOOST_REQUIRE_EQUAL(twice, two);
////
////    // Doubled hash.
////    const auto blocks = to_half(hashes.size());
////    auto buffer = doubled.front().data();
////    sha256::merkle_hash(buffer, blocks, buffer);
////    doubled.resize(one);
////    BOOST_REQUIRE_EQUAL(doubled.front(), twice);
////}

////// Compares sha256::transform to single via sha256_hash (two blocks).
////BOOST_AUTO_TEST_CASE(merkle__merkle_hash__vs_bitcoin_hash_two_blocks__same)
////{
////    const hashes hashes
////    {
////        { 0 },
////        { 1 },
////        { 2 },
////        { 3 }
////    };
////
////    hash_digest once{};
////    hashes doubled{ hashes };
////
////    // Hashed single 0/1 hash x 2.
////    sha256::context context;
////    context.write(hash_size, hashes[0].data());
////    context.write(hash_size, hashes[1].data());
////    context.flush(once.data());
////    const auto first = sha256_hash(once);
////
////    // Hashed single 2/3 hash x 2.
////    context.write(hash_size, hashes[2].data());
////    context.write(hash_size, hashes[3].data());
////    context.flush(once.data());
////    const auto second = sha256_hash(once);
////
////    // Doubled hash.
////    const auto blocks = to_half(hashes.size());
////    auto buffer = doubled.front().data();
////    sha256::merkle_hash(buffer, blocks, buffer);
////    doubled.resize(two);
////
////    BOOST_REQUIRE_EQUAL(doubled[0], first);
////    BOOST_REQUIRE_EQUAL(doubled[1], second);
////}

////// satoshi test case
////// Compares sha256::transform to single via sha256_hash (32 blocks).
////BOOST_AUTO_TEST_CASE(merkle__merkle_hash__vs_sha256x2_writer__same)
////{
////    BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
////
////    using namespace sha256;
////    for (size_t i = 0; i <= digest_size; ++i)
////    {
////        uint8_t in  [block_size  * digest_size]{};
////        uint8_t out1[digest_size * digest_size]{};
////        uint8_t out2[digest_size * digest_size]{};
////
////        // arbitrary fill
////        for (size_t j = 0; j < block_size * i; ++j)
////            in[j] = 42;
////
////        // streaming double hash
////        for (size_t j = 0; j < i; ++j)
////        {
////            hash::sha256x2::copy sink(
////            {
////                &out1[digest_size * j],
////                &out1[digest_size * add1(j)]
////            });
////            sink.write_bytes(
////            {
////                &in[block_size * j],
////                &in[block_size * add1(j)]
////            });
////            sink.flush();
////        }
////
////        // in place double hash
////        merkle_hash(&out2[0], i, &in[0]);
////        BOOST_REQUIRE_EQUAL(std::memcmp(&out1[0], &out2[0], i * digest_size), 0);
////    }
////
////    BC_POP_WARNING()
////}

#ifdef DISABLED

////void single_avx2(state& state, const block8& block) NOEXCEPT;
////void single_avx2(state& state, const block4& block) NOEXCEPT;
////void single_avx2(state& state, const block2& block) NOEXCEPT;
////void single_avx2(state& state, const block1& block) NOEXCEPT;
////void hash_sse41(state& state, const block4& block) NOEXCEPT;
////void hash_sse41(state& state, const block2& block) NOEXCEPT;
////void hash_sse41(state& state, const block1& block) NOEXCEPT;
////void hash_sse41a(state& state, const block4& block) NOEXCEPT;
////void hash_sse41a(state& state, const block2& block) NOEXCEPT;
////void hash_neon(state& state, const block4& block) NOEXCEPT;
////void hash_neon(state& state, const block2& block) NOEXCEPT;
////void hash_shani(state& state, const block2& block) NOEXCEPT;

////void merkle_avx2(digest4& out, const block4& block) NOEXCEPT;
////void merkle_avx2(digest2& out, const block2& block) NOEXCEPT;
////void merkle_avx2(digest1& out, const block1& block) NOEXCEPT;
////void merkle_sse41(digest2& out, const block2& block) NOEXCEPT;
////void merkle_sse41(digest1& out, const block1& block) NOEXCEPT;
////void merkle_neon(digest4& out, const block4& block) NOEXCEPT;
////void merkle_neon(digest2& out, const block2& block) NOEXCEPT;
////void merkle_sse41a(digest4& out, const block4& block) NOEXCEPT;
////void merkle_sse41a(digest2& out, const block2& block) NOEXCEPT;

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
    sha256_single(narrow_array_cast<uint8_t, digest_size>(buffer), in);

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
    sha256_single(narrow_array_cast<uint8_t, digest_size>(buffer), in);

    // Transform result and emit state to out.
    auto state = sha256::initial;
    transform(state, one, buffer.data());
    to_big_endians(array_cast<uint32_t>(out), state);
}

#endif // DISABLED

BOOST_AUTO_TEST_SUITE_END()

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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(sha256_tests)

// hash (see also hash.cpp)

BOOST_AUTO_TEST_CASE(sha256__hash__fractional_block__expected)
{
    constexpr auto expected = base16_array("3a6eb0790f39ac87c94f3856b2dd2c5d110e6811602261a9a923d3bb23adc8b7");
    hash_digest hash{};
    const data_chunk chunk{ 'd', 'a', 't', 'a' };
    sha256::hash(hash.data(), chunk.size(), chunk.data());
    BOOST_REQUIRE_EQUAL(hash, expected);
}

// merkle_root

BOOST_AUTO_TEST_CASE(sha256__merkle_root__empty__null_hash)
{
    BOOST_REQUIRE_EQUAL(sha256::merkle_root(sha256::digests{}), null_hash);
}

BOOST_AUTO_TEST_CASE(sha256__merkle_root__one_hash__one_hash)
{
    BOOST_REQUIRE_EQUAL(sha256::merkle_root(sha256::digests{ one_hash }), one_hash);
}

BC_PUSH_WARNING(NO_USE_OF_MOVED_OBJECT)

// shani1, neon, sse4, native  (1 block iteration)
hash_digest merkle_root_expectation(const hash_list& hashes)
{
    if (hashes.empty())
        return {};

    hash_list update;
    hash_list merkle{ hashes };

    while (merkle.size() > one)
    {
        if (is_odd(merkle.size()))
            merkle.push_back(merkle.back());

        for (auto it = merkle.begin(); it != merkle.end(); it += two)
            update.push_back(bitcoin_hash(it[0], it[1]));

        std::swap(merkle, update);
        update.clear();
    }

    return merkle.front();
}

BOOST_AUTO_TEST_CASE(hash__to_merkle_root__one_block__expected)
{
    hash_list hashes
    {
        { 0 },
        { 1 }
    };

    const auto root = merkle_root_expectation(hashes);

    // shani1, neon, sse4, native  (1 block)
    BOOST_REQUIRE_EQUAL(sha256::merkle_root(std::move(hashes)), root);
}

BOOST_AUTO_TEST_CASE(sha256__to_merkle_root__two_blocks__expected)
{
    hash_list hashes
    {
        { 0 },
        { 1 },
        { 2 },
        { 3 }
    };

    const auto root = merkle_root_expectation(hashes);

    // shani2 (2 blocks)
    // shani1, neon, sse4, native  (1 block)
    BOOST_REQUIRE_EQUAL(sha256::merkle_root(std::move(hashes)), root);
}

BOOST_AUTO_TEST_CASE(sha256__to_merkle_root__four_blocks__expected)
{
    hash_list hashes
    {
        { 0 },
        { 1 },
        { 2 },
        { 3 },
        { 4 },
        { 5 },
        { 6 },
        { 7 }
    };

    const auto root = merkle_root_expectation(hashes);

    // sse41 (4 blocks)
    // shani2 (2 blocks)
    // shani1, neon, sse4, native  (1 block)
    BOOST_REQUIRE_EQUAL(sha256::merkle_root(std::move(hashes)), root);
}

BOOST_AUTO_TEST_CASE(sha256__to_merkle_root__eight_blocks__expected)
{
    hash_list hashes
    {
        { 0 },
        { 1 },
        { 2 },
        { 3 },
        { 4 },
        { 5 },
        { 6 },
        { 7 },
        { 8 },
        { 9 },
        { 10 },
        { 11 },
        { 12 },
        { 13 },
        { 14 },
        { 15 }
    };

    const auto root = merkle_root_expectation(hashes);

    // avx2 (8 blocks)
    // sse41 (4 blocks)
    // shani2 (2 blocks)
    // shani1, neon, sse4, native  (1 block)
    BOOST_REQUIRE_EQUAL(sha256::merkle_root(std::move(hashes)), root);
}

BC_POP_WARNING()

// transform[2]

// Compares sha256::transform to single via sha256_hash (one block).
BOOST_AUTO_TEST_CASE(sha256__transform2__vs_bitcoin_hash_one_block__same)
{
    const hash_list hashes
    {
        { 0 },
        hash_digest{ 1 }
    };

    hash_digest once{};
    hash_list doubled{ hashes };

    // Hashed single 2 x 0/1 hash.
    sha256::context context{};
    sha256::update(context, hashes.size() * hash_size, hashes[0].data());
    sha256::finalize(context, once.data());
    const auto two = sha256_hash(once);

    // Hashed single 0/1 hash x 2.
    context.reset();
    sha256::update(context, hash_size, hashes[0].data());
    sha256::update(context, hash_size, hashes[1].data());
    sha256::finalize(context, once.data());
    const auto twice = sha256_hash(once);
    BOOST_REQUIRE_EQUAL(twice, two);

    // Doubled hash.
    const auto blocks = to_half(hashes.size());
    auto buffer = doubled.front().data();
    sha256::transform(buffer, blocks, buffer);
    doubled.resize(one);
    BOOST_REQUIRE_EQUAL(doubled.front(), twice);
}

// Compares sha256::transform to single via sha256_hash (two blocks).
BOOST_AUTO_TEST_CASE(sha256__transform2__vs_bitcoin_hash_two_blocks__same)
{
    const hash_list hashes
    {
        { 0 },
        { 1 },
        { 2 },
        { 3 }
    };

    hash_digest once{};
    hash_list doubled{ hashes };

    // Hashed single 0/1 hash x 2.
    sha256::context context{};
    sha256::update(context, hash_size, hashes[0].data());
    sha256::update(context, hash_size, hashes[1].data());
    sha256::finalize(context, once.data());
    const auto first = sha256_hash(once);

    // Hashed single 2/3 hash x 2.
    context.reset();
    sha256::update(context, hash_size, hashes[2].data());
    sha256::update(context, hash_size, hashes[3].data());
    sha256::finalize(context, once.data());
    const auto second = sha256_hash(once);

    // Doubled hash.
    const auto blocks = to_half(hashes.size());
    auto buffer = doubled.front().data();
    sha256::transform(buffer, blocks, buffer);
    doubled.resize(two);

    BOOST_REQUIRE_EQUAL(doubled[0], first);
    BOOST_REQUIRE_EQUAL(doubled[1], second);
}

// satoshi test case
// Compares sha256::transform to single via sha256_hash (32 blocks).
BOOST_AUTO_TEST_CASE(sha256__transform2__vs_sha256x2_writer__same)
{
    BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)

    using namespace sha256;
    for (size_t i = 0; i <= digest_size; ++i)
    {
        uint8_t in  [block_size  * digest_size]{};
        uint8_t out1[digest_size * digest_size]{};
        uint8_t out2[digest_size * digest_size]{};

        // arbitrary fill
        for (size_t j = 0; j < block_size * i; ++j)
            in[j] = 42;

        // streaming double hash
        for (size_t j = 0; j < i; ++j)
        {
            hash::sha256x2::copy sink(
            {
                &out1[digest_size * j],
                &out1[digest_size * add1(j)]
            });
            sink.write_bytes(
            {
                &in[block_size * j],
                &in[block_size * add1(j)]
            });
            sink.flush();
        }

        // in place double hash
        transform(&out2[0], i, &in[0]);
        BOOST_REQUIRE_EQUAL(std::memcmp(&out1[0], &out2[0], i * digest_size), 0);
    }

    BC_POP_WARNING()
}

BOOST_AUTO_TEST_SUITE_END()

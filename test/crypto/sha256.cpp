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

BOOST_AUTO_TEST_CASE(sha256__sha256_hash__less_than_one_block__expected)
{
    const data_chunk chunk{ 'd', 'a', 't', 'a' };
    const auto hash = sha256_hash(chunk);
    BOOST_REQUIRE_EQUAL(encode_base16(hash), "3a6eb0790f39ac87c94f3856b2dd2c5d110e6811602261a9a923d3bb23adc8b7");
}

BOOST_AUTO_TEST_CASE(sha256__sha256_double__vs_bitcoin_hash_one_block__same)
{
    const hash_list hashes
    {
        hash_digest{ 0 },
        hash_digest{ 1 }
    };

    hash_digest once{};
    hash_list doubled{ hashes };

    // Hashed single 2 x hash.
    sha256::context context{};
    sha256::update(context, hashes.size() * hash_size, hashes[0].data());
    sha256::finalize(context, once.data());
    const auto two = sha256_hash(once);

    // Hashed single hash x 2.
    context.reset();
    sha256::update(context, hash_size, hashes[0].data());
    sha256::update(context, hash_size, hashes[1].data());
    sha256::finalize(context, once.data());
    const auto twice = sha256_hash(once);
    BOOST_REQUIRE_EQUAL(twice, two);

    // Doubled hash.
    const auto blocks = to_half(hashes.size());
    auto buffer = doubled.front().data();
    sha256::sha256_double(buffer, blocks, buffer);
    doubled.resize(one);
    BOOST_REQUIRE_EQUAL(doubled.front(), twice);
}

BOOST_AUTO_TEST_CASE(sha256__sha256_double__vs_bitcoin_hash_two_blocks__same)
{
    const hash_list hashes
    {
        hash_digest{ 0 },
        hash_digest{ 1 },
        hash_digest{ 2 },
        hash_digest{ 3 }
    };

    hash_digest once{};
    hash_list doubled{ hashes };

    // Hashed single 4 x hash.
    sha256::context context{};
    sha256::update(context, hash_size, hashes[0].data());
    sha256::update(context, hash_size, hashes[1].data());
    sha256::finalize(context, once.data());
    const auto first = sha256_hash(once);

    // Hashed single hash x 4.
    context.reset();
    sha256::update(context, hash_size, hashes[2].data());
    sha256::update(context, hash_size, hashes[3].data());
    sha256::finalize(context, once.data());
    const auto second = sha256_hash(once);

    // Doubled hash.
    const auto blocks = to_half(hashes.size());
    auto buffer = doubled.front().data();
    sha256::sha256_double(buffer, blocks, buffer);
    doubled.resize(one);

    BOOST_REQUIRE_EQUAL(doubled[0], first);
    BOOST_REQUIRE_EQUAL(doubled[1], second);
}

// satoshi test case
BOOST_AUTO_TEST_CASE(sha256__sha256_double__vs_sha256x2_writer__same)
{
    BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)

    for (int i = 0; i <= 32; ++i)
    {
        uint8_t in[64 * 32]{};
        uint8_t out1[32 * 32]{};
        uint8_t out2[32 * 32]{};
        const auto to = [&](int j) { return out1 + 32 * j; };
        const auto from = [&](int j) { return in + 64 * j; };

        // arbitrary fill
        for (int j = 0; j < 64 * i; ++j)
            in[j] = 42;

        // streaming double hash
        for (int j = 0; j < i; ++j)
        {
            hash::sha256x2::copy sink({ to(j), to(j) + 32 });
            sink.write_bytes({ from(j), from(j) + 64 });
            sink.flush();
        }

        // in place double hash
        sha256::sha256_double(&out2[0], i, &in[0]);
        BOOST_REQUIRE_EQUAL(memcmp(&out1[0], &out2[0], 32 * i), 0);
    }

    BC_POP_WARNING()
}

BOOST_AUTO_TEST_SUITE_END()

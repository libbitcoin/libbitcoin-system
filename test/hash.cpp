/*
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "hash.hpp"

#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>
#include "genesis_block.hpp"

using namespace bc;

BOOST_AUTO_TEST_SUITE(hash_tests)

BOOST_AUTO_TEST_CASE(sha1_hash_test)
{
    for (const hash_result& result: sha1_tests)
    {
        data_chunk data;
        BOOST_REQUIRE(decode_base16(data, result.input));
        BOOST_REQUIRE_EQUAL(encode_hex(sha1_hash(data)), result.result);
    }
}

BOOST_AUTO_TEST_CASE(ripemd_hash_test)
{
    for (const hash_result& result: ripemd_tests)
    {
        data_chunk data;
        BOOST_REQUIRE(decode_base16(data, result.input));
        BOOST_REQUIRE_EQUAL(encode_base16(ripemd160_hash(data)),
            result.result);
    }
    auto ripemd_hash = bitcoin_short_hash(to_byte(110));
    BOOST_REQUIRE_EQUAL(encode_base16(ripemd_hash),
        "17d040b739d639c729daaf627eaff88cfe4207f4");
    auto foo = bitcoin_short_hash(base16_literal(
        "020641fde3a85beb8321033516de7ec01c35de96e945bf76c3768784a905471986"));
    BOOST_REQUIRE_EQUAL(encode_base16(foo),
        "c23e37c6fad06deab545f952992c8f28cb02bbe5");
}

BOOST_AUTO_TEST_CASE(sha256_hash_test)
{
    for (const hash_result& result: sha256_tests)
    {
        data_chunk data;
        BOOST_REQUIRE(decode_base16(data, result.input));
        BOOST_REQUIRE_EQUAL(encode_base16(sha256_hash(data)), result.result);
    }

    // This changes based on ENABLE_TESTNET, so the test condition must vary.
    auto genesis = genesis_block();
    auto genesis_hash = genesis.header.hash();

#ifdef ENABLE_TESTNET
    std::cout << "hash: " << encode_hash(genesis_hash) << std::endl;
    std::cout << "expd: 000000000933ea01ad0ee984209779baaec3ced90fa3f408719526f8d77f4943" << std::endl;
    BOOST_REQUIRE_EQUAL(encode_hash(genesis_hash),
        "000000000933ea01ad0ee984209779baaec3ced90fa3f408719526f8d77f4943");
#else
    std::cout << "hash: " << encode_hash(genesis_hash) << std::endl;
    std::cout << "expd: 000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f" << std::endl;
    BOOST_REQUIRE_EQUAL(encode_hash(genesis_hash),
        "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
#endif
}

BOOST_AUTO_TEST_CASE(hmac_sha256_hash_test)
{
    data_chunk chunk{'d', 'a', 't', 'a'};
    data_chunk key{'k', 'e', 'y'};

    auto hash = hmac_sha256_hash(chunk, key);
    BOOST_REQUIRE_EQUAL(encode_base16(hash),
        "5031fe3d989c6d1537a013fa6e739da23463fdaec3b70137d828e36ace221bd0");
}

BOOST_AUTO_TEST_CASE(hmac_sha512_hash_test)
{
    data_chunk chunk{'d', 'a', 't', 'a'};
    data_chunk key{'k', 'e', 'y'};

    auto long_hash = hmac_sha512_hash(chunk, key);
    BOOST_REQUIRE_EQUAL(encode_base16(long_hash),
        "3c5953a18f7303ec653ba170ae334fafa08e3846f2efe317b87efce82376253c"
        "b52a8c31ddcde5a3a2eee183c2b34cb91f85e64ddbc325f7692b199473579c58");
}

BOOST_AUTO_TEST_CASE(sha512_hash_test)
{
    data_chunk chunk{'d', 'a', 't', 'a'};

    auto long_hash = sha512_hash(chunk);
    BOOST_REQUIRE_EQUAL(encode_base16(long_hash),
        "77c7ce9a5d86bb386d443bb96390faa120633158699c8844c30b13ab0bf92760"
        "b7e4416aea397db91b4ac0e5dd56b8ef7e4b066162ab1fdc088319ce6defc876");
}

BOOST_AUTO_TEST_CASE(pkcs5_pbkdf2_hmac_sha512_test)
{
    for (const pkcs5_pbkdf2_hmac_sha512_result& result:
             pkcs5_pbkdf2_hmac_sha512_tests)
    {
        auto hash = pkcs5_pbkdf2_hmac_sha512(
            to_data_chunk(result.passphrase), to_data_chunk(result.salt),
            result.iterations);
        BOOST_REQUIRE_EQUAL(encode_base16(hash), result.result);
    }
}

BOOST_AUTO_TEST_SUITE_END()

/**
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
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

static const ec_secret secret
{{
    0x80, 0x10, 0xB1, 0xBB, 0x11, 0x9A, 0xD3, 0x7D,
    0x4B, 0x65, 0xA1, 0x02, 0x2A, 0x31, 0x48, 0x97,
    0xB1, 0xB3, 0x61, 0x4B, 0x34, 0x59, 0x74, 0x33,
    0x2C, 0xB1, 0xB9, 0x58, 0x2C, 0xF0, 0x35, 0x36
}};

BOOST_AUTO_TEST_SUITE(ec_keys_tests)

BOOST_AUTO_TEST_CASE(secret_to_public_key_test)
{
    ec_point compressed = secret_to_public_key(secret, true);
    BOOST_REQUIRE_EQUAL(encode_base16(compressed),
        "0309ba8621aefd3b6ba4ca6d11a4746e8df8d35d9b51b383338f627ba7fc732731");
    ec_point uncompressed = secret_to_public_key(secret, false);
    BOOST_REQUIRE_EQUAL(encode_base16(uncompressed),
        "0409ba8621aefd3b6ba4ca6d11a4746e8df8d35d9b51b383338f627ba7fc732731"
        "8c3a6ec6acd33c36328b8fb4349b31671bcd3a192316ea4f6236ee1ae4a7d8c9");
}

BOOST_AUTO_TEST_CASE(rfc6979_nonce_test)
{
    struct rfc6979_test
    {
        ec_secret secret;
        std::string message;
        std::string nonce;
    };

    // These test vectors are taken from python-ecdsa
    // https://github.com/warner/python-ecdsa
    const std::vector<rfc6979_test> rfc6979_tests
    {
        {
            base16_literal("000000000000000000000000000000009d0219792467d7d37b4d43298a7d0c05"),
            "sample",
            "8fa1f95d514760e498f28957b824ee6ec39ed64826ff4fecc2b5739ec45b91cd"
        },
        {
            base16_literal("cca9fbcc1b41e5a95d369eaa6ddcff73b61a4efaa279cfc6567e8daa39cbaf50"),
            "sample",
            "2df40ca70e639d89528a6b670d9d48d9165fdc0febc0974056bdce192b8e16a3"
        },
        {
            base16_literal("0000000000000000000000000000000000000000000000000000000000000001"),
            "Satoshi Nakamoto",
            "8f8a276c19f4149656b280621e358cce24f5f52542772691ee69063b74f15d15"
        },
        {
            base16_literal("0000000000000000000000000000000000000000000000000000000000000001"),
            "All those moments will be lost in time, like tears in rain. Time to die...",
            "38aa22d72376b4dbc472e06c3ba403ee0a394da63fc58d88686c611aba98d6b3"
        },
        {
            base16_literal("fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364140"),
            "Satoshi Nakamoto",
            "33a19b60e25fb6f4435af53a3d42d493644827367e6453928554f43e49aa6f90"
        },
        {
            base16_literal("f8b8af8ce3c7cca5e300d33939540c10d45ce001b8f252bfbc57ba0342904181"),
            "Alan Turing",
            "525a82b70e67874398067543fd84c83d30c175fdc45fdeee082fe13b1d7cfdf1"
        }
    };

    for (auto& test: rfc6979_tests)
    {
        hash_digest hash = sha256_hash(to_data_chunk(test.message));
        ec_secret nonce = create_nonce(test.secret, hash);
        BOOST_REQUIRE_EQUAL(encode_base16(nonce), test.nonce);
    }
}

BOOST_AUTO_TEST_CASE(rfc6979_nonce_index_test)
{
    struct rfc6979_index_test
    {
        ec_secret secret;
        std::string message;
        std::string nonce00;
        std::string nonce01;
        std::string nonce15;
    };

    // These test vectors were provided by user bip32JP on Github:
    // https://github.com/bitcoinjs/bitcoinjs-lib/pull/337#issuecomment-68620793
    const std::vector<rfc6979_index_test> rfc6979_index_tests
    {
        {
            base16_literal("fee0a1f7afebf9d2a5a80c0c98a31c709681cce195cbcd06342b517970c0be1e"),
            "test data",
            "fcce1de7a9bcd6b2d3defade6afa1913fb9229e3b7ddf4749b55c4848b2a196e",
            "727fbcb59eb48b1d7d46f95a04991fc512eb9dbf9105628e3aec87428df28fd8",
            "398f0e2c9f79728f7b3d84d447ac3a86d8b2083c8f234a0ffa9c4043d68bd258"
        },
        {
            base16_literal("0000000000000000000000000000000000000000000000000000000000000001"),
            "Everything should be made as simple as possible, but not simpler.",
            "ec633bd56a5774a0940cb97e27a9e4e51dc94af737596a0c5cbb3d30332d92a5",
            "df55b6d1b5c48184622b0ead41a0e02bfa5ac3ebdb4c34701454e80aabf36f56",
            "def007a9a3c2f7c769c75da9d47f2af84075af95cadd1407393dc1e26086ef87"
        },
        {
            base16_literal("0000000000000000000000000000000000000000000000000000000000000002"),
            "Satoshi Nakamoto",
            "d3edc1b8224e953f6ee05c8bbf7ae228f461030e47caf97cde91430b4607405e",
            "f86d8e43c09a6a83953f0ab6d0af59fb7446b4660119902e9967067596b58374",
            "241d1f57d6cfd2f73b1ada7907b199951f95ef5ad362b13aed84009656e0254a"
        },
        {
            base16_literal("7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f"),
            "Diffie Hellman",
            "c378a41cb17dce12340788dd3503635f54f894c306d52f6e9bc4b8f18d27afcc",
            "90756c96fef41152ac9abe08819c4e95f16da2af472880192c69a2b7bac29114",
            "7b3f53300ab0ccd0f698f4d67db87c44cf3e9e513d9df61137256652b2e94e7c"
        },
        {
            base16_literal("8080808080808080808080808080808080808080808080808080808080808080"),
            "Japan",
            "f471e61b51d2d8db78f3dae19d973616f57cdc54caaa81c269394b8c34edcf59",
            "6819d85b9730acc876fdf59e162bf309e9f63dd35550edf20869d23c2f3e6d17",
            "d8e8bae3ee330a198d1f5e00ad7c5f9ed7c24c357c0a004322abca5d9cd17847"
        },
        {
            base16_literal("fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364140"),
            "Bitcoin",
            "36c848ffb2cbecc5422c33a994955b807665317c1ce2a0f59c689321aaa631cc",
            "4ed8de1ec952a4f5b3bd79d1ff96446bcd45cabb00fc6ca127183e14671bcb85",
            "56b6f47babc1662c011d3b1f93aa51a6e9b5f6512e9f2e16821a238d450a31f8"
        },
        {
            base16_literal("fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364140"),
            "i2FLPP8WEus5WPjpoHwheXOMSobUJVaZM1JPMQZq",
            "6e9b434fcc6bbb081a0463c094356b47d62d7efae7da9c518ed7bac23f4e2ed6",
            "ae5323ae338d6117ce8520a43b92eacd2ea1312ae514d53d8e34010154c593bb",
            "3eaa1b61d1b8ab2f1ca71219c399f2b8b3defa624719f1e96fe3957628c2c4ea"
        },
        {
            base16_literal("3881e5286abc580bb6139fe8e83d7c8271c6fe5e5c2d640c1f0ed0e1ee37edc9"),
            "lEE55EJNP7aLrMtjkeJKKux4Yg0E8E1SAJnWTCEh",
            "5b606665a16da29cc1c5411d744ab554640479dd8abd3c04ff23bd6b302e7034",
            "f8b25263152c042807c992eacd2ac2cc5790d1e9957c394f77ea368e3d9923bd",
            "ea624578f7e7964ac1d84adb5b5087dd14f0ee78b49072aa19051cc15dab6f33"
        },
        {
            base16_literal("7259dff07922de7f9c4c5720d68c9745e230b32508c497dd24cb95ef18856631"),
            "2SaVPvhxkAPrayIVKcsoQO5DKA8Uv5X/esZFlf+y",
            "3ab6c19ab5d3aea6aa0c6da37516b1d6e28e3985019b3adb388714e8f536686b",
            "19af21b05004b0ce9cdca82458a371a9d2cf0dc35a813108c557b551c08eb52e",
            "117a32665fca1b7137a91c4739ac5719fec0cf2e146f40f8e7c21b45a07ebc6a"
        },
        {
            base16_literal("0d6ea45d62b334777d6995052965c795a4f8506044b4fd7dc59c15656a28f7aa"),
            "00A0OwO2THi7j5Z/jp0FmN6nn7N/DQd6eBnCS+/b",
            "79487de0c8799158294d94c0eb92ee4b567e4dc7ca18addc86e49d31ce1d2db6",
            "9561d2401164a48a8f600882753b3105ebdd35e2358f4f808c4f549c91490009",
            "b0d273634129ff4dbdf0df317d4062a1dbc58818f88878ffdb4ec511c77976c0"
        }
    };

    for (auto& test: rfc6979_index_tests)
    {
        hash_digest hash = sha256_hash(to_data_chunk(test.message));
        ec_secret nonce00 = create_nonce(test.secret, hash);
        ec_secret nonce01 = create_nonce(test.secret, hash, 1);
        ec_secret nonce15 = create_nonce(test.secret, hash, 15);
        BOOST_REQUIRE_EQUAL(encode_base16(nonce00), test.nonce00);
        BOOST_REQUIRE_EQUAL(encode_base16(nonce01), test.nonce01);
        BOOST_REQUIRE_EQUAL(encode_base16(nonce15), test.nonce15);
    }
}

BOOST_AUTO_TEST_CASE(ec_signature_test)
{
    ec_point public_key = secret_to_public_key(secret, true);
    data_chunk data{'d', 'a', 't', 'a'};
    hash_digest hash = bitcoin_hash(data);

    // Correct signature:
    const auto distinguished = sign(secret, hash);

    ec_signature signature;
    BOOST_REQUIRE(parse_signature(signature, distinguished, false));
    BOOST_REQUIRE(verify_signature(public_key, hash, signature));

    // Invalidate hash data.
    hash[0] = 0;
    BOOST_REQUIRE(!verify_signature(public_key, hash, signature));

    // The nonce is now ignored and only deterministic signing is used.
    // Invalid nonce:
    ec_secret nonce = ec_secret{{0}};
    BOOST_REQUIRE_NE(sign(secret, hash, nonce).size(), 0u);
}

BOOST_AUTO_TEST_CASE(ec_verify_test)
{
    ec_point public_key = to_data_chunk(base16_literal(
        "03bc88a1bd6ebac38e9a9ed58eda735352ad10650e235499b7318315cc26c9b55b"));
    hash_digest signature_hash = hash_literal(
        "ed8f9b40c2d349c8a7e58cebe79faa25c21b6bb85b874901f72a1b3f1ad0a67f");
    data_chunk distinguished = to_data_chunk(base16_literal(
        "3045022100bc494fbd09a8e77d8266e2abdea9aef08b9e71b451c7d8de9f63cda33"
        "a62437802206b93edd6af7c659db42c579eb34a3a4cb60c28b5a6bc86fd5266d42f"
        "6b8bb67d"));

    ec_signature signature;
    BOOST_REQUIRE(parse_signature(signature, distinguished, false));
    BOOST_REQUIRE(verify_signature(public_key, signature_hash, signature));

    // Invalidate hash data.
    signature[10] = 110;
    BOOST_REQUIRE(!verify_signature(public_key, signature_hash, signature));
}

BOOST_AUTO_TEST_CASE(ec_add_test)
{
    ec_secret secret_a{{1, 2, 3}};
    ec_secret secret_b{{3, 2, 1}};
    ec_point public_a = secret_to_public_key(secret_a, true);

    BOOST_REQUIRE(ec_add(secret_a, secret_b));
    BOOST_REQUIRE_EQUAL(encode_base16(secret_a),
        "0404040000000000000000000000000000000000000000000000000000000000");

    BOOST_REQUIRE(ec_add(public_a, secret_b));
    ec_point public_sum = secret_to_public_key(secret_a, true);
    BOOST_REQUIRE(std::equal(public_a.begin(), public_a.end(),
        public_sum.begin()));
}

BOOST_AUTO_TEST_CASE(ec_add_bad_test)
{
    ec_secret secret_a // = n - 1
    {{
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
        0xBA, 0xAE, 0xDC, 0xE6, 0xAF, 0x48, 0xA0, 0x3B,
        0xBF, 0xD2, 0x5E, 0x8C, 0xD0, 0x36, 0x41, 0x40
    }};
    ec_secret secret_b{{0}};
    secret_b[31] = 1;
    ec_point public_a = secret_to_public_key(secret_a, true);

    BOOST_REQUIRE(!ec_add(secret_a, secret_b));
    BOOST_REQUIRE(!ec_add(public_a, secret_b));
}

BOOST_AUTO_TEST_CASE(ec_multiply_test)
{
    ec_secret secret_a{{0}};
    ec_secret secret_b{{0}};
    secret_a[31] = 11;
    secret_b[31] = 22;
    ec_point public_a = secret_to_public_key(secret_a, true);

    BOOST_REQUIRE(ec_multiply(secret_a, secret_b));
    BOOST_REQUIRE_EQUAL(secret_a[31], 242u);

    BOOST_REQUIRE(ec_multiply(public_a, secret_b));
    ec_point public_sum = secret_to_public_key(secret_a, true);
    BOOST_REQUIRE(std::equal(public_a.begin(), public_a.end(),
        public_sum.begin()));
}

BOOST_AUTO_TEST_SUITE_END()

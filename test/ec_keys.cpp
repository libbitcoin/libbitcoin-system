/*
 * Copyright (c) 2011-2014 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_CASE(secret_to_public_key_test)
{
    ec_point compressed = secret_to_public_key(secret, true);
    BOOST_REQUIRE(encode_hex(compressed) ==
        "0309ba8621aefd3b6ba4ca6d11a4746e8df8d35d9b51b383338f627ba7fc732731");
    ec_point uncompressed = secret_to_public_key(secret, false);
    BOOST_REQUIRE(encode_hex(uncompressed) ==
        "0409ba8621aefd3b6ba4ca6d11a4746e8df8d35d9b51b383338f627ba7fc732731"
        "8c3a6ec6acd33c36328b8fb4349b31671bcd3a192316ea4f6236ee1ae4a7d8c9");
}

BOOST_AUTO_TEST_CASE(ec_signature_test)
{
    ec_point public_key = secret_to_public_key(secret, true);
    data_chunk data{'d', 'a', 't', 'a'};
    hash_digest hash = bitcoin_hash(data);
    ec_secret nonce{{42}};

    // Correct signature:
    data_chunk signature = sign(secret, hash, nonce);
    BOOST_REQUIRE(verify_signature(public_key, hash, signature));

    // Incorrect data:
    hash[0] = 0;
    BOOST_REQUIRE(!verify_signature(public_key, hash, signature));

    // Invalid nonce:
    nonce = ec_secret{{0}};
    BOOST_REQUIRE(!sign(secret, hash, nonce).size());
}

BOOST_AUTO_TEST_CASE(ec_verify_test)
{
    ec_point pubkey = decode_hex(
        "03bc88a1bd6ebac38e9a9ed58eda735352ad10650e235499b7318315cc26c9b55b");
    hash_digest sighash = decode_hash(
        "ed8f9b40c2d349c8a7e58cebe79faa25c21b6bb85b874901f72a1b3f1ad0a67f");
    data_chunk signature = decode_hex(
        "3045022100bc494fbd09a8e77d8266e2abdea9aef08b9e71b451c7d8de9f63cda33"
        "a62437802206b93edd6af7c659db42c579eb34a3a4cb60c28b5a6bc86fd5266d42f"
        "6b8bb67d");
    BOOST_REQUIRE(verify_signature(pubkey, sighash, signature));
    signature[10] = 110;
    BOOST_REQUIRE(!verify_signature(pubkey, sighash, signature));
}

BOOST_AUTO_TEST_CASE(ec_add_test)
{
    ec_secret secret_a{{1, 2, 3}};
    ec_secret secret_b{{3, 2, 1}};
    ec_point public_a = secret_to_public_key(secret_a, true);

    BOOST_REQUIRE(ec_add(secret_a, secret_b));
    BOOST_REQUIRE(encode_hex(secret_a) ==
        "0404040000000000000000000000000000000000000000000000000000000000");

    BOOST_REQUIRE(ec_tweak_add(public_a, secret_b));
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
    BOOST_REQUIRE(!ec_tweak_add(public_a, secret_b));
}

BOOST_AUTO_TEST_CASE(ec_multiply_test)
{
    ec_secret secret_a{{0}};
    ec_secret secret_b{{0}};
    secret_a[31] = 11;
    secret_b[31] = 22;
    ec_point public_a = secret_to_public_key(secret_a, true);

    BOOST_REQUIRE(ec_multiply(secret_a, secret_b));
    BOOST_REQUIRE(secret_a[31] = 242);

    BOOST_REQUIRE(ec_multiply(public_a, secret_b));
    ec_point public_sum = secret_to_public_key(secret_a, true);
    BOOST_REQUIRE(std::equal(public_a.begin(), public_a.end(),
        public_sum.begin()));
}

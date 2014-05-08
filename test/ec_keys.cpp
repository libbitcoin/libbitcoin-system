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

BOOST_AUTO_TEST_CASE(ec_add_test)
{
    ec_secret secret_a{{1, 2, 3}};
    ec_secret secret_b{{3, 2, 1}};
    ec_point public_a = secret_to_public_key(secret_a, true);

    secret_a += secret_b;
    BOOST_REQUIRE(encode_hex(secret_a) ==
        "0404040000000000000000000000000000000000000000000000000000000000");

    public_a += secret_b;
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

    BOOST_REQUIRE(!(secret_a += secret_b));
    BOOST_REQUIRE(!(public_a += secret_b));
}

BOOST_AUTO_TEST_CASE(ec_mul_test)
{
    ec_secret secret_a{{0}};
    ec_secret secret_b{{0}};
    secret_a[31] = 11;
    secret_b[31] = 22;
    ec_point public_a = secret_to_public_key(secret_a, true);

    secret_a *= secret_b;
    BOOST_REQUIRE(secret_a[31] = 242);

    public_a *= secret_b;
    ec_point public_sum = secret_to_public_key(secret_a, true);
    BOOST_REQUIRE(std::equal(public_a.begin(), public_a.end(),
        public_sum.begin()));
}

BOOST_AUTO_TEST_CASE(ec_add_test2)
{
    ec_secret secret_a
    {{
         0xdc, 0xc1, 0x25, 0x0b, 0x51, 0xc0, 0xf0, 0x3a,
         0xe4, 0xe9, 0x78, 0xe0, 0x25, 0x6e, 0xde, 0x51,
         0xdc, 0x11, 0x44, 0xe3, 0x45, 0xc9, 0x26, 0x26,
         0x2b, 0x97, 0x17, 0xb1, 0xbc, 0xc9, 0xbd, 0x1b
    }};
    ec_secret secret_b
    {{
         0x4b, 0x49, 0x74, 0x26, 0x6e, 0xe6, 0xc8, 0xbe,
         0xd9, 0xef, 0xf2, 0xcd, 0x10, 0x87, 0xbb, 0xc1,
         0x10, 0x1f, 0x17, 0xba, 0xd9, 0xc3, 0x78, 0x14,
         0xf8, 0x56, 0x1b, 0x67, 0xf5, 0x50, 0xc5, 0x44
    }};
    ec_secret expected_result
    {{
         0x28, 0x0a, 0x99, 0x31, 0xc0, 0xa7, 0xb8, 0xf9,
         0xbe, 0xd9, 0x6b, 0xad, 0x35, 0xf6, 0x9a, 0x14,
         0x31, 0x81, 0x7f, 0xb7, 0x70, 0x43, 0xfd, 0xff,
         0x64, 0x1a, 0xd4, 0x8c, 0xe1, 0xe4, 0x41, 0x1e
    }};
    secret_a += secret_b;
    BOOST_REQUIRE(secret_a == expected_result);
    ec_point expected_sum
    {{
         0x03, 0x05, 0xf6, 0xb9, 0x9a, 0x44, 0xa2, 0xbd,
         0xec, 0x8b, 0x48, 0x4f, 0xfc, 0xee, 0x56, 0x1c,
         0xf9, 0xa0, 0xc3, 0xb7, 0xea, 0x92, 0xea, 0x8e,
         0x63, 0x34, 0xe6, 0xfb, 0xc4, 0xf1, 0xc1, 0x78, 0x99
    }};
    ec_point public_sum = secret_to_public_key(secret_a, true);
    BOOST_REQUIRE(public_sum == expected_sum);
}


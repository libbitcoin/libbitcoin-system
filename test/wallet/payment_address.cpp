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
using namespace bc::chain;
using namespace bc::wallet;

BOOST_AUTO_TEST_SUITE(payment_address_tests)

// $ bx base16-encode "Satoshi" | bx sha256
#define SECRET "002688cc350a5333a87fa622eacec626c3d1c0ebf9f3793de3885fa254d7e393"
#define SCRIPT "dup hash160 [ 18c0bd8d1818f1bf99cb1df2269c645318ef7b73 ] equalverify checksig"

// $ bx base16-encode "Satoshi" | bx sha256 | bx ec-to-public
#define COMPRESSED "03d24123978d696a6c964f2dcb1d1e000d4150102fbbcc37f020401e35fb4cb745"
// $ bx base16-encode "Satoshi" | bx sha256 | bx ec-to-public -u
#define UNCOMPRESSED "04d24123978d696a6c964f2dcb1d1e000d4150102fbbcc37f020401e35fb4cb74561a3362716303b0469f04c3d0e3cbc4b5b62a2da7add6ecc3b254404b12d2f83"

// $ bx base16-encode "Satoshi" | bx sha256 | bx ec-to-public | bx bitcoin160
#define COMPRESSED_HASH "f85beb6356d0813ddb0dbb14230a249fe931a135"
// $ bx base16-encode "Satoshi" | bx sha256 | bx ec-to-public -u | bx bitcoin160
#define UNCOMPRESSED_HASH "96ec4e06c665b7bd62cbe3d232f7c2d34016e136"

// $ bx base16-encode "Satoshi" | bx sha256 | bx ec-to-public | bx ec-to-address
#define ADDRESS_COMPRESSED "1PeChFbhxDD9NLbU21DfD55aQBC4ZTR3tE"
// $ bx base16-encode "Satoshi" | bx sha256 | bx ec-to-public -u | bx ec-to-address
#define ADDRESS_UNCOMPRESSED "1Em1SX7qQq1pTmByqLRafhL1ypx2V786tP"

// $ bx base16-encode "Satoshi" | bx sha256 | bx ec-to-public | bx ec-to-address -v 111
#define ADDRESS_COMPRESSED_TESTNET "n4A9zJggmEeQ9T55jaC32zHuGAnmSzPU2L"
// $ bx script-encode "dup hash160 [ 18c0bd8d1818f1bf99cb1df2269c645318ef7b73 ] equalverify checksig"
#define ADDRESS_UNCOMPRESSED_TESTNET "muGxjaCpDrT5EsfbYuPxVcYLqpYjNQnbkR"

// $ bx script-to-address "dup hash160 [ 18c0bd8d1818f1bf99cb1df2269c645318ef7b73 ] equalverify checksig"
#define ADDRESS_SCRIPT "3CPSWnCGjkePffNyVptkv45Bx35SaAwm7d"
// $ bx script-to-address "dup hash160 [ 18c0bd8d1818f1bf99cb1df2269c645318ef7b73 ] equalverify checksig" -v 196
#define ADDRESS_SCRIPT_TESTNET "2N3weaX8JMD9jsT1XAxWdY14TAPHcKYKHCT"

// $ bx script-to-address "dup hash160 [ 18c0bd8d1818f1bf99cb1df2269c645318ef7b73 ] equalverify checksig" | bx base58-decode
#define PAYMENT "0575566c599452b7bcb7f8cd4087bde9686fa9c52d8c2a7d90"
// $ bx script-to-address "dup hash160 [ 18c0bd8d1818f1bf99cb1df2269c645318ef7b73 ] equalverify checksig" -v 196 | bx base58-decode
#define PAYMENT_TESTNET "c475566c599452b7bcb7f8cd4087bde9686fa9c52d2fba2898"

// $ bx base58-decode 1111111111111111111114oLvT2 | bx wrap-decode
// wrapper
// {
//     checksum 285843604
//     payload 0000000000000000000000000000000000000000
//     version 0
// }
#define UNINITIALIZED_ADDRESS "1111111111111111111114oLvT2"

// negative tests:

BOOST_AUTO_TEST_CASE(payment_address__construct__default__invalid)
{
    const payment_address address;
    BOOST_REQUIRE(!address);
    BOOST_REQUIRE_EQUAL(address.encoded(), UNINITIALIZED_ADDRESS);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__string_invalid__invalid)
{
    const payment_address address("bogus");
    BOOST_REQUIRE(!address);
    BOOST_REQUIRE_EQUAL(address.encoded(), UNINITIALIZED_ADDRESS);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__compressed_from_uncompressed__invalid)
{
    ec_uncompressed point;
    BOOST_REQUIRE(decode_base16(point, UNCOMPRESSED));
    const payment_address address(point, 42);
    BOOST_REQUIRE(!address);
}

// secret:

BOOST_AUTO_TEST_CASE(payment_address__construct__secret__valid_expected)
{
    ec_secret secret;
    BOOST_REQUIRE(decode_base16(secret, SECRET));
    const payment_address address(secret);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_COMPRESSED);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__secret_testnet__valid_expected)
{
    ec_secret secret;
    BOOST_REQUIRE(decode_base16(secret, SECRET));
    const payment_address address(secret, 111);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_COMPRESSED_TESTNET);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__secret_mainnet_uncompressed__valid_expected)
{
    ec_secret secret;
    BOOST_REQUIRE(decode_base16(secret, SECRET));
    const payment_address address(secret, payment_address::mainnet, false);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_UNCOMPRESSED);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__secret_testnet_uncompressed__valid_expected)
{
    ec_secret secret;
    BOOST_REQUIRE(decode_base16(secret, SECRET));
    const payment_address address(secret, 111, false);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_UNCOMPRESSED_TESTNET);
}

// public:

BOOST_AUTO_TEST_CASE(payment_address__construct__public__valid_expected)
{
    ec_compressed point;
    BOOST_REQUIRE(decode_base16(point, COMPRESSED));
    const payment_address address(point);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_COMPRESSED);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__public_testnet__valid_expected)
{
    ec_compressed point;
    BOOST_REQUIRE(decode_base16(point, COMPRESSED));
    const payment_address address(point, 111);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_COMPRESSED_TESTNET);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__public_mainnet_uncompressed__valid_expected)
{
    ec_uncompressed point;
    BOOST_REQUIRE(decode_base16(point, UNCOMPRESSED));
    const payment_address address(point, payment_address::mainnet, false);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_UNCOMPRESSED);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__public_testnet_uncompressed__valid_expected)
{
    ec_uncompressed point;
    BOOST_REQUIRE(decode_base16(point, UNCOMPRESSED));
    const payment_address address(point, 111, false);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_UNCOMPRESSED_TESTNET);
}

// hash:

BOOST_AUTO_TEST_CASE(payment_address__construct__hash__valid_expected)
{
    short_hash hash;
    BOOST_REQUIRE(decode_base16(hash, COMPRESSED_HASH));
    const payment_address address(hash);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_COMPRESSED);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__uncompressed_hash_testnet__valid_expected)
{
    short_hash hash;
    BOOST_REQUIRE(decode_base16(hash, UNCOMPRESSED_HASH));
    const payment_address address(hash, 111);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_UNCOMPRESSED_TESTNET);
}

// script:

BOOST_AUTO_TEST_CASE(payment_address__construct__script__valid_expected)
{
    script ops;
    BOOST_REQUIRE(ops.from_string(SCRIPT));
    const payment_address address(ops);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_SCRIPT);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__script_testnet__valid_expected)
{
    script ops;
    BOOST_REQUIRE(ops.from_string(SCRIPT));
    const payment_address address(ops, 196);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_SCRIPT_TESTNET);
}

// payment:

BOOST_AUTO_TEST_CASE(payment_address__construct__payment__valid_expected)
{
    payment pay;
    BOOST_REQUIRE(decode_base16(pay, PAYMENT));
    const payment_address address(pay);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_SCRIPT);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__payment_testnet__valid_expected)
{
    payment pay;
    BOOST_REQUIRE(decode_base16(pay, PAYMENT_TESTNET));
    const payment_address address(pay);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_SCRIPT_TESTNET);
}

// copy:

BOOST_AUTO_TEST_CASE(payment_address__construct__copy__valid_expected)
{
    payment pay;
    BOOST_REQUIRE(decode_base16(pay, PAYMENT));
    const payment_address address(pay);
    const payment_address copy(address);
    BOOST_REQUIRE(copy);
    BOOST_REQUIRE_EQUAL(copy.encoded(), ADDRESS_SCRIPT);
}

// version:

BOOST_AUTO_TEST_CASE(payment_address__version__default__mainnet)
{
    ec_compressed point;
    BOOST_REQUIRE(decode_base16(point, COMPRESSED));
    const payment_address address(point);
    BOOST_REQUIRE_EQUAL(address.version(), payment_address::mainnet);
}

BOOST_AUTO_TEST_CASE(payment_address__version__testnet__testnet)
{
    ec_compressed point;
    const uint8_t testnet = 111;
    BOOST_REQUIRE(decode_base16(point, COMPRESSED));
    const payment_address address(point, testnet);
    BOOST_REQUIRE_EQUAL(address.version(), testnet);
}

BOOST_AUTO_TEST_CASE(payment_address__version__script__valid_mainnet_p2sh)
{
    script ops;
    BOOST_REQUIRE(ops.from_string(SCRIPT));
    const payment_address address(ops);
    BOOST_REQUIRE_EQUAL(address.version(), payment_address::mainnet_p2sh);
}

// hash:

BOOST_AUTO_TEST_CASE(payment_address__hash__compressed_point__expected)
{
    ec_compressed point;
    BOOST_REQUIRE(decode_base16(point, COMPRESSED));
    const payment_address address(point);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(encode_base16(address.hash()), COMPRESSED_HASH);
}

BOOST_AUTO_TEST_SUITE_END()

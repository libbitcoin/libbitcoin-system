/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(witness_address_tests)

using namespace bc::system::chain;
using namespace bc::system::wallet;

// BIP173 test constants.
#define MAINNET_WITNESS_PUBKEY_HASH_ADDRESS "bc1qw508d6qejxtdg4y5r3zarvary0c5xw7kv8f3t4"
#define TESTNET_WITNESS_PUBKEY_HASH_ADDRESS "tb1qw508d6qejxtdg4y5r3zarvary0c5xw7kxpjzsx"
#define TESTNET_WITNESS_PUBKEY_HASH_ADDRESS2 "tb1qr47dd36u96r0fjle36hdygdnp0v6pwfgqe6jxg"

#define MAINNET_WITNESS_SCRIPT_HASH_ADDRESS "bc1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3qccfmv3"
#define MAINNET_WITNESS_SCRIPT_HASH_SCRIPT "[0279BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798] checksig"

#define TESTNET_WITNESS_SCRIPT_HASH_ADDRESS "tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sl5k7"
#define TESTNET_WITNESS_SCRIPT_HASH_SCRIPT "[0279BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798] checksig"

// BIP173 witness address vectors
// github.com/bitcoin/bips/blob/master/bip-0173.mediawiki#test-vectors
struct test_address
{
    std::string address;
    std::string decoded;
};

typedef std::vector<test_address> test_list;

const test_list zero_version_tests
{
    { "BC1QW508D6QEJXTDG4Y5R3ZARVARY0C5XW7KV8F3T4", "0014751e76e8199196d454941c45d1b3a323f1433bd6" },
    { "tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sl5k7", "00201863143c14c5166804bd19203356da136c985678cd4d27a1b8c6329604903262" },
    { "tb1qqqqqp399et2xygdj5xreqhjjvcmzhxw4aywxecjdzew6hylgvsesrxh6hy", "0020000000c4a5cad46221b2a187905e5266362b99d5e91c6ce24d165dab93e86433" }
};

const test_list nonzero_version_tests
{
    { "bc1pw508d6qejxtdg4y5r3zarvary0c5xw7kw508d6qejxtdg4y5r3zarvary0c5xw7k7grplx", "5128751e76e8199196d454941c45d1b3a323f1433bd6751e76e8199196d454941c45d1b3a323f1433bd6" },
    { "BC1SW50QA3JX3S", "6002751e" },
    { "bc1zw508d6qejxtdg4y5r3zarvaryvg6kdaj", "5210751e76e8199196d454941c45d1b3a323" },
};

// Also:
// blockchain.com/btctest/tx/d869f854e1f8788bcff294cc83b280942a8c728de71eb709a2c29d10bfe21b7c

BOOST_AUTO_TEST_SUITE_END()

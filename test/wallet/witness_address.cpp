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
#include <boost/test/unit_test.hpp>
#include <bitcoin/system.hpp>

using namespace bc::system;
using namespace bc::system::chain;
using namespace bc::system::wallet;

BOOST_AUTO_TEST_SUITE(witness_address_tests)

// BIP 142 test constants.
#define BIP142_PUBLIC_KEY "0450863ad64a87ae8a2fe83c1af1a8403cb53f53e486d8511dad8a04887e5b23522cd470243453a299fa9e77237716103abc11a1df38855ed6f2ee187e9c582ba6"
#define ADDRESS_P2WPKH "p2xtZoXeX5X8BP8JfFhQK2nD3emtjch7UeFm"
#define ADDRESS_P2WPKH_TESTNET "QWywq9EVRcURXVarfKmHtyZ1MUGY4sJy5Dpq"

// BIP 142 witness test vectors
// https://github.com/theuni/bitcoin/blob/segwit-bip142/src/test/data/base58_keys_valid.json
struct test_vector
{
    std::string hash;
    std::string address;
    bool testnet;
    witness_address::encoding encoding;
    uint8_t witness_version;
};

typedef std::vector<test_vector> test_list;

test_list native_witness_tests =
{
    // hash, address, testnet, encoding, witness_version
    { "78f0c6aeafc418a10bc92c2bb24cdd2d27dc0a0b", "QWz8m8pbR6nqXJHEdtRTAxoQ6YHkxRNxLw7x", true, witness_address::encoding::testnet_base58_p2wpkh, 0 },
    { "a2594531c857b98e4f1a8aa78e2edb74efe8ff37", "p2y9GjzzvXoHJ8WWMaYaz7hf8sv74LiU8dLK", false, witness_address::encoding::mainnet_base58_p2wpkh, 0 },
    { "91df9b382022f8dbcf33a960d9c5d82e996d2020", "p2y7mdSZzcPLZL1S5woeBmafWvnE1yFk3H2f", false, witness_address::encoding::mainnet_base58_p2wpkh, 0 },
    { "59bc07ec402a63b072f07115bfc83d73a8a88c1654c85e2dd64bc6aee40cc239", "T7nYVDjPz5V9vPCfBJq4R55LvFkA3ReMfbvT5AX6vxAEgZRGtKF5d", true, witness_address::encoding::testnet_base58_p2wsh, 0 },
    { "0b1572dae6a95c50e7e8203b4de0d06291be349b9a8b9ef49d6389fc2f785dcf", "T7nXtahqZTvcSfU3QHhKSfJsTVb57oEM9aFRTQa6i5jnaUw5wvNAb", true, witness_address::encoding::testnet_base58_p2wsh, 0 },
    { "99071ccd792c8bb97138de5817169043c09676872e8f90e6aab433f8e532a693", "7XhQN533dfoMqSCNMt85Pu77pmjXE84Pq9FSsVTb6Rzu5h8oAc8Tn", false, witness_address::encoding::mainnet_base58_p2wsh, 0 }
};

// BIP 173 test constants.
#define MAINNET_P2WPKH_ADDRESS "bc1qw508d6qejxtdg4y5r3zarvary0c5xw7kv8f3t4"
#define TESTNET_P2WPKH_ADDRESS "tb1qw508d6qejxtdg4y5r3zarvary0c5xw7kxpjzsx"

#define MAINNET_P2WSH_ADDRESS "bc1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3qccfmv3"
#define MAINNET_P2WSH_SCRIPT "[0279BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798] checksig"

#define TESTNET_P2WSH_ADDRESS "tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sl5k7"
#define TESTNET_P2WSH_SCRIPT "[0279BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798] checksig"

// BIP 173 witness address vectors
// https://github.com/bitcoin/bips/blob/master/bip-0173.mediawiki#test-vectors
struct test_address
{
    std::string address;
    std::string decoded;
};

typedef std::vector<test_address> test_address_list;

test_address_list witness_address_tests =
{
    // witness address, decoded
    { "BC1QW508D6QEJXTDG4Y5R3ZARVARY0C5XW7KV8F3T4", "0014751e76e8199196d454941c45d1b3a323f1433bd6" },
    { "tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sl5k7", "00201863143c14c5166804bd19203356da136c985678cd4d27a1b8c6329604903262" },
    { "bc1pw508d6qejxtdg4y5r3zarvary0c5xw7kw508d6qejxtdg4y5r3zarvary0c5xw7k7grplx", "5128751e76e8199196d454941c45d1b3a323f1433bd6751e76e8199196d454941c45d1b3a323f1433bd6" },
    { "BC1SW50QA3JX3S", "6002751e" },
    { "bc1zw508d6qejxtdg4y5r3zarvaryvg6kdaj", "5210751e76e8199196d454941c45d1b3a323" },
    { "tb1qqqqqp399et2xygdj5xreqhjjvcmzhxw4aywxecjdzew6hylgvsesrxh6hy", "0020000000c4a5cad46221b2a187905e5266362b99d5e91c6ce24d165dab93e86433" }
};

BOOST_AUTO_TEST_CASE(witness_address__construct__to_p2wpkh__valid_expected)
{
    const witness_address address(ec_public(BIP142_PUBLIC_KEY), witness_address::encoding::mainnet_base58_p2wpkh);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_P2WPKH);
}

BOOST_AUTO_TEST_CASE(witness_address__construct__from_base58_p2wpkh_string__valid_expected)
{
    const witness_address address(std::string(ADDRESS_P2WPKH), witness_address::encoding::mainnet_base58_p2wpkh);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_P2WPKH);
}

BOOST_AUTO_TEST_CASE(witness_address__construct__testnet_to_p2wpkh__valid_expected)
{
    const witness_address address(ec_public(BIP142_PUBLIC_KEY), witness_address::encoding::testnet_base58_p2wpkh);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_P2WPKH_TESTNET);
}

BOOST_AUTO_TEST_CASE(witness_address__construct__testnet_to_p2wpkh_string__valid_expected)
{
    const witness_address address(std::string(ADDRESS_P2WPKH_TESTNET), witness_address::encoding::testnet_base58_p2wpkh);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_P2WPKH_TESTNET);
}

BOOST_AUTO_TEST_CASE(witness_address__construct__mainnet_to_p2wsh__valid_expected)
{
    chain::script script;
    script.from_string(MAINNET_P2WSH_SCRIPT);
    const witness_address address(script, witness_address::encoding::mainnet_p2wsh);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), MAINNET_P2WSH_ADDRESS);
}

BOOST_AUTO_TEST_CASE(witness_address__construct__testnet_to_p2wsh__valid_expected)
{
    chain::script script;
    script.from_string(TESTNET_P2WSH_SCRIPT);
    const witness_address address(script, witness_address::encoding::testnet_p2wsh);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), TESTNET_P2WSH_ADDRESS);
}

BOOST_AUTO_TEST_CASE(witness_address__construct__witness_data_to_testnet_p2wpkh__valid_expected)
{
    // Based on witness data from:
    // https://www.blockchain.com/btctest/tx/d869f854e1f8788bcff294cc83b280942a8c728de71eb709a2c29d10bfe21b7c

    // Create witness address from public key data(_chunk).
    data_chunk witness_data;
    decode_base16(witness_data, std::string("038262a6c6cec93c2d3ecd6c6072efea86d02ff8e3328bbd0242b20af3425990ac"));
    const witness_address address(witness_data, witness_address::encoding::testnet_p2wpkh);
    BOOST_REQUIRE_EQUAL(address.bech32("tb"), "tb1qr47dd36u96r0fjle36hdygdnp0v6pwfgqe6jxg");

    // Create the same witness address from ec_public.
    const witness_address address2(ec_public("038262a6c6cec93c2d3ecd6c6072efea86d02ff8e3328bbd0242b20af3425990ac"), witness_address::encoding::testnet_p2wpkh);

    // Ensure they encode equally.
    BOOST_REQUIRE_EQUAL(address.bech32("tb"), address2.bech32("tb"));
    BOOST_REQUIRE_EQUAL(address.encoded(), address2.encoded());
}

BOOST_AUTO_TEST_CASE(witness_address__witness_vectors__valid_expected)
{
    for (const auto& test: native_witness_tests)
    {
        if (test.encoding == witness_address::encoding::mainnet_base58_p2wpkh ||
            test.encoding == witness_address::encoding::testnet_base58_p2wpkh)
        {
            short_hash hash;
            BOOST_REQUIRE(decode_base16(hash, test.hash));
            const witness_address address(hash, test.encoding);
            BOOST_REQUIRE(address);
            BOOST_REQUIRE_EQUAL(address.encoded(), test.address);
        }
        else
        {
            hash_digest hash;
            BOOST_REQUIRE(decode_base16(hash, test.hash));
            const witness_address address(hash, test.encoding);
            BOOST_REQUIRE(address);
            BOOST_REQUIRE_EQUAL(address.encoded(), test.address);
        }
    }
}

BOOST_AUTO_TEST_CASE(base32__witness_address_vectors__valid_expected)
{
    for (const auto& test: witness_address_tests)
    {
        base32 decoded;
        BOOST_REQUIRE(decode_base32(decoded, test.address));
        uint8_t witness_version = decoded.payload.front();
        // https://github.com/sipa/bech32/blob/master/ref/javascript/tests.js#L24
        if (witness_version != 0)
            witness_version += 0x50;

        const auto witness_program = witness_address::convert_bits(5, 8, false,
            decoded.payload, 1);
        BOOST_REQUIRE(witness_program.size() > 1 &&
                      witness_program.size() < 41);
        const auto witness_program_size = static_cast<uint8_t>(
            witness_program.size());

        data_chunk result = { witness_version, witness_program_size };
        extend_data(result, witness_program);
        BOOST_REQUIRE_EQUAL(encode_base16(result), test.decoded);
    }
}

BOOST_AUTO_TEST_SUITE_END()

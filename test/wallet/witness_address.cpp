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
    uint8_t version;
    uint8_t witness_version;
};

const std::vector<test_vector> native_witness_p2wpkh_tests =
{
    // hash, address, testnet, version, witness_version
    { "78f0c6aeafc418a10bc92c2bb24cdd2d27dc0a0b", "QWz8m8pbR6nqXJHEdtRTAxoQ6YHkxRNxLw7x", true, witness_address::testnet_base58_p2wpkh, 0 },
    { "a2594531c857b98e4f1a8aa78e2edb74efe8ff37", "p2y9GjzzvXoHJ8WWMaYaz7hf8sv74LiU8dLK", false, witness_address::mainnet_base58_p2wpkh, 0 },
    { "91df9b382022f8dbcf33a960d9c5d82e996d2020", "p2y7mdSZzcPLZL1S5woeBmafWvnE1yFk3H2f", false, witness_address::mainnet_base58_p2wpkh, 0 },
};

const std::vector<test_vector> native_witness_p2wsh_tests =
{
    // hash, address, testnet, version, witness_version
    { "59bc07ec402a63b072f07115bfc83d73a8a88c1654c85e2dd64bc6aee40cc239", "T7nYVDjPz5V9vPCfBJq4R55LvFkA3ReMfbvT5AX6vxAEgZRGtKF5d", true, witness_address::testnet_base58_p2wsh, 0 },
    { "0b1572dae6a95c50e7e8203b4de0d06291be349b9a8b9ef49d6389fc2f785dcf", "T7nXtahqZTvcSfU3QHhKSfJsTVb57oEM9aFRTQa6i5jnaUw5wvNAb", true, witness_address::testnet_base58_p2wsh, 0 },
    { "99071ccd792c8bb97138de5817169043c09676872e8f90e6aab433f8e532a693", "7XhQN533dfoMqSCNMt85Pu77pmjXE84Pq9FSsVTb6Rzu5h8oAc8Tn", false, witness_address::mainnet_base58_p2wsh, 0 }
};

// BIP 173 test constants.
#define MAINNET_P2WPKH_ADDRESS "bc1qw508d6qejxtdg4y5r3zarvary0c5xw7kv8f3t4"
#define TESTNET_P2WPKH_ADDRESS "tb1qw508d6qejxtdg4y5r3zarvary0c5xw7kxpjzsx"
#define TESTNET_P2WPKH_ADDRESS2 "tb1qr47dd36u96r0fjle36hdygdnp0v6pwfgqe6jxg"

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

const test_address_list witness_address_zero_version_tests =
{
    // witness address, decoded
    { "BC1QW508D6QEJXTDG4Y5R3ZARVARY0C5XW7KV8F3T4", "0014751e76e8199196d454941c45d1b3a323f1433bd6" },
    { "tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sl5k7", "00201863143c14c5166804bd19203356da136c985678cd4d27a1b8c6329604903262" },
    { "tb1qqqqqp399et2xygdj5xreqhjjvcmzhxw4aywxecjdzew6hylgvsesrxh6hy", "0020000000c4a5cad46221b2a187905e5266362b99d5e91c6ce24d165dab93e86433" }
};

const test_address_list witness_address_nonzero_version_tests =
{
    { "bc1pw508d6qejxtdg4y5r3zarvary0c5xw7kw508d6qejxtdg4y5r3zarvary0c5xw7k7grplx", "5128751e76e8199196d454941c45d1b3a323f1433bd6751e76e8199196d454941c45d1b3a323f1433bd6" },
    { "BC1SW50QA3JX3S", "6002751e" },
    { "bc1zw508d6qejxtdg4y5r3zarvaryvg6kdaj", "5210751e76e8199196d454941c45d1b3a323" },
};

BOOST_AUTO_TEST_CASE(witness_address__construct__from_ec_public__valid_expected)
{
    const witness_address address(ec_public(BIP142_PUBLIC_KEY), witness_address::mainnet_base58_p2wpkh);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_P2WPKH);
}

BOOST_AUTO_TEST_CASE(witness_address__construct__from_base58_p2wpkh_string__valid_expected)
{
    const witness_address address(std::string(ADDRESS_P2WPKH), witness_address::mainnet_base58_p2wpkh);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_P2WPKH);
}

BOOST_AUTO_TEST_CASE(witness_address__construct__testnet_to_p2wpkh__valid_expected)
{
    const witness_address address(ec_public(BIP142_PUBLIC_KEY), witness_address::testnet_base58_p2wpkh);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_P2WPKH_TESTNET);
}

BOOST_AUTO_TEST_CASE(witness_address__construct__testnet_to_p2wpkh_string__valid_expected)
{
    const witness_address address(std::string(ADDRESS_P2WPKH_TESTNET), witness_address::testnet_base58_p2wpkh);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_P2WPKH_TESTNET);
}

BOOST_AUTO_TEST_CASE(witness_address__construct__mainnet_to_p2wsh__valid_expected)
{
    chain::script script;
    script.from_string(MAINNET_P2WSH_SCRIPT);
    const witness_address address(script, witness_address::mainnet_bech32_p2wsh, witness_address::mainnet_prefix);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), MAINNET_P2WSH_ADDRESS);
}

BOOST_AUTO_TEST_CASE(witness_address__construct__testnet_to_p2wsh__valid_expected)
{
    chain::script script;
    script.from_string(TESTNET_P2WSH_SCRIPT);
    const witness_address address(script, witness_address::testnet_bech32_p2wsh, witness_address::testnet_prefix);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), TESTNET_P2WSH_ADDRESS);
}

BOOST_AUTO_TEST_CASE(witness_address__construct__ec_public_to_testnet_p2wpkh__valid_expected)
{
    // Based on witness data from:
    // https://www.blockchain.com/btctest/tx/d869f854e1f8788bcff294cc83b280942a8c728de71eb709a2c29d10bfe21b7c

    // Create the same witness address from ec_public.
    const std::string test_public_key = "038262a6c6cec93c2d3ecd6c6072efea86d02ff8e3328bbd0242b20af3425990ac";
    const witness_address address(ec_public(test_public_key), witness_address::testnet_bech32_p2wpkh, witness_address::testnet_prefix);

    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.bech32(), TESTNET_P2WPKH_ADDRESS2);
    BOOST_REQUIRE_EQUAL(address.encoded(), TESTNET_P2WPKH_ADDRESS2);
}

BOOST_AUTO_TEST_CASE(witness_address__witness_p2wpkh_vectors__valid_expected)
{
    for (const auto& test: native_witness_p2wpkh_tests)
    {
        short_hash hash;
        BOOST_REQUIRE(decode_base16(hash, test.hash));
        const witness_address address(hash, test.version);
        BOOST_REQUIRE(address);
        BOOST_REQUIRE_EQUAL(address.encoded(), test.address);
    }
}

BOOST_AUTO_TEST_CASE(witness_address__witness_p2wsh_vectors__valid_expected)
{
    for (const auto& test: native_witness_p2wsh_tests)
    {
        hash_digest hash;
        BOOST_REQUIRE(decode_base16(hash, test.hash));
        const witness_address address(hash, test.version);
        BOOST_REQUIRE(address);
        BOOST_REQUIRE_EQUAL(address.encoded(), test.address);
    }
}

struct witness_address_accessor
  : public witness_address
{
    data_chunk convert_bits(uint32_t from_bits, uint32_t to_bits, bool pad,
        const data_chunk& in, size_t in_offset)
    {
        return witness_address::convert_bits(from_bits, to_bits, pad, in,
            in_offset);
    }
};

BOOST_AUTO_TEST_CASE(base32__witness_address_zero_version_tests__valid_expected)
{
    const auto bech32_contracted_bit_size = 5;
    const auto bech32_expanded_bit_size = 8;
    const auto witness_v0_program_minimum_limit = 1;
    const auto witness_v0_program_maximum_limit = 41;

    for (const auto& test: witness_address_zero_version_tests)
    {
        base32 decoded;
        BOOST_REQUIRE(decode_base32(decoded, test.address));

        uint8_t witness_version = decoded.payload.front();
        BOOST_REQUIRE(witness_version == 0);

        witness_address_accessor converter;
        const auto witness_program = converter.convert_bits(
            bech32_contracted_bit_size, bech32_expanded_bit_size, false,
            decoded.payload, 1);

        const auto witness_program_size = static_cast<uint8_t>(witness_program.size());
        BOOST_REQUIRE(witness_program_size > witness_v0_program_minimum_limit &&
                      witness_program_size < witness_v0_program_maximum_limit);

        const auto result = build_chunk(
        {
            to_chunk(witness_version),
            to_chunk(witness_program_size),
            witness_program
        });

        BOOST_REQUIRE_EQUAL(encode_base16(result), test.decoded);
    }
}

BOOST_AUTO_TEST_CASE(base32__witness_address_nonzero_version_tests__valid_expected)
{
    // This op_version gap is used to normalize the value difference
    // between the defined OP_0 opcode (value 0) and the OP_1 opcode
    // (value 0x51)
    const auto op_version_gap = 0x50;
    const auto witness_v0_program_minimum_limit = 1;
    const auto witness_v0_program_maximum_limit = 41;
    const auto bech32_contracted_bit_size = 5;
    const auto bech32_expanded_bit_size = 8;

    for (const auto& test: witness_address_nonzero_version_tests)
    {
        base32 decoded;
        BOOST_REQUIRE(decode_base32(decoded, test.address));
        uint8_t witness_version = decoded.payload.front();
        BOOST_REQUIRE(witness_version != 0);
        witness_version += op_version_gap;

        witness_address_accessor converter;
        const auto witness_program = converter.convert_bits(
            bech32_contracted_bit_size, bech32_expanded_bit_size, false,
            decoded.payload, 1);

        const auto witness_program_size = static_cast<uint8_t>(witness_program.size());
        BOOST_REQUIRE(witness_program_size > witness_v0_program_minimum_limit &&
                      witness_program_size < witness_v0_program_maximum_limit);

        const auto result = build_chunk(
        {
            to_chunk(witness_version),
            to_chunk(witness_program_size),
            witness_program
        });

        BOOST_REQUIRE_EQUAL(encode_base16(result), test.decoded);
    }
}

BOOST_AUTO_TEST_SUITE_END()

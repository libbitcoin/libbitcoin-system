/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#include "../../test.hpp"

BOOST_AUTO_TEST_SUITE(payment_address_tests)

using namespace system::chain;
using namespace bc::system::wallet;

// $ bx base16-encode "Satoshi" | bx sha256
#define SECRET "002688cc350a5333a87fa622eacec626c3d1c0ebf9f3793de3885fa254d7e393"
#define SCRIPT "dup hash160 [18c0bd8d1818f1bf99cb1df2269c645318ef7b73] equalverify checksig"

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
// $ bx script-encode "dup hash160 [18c0bd8d1818f1bf99cb1df2269c645318ef7b73] equalverify checksig"
#define ADDRESS_UNCOMPRESSED_TESTNET "muGxjaCpDrT5EsfbYuPxVcYLqpYjNQnbkR"

// $ bx script-to-address "dup hash160 [18c0bd8d1818f1bf99cb1df2269c645318ef7b73] equalverify checksig"
#define ADDRESS_SCRIPT "3CPSWnCGjkePffNyVptkv45Bx35SaAwm7d"
// $ bx script-to-address "dup hash160 [18c0bd8d1818f1bf99cb1df2269c645318ef7b73] equalverify checksig" -v 196
#define ADDRESS_SCRIPT_TESTNET "2N3weaX8JMD9jsT1XAxWdY14TAPHcKYKHCT"

// $ bx script-to-address "dup hash160 [18c0bd8d1818f1bf99cb1df2269c645318ef7b73] equalverify checksig" | bx base58-decode
#define PAYMENT "0575566c599452b7bcb7f8cd4087bde9686fa9c52d8c2a7d90"
// $ bx script-to-address "dup hash160 [18c0bd8d1818f1bf99cb1df2269c645318ef7b73] equalverify checksig" -v 196 | bx base58-decode
#define PAYMENT_TESTNET "c475566c599452b7bcb7f8cd4087bde9686fa9c52d2fba2898"

// Null hash and checksum.
#define UNINITIALIZED_ADDRESS "1111111111111111111111111"

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

// construct secret:

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

    // MSVC CTP loses the MSB (WIF prefix) byte of the literal version when
    // using this initializer, but the MSB isn't used by payment_address.
    const payment_address address({ secret, 0x806f, true });
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_COMPRESSED_TESTNET);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__secret_mainnet_uncompressed__valid_expected)
{
    ec_secret secret;
    BOOST_REQUIRE(decode_base16(secret, SECRET));

    const payment_address address({ secret, payment_address::mainnet_p2kh, false });
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_UNCOMPRESSED);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__secret_testnet_uncompressed__valid_expected)
{
    ec_secret secret;
    BOOST_REQUIRE(decode_base16(secret, SECRET));

    // MSVC CTP loses the MSB (WIF prefix) byte of the literal version when
    // using this initializer, but the MSB isn't used by payment_address.
    const payment_address address({ secret, 0x806f, false });
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_UNCOMPRESSED_TESTNET);
}

// construct public:

BOOST_AUTO_TEST_CASE(payment_address__construct__public__valid_expected)
{
    const payment_address address(ec_public(COMPRESSED));
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_COMPRESSED);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__public_testnet__valid_expected)
{
    const payment_address address(ec_public(COMPRESSED), 0x6f);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_COMPRESSED_TESTNET);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__public_uncompressed__valid_expected)
{
    const payment_address address(ec_public(UNCOMPRESSED));
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_UNCOMPRESSED);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__public_testnet_uncompressed__valid_expected)
{
    const payment_address address(ec_public(UNCOMPRESSED), 0x6f);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_UNCOMPRESSED_TESTNET);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__public_compressed_from_uncompressed_testnet__valid_expected)
{
    ec_uncompressed point;
    BOOST_REQUIRE(decode_base16(point, UNCOMPRESSED));

    const payment_address address(ec_public{ point, true }, 0x6f);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_COMPRESSED_TESTNET);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__public_uncompressed_from_compressed_testnet__valid_expected)
{
    ec_compressed point;
    BOOST_REQUIRE(decode_base16(point, COMPRESSED));

    const payment_address address(ec_public{ point, false }, 0x6f);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_UNCOMPRESSED_TESTNET);
}

// construct hash:

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

    const payment_address address(hash, 0x6f);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_UNCOMPRESSED_TESTNET);
}

// construct script:

BOOST_AUTO_TEST_CASE(payment_address__construct__script__valid_expected)
{
    script ops(SCRIPT);
    BOOST_REQUIRE(ops.is_valid());

    const payment_address address(ops);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_SCRIPT);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__script_testnet__valid_expected)
{
    script ops(SCRIPT);
    BOOST_REQUIRE(ops.is_valid());

    const payment_address address(ops, 0xc4);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_SCRIPT_TESTNET);
}

// construct payment:

BOOST_AUTO_TEST_CASE(payment_address__construct__copy_payment__valid_expected)
{
    payment::value_type pay;
    BOOST_REQUIRE(decode_base16(pay, PAYMENT));

    const payment_address address(pay);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), ADDRESS_SCRIPT);
}

// construct self:

BOOST_AUTO_TEST_CASE(payment_address__construct__copy__valid_expected)
{
    payment::value_type pay;
    BOOST_REQUIRE(decode_base16(pay, PAYMENT));

    const payment_address address(pay);
    const payment_address copy(address);
    BOOST_REQUIRE(copy);
    BOOST_REQUIRE_EQUAL(copy.encoded(), ADDRESS_SCRIPT);
}

BOOST_AUTO_TEST_CASE(payment_address__construct__move__valid_expected)
{
    payment::value_type pay;
    BOOST_REQUIRE(decode_base16(pay, PAYMENT));

    payment_address address(pay);
    const payment_address copy(std::move(address));
    BOOST_REQUIRE(copy);
    BOOST_REQUIRE_EQUAL(copy.encoded(), ADDRESS_SCRIPT);
}

// prefix property:

BOOST_AUTO_TEST_CASE(payment_address__prefix__default__mainnet)
{
    const payment_address address(ec_public(COMPRESSED));
    BOOST_REQUIRE_EQUAL(address.prefix(), payment_address::mainnet_p2kh);
}

BOOST_AUTO_TEST_CASE(payment_address__prefix__testnet__testnet)
{
    const uint8_t testnet = 0x6f;
    const payment_address address(ec_public(COMPRESSED), testnet);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.prefix(), testnet);
}

BOOST_AUTO_TEST_CASE(payment_address__prefix__script__valid_mainnet_p2sh)
{
    script ops(SCRIPT);
    BOOST_REQUIRE(ops.is_valid());

    const payment_address address(ops);
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.prefix(), payment_address::mainnet_p2sh);
}

// hash property:

BOOST_AUTO_TEST_CASE(payment_address__hash__compressed_point__expected)
{
    const payment_address address(ec_public(COMPRESSED));
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(encode_base16(address.hash()), COMPRESSED_HASH);
}

// bitcoind cross-check, from its src/test/data/key_io_valid.json.
#define BITCOIND_P2KH_ADDRESS "1FsSia9rv4NeEwvJ2GvXrX7LyxYspbN2mo"
#define BITCOIND_P2KH_SCRIPT "76a914a31c06bd463e3923bc1aadbde48b16976c08071788ac"
#define BITCOIND_P2SH_ADDRESS "36j4NfKv6Akva9amjWrLG6MuSQym1GuEmm"
#define BITCOIND_P2SH_SCRIPT "a914373b819a068f32b7a6b38b6b38729647cfde01c287"
#define BITCOIND_TESTNET_P2KH_ADDRESS "mzK2FFDEhxqHcmrJw1ysqFkVyhUULo45hZ"
#define BITCOIND_TESTNET_P2KH_SCRIPT "76a914ce28b26c57472737f5c3561a1761185bd8589a4388ac"
#define BITCOIND_TESTNET_P2SH_ADDRESS "2NC2hEhe28ULKAJkW5MjZ3jtTMJdvXmByvK"
#define BITCOIND_TESTNET_P2SH_SCRIPT "a914ce0bba75891ff9ec60148d4bd4a09ee2dc5c933187"

// output_script
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(payment_address__output_script__bitcoind_p2kh__expected)
{
    const payment_address instance(BITCOIND_P2KH_ADDRESS);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(encode_base16(instance.output_script().to_data(false)), BITCOIND_P2KH_SCRIPT);
}

BOOST_AUTO_TEST_CASE(payment_address__output_script__bitcoind_p2sh__expected)
{
    const payment_address instance(BITCOIND_P2SH_ADDRESS);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(encode_base16(instance.output_script().to_data(false)), BITCOIND_P2SH_SCRIPT);
}

BOOST_AUTO_TEST_CASE(payment_address__output_script__bitcoind_testnet_p2kh__expected)
{
    const payment_address instance(BITCOIND_TESTNET_P2KH_ADDRESS);
    BOOST_REQUIRE(instance);
    const auto script = instance.output_script(payment_address::testnet_p2kh, payment_address::testnet_p2sh);
    BOOST_REQUIRE_EQUAL(encode_base16(script.to_data(false)), BITCOIND_TESTNET_P2KH_SCRIPT);
}

BOOST_AUTO_TEST_CASE(payment_address__output_script__bitcoind_testnet_p2sh__expected)
{
    const payment_address instance(BITCOIND_TESTNET_P2SH_ADDRESS);
    BOOST_REQUIRE(instance);
    const auto script = instance.output_script(payment_address::testnet_p2kh, payment_address::testnet_p2sh);
    BOOST_REQUIRE_EQUAL(encode_base16(script.to_data(false)), BITCOIND_TESTNET_P2SH_SCRIPT);
}

// A prefix matching neither parameter yields no script.
BOOST_AUTO_TEST_CASE(payment_address__output_script__unmatched_prefix__empty)
{
    const payment_address instance(BITCOIND_TESTNET_P2KH_ADDRESS);
    BOOST_REQUIRE(instance.output_script().ops().empty());
}

// extract_output
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(payment_address__extract_output__bitcoind_p2kh__expected)
{
    const chain::script script(base16_chunk(BITCOIND_P2KH_SCRIPT), false);
    BOOST_REQUIRE_EQUAL(payment_address::extract_output(script).encoded(), BITCOIND_P2KH_ADDRESS);
}

BOOST_AUTO_TEST_CASE(payment_address__extract_output__bitcoind_p2sh__expected)
{
    const chain::script script(base16_chunk(BITCOIND_P2SH_SCRIPT), false);
    BOOST_REQUIRE_EQUAL(payment_address::extract_output(script).encoded(), BITCOIND_P2SH_ADDRESS);
}

BOOST_AUTO_TEST_CASE(payment_address__extract_output__bitcoind_testnet_p2kh__expected)
{
    const chain::script script(base16_chunk(BITCOIND_TESTNET_P2KH_SCRIPT), false);
    const auto address = payment_address::extract_output(script, payment_address::testnet_p2kh, payment_address::testnet_p2sh);
    BOOST_REQUIRE_EQUAL(address.encoded(), BITCOIND_TESTNET_P2KH_ADDRESS);
}

BOOST_AUTO_TEST_CASE(payment_address__extract_output__bitcoind_testnet_p2sh__expected)
{
    const chain::script script(base16_chunk(BITCOIND_TESTNET_P2SH_SCRIPT), false);
    const auto address = payment_address::extract_output(script, payment_address::testnet_p2kh, payment_address::testnet_p2sh);
    BOOST_REQUIRE_EQUAL(address.encoded(), BITCOIND_TESTNET_P2SH_ADDRESS);
}

BOOST_AUTO_TEST_CASE(payment_address__extract_output__non_standard__invalid)
{
    const chain::script script{ chain::operations{ { chain::opcode::pick } } };
    BOOST_REQUIRE(!payment_address::extract_output(script));
}

// extract_input
// ----------------------------------------------------------------------------

// The sign_key_hash pattern is [endorsement][public key].
BOOST_AUTO_TEST_CASE(payment_address__extract_input__sign_key_hash__public_key_address)
{
    const chain::operations ops{ { data_chunk(70, 0x42), true }, { base16_chunk(COMPRESSED), true } };
    const chain::script script{ ops };
    BOOST_REQUIRE(script.input_pattern() == chain::script_pattern::sign_key_hash);

    const auto addresses = payment_address::extract_input(script);
    BOOST_REQUIRE_EQUAL(addresses.size(), 1u);
    BOOST_REQUIRE_EQUAL(addresses.front().encoded(), ADDRESS_COMPRESSED);
}

BOOST_AUTO_TEST_CASE(payment_address__extract_input__sign_script_hash__embedded_script_address)
{
    const auto embedded = base16_chunk("51");
    const chain::script script{ chain::operations{ { embedded, false } } };
    BOOST_REQUIRE(script.input_pattern() == chain::script_pattern::sign_script_hash);

    const auto addresses = payment_address::extract_input(script);
    BOOST_REQUIRE_EQUAL(addresses.size(), 1u);
    BOOST_REQUIRE_EQUAL(addresses.front().hash(), bitcoin_short_hash(embedded));
}

BOOST_AUTO_TEST_CASE(payment_address__extract_input__non_standard__empty)
{
    const chain::script script{ chain::operations{ { chain::opcode::pick } } };
    BOOST_REQUIRE(payment_address::extract_input(script).empty());
}

// extract prefers input extraction, falling back to output extraction.
BOOST_AUTO_TEST_CASE(payment_address__extract__output_script__output_address)
{
    const chain::script script(base16_chunk(BITCOIND_P2KH_SCRIPT), false);
    const auto addresses = payment_address::extract(script);
    BOOST_REQUIRE_EQUAL(addresses.size(), 1u);
    BOOST_REQUIRE_EQUAL(addresses.front().encoded(), BITCOIND_P2KH_ADDRESS);
}

// operators and accessors
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(payment_address__to_payment__always__round_trips)
{
    const payment_address instance(BITCOIND_P2KH_ADDRESS);
    const payment_address copy(instance.to_payment());
    BOOST_REQUIRE(copy == instance);
    BOOST_REQUIRE(!(copy != instance));
}

BOOST_AUTO_TEST_CASE(payment_address__inequality__different__true)
{
    const payment_address left(BITCOIND_P2KH_ADDRESS);
    const payment_address right(BITCOIND_P2SH_ADDRESS);
    BOOST_REQUIRE(left != right);
}

BOOST_AUTO_TEST_CASE(payment_address__lesser__by_encoding__expected)
{
    const payment_address left(BITCOIND_P2KH_ADDRESS);
    const payment_address right(BITCOIND_P2SH_ADDRESS);
    BOOST_REQUIRE_EQUAL(left < right, std::string(BITCOIND_P2KH_ADDRESS) < std::string(BITCOIND_P2SH_ADDRESS));
}

BOOST_AUTO_TEST_CASE(payment_address__stream__round_trips)
{
    std::istringstream in{ BITCOIND_P2KH_ADDRESS };
    payment_address instance{};
    in >> instance;
    BOOST_REQUIRE_EQUAL(instance.encoded(), BITCOIND_P2KH_ADDRESS);

    std::ostringstream out{};
    out << instance;
    BOOST_REQUIRE_EQUAL(out.str(), BITCOIND_P2KH_ADDRESS);
}

BOOST_AUTO_TEST_CASE(payment_address__stream__invalid__throws)
{
    payment_address instance{};
    BOOST_REQUIRE_THROW(std::istringstream("bogus") >> instance, istream_exception);
}

BOOST_AUTO_TEST_SUITE_END()

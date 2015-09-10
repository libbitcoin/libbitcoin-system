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
#include <algorithm>
#include <cstddef>
#include <string>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;
using namespace bc::bip38;
using namespace bc::wallet;

BOOST_AUTO_TEST_SUITE(bip38_tests)

BOOST_AUTO_TEST_CASE(bip38__fixture__unicode_passphrase__validated)
{
    const auto encoded_password = base16_literal("cf92cc8100f0909080f09f92a9");
    std::string passphrase(encoded_password.begin(), encoded_password.end());

    // This confirms that the passphrase decodes as expected in BIP38.
    const auto normal = to_normal_nfc_form(passphrase);
    data_chunk normalized(normal.size());
    std::copy(normal.begin(), normal.end(), normalized.begin());
    BOOST_REQUIRE_EQUAL(encode_base16(normalized), "cf9300f0909080f09f92a9");
}

// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(bip38__create_token_lot)

// TODO: obtain external vectors to validate the create_token implementation.

#define BC_REQUIRE_CREATE_TOKEN_LOT(passphrase, bytes, lot, sequence) \
    token out_token; \
    BOOST_REQUIRE(create_token(out_token, passphrase, bytes, lot, sequence))

BOOST_AUTO_TEST_CASE(bip38__create_token_lot__lot_overlow__false)
{
    const auto passphrase = "";
    const auto salt = base16_literal("baadf00d");
    token out_token;
    BOOST_REQUIRE(!create_token(out_token, passphrase, salt, 1048575 + 1, 0));
}

BOOST_AUTO_TEST_CASE(bip38__create_token_lot__sequence_overlow__false)
{
    const auto passphrase = "";
    const auto salt = base16_literal("baadf00d");
    token out_token;
    BOOST_REQUIRE(!create_token(out_token, passphrase, salt, 0, 4095 + 1));
}

BOOST_AUTO_TEST_CASE(bip38__create_token_lot__defaults__expected)
{
    const auto passphrase = "";
    const auto salt = base16_literal("baadf00d");
    BC_REQUIRE_CREATE_TOKEN_LOT(passphrase, salt, 0, 0);
    BOOST_CHECK_EQUAL(encode_base58(out_token), "passphrasecpXbDpHuo8F7yQVcg1eQKPuX7rzGwBtEH1YSZnKbyk75x3rugZu1ci4RyF4rEn");
}

BOOST_AUTO_TEST_CASE(bip38__create_token_lot__passphrase__expected)
{
    const auto passphrase = "passphrase";
    const auto salt = base16_literal("baadf00d");
    BC_REQUIRE_CREATE_TOKEN_LOT(passphrase, salt, 0, 0);
    BOOST_CHECK_EQUAL(encode_base58(out_token), "passphrasecpXbDpHuo8F7x4pQXMhsJs2j7L8LTV8ujk9jGqgzUrafBeto9VrabP5SmvANvz");
}

BOOST_AUTO_TEST_CASE(bip38__create_token_lot__passphrase_lot_max__expected)
{
    const auto passphrase = "passphrase";
    const auto salt = base16_literal("baadf00d");
    BC_REQUIRE_CREATE_TOKEN_LOT(passphrase, salt, 1048575, 0);
    BOOST_CHECK_EQUAL(encode_base58(out_token), "passphrasecpXbDpHuo8FGWnwMTnTFiHSDnqyARArE2YSFQzMHtCZvM2oWg2K3Ua2crKyc11");
}

BOOST_AUTO_TEST_CASE(bip38__create_token_lot__passphrase_sequence_max__expected)
{
    const auto passphrase = "passphrase";
    const auto salt = base16_literal("baadf00d");
    BC_REQUIRE_CREATE_TOKEN_LOT(passphrase, salt, 0, 4095);
    BOOST_CHECK_EQUAL(encode_base58(out_token), "passphrasecpXbDpHuo8FGWnwMTnTFiHSDnqyARArE2YSFQzMHtCZvM2oWg2K3Ua2crKyc11");
}

BOOST_AUTO_TEST_CASE(bip38__create_token_lot__passphrase_lot_sequence__expected)
{
    const auto passphrase = "passphrase";
    const auto salt = base16_literal("baadf00d");
    BC_REQUIRE_CREATE_TOKEN_LOT(passphrase, salt, 42, 42);
    BOOST_CHECK_EQUAL(encode_base58(out_token), "passphrasecpXbDpHuo8FGWnwMTnTFiHSDnqyARArE2YSFQzMHtCZvM2oWg2K3Ua2crKyc11");
}

BOOST_AUTO_TEST_SUITE_END()

// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(bip38__create_token_entropy)

// TODO: obtain external vectors to validate the create_token implementation.

#define BC_CREATE_TOKEN_ENTROPY(passphrase, bytes) \
    token out_token; \
    create_token(out_token, passphrase, bytes)

BOOST_AUTO_TEST_CASE(bip38__create_token_entropy__defaults__expected)
{
    const auto passphrase = "";
    const auto entropy = base16_literal("baadf00dbaadf00d");
    BC_CREATE_TOKEN_ENTROPY(passphrase, entropy);
    BOOST_CHECK_EQUAL(encode_base58(out_token), "passphraseqVHzjNrYRo5G865e1hHL8fwmEKFDiHDW4BdYQfC1vupQtmFEsxjAh5Bwe4taiX");
}

BOOST_AUTO_TEST_CASE(bip38__create_token_entropy__passphrase__expected)
{
    const auto passphrase = "passphrase";
    const auto entropy = base16_literal("baadf00dbaadf00d");
    BC_CREATE_TOKEN_ENTROPY(passphrase, entropy);
    BOOST_CHECK_EQUAL(encode_base58(out_token), "passphraseqVHzjNrYRo5G7SWTZ5WCjnDFtzLHYWKXLgo7Z7DuBcWNnP9y4ciffZwbdyYadM");
}

BOOST_AUTO_TEST_SUITE_END()

// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(bip38__create_key_pair)

// TODO: create compressed vector(s).

#define BC_REQUIRE_CREATE_KEY_PAIR(token, seed, version, compressed) \
    ec_point out_point; \
    private_key out_private; \
    BOOST_REQUIRE(create_key_pair(out_private, out_point, token, seed, version, compressed))

BOOST_AUTO_TEST_CASE(bip38__create_key_pair__bad_checksum__false)
{
    const auto seed = base16_literal("d36d8e703d8bd5445044178f69087657fba73d9f3ff211f7");
    const auto token = base58_literal("passphraseo59BauW85etaRsKpbbTrEa5RRYw6bq5K9yrDf4r4N5fcirPdtDKmfJw9oYNoGN");
    ec_point out_point;
    private_key out_private;
    BOOST_REQUIRE(!create_key_pair(out_private, out_point, token, seed, 0, false));
}

// generated and verified using bit2factor.com, no lot/sequence
BOOST_AUTO_TEST_CASE(bip38__create_key_pair__vector_8__expected)
{
    const auto seed = base16_literal("d36d8e703d8bd5445044178f69087657fba73d9f3ff211f7");
    const auto token = base58_literal("passphraseo59BauW85etaRsKpbbTrEa5RRYw6bq5K9yrDf4r4N5fcirPdtDKmfJw9oYNoGM");
    BC_REQUIRE_CREATE_KEY_PAIR(token, seed, 0, false);
    BOOST_CHECK_EQUAL(encode_base58(out_private), "6PfPAw5HErFdzMyBvGMwSfSWjKmzgm3jDg7RxQyVCSSBJFZLAZ6hVupmpn");
    BOOST_CHECK_EQUAL(encode_base16(out_point), "04c13b65302bbbed4f7ad67bc68e928b58e7748d84091a2d42680dc52e7916079e103bd025079e984fb4439177224e48a2d9da5768d9b886d89d22c714169723a6");
}

// generated and verified using bit2factor.com, no lot/sequence
BOOST_AUTO_TEST_CASE(bip38__create_key_pair__vector_9__expected)
{
    const auto seed = base16_literal("bbeac8b9bb39381520b6873553544b387bcaa19112602230");
    const auto token = base58_literal("passphraseouGLY8yjTZQ5Q2bTo8rtKfdbHz4tme7QuPheRgES8KnT6pX5yxFauYhv3SVPDD");
    BC_REQUIRE_CREATE_KEY_PAIR(token, seed, 0, false);
    BOOST_CHECK_EQUAL(encode_base58(out_private), "6PfU2yS6DUHjgH8wmsJRT1rHWXRofmDV5UJ3dypocew56BDcw5TQJXFYfm");
    BOOST_CHECK_EQUAL(encode_base16(out_point), "04c3b28a224e38af4219cd782653250d2e4b67ed85ac342201f8f05ff909efdc52858af96a727252a99c54e871ff7bcf9b53cb74e4da1e15d9e83625e3c91222c0");
}

// generated and verified using bit2factor.com, no lot/sequence
BOOST_AUTO_TEST_CASE(bip38__create_key_pair__vector_9_compressed__expected)
{
    const auto seed = base16_literal("bbeac8b9bb39381520b6873553544b387bcaa19112602230");
    const auto token = base58_literal("passphraseouGLY8yjTZQ5Q2bTo8rtKfdbHz4tme7QuPheRgES8KnT6pX5yxFauYhv3SVPDD");
    BC_REQUIRE_CREATE_KEY_PAIR(token, seed, 0, true);
    BOOST_CHECK_EQUAL(encode_base58(out_private), "6PnQ4ihgH1pxeUWa1SDPZ4xToaTdLtjebd8Qw6KJf8xDCW67ssaAqWuJkw");
    BOOST_CHECK_EQUAL(encode_base16(out_point), "02c3b28a224e38af4219cd782653250d2e4b67ed85ac342201f8f05ff909efdc52");
}

// Altchan vectors are based on preliminary bidirectional mapping proposal.
BOOST_AUTO_TEST_CASE(bip38__create_key_pair__vector_9_compressed_testnet__expected)
{
    const auto seed = base16_literal("bbeac8b9bb39381520b6873553544b387bcaa19112602230");
    const auto token = base58_literal("passphraseouGLY8yjTZQ5Q2bTo8rtKfdbHz4tme7QuPheRgES8KnT6pX5yxFauYhv3SVPDD");
    BC_REQUIRE_CREATE_KEY_PAIR(token, seed, 111, true);
    BOOST_CHECK_EQUAL(encode_base58(out_private), "9CNPN5U7RUnisUEm4w5g8WEuV2MBoQEbjxyw7NbDAPdFjDuiKRze4ztPrmA");
    BOOST_CHECK_EQUAL(encode_base16(out_point), "02c3b28a224e38af4219cd782653250d2e4b67ed85ac342201f8f05ff909efdc52");
}

BOOST_AUTO_TEST_SUITE_END()

// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(bip38__create_key_pair_with_confirmation)

// TODO: create compressed vector(s).

#define BC_REQUIRE_CREATE_KEY_PAIR_CONFIRMATION(token, seed, version, compressed) \
    ec_point out_point; \
    public_key out_public; \
    private_key out_private; \
    BOOST_REQUIRE(create_key_pair(out_private, out_public, out_point, token, seed, version, compressed))

BOOST_AUTO_TEST_CASE(bip38__create_key_pair_with_confirmation__bad_checksum__false)
{
    const auto seed = base16_literal("d36d8e703d8bd5445044178f69087657fba73d9f3ff211f7");
    const auto token = base58_literal("passphraseo59BauW85etaRsKpbbTrEa5RRYw6bq5K9yrDf4r4N5fcirPdtDKmfJw9oYNoGN");
    ec_point out_point;
    public_key out_public;
    private_key out_private;
    BOOST_REQUIRE(!create_key_pair(out_private, out_public, out_point, token, seed, 0, false));
}

// generated and verified using bit2factor.com, no lot/sequence
BOOST_AUTO_TEST_CASE(bip38__create_key_pair_with_confirmation__vector_8__expected)
{
    const auto seed = base16_literal("d36d8e703d8bd5445044178f69087657fba73d9f3ff211f7");
    const auto token = base58_literal("passphraseo59BauW85etaRsKpbbTrEa5RRYw6bq5K9yrDf4r4N5fcirPdtDKmfJw9oYNoGM");
    BC_REQUIRE_CREATE_KEY_PAIR_CONFIRMATION(token, seed, 0, false);
    BOOST_CHECK_EQUAL(encode_base58(out_private), "6PfPAw5HErFdzMyBvGMwSfSWjKmzgm3jDg7RxQyVCSSBJFZLAZ6hVupmpn");
    BOOST_CHECK_EQUAL(encode_base58(out_public), "cfrm38V5Nm1mn7GxPBAGTXawqXRwE1EbR19GqsvJ9JmF5VKLqi8nETmULpELkQvExCGkTNCH2An");
    BOOST_CHECK_EQUAL(encode_base16(out_point), "04c13b65302bbbed4f7ad67bc68e928b58e7748d84091a2d42680dc52e7916079e103bd025079e984fb4439177224e48a2d9da5768d9b886d89d22c714169723a6");
}

// generated and verified using bit2factor.com, no lot/sequence
BOOST_AUTO_TEST_CASE(bip38__create_key_pair_with_confirmation__vector_9__expected)
{
    const auto seed = base16_literal("bbeac8b9bb39381520b6873553544b387bcaa19112602230");
    const auto token = base58_literal("passphraseouGLY8yjTZQ5Q2bTo8rtKfdbHz4tme7QuPheRgES8KnT6pX5yxFauYhv3SVPDD");
    BC_REQUIRE_CREATE_KEY_PAIR_CONFIRMATION(token, seed, 0, false);
    BOOST_CHECK_EQUAL(encode_base58(out_private), "6PfU2yS6DUHjgH8wmsJRT1rHWXRofmDV5UJ3dypocew56BDcw5TQJXFYfm");
    BOOST_CHECK_EQUAL(encode_base58(out_public), "cfrm38V5ec4E5RKwBu46Jf5zfaE54nuB1NWHpHSpgX4GQqfzx7fvqm43mBHvr89pPgykDHts9VC");
    BOOST_CHECK_EQUAL(encode_base16(out_point), "04c3b28a224e38af4219cd782653250d2e4b67ed85ac342201f8f05ff909efdc52858af96a727252a99c54e871ff7bcf9b53cb74e4da1e15d9e83625e3c91222c0");
}

// generated and verified using bit2factor.com, no lot/sequence
BOOST_AUTO_TEST_CASE(bip38__create_key_pair_with_confirmation__vector_9_compressed__expected)
{
    const auto seed = base16_literal("bbeac8b9bb39381520b6873553544b387bcaa19112602230");
    const auto token = base58_literal("passphraseouGLY8yjTZQ5Q2bTo8rtKfdbHz4tme7QuPheRgES8KnT6pX5yxFauYhv3SVPDD");
    BC_REQUIRE_CREATE_KEY_PAIR_CONFIRMATION(token, seed, 0, true);
    BOOST_CHECK_EQUAL(encode_base58(out_private), "6PnQ4ihgH1pxeUWa1SDPZ4xToaTdLtjebd8Qw6KJf8xDCW67ssaAqWuJkw");
    BOOST_CHECK_EQUAL(encode_base58(out_public), "cfrm38VUEdzHWKfUjdNjV22wyFNGgtRHYhXdBFT7fWw7cCJbCobryAYUThq4BbTPP15g4SeBsug");
    BOOST_CHECK_EQUAL(encode_base16(out_point), "02c3b28a224e38af4219cd782653250d2e4b67ed85ac342201f8f05ff909efdc52");
}

// Altchan vectors are based on preliminary bidirectional mapping proposal.
BOOST_AUTO_TEST_CASE(bip38__create_key_pair_with_confirmation__vector_9_compressed_testnet__expected)
{
    const auto seed = base16_literal("bbeac8b9bb39381520b6873553544b387bcaa19112602230");
    const auto token = base58_literal("passphraseouGLY8yjTZQ5Q2bTo8rtKfdbHz4tme7QuPheRgES8KnT6pX5yxFauYhv3SVPDD");
    BC_REQUIRE_CREATE_KEY_PAIR_CONFIRMATION(token, seed, 111, true);
    BOOST_CHECK_EQUAL(encode_base58(out_private), "9CNPN5U7RUnisUEm4w5g8WEuV2MBoQEbjxyw7NbDAPdFjDuiKRze4ztPrmA");
    BOOST_CHECK_EQUAL(encode_base58(out_public), "gauEDoSi7eYQiggVLBbkibgy2HgezVNRQBD2FJ69edWqCjgsDJTAFySGK88RZVnv6zYBNe1mU6y");
    BOOST_CHECK_EQUAL(encode_base16(out_point), "02c3b28a224e38af4219cd782653250d2e4b67ed85ac342201f8f05ff909efdc52");
}

BOOST_AUTO_TEST_SUITE_END()

#ifdef WITH_ICU

// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(bip38__encrypt_private)

#define BC_REQUIRE_ENCRYPT(secret, passphrase, version, compressed, expected) \
    private_key out_private; \
    encrypt(out_private, secret, passphrase, version, compressed); \
    BOOST_REQUIRE_EQUAL(encode_base58(out_private), expected)

// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#no-compression-no-ec-multiply
BOOST_AUTO_TEST_CASE(bip38__encrypt_private__vector_0__expected)
{
    const auto expected = "6PRVWUbkzzsbcVac2qwfssoUJAN1Xhrg6bNk8J7Nzm5H7kxEbn2Nh2ZoGg";
    const auto secret = base16_literal("cbf4b9f70470856bb4f40f80b87edb90865997ffee6df315ab166d713af433a5");
    BC_REQUIRE_ENCRYPT(secret, "TestingOneTwoThree", 0, false, expected);
}

// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#no-compression-no-ec-multiply
BOOST_AUTO_TEST_CASE(bip38__encrypt_private__vector_1__expected)
{
    const auto expected = "6PRNFFkZc2NZ6dJqFfhRoFNMR9Lnyj7dYGrzdgXXVMXcxoKTePPX1dWByq";
    const auto secret = base16_literal("09c2686880095b1a4c249ee3ac4eea8a014f11e6f986d0b5025ac1f39afbd9ae");
    BC_REQUIRE_ENCRYPT(secret, "Satoshi", 0, false, expected);
}

// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#compression-no-ec-multiply
BOOST_AUTO_TEST_CASE(bip38__encrypt_private__vector_2_compressed__expected)
{
    const auto expected = "6PYNKZ1EAgYgmQfmNVamxyXVWHzK5s6DGhwP4J5o44cvXdoY7sRzhtpUeo";
    const auto secret = base16_literal("cbf4b9f70470856bb4f40f80b87edb90865997ffee6df315ab166d713af433a5");
    BC_REQUIRE_ENCRYPT(secret, "TestingOneTwoThree", 0, true, expected);
}

// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#compression-no-ec-multiply
BOOST_AUTO_TEST_CASE(bip38__encrypt_private__vector_3_compressed__expected)
{
    const auto expected = "6PYLtMnXvfG3oJde97zRyLYFZCYizPU5T3LwgdYJz1fRhh16bU7u6PPmY7";
    const auto secret = base16_literal("09c2686880095b1a4c249ee3ac4eea8a014f11e6f986d0b5025ac1f39afbd9ae");
    BC_REQUIRE_ENCRYPT(secret, "Satoshi", 0, true, expected);
}

// #3 from: github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#no-compression-no-ec-multiply
BOOST_AUTO_TEST_CASE(bip38__encrypt_private__vector_unicode___expected)
{
    const auto encoded_password = base16_literal("cf92cc8100f0909080f09f92a9");
    std::string passphrase(encoded_password.begin(), encoded_password.end());
    const auto expected = "6PRW5o9FLp4gJDDVqJQKJFTpMvdsSGJxMYHtHaQBF3ooa8mwD69bapcDQn";
    const auto secret = base16_literal("64eeab5f9be2a01a8365a579511eb3373c87c40da6d2a25f05bda68fe077b66e");
    BC_REQUIRE_ENCRYPT(secret, passphrase, 0, false, expected);
}

BOOST_AUTO_TEST_SUITE_END()

// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(bip38__decrypt1)

// TODO: create compressed+multiplied and altchain/testnet vector(s).

#define BC_REQUIRE_DECRYPT_SECRET(key, passphrase) \
    ec_secret out_secret; \
    uint8_t out_version; \
    bool out_compressed; \
    BOOST_REQUIRE(decrypt(out_secret, out_version, out_compressed, key, passphrase))

// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#no-compression-no-ec-multiply
BOOST_AUTO_TEST_CASE(bip38__decrypt_private__vector_0__expected)
{
    const auto key = base58_literal("6PRVWUbkzzsbcVac2qwfssoUJAN1Xhrg6bNk8J7Nzm5H7kxEbn2Nh2ZoGg");
    BC_REQUIRE_DECRYPT_SECRET(key, "TestingOneTwoThree");
    BOOST_CHECK_EQUAL(encode_base16(out_secret), "cbf4b9f70470856bb4f40f80b87edb90865997ffee6df315ab166d713af433a5");
    BOOST_CHECK_EQUAL(out_version, 0);
    BOOST_CHECK_EQUAL(out_compressed, false);
}

// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#no-compression-no-ec-multiply
BOOST_AUTO_TEST_CASE(bip38__decrypt_private__vector_1__expected)
{
    const auto key = base58_literal("6PRNFFkZc2NZ6dJqFfhRoFNMR9Lnyj7dYGrzdgXXVMXcxoKTePPX1dWByq");
    BC_REQUIRE_DECRYPT_SECRET(key, "Satoshi");
    BOOST_CHECK_EQUAL(encode_base16(out_secret), "09c2686880095b1a4c249ee3ac4eea8a014f11e6f986d0b5025ac1f39afbd9ae");
    BOOST_CHECK_EQUAL(out_version, 0);
    BOOST_CHECK_EQUAL(out_compressed, false);
}

// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#compression-no-ec-multiply
BOOST_AUTO_TEST_CASE(bip38__decrypt_private__vector_2_compressed__expected)
{
    const auto key = base58_literal("6PYNKZ1EAgYgmQfmNVamxyXVWHzK5s6DGhwP4J5o44cvXdoY7sRzhtpUeo");
    BC_REQUIRE_DECRYPT_SECRET(key, "TestingOneTwoThree");
    BOOST_CHECK_EQUAL(encode_base16(out_secret), "cbf4b9f70470856bb4f40f80b87edb90865997ffee6df315ab166d713af433a5");
    BOOST_CHECK_EQUAL(out_version, 0);
    BOOST_CHECK_EQUAL(out_compressed, true);
}

// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#compression-no-ec-multiply
BOOST_AUTO_TEST_CASE(bip38__decrypt_private__vector_3_compressed__expected)
{
    const auto key = base58_literal("6PYLtMnXvfG3oJde97zRyLYFZCYizPU5T3LwgdYJz1fRhh16bU7u6PPmY7");
    BC_REQUIRE_DECRYPT_SECRET(key, "Satoshi");
    BOOST_CHECK_EQUAL(encode_base16(out_secret), "09c2686880095b1a4c249ee3ac4eea8a014f11e6f986d0b5025ac1f39afbd9ae");
    BOOST_CHECK_EQUAL(out_version, 0);
    BOOST_CHECK_EQUAL(out_compressed, true);
}

// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#no-compression-no-ec-multiply
BOOST_AUTO_TEST_CASE(bip38__decrypt_private__vector_4_multiplied__expected)
{
    const auto key = base58_literal("6PfQu77ygVyJLZjfvMLyhLMQbYnu5uguoJJ4kMCLqWwPEdfpwANVS76gTX");
    BC_REQUIRE_DECRYPT_SECRET(key, "TestingOneTwoThree");
    BOOST_CHECK_EQUAL(encode_base16(out_secret), "a43a940577f4e97f5c4d39eb14ff083a98187c64ea7c99ef7ce460833959a519");
    BOOST_CHECK_EQUAL(out_version, 0);
    BOOST_CHECK_EQUAL(out_compressed, false);
}

// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#no-compression-no-ec-multiply
BOOST_AUTO_TEST_CASE(bip38__decrypt_private__vector_5_multiplied__expected)
{
    const auto key = base58_literal("6PfLGnQs6VZnrNpmVKfjotbnQuaJK4KZoPFrAjx1JMJUa1Ft8gnf5WxfKd");
    BC_REQUIRE_DECRYPT_SECRET(key, "Satoshi");
    BOOST_CHECK_EQUAL(encode_base16(out_secret), "c2c8036df268f498099350718c4a3ef3984d2be84618c2650f5171dcc5eb660a");
    BOOST_CHECK_EQUAL(out_version, 0);
    BOOST_CHECK_EQUAL(out_compressed, false);
}

// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#ec-multiply-no-compression-lotsequence-numbers
BOOST_AUTO_TEST_CASE(bip38__decrypt_private__vector_6_multiplied_lot__expected)
{
    const auto key = base58_literal("6PgNBNNzDkKdhkT6uJntUXwwzQV8Rr2tZcbkDcuC9DZRsS6AtHts4Ypo1j");
    BC_REQUIRE_DECRYPT_SECRET(key, "MOLON LABE");
    BOOST_CHECK_EQUAL(encode_base16(out_secret), "44ea95afbf138356a05ea32110dfd627232d0f2991ad221187be356f19fa8190");
    BOOST_CHECK_EQUAL(out_version, 0);
    BOOST_CHECK_EQUAL(out_compressed, false);
}

// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#ec-multiply-no-compression-lotsequence-numbers
BOOST_AUTO_TEST_CASE(bip38__decrypt_private__vector_7_multiplied_lot__expected)
{
    const auto key = base58_literal("6PgGWtx25kUg8QWvwuJAgorN6k9FbE25rv5dMRwu5SKMnfpfVe5mar2ngH");
    BC_REQUIRE_DECRYPT_SECRET(key, "ΜΟΛΩΝ ΛΑΒΕ");
    BOOST_CHECK_EQUAL(encode_base16(out_secret), "ca2759aa4adb0f96c414f36abeb8db59342985be9fa50faac228c8e7d90e3006");
    BOOST_CHECK_EQUAL(out_version, 0);
    BOOST_CHECK_EQUAL(out_compressed, false);
}

// generated and verified using bit2factor.com, no lot/sequence
BOOST_AUTO_TEST_CASE(bip38__decrypt_private__vector_8_multiplied__expected)
{
    const auto key = base58_literal("6PfPAw5HErFdzMyBvGMwSfSWjKmzgm3jDg7RxQyVCSSBJFZLAZ6hVupmpn");
    BC_REQUIRE_DECRYPT_SECRET(key, "libbitcoin test");
    BOOST_CHECK_EQUAL(encode_base16(out_secret), "fb4bfb0bfe151d524b0b11983b9f826d6a0bc7f7bdc480864a1b557ff0c59eb4");
    BOOST_CHECK_EQUAL(out_version, 0);
    BOOST_CHECK_EQUAL(out_compressed, false);
}

// generated and verified using bit2factor.com, no lot/sequence
BOOST_AUTO_TEST_CASE(bip38__decrypt_private__vector_9_multiplied__expected)
{
    const auto key = base58_literal("6PfU2yS6DUHjgH8wmsJRT1rHWXRofmDV5UJ3dypocew56BDcw5TQJXFYfm");
    BC_REQUIRE_DECRYPT_SECRET(key, "Libbitcoin BIP38 Test Vector");
    BOOST_CHECK_EQUAL(encode_base16(out_secret), "97c745cc980e5a070e12d0bff3f539b70748aadb406045fc1b42d4ded559a564");
    BOOST_CHECK_EQUAL(out_version, 0);
    BOOST_CHECK_EQUAL(out_compressed, false);
}

BOOST_AUTO_TEST_SUITE_END()

// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(bip38__decrypt_public)

// TODO: create compressed and altchain/testnet vector(s).

#define BC_REQUIRE_DECRYPT_POINT(key, passphrase, version) \
    ec_point out_point; \
    uint8_t out_version; \
    BOOST_REQUIRE(decrypt(out_point, out_version, key, passphrase)); \
    BOOST_CHECK_EQUAL(out_version, version); \
    const auto derived_address = payment_address(version, out_point).to_string()

// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#ec-multiply-no-compression-lotsequence-numbers
BOOST_AUTO_TEST_CASE(bip38__decrypt_public__vector_6_lot__expected)
{
    const auto key = base58_literal("cfrm38V8aXBn7JWA1ESmFMUn6erxeBGZGAxJPY4e36S9QWkzZKtaVqLNMgnifETYw7BPwWC9aPD");
    BC_REQUIRE_DECRYPT_POINT(key, "MOLON LABE", 0);
    BOOST_CHECK_EQUAL(encode_base16(out_point), "04e20f3b812f630d0374eefe776e983549d5cdde87780be45cb9def7ee339dfed47dd64392855f31a7d1beb48b6a643e997e49168f883107ada1d321fed28c061b");
    BOOST_CHECK_EQUAL(derived_address, "1Jscj8ALrYu2y9TD8NrpvDBugPedmbj4Yh");
}

// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#ec-multiply-no-compression-lotsequence-numbers
BOOST_AUTO_TEST_CASE(bip38__decrypt_public__vector_7_lot__expected)
{
    const auto key = base58_literal("cfrm38V8G4qq2ywYEFfWLD5Cc6msj9UwsG2Mj4Z6QdGJAFQpdatZLavkgRd1i4iBMdRngDqDs51");
    BC_REQUIRE_DECRYPT_POINT(key, "ΜΟΛΩΝ ΛΑΒΕ", 0);
    BOOST_CHECK_EQUAL(encode_base16(out_point), "0415fb4e4e62fcec936920dbda69e83facfe2cc5e152fafcf474c8fa0dcf5023f39392712103c03b3c79ae871134ba3cfeafd98787614e52e54c59888c23a3d1c4");
    BOOST_CHECK_EQUAL(derived_address, "1Lurmih3KruL4xDB5FmHof38yawNtP9oGf");
}

// generated and verified using bit2factor.com, no lot/sequence
BOOST_AUTO_TEST_CASE(bip38__decrypt_public__vector_8__expected)
{
    const auto key = base58_literal("cfrm38V5Nm1mn7GxPBAGTXawqXRwE1EbR19GqsvJ9JmF5VKLqi8nETmULpELkQvExCGkTNCH2An");
    BC_REQUIRE_DECRYPT_POINT(key, "libbitcoin test", 0);
    BOOST_CHECK_EQUAL(encode_base16(out_point), "04c13b65302bbbed4f7ad67bc68e928b58e7748d84091a2d42680dc52e7916079e103bd025079e984fb4439177224e48a2d9da5768d9b886d89d22c714169723a6");
    BOOST_CHECK_EQUAL(derived_address, "1NQgLnFz1ZzF6KkCJ4SM3xz3Jy1q2hEEax");
}

// generated and verified using bit2factor.com, no lot/sequence
BOOST_AUTO_TEST_CASE(bip38__decrypt_public__vector_9__expected)
{
    const auto key = base58_literal("cfrm38V5ec4E5RKwBu46Jf5zfaE54nuB1NWHpHSpgX4GQqfzx7fvqm43mBHvr89pPgykDHts9VC");
    BC_REQUIRE_DECRYPT_POINT(key, "Libbitcoin BIP38 Test Vector", 0);
    BOOST_CHECK_EQUAL(encode_base16(out_point), "04c3b28a224e38af4219cd782653250d2e4b67ed85ac342201f8f05ff909efdc52858af96a727252a99c54e871ff7bcf9b53cb74e4da1e15d9e83625e3c91222c0");
    BOOST_CHECK_EQUAL(derived_address, "1NjjvGqXDrx1DvrhjYJxzrpyopk1ygHTSJ");
}

BOOST_AUTO_TEST_SUITE_END()

// ----------------------------------------------------------------------------

#endif // WITH_ICU

BOOST_AUTO_TEST_SUITE_END()

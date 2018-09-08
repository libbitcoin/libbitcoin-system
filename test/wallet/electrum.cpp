/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin.hpp>

using namespace bc;
using namespace bc::wallet;

BOOST_AUTO_TEST_SUITE(electrum_tests)

// These test cases were generated using electrum code and verified to
// match output from:
//
// https://github.com/spesmilo/electrum/blob/master/lib/mnemonic.py

BOOST_AUTO_TEST_CASE(electrum__decode_mnemonic__no_passphrase_1__valid)
{
    static const word_list mnemonic{ "foo", "bar", "baz" };
    const auto seed = electrum::decode_mnemonic(mnemonic);
    BOOST_REQUIRE_EQUAL(encode_base16(seed), "c4033901dd61ba26cfd0a1cf1ceb4b347606635aa3cb951eb6e819d58beedc04dd400a2e600d783c83c75879d6538abeecc7bb1b292b2a4d775d348d5d686427");
}

BOOST_AUTO_TEST_CASE(electrum__decode_mnemonic__no_passphrase_2__valid)
{
    static const word_list mnemonic
    {
        "giggle", "crush", "argue", "inflict", "wear", "defy", "combine", "evolve", "tiger", "spatial", "crumble", "fury"
    };
    const auto seed = electrum::decode_mnemonic(mnemonic);
    BOOST_REQUIRE_EQUAL(encode_base16(seed), "eb095e70f0eb8d24f2cef276c75b136c2ff41eb72a9291eb22ba45d11fbb875a965bd6ae09873543eea98bf3ae05067203cd56388e0978aebc3a511932704d80");
}

#ifdef WITH_ICU

BOOST_AUTO_TEST_CASE(electrum__decode_mnemonic__passphrase__valid)
{
    static const word_list mnemonic{ "foobar" };
    const auto passphrase = "none";
    const auto seed = electrum::decode_mnemonic(mnemonic, passphrase);
    BOOST_REQUIRE_EQUAL(encode_base16(seed), "741b72fd15effece6bfe5a26a52184f66811bd2be363190e07a42cca442b1a5bb22b3ad0eb338197287e6d314866c7fba863ac65d3f156087a5052ebc7157fce");
}

BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__no_dictionary__valid)
{
    data_chunk entropy;
    decode_base16(entropy, "05e669b4270f4e25bce6fc3736170d423c");
    const auto mnemonic = electrum::create_mnemonic(entropy);
    BOOST_REQUIRE(!mnemonic.empty());
    BOOST_REQUIRE_EQUAL(join(mnemonic), "giggle crush argue inflict wear defy combine evolve tiger spatial crumble fury");
    BOOST_REQUIRE(electrum::validate_mnemonic(mnemonic));
}

BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__en_dictionary__valid)
{
    data_chunk entropy;
    decode_base16(entropy, "05e669b4270f4e25bce6fc3736170d423c");
    const auto mnemonic = electrum::create_mnemonic(entropy, language::en);
    BOOST_REQUIRE(!mnemonic.empty());
    BOOST_REQUIRE_EQUAL(join(mnemonic), "giggle crush argue inflict wear defy combine evolve tiger spatial crumble fury");
    BOOST_REQUIRE(electrum::validate_mnemonic(mnemonic, language::en));
}

BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__en_dictionary_prefix__valid)
{
    data_chunk entropy;
    decode_base16(entropy, "0b0d80f992a51348a89aeb5196fa0bd0d6");
    const auto mnemonic = electrum::create_mnemonic(entropy, language::en, electrum::seed::standard);
    BOOST_REQUIRE(!mnemonic.empty());
    BOOST_REQUIRE_EQUAL(join(mnemonic), "crawl consider laptop bonus stove chase earn battle feed town scatter radio");
    BOOST_REQUIRE(electrum::validate_mnemonic(mnemonic, language::en, electrum::seed::standard));
}

BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__es_dictionary_prefix__valid)
{
    data_chunk entropy;
    decode_base16(entropy, "05e669b4270f4e25bce6fc3736170d423c");
    const auto mnemonic = electrum::create_mnemonic(entropy, language::es, electrum::seed::standard);
    BOOST_REQUIRE(!mnemonic.empty());
    BOOST_REQUIRE_EQUAL(join(mnemonic), "gigante codo ámbar insecto verbo cráter celoso entrar tarjeta sala coco frito");
    BOOST_REQUIRE(electrum::validate_mnemonic(mnemonic, language::es, electrum::seed::standard));
}

BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__large_entropy_1__valid)
{
    data_chunk entropy;
    decode_base16(entropy, "2b18cf24d3c26529b18d733abadff49f06e65ebf74263f0ba00d11a9b0cba8d303");
    const auto mnemonic = electrum::create_mnemonic(entropy);
    BOOST_REQUIRE(!mnemonic.empty());
    BOOST_REQUIRE_EQUAL(join(mnemonic), "icon person grape only cash addict fringe disease luggage worry consider hover dignity wood street deny purpose shiver network chaos pole since shoe climb");
    BOOST_REQUIRE(electrum::validate_mnemonic(mnemonic, language::en, electrum::seed::standard));
}

BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__large_entropy_2__valid)
{
    data_chunk entropy;
    decode_base16(entropy, "b0756302179e800b182514c729f1d6814c377ff06097569ef540e6c1f1950f08");
    const auto mnemonic = electrum::create_mnemonic(entropy);
    BOOST_REQUIRE(!mnemonic.empty());
    BOOST_REQUIRE_EQUAL(join(mnemonic), "divide february web hire limb run reject nuclear army zone brick below public ladder deer below again cluster divorce ketchup aerobic flee lonely absent");
    BOOST_REQUIRE(electrum::validate_mnemonic(mnemonic, language::en, electrum::seed::standard));
}

BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__incorrent_mnemonic__invalid)
{
    data_chunk entropy;
    decode_base16(entropy, "2b18cf24d3c26529b18d733abadff49f06e65ebf74263f0ba00d11a9b0cba8d303");
    auto mnemonic = electrum::create_mnemonic(entropy);
    BOOST_REQUIRE(!mnemonic.empty());
    mnemonic[0] = "invalid";
    BOOST_REQUIRE(!electrum::validate_mnemonic(mnemonic, language::en, electrum::seed::standard));
}

#endif

BOOST_AUTO_TEST_SUITE_END()

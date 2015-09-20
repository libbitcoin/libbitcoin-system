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
using namespace bc::wallet;

BOOST_AUTO_TEST_SUITE(wif_keys_tests)

// TODO: add version tests

#define SECRET "8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536"
#define WIF_COMPRESSED "L1WepftUBemj6H4XQovkiW1ARVjxMqaw4oj2kmkYqdG1xTnBcHfC"
#define WIF_UNCOMPRESSED "5JngqQmHagNTknnCshzVUysLMWAjT23FWs1TgNU5wyFH5SB3hrP"

// TODO: implement testnet version tests
//#define WIF_COMPRESSED_TESTNET "cRseHatKciTzFiXnoDjt5pWE3j3N2Hgd8qsVsCD4Ljv2DCwuD1V6"
//#define WIF_UNCOMPRESSED_TESTNET "92ZKR9aqAuSbirHVW3tQMaRJ1AXScBaSrosQkzpbHhzKrVBsZBL"

BOOST_AUTO_TEST_CASE(wif_keys__is_wif_compressed__positive__test)
{
    BOOST_REQUIRE(is_wif_compressed(WIF_COMPRESSED));
}

BOOST_AUTO_TEST_CASE(wif_keys__is_wif_compressed__negative__test)
{
    BOOST_REQUIRE(!is_wif_compressed(WIF_UNCOMPRESSED));
}

BOOST_AUTO_TEST_CASE(wif_keys__encode_wif__compressed__test)
{
    const auto compressed = true;
    const uint8_t version = 0x80;
    const auto secret = base16_literal(SECRET);
    BOOST_REQUIRE_EQUAL(encode_wif(secret, version, compressed), WIF_COMPRESSED);
}

BOOST_AUTO_TEST_CASE(wif_keys__encode_wif__uncompressed__test)
{
    const auto compressed = false;
    const uint8_t version = 0x80;
    ec_secret secret = base16_literal(SECRET);
    BOOST_REQUIRE_EQUAL(encode_wif(secret, version, compressed), WIF_UNCOMPRESSED);
}

BOOST_AUTO_TEST_CASE(wif_keys__decode_wif__compressed__test)
{
    const auto compressed = true;
    const uint8_t version = 0x80;
    bool out_compressed;
    uint8_t out_version;
    ec_secret out_secret;
    BOOST_REQUIRE(decode_wif(out_secret, out_version, out_compressed, WIF_COMPRESSED));
    BOOST_REQUIRE_EQUAL(encode_base16(out_secret), SECRET);
    BOOST_REQUIRE_EQUAL(out_version, version);
    BOOST_REQUIRE(out_compressed);
}

BOOST_AUTO_TEST_CASE(wif_keys__decode_wif__uncompressed__test)
{
    const auto compressed = false;
    const uint8_t version = 0x80;
    bool out_compressed;
    uint8_t out_version;
    ec_secret out_secret;
    BOOST_REQUIRE(decode_wif(out_secret, out_version, out_compressed, WIF_UNCOMPRESSED));
    BOOST_REQUIRE_EQUAL(encode_base16(out_secret), SECRET);
    BOOST_REQUIRE_EQUAL(out_version, version);
    BOOST_REQUIRE(!out_compressed);
}

BOOST_AUTO_TEST_SUITE_END()

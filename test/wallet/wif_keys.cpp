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

BOOST_AUTO_TEST_SUITE(wif_tests)

static const ec_secret secret =
{{
    0x80, 0x10, 0xB1, 0xBB, 0x11, 0x9A, 0xD3, 0x7D,
    0x4B, 0x65, 0xA1, 0x02, 0x2A, 0x31, 0x48, 0x97,
    0xB1, 0xB3, 0x61, 0x4B, 0x34, 0x59, 0x74, 0x33,
    0x2C, 0xB1, 0xB9, 0x58, 0x2C, 0xF0, 0x35, 0x36
}};

#ifdef ENABLE_TESTNET
    static const char wif_compressed[] =
        "cRseHatKciTzFiXnoDjt5pWE3j3N2Hgd8qsVsCD4Ljv2DCwuD1V6";
    static const char wif_uncompressed[] =
        "92ZKR9aqAuSbirHVW3tQMaRJ1AXScBaSrosQkzpbHhzKrVBsZBL";
#else
    static const char wif_compressed[] =
        "L1WepftUBemj6H4XQovkiW1ARVjxMqaw4oj2kmkYqdG1xTnBcHfC";
    static const char wif_uncompressed[] =
        "5JngqQmHagNTknnCshzVUysLMWAjT23FWs1TgNU5wyFH5SB3hrP";
#endif

BOOST_AUTO_TEST_CASE(is_wif_compressed_test)
{
    BOOST_REQUIRE(wallet::is_wif_compressed(wif_compressed));
    BOOST_REQUIRE(!wallet::is_wif_compressed(wif_uncompressed));
}

BOOST_AUTO_TEST_CASE(wif_to_secret_test)
{
    BOOST_REQUIRE(secret == wallet::wif_to_secret(wif_compressed));
    BOOST_REQUIRE(secret == wallet::wif_to_secret(wif_uncompressed));
}

BOOST_AUTO_TEST_CASE(secret_to_wif_test)
{
    BOOST_REQUIRE_EQUAL(wallet::secret_to_wif(secret, true),
        std::string(wif_compressed));
    BOOST_REQUIRE_EQUAL(wallet::secret_to_wif(secret, false),
        std::string(wif_uncompressed));
}

BOOST_AUTO_TEST_SUITE_END()

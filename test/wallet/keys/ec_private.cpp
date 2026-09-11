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

BOOST_AUTO_TEST_SUITE(ec_private_tests)

using namespace bc::system::wallet;

#define SECRET "8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536"
#define WIF_COMPRESSED "L1WepftUBemj6H4XQovkiW1ARVjxMqaw4oj2kmkYqdG1xTnBcHfC"
#define WIF_UNCOMPRESSED "5JngqQmHagNTknnCshzVUysLMWAjT23FWs1TgNU5wyFH5SB3hrP"

#define WIF_COMPRESSED_TESTNET "cRseHatKciTzFiXnoDjt5pWE3j3N2Hgd8qsVsCD4Ljv2DCwuD1V6"
#define WIF_UNCOMPRESSED_TESTNET "92ZKR9aqAuSbirHVW3tQMaRJ1AXScBaSrosQkzpbHhzKrVBsZBL"

BOOST_AUTO_TEST_CASE(ec_private__seed__empty__valid)
{
    const data_chunk empty_seed;
    BOOST_REQUIRE(ec_private(empty_seed));
}

BOOST_AUTO_TEST_CASE(ec_private__compressed_wif__compressed__true)
{
    BOOST_REQUIRE(ec_private(WIF_COMPRESSED).compressed());
}

BOOST_AUTO_TEST_CASE(ec_private__uncompressed_wif__not_compressed__true)
{
    BOOST_REQUIRE(!ec_private(WIF_UNCOMPRESSED).compressed());
}

BOOST_AUTO_TEST_CASE(ec_private__encode_wif__compressed__expected)
{
    BOOST_REQUIRE_EQUAL(ec_private(base16_array(SECRET)).encoded(), WIF_COMPRESSED);
}

BOOST_AUTO_TEST_CASE(ec_private__encode_wif__uncompressed__expected)
{
    BOOST_REQUIRE_EQUAL(ec_private(base16_array(SECRET), 0x8000, false).encoded(), WIF_UNCOMPRESSED);
}

BOOST_AUTO_TEST_CASE(ec_private__decode_wif__compressed__true)
{
    const ec_private secret(WIF_COMPRESSED);
    BOOST_REQUIRE_EQUAL(encode_base16(secret.secret()), SECRET);
    BOOST_REQUIRE_EQUAL(secret.versions(), 0x8000);
    BOOST_REQUIRE(secret.compressed());
}

BOOST_AUTO_TEST_CASE(ec_private__decode_wif__uncompressed__false)
{
    const ec_private secret(WIF_UNCOMPRESSED);
    BOOST_REQUIRE_EQUAL(encode_base16(secret.secret()), SECRET);
    BOOST_REQUIRE_EQUAL(secret.versions(), 0x8000);
    BOOST_REQUIRE(!secret.compressed());
}


// Versions.
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(ec_private__wif_version__entropy__mainnet)
{
    const data_chunk empty_seed;
    BOOST_REQUIRE_EQUAL(ec_private(empty_seed).wif_version(), ec_private::mainnet_wif);
}

BOOST_AUTO_TEST_CASE(ec_private__wif_version__scalar__mainnet)
{
    BOOST_REQUIRE_EQUAL(ec_private(ec_scalar(base16_array(SECRET))).wif_version(), ec_private::mainnet_wif);
}

BOOST_AUTO_TEST_CASE(ec_private__encoded__testnet_versions__round_trips)
{
    const ec_private secret(WIF_COMPRESSED_TESTNET, ec_private::testnet);
    BOOST_REQUIRE(secret);
    BOOST_REQUIRE_EQUAL(secret.versions(), ec_private::testnet);
    BOOST_REQUIRE_EQUAL(secret.encoded(), WIF_COMPRESSED_TESTNET);
}

BOOST_AUTO_TEST_CASE(ec_private__encoded__uncompressed_testnet_versions__round_trips)
{
    const ec_private secret(WIF_UNCOMPRESSED_TESTNET, ec_private::testnet);
    BOOST_REQUIRE(secret);
    BOOST_REQUIRE(!secret.compressed());
    BOOST_REQUIRE_EQUAL(secret.encoded(), WIF_UNCOMPRESSED_TESTNET);
}

BOOST_AUTO_TEST_CASE(ec_private__construct__testnet_wif_mainnet_versions__invalid)
{
    BOOST_REQUIRE(!ec_private(WIF_COMPRESSED_TESTNET));
    BOOST_REQUIRE(!ec_private(WIF_UNCOMPRESSED_TESTNET));
}

BOOST_AUTO_TEST_CASE(ec_private__construct__mainnet_wif_testnet_versions__invalid)
{
    BOOST_REQUIRE(!ec_private(WIF_COMPRESSED, ec_private::testnet));
    BOOST_REQUIRE(!ec_private(WIF_UNCOMPRESSED, ec_private::testnet));
}

BOOST_AUTO_TEST_CASE(ec_private__payment_version__testnet_versions__testnet)
{
    const ec_private secret(WIF_COMPRESSED_TESTNET, ec_private::testnet);
    BOOST_REQUIRE_EQUAL(secret.payment_version(), ec_private::testnet_p2kh);
    BOOST_REQUIRE_EQUAL(secret.to_payment_address().prefix(), ec_private::testnet_p2kh);
}

// operators

BOOST_AUTO_TEST_CASE(ec_private__equality__same__equal)
{
    const ec_private left(WIF_COMPRESSED);
    const ec_private right(WIF_COMPRESSED);
    BOOST_REQUIRE(left == right);
    BOOST_REQUIRE(!(left != right));
    BOOST_REQUIRE(!(left < right));
}

BOOST_AUTO_TEST_CASE(ec_private__equality__different_compression__unequal)
{
    const ec_private compressed(WIF_COMPRESSED);
    const ec_private uncompressed(WIF_UNCOMPRESSED);
    BOOST_REQUIRE_EQUAL(compressed.secret(), uncompressed.secret());
    BOOST_REQUIRE(compressed != uncompressed);
}

BOOST_AUTO_TEST_CASE(ec_private__equality__different_versions__unequal)
{
    const ec_private mainnet(base16_array(SECRET), ec_private::mainnet);
    const ec_private testnet(base16_array(SECRET), ec_private::testnet);
    BOOST_REQUIRE_EQUAL(mainnet.secret(), testnet.secret());
    BOOST_REQUIRE(mainnet != testnet);
}

BOOST_AUTO_TEST_CASE(ec_private__stream__valid__round_trips)
{
    std::istringstream in{ WIF_COMPRESSED };
    ec_private instance{};
    in >> instance;
    BOOST_REQUIRE_EQUAL(instance.encoded(), WIF_COMPRESSED);

    std::ostringstream out{};
    out << instance;
    BOOST_REQUIRE_EQUAL(out.str(), WIF_COMPRESSED);
}

BOOST_AUTO_TEST_CASE(ec_private__stream__invalid__throws)
{
    ec_private instance{};
    BOOST_REQUIRE_THROW(std::istringstream("bogus") >> instance, istream_exception);
}

// methods

BOOST_AUTO_TEST_CASE(ec_private__to_public__invalid__invalid)
{
    BOOST_REQUIRE(!ec_private{}.to_public());
}

// Validity is a null test, so a secret above the curve order is valid.
BOOST_AUTO_TEST_CASE(ec_private__to_public__secret_above_order__invalid)
{
    const ec_private instance(base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"), ec_private::mainnet);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(!instance.to_public());
}

BOOST_AUTO_TEST_CASE(ec_private__to_public__compressed_wif__compressed)
{
    const auto point = ec_private(WIF_COMPRESSED).to_public();
    BOOST_REQUIRE(point);
    BOOST_REQUIRE(point.compressed());
}

BOOST_AUTO_TEST_CASE(ec_private__to_public__uncompressed_wif__uncompressed)
{
    const auto point = ec_private(WIF_UNCOMPRESSED).to_public();
    BOOST_REQUIRE(point);
    BOOST_REQUIRE(!point.compressed());
}

// wif

BOOST_AUTO_TEST_CASE(ec_private__construct__wif_bad_checksum__invalid)
{
    const auto wif = base16_array("80" SECRET "01ffffffff");
    BOOST_REQUIRE_EQUAL(wif.size(), wif_compressed_size);
    BOOST_REQUIRE(!ec_private(wif));
}

BOOST_AUTO_TEST_CASE(ec_private__construct__wif_wrong_size__invalid)
{
    BOOST_REQUIRE(!ec_private{ "3" });
}
BOOST_AUTO_TEST_SUITE_END()

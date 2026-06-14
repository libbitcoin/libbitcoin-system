/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(secp256k1_tests)

const ec_secret secret1 = base16_array("8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536");
const ec_compressed compressed1 = base16_array("0309ba8621aefd3b6ba4ca6d11a4746e8df8d35d9b51b383338f627ba7fc732731");
const ec_uncompressed uncompressed1 = base16_array("0409ba8621aefd3b6ba4ca6d11a4746e8df8d35d9b51b383338f627ba7fc7327318c3a6ec6acd33c36328b8fb4349b31671bcd3a192316ea4f6236ee1ae4a7d8c9");

// generator

BOOST_AUTO_TEST_CASE(secp256k1__generator__expected)
{
    const ec_secret one = base16_array("0000000000000000000000000000000000000000000000000000000000000001");

    ec_compressed generator;
    BOOST_REQUIRE(secret_to_public(generator, one));
    BOOST_REQUIRE_EQUAL(generator, ec_compressed_generator);
}

BOOST_AUTO_TEST_CASE(secp256k1__compress__round_trip__expected)
{
    ec_uncompressed uncompressed;
    BOOST_REQUIRE(decompress(uncompressed, compressed1));

    ec_compressed compressed;
    BOOST_REQUIRE(compress(compressed, uncompressed));
    BOOST_REQUIRE_EQUAL(compressed, compressed1);
}

BOOST_AUTO_TEST_CASE(secp256k1__decompress__positive__expected)
{
    ec_uncompressed point;
    BOOST_REQUIRE(decompress(point, compressed1));
    BOOST_REQUIRE_EQUAL(point, uncompressed1);
}

BOOST_AUTO_TEST_CASE(secp256k1__secret_to_public__compressed_positive__expected)
{
    ec_compressed point;
    BOOST_REQUIRE(secret_to_public(point, secret1));
    BOOST_REQUIRE_EQUAL(point, compressed1);
}

BOOST_AUTO_TEST_CASE(secp256k1__secret_to_public__uncompressed_positive__expected)
{
    ec_uncompressed point;
    BOOST_REQUIRE(secret_to_public(point, secret1));
    BOOST_REQUIRE_EQUAL(point, uncompressed1);
}

BOOST_AUTO_TEST_SUITE_END()

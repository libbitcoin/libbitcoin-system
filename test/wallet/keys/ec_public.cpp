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

BOOST_AUTO_TEST_SUITE(ec_public_tests)

using namespace bc::system::wallet;


// The genesis coinbase key, its compressed form and its p2kh address.
#define UNCOMPRESSED "04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f"
#define COMPRESSED "03678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb6"
#define UNCOMPRESSED_ADDRESS "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa"
#define SECRET "8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536"
#define OFF_CURVE "04ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"

// construct

BOOST_AUTO_TEST_CASE(ec_public__construct__default__invalid)
{
    BOOST_REQUIRE(!ec_public{});
}

BOOST_AUTO_TEST_CASE(ec_public__construct__compressed_string__round_trips)
{
    const ec_public instance(COMPRESSED);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.compressed());
    BOOST_REQUIRE_EQUAL(instance.encoded(), COMPRESSED);
}

BOOST_AUTO_TEST_CASE(ec_public__construct__uncompressed_string__round_trips)
{
    const ec_public instance(UNCOMPRESSED);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(!instance.compressed());
    BOOST_REQUIRE_EQUAL(instance.encoded(), UNCOMPRESSED);
}

BOOST_AUTO_TEST_CASE(ec_public__construct__uncompressed_string__compressed_point)
{
    const ec_public instance(UNCOMPRESSED);
    BOOST_REQUIRE_EQUAL(encode_base16(instance.point()), COMPRESSED);
}

BOOST_AUTO_TEST_CASE(ec_public__construct__not_base16__invalid)
{
    BOOST_REQUIRE(!ec_public{ "bogus" });
}

BOOST_AUTO_TEST_CASE(ec_public__construct__not_a_point__invalid)
{
    BOOST_REQUIRE(!ec_public{ base16_chunk("0102030405") });
}

BOOST_AUTO_TEST_CASE(ec_public__construct__off_curve_uncompressed__invalid)
{
    BOOST_REQUIRE(!ec_public{ base16_chunk(OFF_CURVE) });
}

BOOST_AUTO_TEST_CASE(ec_public__construct__uncompressed_point__expected)
{
    const ec_public instance(base16_array(UNCOMPRESSED), true);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.compressed());
    BOOST_REQUIRE_EQUAL(instance.encoded(), COMPRESSED);
}

BOOST_AUTO_TEST_CASE(ec_public__construct__off_curve_point__invalid)
{
    BOOST_REQUIRE(!ec_public(base16_array(OFF_CURVE), true));
}

BOOST_AUTO_TEST_CASE(ec_public__construct__bad_prefix_point__invalid)
{
    auto point = base16_array(UNCOMPRESSED);
    point.front() = 0x05;
    BOOST_REQUIRE(!ec_public(point, true));
}

BOOST_AUTO_TEST_CASE(ec_public__construct__private__expected)
{
    const ec_private secret(base16_array(SECRET));
    BOOST_REQUIRE(secret);

    const ec_public instance(secret);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(instance.point(), secret.to_public().point());
}

BOOST_AUTO_TEST_CASE(ec_public__construct__invalid_private__invalid)
{
    BOOST_REQUIRE(!ec_public{ ec_private{} });
}

// methods

BOOST_AUTO_TEST_CASE(ec_public__to_data__compressed__expected)
{
    data_chunk out;
    BOOST_REQUIRE(ec_public(COMPRESSED).to_data(out));
    BOOST_REQUIRE_EQUAL(encode_base16(out), COMPRESSED);
}

BOOST_AUTO_TEST_CASE(ec_public__to_data__uncompressed__expected)
{
    data_chunk out;
    BOOST_REQUIRE(ec_public(UNCOMPRESSED).to_data(out));
    BOOST_REQUIRE_EQUAL(encode_base16(out), UNCOMPRESSED);
}

BOOST_AUTO_TEST_CASE(ec_public__to_data__invalid__false)
{
    data_chunk out;
    BOOST_REQUIRE(!ec_public{}.to_data(out));
}

BOOST_AUTO_TEST_CASE(ec_public__to_uncompressed__valid__expected)
{
    ec_uncompressed out;
    BOOST_REQUIRE(ec_public(COMPRESSED).to_uncompressed(out));
    BOOST_REQUIRE_EQUAL(encode_base16(out), UNCOMPRESSED);
}

BOOST_AUTO_TEST_CASE(ec_public__to_uncompressed__invalid__false)
{
    ec_uncompressed out;
    BOOST_REQUIRE(!ec_public{}.to_uncompressed(out));
}

BOOST_AUTO_TEST_CASE(ec_public__to_payment_address__uncompressed__genesis_address)
{
    const auto address = ec_public(UNCOMPRESSED).to_payment_address();
    BOOST_REQUIRE(address);
    BOOST_REQUIRE_EQUAL(address.encoded(), UNCOMPRESSED_ADDRESS);
}

// operators

BOOST_AUTO_TEST_CASE(ec_public__equality__same__equal)
{
    const ec_public left(COMPRESSED);
    const ec_public right(COMPRESSED);
    BOOST_REQUIRE(left == right);
    BOOST_REQUIRE(!(left != right));
    BOOST_REQUIRE(!(left < right));
}

BOOST_AUTO_TEST_CASE(ec_public__equality__same_point_different_compression__unequal)
{
    const ec_public compressed(COMPRESSED);
    const ec_public uncompressed(UNCOMPRESSED);
    BOOST_REQUIRE_EQUAL(compressed.point(), uncompressed.point());
    BOOST_REQUIRE(compressed != uncompressed);
    BOOST_REQUIRE(compressed < uncompressed);
}

BOOST_AUTO_TEST_CASE(ec_public__stream__valid__round_trips)
{
    std::istringstream in{ COMPRESSED };
    ec_public instance{};
    in >> instance;
    BOOST_REQUIRE_EQUAL(instance.encoded(), COMPRESSED);

    std::ostringstream out{};
    out << instance;
    BOOST_REQUIRE_EQUAL(out.str(), COMPRESSED);
}

BOOST_AUTO_TEST_CASE(ec_public__stream__invalid__throws)
{
    ec_public instance{};
    BOOST_REQUIRE_THROW(std::istringstream("bogus") >> instance, istream_exception);
}
BOOST_AUTO_TEST_SUITE_END()

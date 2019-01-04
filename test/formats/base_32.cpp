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

#include <string>
#include <bitcoin/system.hpp>

using namespace bc::system;

BOOST_AUTO_TEST_SUITE(base_32_tests)

// encode_base32 (always valid)

////// See comments on encode_base32 implementation.
////BOOST_AUTO_TEST_CASE(base_32__encode_base32__vector1__expected)
////{
////    BOOST_REQUIRE_EQUAL(encode_base32({ "A", {} }), "A12UEL5L");
////}

BOOST_AUTO_TEST_CASE(base_32__encode_base32__vector2__expected)
{
    BOOST_REQUIRE_EQUAL(encode_base32({ "a", {} }), "a12uel5l");
}

BOOST_AUTO_TEST_CASE(base_32__encode_base32__vector3__expected)
{
    BOOST_REQUIRE_EQUAL(encode_base32({ "an83characterlonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio", {} }), "an83characterlonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio1tt5tgs");
}

BOOST_AUTO_TEST_CASE(base_32__encode_base32__vector4__expected)
{
    data_chunk payload;
    BOOST_REQUIRE(decode_base16(payload, "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"));
    BOOST_REQUIRE_EQUAL(encode_base32({ "abcdef", payload }), "abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw");
}

BOOST_AUTO_TEST_CASE(base_32__encode_base32__vector5__expected)
{
    data_chunk payload;
    BOOST_REQUIRE(decode_base16(payload, "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
    BOOST_REQUIRE_EQUAL(encode_base32({ "1", payload }), "11qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqc8247j");
}

BOOST_AUTO_TEST_CASE(base_32__encode_base32__vector6__expected)
{
    data_chunk payload;
    BOOST_REQUIRE(decode_base16(payload, "18171918161c01100b1d0819171d130d10171d16191c01100b03191d1b1903031d130b190303190d181d01190303190d"));
    BOOST_REQUIRE_EQUAL(encode_base32({ "split", payload }), "split1checkupstagehandshakeupstreamerranterredcaperred2y9e3w");
}

BOOST_AUTO_TEST_CASE(base_32__encode_base32__vector7__expected)
{
    BOOST_REQUIRE_EQUAL(encode_base32({ "?", {} }), "?1ezyfcl");
}

// decode_base32 valid

BOOST_AUTO_TEST_CASE(base_32__decode_base32__valid_vector1__true_expected)
{
    base32 decoded;
    BOOST_REQUIRE(decode_base32(decoded, "A12UEL5L"));
    BOOST_REQUIRE_EQUAL(decoded.prefix, "a");
    BOOST_REQUIRE(decoded.payload.empty());
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__valid_vector2__true_expected)
{
    base32 decoded;
    BOOST_REQUIRE(decode_base32(decoded, "a12uel5l"));
    BOOST_REQUIRE_EQUAL(decoded.prefix, "a");
    BOOST_REQUIRE(decoded.payload.empty());
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__valid_vector3__true_expected)
{
    base32 decoded;
    BOOST_REQUIRE(decode_base32(decoded, "an83characterlonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio1tt5tgs"));
    BOOST_REQUIRE_EQUAL(decoded.prefix, "an83characterlonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio");
    BOOST_REQUIRE(decoded.payload.empty());
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__valid_vector4__true_expected)
{
    data_chunk payload;
    BOOST_REQUIRE(decode_base16(payload, "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"));

    base32 decoded;
    BOOST_REQUIRE(decode_base32(decoded, "abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw"));
    BOOST_REQUIRE_EQUAL(decoded.prefix, "abcdef");
    BOOST_REQUIRE(decoded.payload == payload);
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__valid_vector5__true_expected)
{
    data_chunk payload;
    BOOST_REQUIRE(decode_base16(payload, "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));

    base32 decoded;
    BOOST_REQUIRE(decode_base32(decoded, "11qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqc8247j"));
    BOOST_REQUIRE_EQUAL(decoded.prefix, "1");
    BOOST_REQUIRE(decoded.payload == payload);
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__valid_vector6__true_expected)
{
    data_chunk payload;
    BOOST_REQUIRE(decode_base16(payload, "18171918161c01100b1d0819171d130d10171d16191c01100b03191d1b1903031d130b190303190d181d01190303190d"));

    base32 decoded;
    BOOST_REQUIRE(decode_base32(decoded, "split1checkupstagehandshakeupstreamerranterredcaperred2y9e3w"));
    BOOST_REQUIRE_EQUAL(decoded.prefix, "split");
    BOOST_REQUIRE(decoded.payload == payload);
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__valid_vector7__true_expected)
{
    base32 decoded;
    BOOST_REQUIRE(decode_base32(decoded, "?1ezyfcl"));
    BOOST_REQUIRE_EQUAL(decoded.prefix, "?");
    BOOST_REQUIRE(decoded.payload.empty());
}

// decode_base32 invalid

BOOST_AUTO_TEST_CASE(base_32__decode_base32__prefix_character_out_of_range1__false)
{
    base32 decoded;
    BOOST_REQUIRE(!decode_base32(decoded, "\x20""1nwldj5"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__prefix_character_out_of_range2__false)
{
    base32 decoded;
    BOOST_REQUIRE(!decode_base32(decoded, "\x7F""1axkwrx"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__prefix_character_out_of_range3__false)
{
    base32 decoded;
    BOOST_REQUIRE(!decode_base32(decoded, "\x80""1eym55h"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__overall_max_length_exceeded__false)
{
    base32 decoded;
    BOOST_REQUIRE(!decode_base32(decoded, "an84characterslonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio1569pvx"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__no_separator_character__false)
{
    base32 decoded;
    BOOST_REQUIRE(!decode_base32(decoded, "pzry9x0s0muk"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__empty_prefix__false)
{
    base32 decoded;
    BOOST_REQUIRE(!decode_base32(decoded, "1pzry9x0s0muk"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__invalid_payload_character__false)
{
    base32 decoded;
    BOOST_REQUIRE(!decode_base32(decoded, "x1b4n0q5v"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__too_short_checksum__false)
{
    base32 decoded;
    BOOST_REQUIRE(!decode_base32(decoded, "li1dgmt3"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__invalid_character_in_checksum__false)
{
    base32 decoded;
    BOOST_REQUIRE(!decode_base32(decoded, "de1lg7wt\xFF"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__checksum_calculated_with_uppercase_form_of_prefix__false)
{
    base32 decoded;
    BOOST_REQUIRE(!decode_base32(decoded, "A1G7SGD8"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__empty_prefix1__false)
{
    base32 decoded;
    BOOST_REQUIRE(!decode_base32(decoded, "10a06t8"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__empty_prefix2__false)
{
    base32 decoded;
    BOOST_REQUIRE(!decode_base32(decoded, "1qzzfhee"));
}

BOOST_AUTO_TEST_SUITE_END()
